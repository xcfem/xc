//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
//  XC is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//ThreedimMecLoad.cpp

#include "ThreedimMecLoad.h"
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include "utility/matrix/ID.h"

#include "domain/mesh/element/utils/coordTransformation/CrdTransf.h"
#include "utility/actor/actor/MovableVector.h"
#include "domain/mesh/node/Node.h"
#include "utility/geom/pos_vec/SlidingVectorsSystem3d.h"

XC::ThreedimMecLoad::ThreedimMecLoad(int tag, int classTag, const ID &theElementTags)
  :ThreedimLoad(tag, classTag, theElementTags) {}

XC::ThreedimMecLoad::ThreedimMecLoad(int tag,int classTag)
  :ThreedimLoad(tag, classTag) {}

//! @brief Return the dimension of the force vector.
size_t XC::ThreedimMecLoad::getForceVectorDimension(void)
  { return 3; }

//! @brief Returns force expressed in local coordinates.
XC::Vector XC::ThreedimMecLoad::getLocalForce(void) const
  {
    Vector retval(3);
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented." << std::endl;
    return retval;
  }

//! @brief Return the local components of the
//! force in a Vector3d. 
Vector3d XC::ThreedimMecLoad::getVector3dLocalForce(void) const
  {
    Vector f= this->getLocalForce();
    return Vector3d(f[0], f[1], f[2]);
  }

//! @brief Returns the components of the force vectors.
const XC::Matrix &XC::ThreedimMecLoad::getLocalForces(void) const
  {
    static Matrix retval;
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented." << std::endl;
    return retval;
  }

//! @brief Return the force expressed in global coordinates.
const XC::Matrix &XC::ThreedimMecLoad::getGlobalForces(void) const
  { return getGlobalVectors(getLocalForces()); }

//! brief Returns load resultant (force and moment integration over the elements).
//!
//! @param center: origin for the sliding vector system.
//! @param initialGeometry: if true don't use deformed geometry.
SlidingVectorsSystem3d XC::ThreedimMecLoad::getResultant(const Pos3d &center, bool initialGeometry) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; not yet implemented." << std::endl;
    return SlidingVectorsSystem3d(center);
  }

