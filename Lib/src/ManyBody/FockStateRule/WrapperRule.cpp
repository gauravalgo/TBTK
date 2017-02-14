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

/** @file WrapperRule.h
 *
 *  @author Kristofer Björnson
 */

#include "WrapperRule.h"
#include "TBTKMacros.h"

namespace TBTK{
namespace FockStateRule{

WrapperRule::WrapperRule(
	const FockStateRule &fockStateRule
){
	this->fockStateRule = fockStateRule.clone();
}

WrapperRule::WrapperRule(const WrapperRule &wrapperRule){
	this->fockStateRule = (FockStateRule*)wrapperRule.clone();
}

WrapperRule::~WrapperRule(){
	delete fockStateRule;
}

WrapperRule* WrapperRule::clone() const{
	return new WrapperRule(*fockStateRule);
}

WrapperRule& WrapperRule::operator=(const WrapperRule &wrapperRule){
	this->fockStateRule = (FockStateRule*)wrapperRule.clone();

	return *this;
}

bool WrapperRule::isSatisfied(
	const FockSpace<BitRegister> &fockSpace,
	const FockState<BitRegister> &fockState
) const{
	return fockStateRule->isSatisfied(fockSpace, fockState);
}

bool WrapperRule::isSatisfied(
	const FockSpace<ExtensiveBitRegister> &fockSpace,
	const FockState<ExtensiveBitRegister> &fockState
) const{
	return fockStateRule->isSatisfied(fockSpace, fockState);
}

};	//End of namespace FockSpaceRule
};	//End of namespace TBTK