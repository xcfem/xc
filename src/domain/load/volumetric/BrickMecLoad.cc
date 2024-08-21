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
//BrickMecLoad.cpp

#include "BrickMecLoad.h"
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include "utility/matrix/ID.h"

#include "domain/mesh/element/volumetric/BrickBase.h"
#include "domain/mesh/node/Node.h"
#include "utility/geom/pos_vec/SlidingVectorsSystem3d.h"
#include "utility/geom/pos_vec/SlidingVector3d.h"

XC::BrickMecLoad::BrickMecLoad(int tag,int classTag,const ID &theElementTags)
  : ThreedimMecLoad(tag, classTag, theElementTags) {}

XC::BrickMecLoad::BrickMecLoad(int tag,int classTag)
  : ThreedimMecLoad(tag, classTag) {}

//! @brief Adds the load the consistent load vector (see page 108 libro Eugenio Oñate).
//! @param areas tributary areas for each node.
//! @param loadFactor Load factor.
//! @param p0 element load vector.
void XC::BrickMecLoad::addReactionsInBasicSystem(const std::vector<double> &,const double &,FVectorBrick &) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented." << std::endl;
  }

//! @brief ??
//! @param areas tributary areas for each node.
//! @param loadFactor Load factor.
//! @param q0 
void XC::BrickMecLoad::addFixedEndForcesInBasicSystem(const std::vector<double> &,const double &loadFactor,FVectorBrick &) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented." << std::endl;
  }

//! @brief Return the dimension of the force vector.
size_t XC::BrickMecLoad::getForceVectorDimension(void) const
  { return 3; }

//! @brief Returns force expressed in local coordinates.
XC::Vector XC::BrickMecLoad::getLocalForce(void) const
  {
    Vector retval(3);
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented." << std::endl;
    return retval;
  }

//! @brief Return the local components of the
//! force in a Vector3d. 
Vector3d XC::BrickMecLoad::getVector3dLocalForce(void) const
  {
    Vector f= getLocalForce();
    return Vector3d(f[0], f[1], f[2]);
  }

//! @brief Returns the components of the force vectors.
const XC::Matrix &XC::BrickMecLoad::getLocalForces(void) const
  {
    static Matrix retval;
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented." << std::endl;
    return retval;
  }

//! @brief Returns the vectors expressed in global coordinates.
const XC::Matrix &XC::BrickMecLoad::getGlobalVectors(const Matrix &localVectors) const
  {
    static Matrix retval;
    retval= localVectors;
    const Domain *ptrDom= getDomain();
    if(ptrDom)
      {
        const size_t sz= localVectors.noRows();
        for(size_t i=0; i<sz; i++)
          {
            const size_t elemTag= getElementTags()(i);
            const Element *ptrElem= ptrDom->getElement(elemTag);
	    const BrickBase *ptrBrick= dynamic_cast<const BrickBase *>(ptrElem);
            if(ptrBrick)
              {
                retval= localVectors; // No transformation needed.
              }
            else
	      std::cerr << getClassName() << "::" << __FUNCTION__ << ": the element: "
                        << elemTag << " is not a brick element." << std::endl;
          }
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< ": pointer to domain is NULL." << std::endl;
    return retval;
  }

//! @brief Return the force expressed in global coordinates.
const XC::Matrix &XC::BrickMecLoad::getGlobalForces(void) const
  { return getGlobalVectors(getLocalForces()); }

//! brief Returns load resultant (force and moment integration over the elements).
SlidingVectorsSystem3d XC::BrickMecLoad::getResultant(const Pos3d &center, bool initialGeometry) const
  {
    SlidingVectorsSystem3d retval(center);
    Matrix forces= getGlobalForces();
    const Domain *ptrDom= getDomain();
    if(ptrDom)
      {
        const size_t sz= forces.noRows();
        for(size_t i=0; i<sz; i++)
          {
            const size_t elemTag= getElementTags()(i);
            const Element *ptrElem= ptrDom->getElement(elemTag);
            if(const BrickBase *ptrBrick= dynamic_cast<const BrickBase *>(ptrElem))
	      {
  	        const double volume= ptrBrick->getVolume();
		const Vector3d force(volume*forces(i,0), volume*forces(i,1), volume*forces(i,2));
		retval+= SlidingVector3d(ptrBrick->getCenterOfMassPosition(),force);
              }
            else
	      std::cerr << getClassName() << "::" << __FUNCTION__
			<< "; the element: "
                        << elemTag << " is not a brick element." << std::endl;
          }
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; pointer to domain is NULL." << std::endl;
    return retval;
  }
