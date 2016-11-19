/* Copyright 2016 Kristofer Björnson
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

/** @file MonoclinicBaseCentered.cpp
 *
 *  @author Kristofer Björnson
 */

#include "MonoclinicBaseCentered.h"
#include "Vector3d.h"

#include <cmath>

using namespace std;

namespace TBTK{
namespace Lattice{
namespace D3{

MonoclinicBaseCentered::MonoclinicBaseCentered(
	double side1Length,
	double side2Length,
	double side3Length,
	double angle23
) :
	MonoclinicPrimitive(
		side1Length,
		side2Length,
		side3Length,
		angle23
	)
{
	const vector<vector<double>> &latticeVectors = getLatticeVectors();

	Vector3d v0(latticeVectors.at(0));
	Vector3d v1(latticeVectors.at(1));

	vector<vector<double>> additionalSites;
	additionalSites.push_back(((v0 + v1)/2.).getStdVector());

	setAdditionalSites(additionalSites);
}

MonoclinicBaseCentered::~MonoclinicBaseCentered(){
}

};	//End of namespace D3
};	//End of namespace Lattice
};	//End of namespace TBTK
