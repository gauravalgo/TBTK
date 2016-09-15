/** @file DOS.cpp
 *
 *  @author Kristofer Björnson
 */

#include "../include/DOS.h"

namespace TBTK{
namespace Property{

DOS::DOS(double lowerBound, double upperBound, int resolution){
	this->lowerBound = lowerBound;
	this->upperBound = upperBound;
	this->resolution = resolution;
	data = new double[resolution];
	for(int n = 0; n < resolution; n++)
		data[n] = 0.;
}

DOS::~DOS(){
	delete [] data;
}

};	//End of namespace Property
};	//End of namespace TBTK