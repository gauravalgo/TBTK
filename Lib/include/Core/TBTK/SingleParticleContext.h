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

/** @package TBTKcalc
 *  @file SingleParticleContext.h
 *  @brief The context for the single particle part of a Model.
 *
 *  @author Kristofer Björnson
 */

#ifndef COM_DAFER45_TBTK_SINGLE_PARTICLE_CONTEXT
#define COM_DAFER45_TBTK_SINGLE_PARTICLE_CONTEXT

#include "TBTK/Geometry.h"
#include "TBTK/HoppingAmplitudeSet.h"
#include "TBTK/OverlapAmplitudeSet.h"
#include "TBTK/Serializable.h"
#include "TBTK/SourceAmplitudeSet.h"
#include "TBTK/Statistics.h"

namespace TBTK{

class FileReader;

/** @brief The context for the single particle part of a Model. */
class SingleParticleContext : public Serializable{
public:
	/** Constructor. */
	SingleParticleContext();

	/** Constructor. */
	SingleParticleContext(const std::vector<unsigned int> &capacity);

	/** Constructor. Constructs the SingleParticleContext from a
	 *  serializeation string. */
	SingleParticleContext(const std::string &serialization, Mode mode);

	/** Set statistics.
	 *
	 *  @param statistics The Statistics to use.*/
	void setStatistics(Statistics statistics);

	/** Get statistics.
	 *
	 *  @return The currently set Statistics. */
	Statistics getStatistics() const;

	/** Get the contained HoppingAmplitudeSet.
	 *
	 *  @return The contained HoppingAmplitudeSet. */
	HoppingAmplitudeSet& getHoppingAmplitudeSet();

	/** Get the contained HoppingAmplitudeSet.
	 *
	 *  @return The contained HoppingAmplitudeSet. */
	const HoppingAmplitudeSet& getHoppingAmplitudeSet() const;

	/** Get the contained SourceAmplitudeSet.
	 *
	 *  @return The contained SourceAmplitudeSet. */
	SourceAmplitudeSet& getSourceAmplitudeSet();

	/** Get the contained SourceAmplitudeSet.
	 *
	 *  @return The contained SourceAmplitudeSet. */
	const SourceAmplitudeSet& getSourceAmplitudeSet() const;

	/** Get the contained OverlapAmplitudeSet.
	 *
	 *  @return The contained OverlapAmplitudeSet. */
	OverlapAmplitudeSet& getOverlapAmplitudeSet();

	/** Get the contained OverlapAmplitudeSet.
	 *
	 *  @return The contained OverlapAmplitudeSet. */
	const OverlapAmplitudeSet& getOverlapAmplitudeSet() const;

	/** Get Geometry. */
	Geometry& getGeometry();

	/** Get Geometry. */
	const Geometry& getGeometry() const;

	/** Implements Serializable::serialize(). */
	std::string serialize(Mode mode) const;
private:
	/** HoppingAmplitudeSet. */
	HoppingAmplitudeSet hoppingAmplitudeSet;

	/** SourceAmplitudeSet. */
	SourceAmplitudeSet sourceAmplitudeSet;

	/** OverlapAmplitudeSet. */
	OverlapAmplitudeSet overlapAmplitudeSet;

	/** Statistics (Fermi-Dirac or Bose-Einstein).*/
	Statistics statistics;

	/** Geometry. */
	Geometry geometry;

	/** FileReader is a friend class to allow it to write Model data. */
	friend class FileReader;
};

inline void SingleParticleContext::setStatistics(Statistics statistics){
	this->statistics = statistics;
}

inline Statistics SingleParticleContext::getStatistics() const{
	return statistics;
}

inline HoppingAmplitudeSet& SingleParticleContext::getHoppingAmplitudeSet(){
	return hoppingAmplitudeSet;
}

inline const HoppingAmplitudeSet&
SingleParticleContext::getHoppingAmplitudeSet() const{
	return hoppingAmplitudeSet;
}

inline SourceAmplitudeSet& SingleParticleContext::getSourceAmplitudeSet(){
	return sourceAmplitudeSet;
}

inline const SourceAmplitudeSet&
SingleParticleContext::getSourceAmplitudeSet() const{
	return sourceAmplitudeSet;
}

inline OverlapAmplitudeSet& SingleParticleContext::getOverlapAmplitudeSet(){
	return overlapAmplitudeSet;
}

inline const OverlapAmplitudeSet&
SingleParticleContext::getOverlapAmplitudeSet() const{
	return overlapAmplitudeSet;
}

inline Geometry& SingleParticleContext::getGeometry(){
	return geometry;
}

inline const Geometry& SingleParticleContext::getGeometry() const{
	return geometry;
}

};	//End of namespace TBTK

#endif
