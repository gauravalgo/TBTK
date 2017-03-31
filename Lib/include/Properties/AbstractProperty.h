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

/** @package TBTKcalc
 *  @file AbstractPoperty.h
 *  @brief Abstract Property class
 *
 *  @author Kristofer Björnson
 */

#ifndef COM_DAFER45_TBTK_ABSTRACT_PROPERTY
#define COM_DAFER45_TBTK_ABSTRACT_PROPERTY

#include "IndexDescriptor.h"

namespace TBTK{
namespace Property{

template<typename DataType>
class AbstractProperty{
public:
	/** Set size. */
	void setSize(unsigned int size);

	/** Get size. */
	unsigned int getSize() const;

	/** Get data. */
	const DataType* getData() const;

	/** Same as getData, but with write access. */
	DataType* getDataRW();
protected:
	/** Constructor. */
	AbstractProperty();

	/** Copy constructor. */
	AbstractProperty(const AbstractProperty &abstractProperty);

	/** Move constructor. */
	AbstractProperty(AbstractProperty &&abstractProperty);

	/** Assignment operator. */
	AbstractProperty& operator=(const AbstractProperty &abstractProperty);

	/** Move assignment operator. */
	AbstractProperty& operator=(AbstractProperty &&abstractProperty);

	/** Destructor. */
	~AbstractProperty();
private:
	/** Number of data elements. */
	unsigned int size;

	/** Data. */
	DataType *data;
};

template<typename DataType>
inline void AbstractProperty<DataType>::setSize(unsigned int size){
	this->size = size;
	if(data != nullptr)
		delete [] data;
	data = new DataType[size];
}

template<typename DataType>
inline unsigned int AbstractProperty<DataType>::getSize() const{
	return size;
}

template<typename DataType>
inline const DataType* AbstractProperty<DataType>::getData() const{
	return data;
}

template<typename DataType>
inline DataType* AbstractProperty<DataType>::getDataRW(){
	return data;
}

template<typename DataType>
AbstractProperty<DataType>::AbstractProperty(){
	size = 0;
	data = nullptr;
}

template<typename DataType>
AbstractProperty<DataType>::AbstractProperty(
	const AbstractProperty &abstractProperty
){
	size = abstractProperty.size;
	if(abstractProperty.data == nullptr){
		data = nullptr;
	}
	else{
		data = new DataType[size];
		for(int n = 0; n < size; n++)
			data[n] = abstractProperty.data[n];
	}
}

template<typename DataType>
AbstractProperty<DataType>::AbstractProperty(
	AbstractProperty &&abstractProperty
){
	size = abstractProperty.size;
	if(abstractProperty.data == nullptr){
		data = nullptr;
	}
	else{
		data = abstractProperty.data;
		abstractProperty.data = nullptr;
	}
}

template<typename DataType>
AbstractProperty<DataType>::~AbstractProperty(){
	if(data != nullptr)
		delete [] data;
}

template<typename DataType>
AbstractProperty<DataType>& AbstractProperty<DataType>::operator=(
	const AbstractProperty &rhs
){
	size = rhs.size;
	if(rhs.data == nullptr){
		data = nullptr;
	}
	else{
		data = new DataType[size];
		for(int n = 0; n < size; n++)
			data[n] = rhs.data[n];
	}

	return *this;
}

template<typename DataType>
AbstractProperty<DataType>& AbstractProperty<DataType>::operator=(
	AbstractProperty &&rhs
){
	if(this != &rhs){
		size = rhs.size;
		if(rhs.data == nullptr){
			data = nullptr;
		}
		else{
			data = rhs.data;
			rhs.data = nullptr;
		}
	}

	return *this;
}

};	//End namespace Property
};	//End namespace TBTK

#endif
