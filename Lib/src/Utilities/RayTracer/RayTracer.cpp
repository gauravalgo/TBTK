/* Copyright 2017 Kristofer Björnson
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/** @file RayTracer.cpp
 *
 *  @author Kristofer Björnson
 */

#include "Plotter/Plotter.h"
#include "../../../include/Utilities/RayTracer/RayTracer.h"
#include "Smooth.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

namespace TBTK{

bool RayTracer::EventHandler::isLocked = false;
RayTracer *RayTracer::EventHandler::owner = nullptr;
function<void(int event, int x, int y, int flags, void *userData)> &&RayTracer::EventHandler::lambdaOnMouseChange = {};

RayTracer::RayTracer(){
}

RayTracer::~RayTracer(){
}

void RayTracer::plot(const Model& model, const Property::Density &density){
	const IndexDescriptor &indexDescriptor = density.getIndexDescriptor();
	TBTKAssert(
		indexDescriptor.getFormat() == IndexDescriptor::Format::Custom,
		"RayTracer::plot()",
		"Only storage format IndexDescriptor::Format::Custom supported.",
		"Use calculateProperty(patterns) instead of "
			<< "calculateProperty(pattern, ranges) when extracting"
			<< " properties."
	);

	render(
		indexDescriptor,
		model,
		[&density](const HitDescriptor &hitDescriptor) -> RayTracer::Material
		{
			Material material;
			material.color.r = density(hitDescriptor.getIndex());
			material.color.g = density(hitDescriptor.getIndex());
			material.color.b = density(hitDescriptor.getIndex());

			return material;
		}
	);
}

void RayTracer::plot(
	const Model& model,
	const Property::Magnetization &magnetization
){
	const IndexDescriptor &indexDescriptor = magnetization.getIndexDescriptor();
	TBTKAssert(
		indexDescriptor.getFormat() == IndexDescriptor::Format::Custom,
		"RayTracer::plot()",
		"Only storage format IndexDescriptor::Format::Custom supported.",
		"Use calculateProperty(patterns) instead of "
			<< "calculateProperty(pattern, ranges) when extracting"
			<< " properties."
	);

	render(
		indexDescriptor,
		model,
		[&magnetization](HitDescriptor &hitDescriptor) -> RayTracer::Material
		{
			Vector3d directionFromObject = hitDescriptor.getDirectionFromObject();
			const SpinMatrix& spinMatrix = magnetization(
				hitDescriptor.getIndex()
			);
			Vector3d spinDirection = spinMatrix.getDirection();
			double projection = Vector3d::dotProduct(
				directionFromObject,
				spinDirection
			);
//			double density = spinMatrix.getDensity();

			Material material;
			if(projection > 0){
				material.color.r = 255;
				material.color.g = 0;
				material.color.b = 0;
			}
			else{
				material.color.r = 255;
				material.color.g = 255;
				material.color.b = 255;
			}

			return material;
		}
	);
}

void RayTracer::plot(
	const Model& model,
	const Property::WaveFunction &waveFunction,
	unsigned int state
){
	const IndexDescriptor &indexDescriptor = waveFunction.getIndexDescriptor();
	TBTKAssert(
		indexDescriptor.getFormat() == IndexDescriptor::Format::Custom,
		"RayTracer::plot()",
		"Only storage format IndexDescriptor::Format::Custom supported.",
		"Use calculateProperty(patterns) instead of "
			<< "calculateProperty(pattern, ranges) when extracting"
			<< " properties."
	);

	render(
		indexDescriptor,
		model,
		[&waveFunction, state](HitDescriptor &hitDescriptor) -> RayTracer::Material
		{
			complex<double> amplitude = waveFunction(hitDescriptor.getIndex(), state);
			double absolute = abs(amplitude);
			double argument = arg(amplitude);
			if(argument < 0)
				argument += 2*M_PI;

			Material material;
			material.color.r = absolute*(2*M_PI - argument);
			material.color.g = 0;
			material.color.b = absolute*argument;

			return material;
		}
	);
}

void RayTracer::interactivePlot(
	const Model &model,
	const Property::LDOS &ldos,
	double sigma,
	unsigned int windowSize
){
	const IndexDescriptor &indexDescriptor = ldos.getIndexDescriptor();
	TBTKAssert(
		indexDescriptor.getFormat() == IndexDescriptor::Format::Custom,
		"RayTracer::plot()",
		"Only storage format IndexDescriptor::Format::Custom supported.",
		"Use calculateProperty(patterns) instead of "
			<< "calculateProperty(pattern, ranges) when extracting"
			<< " properties."
	);

	render(
		indexDescriptor,
		model,
		[&ldos](HitDescriptor &hitDescriptor) -> RayTracer::Material
		{
			Material material;
			material.color.r = 255;
			material.color.g = 255;
			material.color.b = 255;

			return material;
		},
		[&ldos, sigma, windowSize](Mat &canvas, const Index &index){
			vector<double> data;
			vector<double> axis;
			double lowerBound = ldos.getLowerBound();
			double upperBound = ldos.getUpperBound();
			unsigned int resolution = ldos.getResolution();
			double dE = (upperBound - lowerBound)/resolution;
			for(int n = 0; n < ldos.getResolution(); n++){
				data.push_back(ldos(index, n));
				axis.push_back(lowerBound + n*dE);
			}

			Plotter plotter;
			plotter.setCanvas(canvas);
			if(sigma != 0){
				double scaledSigma = sigma/(ldos.getUpperBound() - ldos.getLowerBound())*ldos.getResolution();
				data = Smooth::gaussian(data, scaledSigma, windowSize);
			}
			plotter.plot(axis, data);

			int baseLine;
			Size size = getTextSize(
				index.toString(),
				FONT_HERSHEY_SIMPLEX,
				0.5,
				1,
				&baseLine
			);
			putText(
				canvas,
				index.toString(),
				Point(
					canvas.cols - size.width - 10,
					size.height + 10
				),
				FONT_HERSHEY_SIMPLEX,
				0.5,
				Scalar(0, 0, 0),
				1,
				true
			);
		}
	);
}

void RayTracer::render(
	const IndexDescriptor &indexDescriptor,
	const Model &model,
	function<Material(HitDescriptor &hitDescriptor)> &&lambdaColorPicker,
	function<void(Mat &canvas, const Index &index)> &&lambdaInteractive
){
	const Vector3d &cameraPosition = renderContext.getCameraPosition();
	const Vector3d &focus = renderContext.getFocus();
	const Vector3d &up = renderContext.getUp();
	unsigned int width = renderContext.getWidth();
	unsigned int height = renderContext.getHeight();
//	double stateRadius = renderContext.getStateRadius();

	Vector3d unitY = up.unit();
	Vector3d unitX = ((focus - cameraPosition)*up).unit();
	unitY = (unitX*(focus - cameraPosition)).unit();
	double scaleFactor = (focus - cameraPosition).norm()/(double)width;

	const Geometry *geometry = model.getGeometry();

	const IndexTree &indexTree = indexDescriptor.getIndexTree();
	IndexTree::Iterator iterator = indexTree.begin();
	const Index *index;
	vector<Vector3d> coordinates;
	while((index = iterator.getIndex())){
		Index i = *index;
		for(unsigned int n = 0; n < i.size(); n++)
			if(i.at(n) < 0)
				i.at(n) = IDX_ALL;

		vector<Index> indices = model.getHoppingAmplitudeSet()->getIndexList(i);

		coordinates.push_back(Vector3d({0., 0., 0.}));
		for(unsigned int n = 0; n < indices.size(); n++){
			const double *c = geometry->getCoordinates(indices.at(n));
			coordinates.back().x += c[0]/indices.size();
			coordinates.back().y += c[1]/indices.size();
			coordinates.back().z += c[2]/indices.size();
		}

		iterator.searchNext();
	}

	Mat canvas = Mat::zeros(height, width, CV_32FC3);
	vector<HitDescriptor> **hitDescriptors = new vector<HitDescriptor>*[width];
	for(unsigned int x = 0; x < width; x++)
		hitDescriptors[x] = new vector<HitDescriptor>[height];
	for(unsigned int x = 0; x < width; x++){
		for(unsigned int y = 0; y < height; y++){
			Vector3d target = focus
				+ (scaleFactor*((double)x - width/2.))*unitX
				+ (scaleFactor*((double)y - height/2.))*unitY;
			Vector3d rayDirection = (target - cameraPosition).unit();

			Color color = trace(
				coordinates,
				cameraPosition,
				rayDirection,
				indexTree,
				hitDescriptors[x][y],
				lambdaColorPicker
			);

			canvas.at<Vec3f>(height - 1 - y, x)[0] = color.b;
			canvas.at<Vec3f>(height - 1 - y, x)[1] = color.g;
			canvas.at<Vec3f>(height - 1 - y, x)[2] = color.r;
		}
	}

	double minValue = canvas.at<Vec3f>(0, 0)[0];
	double maxValue = canvas.at<Vec3f>(0, 0)[0];
	for(unsigned int x = 0; x < width; x++){
		for(unsigned int y = 0; y < height; y++){
			for(int n = 0; n < 3; n++){
				if(canvas.at<Vec3f>(y, x)[n] < minValue)
					minValue = canvas.at<Vec3f>(y, x)[n];
				if(canvas.at<Vec3f>(y, x)[n] > maxValue)
					maxValue = canvas.at<Vec3f>(y, x)[n];
			}
		}
	}

	Mat image = Mat::zeros(height, width, CV_8UC3);
	for(unsigned int x = 0; x < width; x++){
		for(unsigned int y = 0; y < height; y++){
			for(unsigned int n = 0; n < 3; n++){
				if(hitDescriptors[x][y].size() > 0)
					image.at<Vec3b>(height - 1 - y, x)[n] = 255*((canvas.at<Vec3f>(height - 1 - y, x)[n] - minValue)/(maxValue - minValue));
				else
					image.at<Vec3b>(height - 1 - y, x)[n] = 0;
			}
		}
	}

	if(lambdaInteractive){
		namedWindow("Traced image", WINDOW_AUTOSIZE);
		namedWindow("Property window");
		imshow("Traced image", image);
		Mat propertyCanvas = Mat::zeros(400, 600, CV_8UC3);
		TBTKAssert(
			EventHandler::lock(
				this,
				[&lambdaInteractive, &hitDescriptors, &propertyCanvas, height, width](
					int event,
					int x,
					int y,
					int flags,
					void *userData
				){
					Plotter plotter;
					plotter.setCanvas(propertyCanvas);
					if(hitDescriptors[x][height - 1 - y].size() > 0){
						const Index& index = hitDescriptors[x][height - 1 -y].at(0).getIndex();
						lambdaInteractive(propertyCanvas, index);
					}
					imshow("Property window", propertyCanvas);
				}
			),
			"RayTracer::render()",
			"Unable to get lock from EventHandler.",
			""
		);
		setMouseCallback(
			"Traced image",
			EventHandler::onMouseChange,
			NULL
		);
		waitKey(0);
	}
	else{
		imwrite("figures/Density.png", image);
	}

	for(unsigned int x = 0; x < width; x++)
		delete [] hitDescriptors[x];
	delete [] hitDescriptors;
}

RayTracer::Color RayTracer::trace(
	const vector<Vector3d> &coordinates,
	const Vector3d &raySource,
	const Vector3d &rayDirection,
	const IndexTree &indexTree,
	vector<HitDescriptor> &hitDescriptors,
	function<Material(HitDescriptor &hitDescriptor)> lambdaColorPicker
){
	double stateRadius = renderContext.getStateRadius();

	vector<unsigned int> hits;
	for(unsigned int n = 0; n < coordinates.size(); n++)
		if(((coordinates.at(n) - raySource)*rayDirection).norm() < stateRadius)
			hits.push_back(n);

	Color color;
	color.r = 0;
	color.g = 0;
	color.b = 0;
	if(hits.size() > 0){
		double minDistance = (coordinates.at(hits.at(0)) - raySource).norm();
		unsigned int minDistanceIndex = hits.at(0);
		for(unsigned int n = 1; n < hits.size(); n++){
			double distance = (coordinates.at(hits.at(n)) - raySource).norm();
			if(distance < minDistance){
				minDistance = distance;
				minDistanceIndex = hits.at(n);
			}
		}

		HitDescriptor hitDescriptor(renderContext);
		hitDescriptor.setRayDirection(
			rayDirection
		);
		hitDescriptor.setIndex(
			indexTree.getPhysicalIndex(
				minDistanceIndex
			)
		);
		hitDescriptor.setCoordinate(
			coordinates.at(
				minDistanceIndex
			)
		);

		hitDescriptors.push_back(hitDescriptor);

		Material material = lambdaColorPicker(
			hitDescriptor
		);

		Vector3d directionFromObject = hitDescriptor.getDirectionFromObject();
		double lightProjection = Vector3d::dotProduct(
			directionFromObject.unit(),
			Vector3d({0, 0, 1})
		);
		color.r = material.color.r*(material.ambient + material.diffusive*lightProjection);
		color.g = material.color.g*(material.ambient + material.diffusive*lightProjection);
		color.b = material.color.b*(material.ambient + material.diffusive*lightProjection);

/*		TODO: Implement reflection.
		if(levels != 0){
			Color specularColor = trace(
				coordinates,
				impactPosition,
				rayDirection,
				indexTree,
				hitDescriptors[x][y],
				lambdaColorPicker
			);
		}*/
	}

	return color;
}

RayTracer::RenderContext::RenderContext(){
	cameraPosition = Vector3d({0, 0, 10});
	focus = Vector3d({0, 0, 0});
	up = Vector3d({0, 1, 0});

	width = 600;
	height = 400;

	stateRadius = 0.5;
}

RayTracer::RenderContext::~RenderContext(){
}

RayTracer::HitDescriptor::HitDescriptor(const RenderContext &renderContext){
	this->renderContext = &renderContext;
	directionFromObject = nullptr;
}

RayTracer::HitDescriptor::HitDescriptor(
	const HitDescriptor &hitDescriptor
) :
	renderContext(hitDescriptor.renderContext),
	rayDirection(hitDescriptor.rayDirection),
	index(hitDescriptor.index),
	coordinate(hitDescriptor.coordinate)
{

	if(hitDescriptor.directionFromObject == nullptr){
		directionFromObject = nullptr;
	}
	else{
		directionFromObject = new Vector3d(
			*hitDescriptor.directionFromObject
		);
	}
}

RayTracer::HitDescriptor::HitDescriptor(
	HitDescriptor &&hitDescriptor
) :
	renderContext(std::move(hitDescriptor.renderContext)),
	rayDirection(std::move(hitDescriptor.rayDirection)),
	index(std::move(hitDescriptor.index)),
	coordinate(std::move(hitDescriptor.coordinate))
{
	if(hitDescriptor.directionFromObject == nullptr){
		directionFromObject = nullptr;
	}
	else{
		directionFromObject = hitDescriptor.directionFromObject;
		hitDescriptor.directionFromObject = nullptr;
	}
}

RayTracer::HitDescriptor::~HitDescriptor(){
	if(directionFromObject != nullptr)
		delete directionFromObject;
}

RayTracer::HitDescriptor& RayTracer::HitDescriptor::operator=(
	const HitDescriptor &rhs
){
	renderContext = rhs.renderContext;
	rayDirection = rhs.rayDirection;
	index = rhs.index;
	coordinate = rhs.coordinate;

	if(rhs.directionFromObject == nullptr)
		directionFromObject = nullptr;
	else
		directionFromObject = new Vector3d(*rhs.directionFromObject);

	return *this;
}

RayTracer::HitDescriptor& RayTracer::HitDescriptor::operator=(
	HitDescriptor &&rhs
){
	if(this != &rhs){
		renderContext = rhs.renderContext;
		rayDirection = rhs.rayDirection;
		index = rhs.index;
		coordinate = rhs.coordinate;

		if(rhs.directionFromObject == nullptr){
			directionFromObject = nullptr;
		}
		else{
			directionFromObject = rhs.directionFromObject;
			rhs.directionFromObject = nullptr;
		}
	}

	return *this;
}

const Vector3d& RayTracer::HitDescriptor::getDirectionFromObject(){
	if(directionFromObject != nullptr)
		return *directionFromObject;

	const Vector3d &cameraPosition = renderContext->getCameraPosition();
//	const Vector3d &rayDirection = renderContext->getRayDirection();
	double stateRadius = renderContext->getStateRadius();

	//Here v is the vector from the object to the camera, t is the unit
	//vector in the direction of the ray, and lamvda*t is the vector from
	//the camera to the hit position.
	Vector3d v = coordinate - cameraPosition;
	double a = Vector3d::dotProduct(v, rayDirection);
	double b = Vector3d::dotProduct(v, v);
	double lambda = a - sqrt(stateRadius*stateRadius + a*a - b);
	Vector3d hitPoint = cameraPosition + lambda*rayDirection;

	directionFromObject = new Vector3d((hitPoint - coordinate).unit());

	return *directionFromObject;
}

};	//End of namespace TBTK