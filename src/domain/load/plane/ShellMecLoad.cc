//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
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
//ShellMecLoad.cpp

#include "ShellMecLoad.h"
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include "utility/matrix/ID.h"

#include "domain/mesh/element/plane/shell/ShellMITC4Base.h"
#include "domain/mesh/element/plane/shell/ShellMITC9.h"
#include "domain/mesh/element/plane/shell/ShellNLDKGQ.h"
#include "domain/mesh/element/utils/coordTransformation/CrdTransf.h"
#include "domain/mesh/node/Node.h"
#include "utility/geom/pos_vec/SlidingVectorsSystem3d.h"
#include "utility/geom/pos_vec/SlidingVector3d.h"

XC::ShellMecLoad::ShellMecLoad(int tag,int classTag,const ID &theElementTags)
  :BidimMecLoad(tag, classTag, theElementTags) {}

XC::ShellMecLoad::ShellMecLoad(int tag,int classTag)
  :BidimMecLoad(tag, classTag) {}

//! @brief Adds the load the consistent load vector (see page 108 libro Eugenio Oñate).
//! @param areas tributary areas for each node.
//! @param loadFactor Load factor.
//! @param p0 element load vector.
void XC::ShellMecLoad::addReactionsInBasicSystem(const std::vector<double> &,const double &,FVectorShell &) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented." << std::endl;
  }

//! @brief ??
//! @param areas tributary areas for each node.
//! @param loadFactor Load factor.
//! @param q0 
void XC::ShellMecLoad::addFixedEndForcesInBasicSystem(const std::vector<double> &,const double &loadFactor,FVectorShell &) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented." << std::endl;
  }

//! @brief Return the dimension of the force vector.
size_t XC::ShellMecLoad::getForceVectorDimension(void) const
  { return 3; }

//! @brief Return the dimension of the bending moment vector.
size_t XC::ShellMecLoad::getMomentVectorDimension(void) const
  { return 3; }

//! @brief Returns force expressed in local coordinates.
XC::Vector XC::ShellMecLoad::getLocalForce(void) const
  {
    Vector retval(3);
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented." << std::endl;
    return retval;
  }

//! @brief Returns moment expressed in local coordinates.
XC::Vector XC::ShellMecLoad::getLocalMoment(void) const
  {
    Vector retval(3);
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented." << std::endl;
    return retval;
  }

//! @brief Return the local components of the
//! force in a Vector3d. 
Vector3d XC::ShellMecLoad::getVector3dLocalForce(void) const
  {
    Vector f= getLocalForce();
    return Vector3d(f[0],f[1],f[2]);
  }

//! @brief Return the local components of the
//! moment in a Vector3d. 
Vector3d XC::ShellMecLoad::getVector3dLocalMoment(void) const
  {
    Vector m= getLocalMoment();
    return Vector3d(m[0],m[1],m[2]);
  }

//! @brief Returns the components of the force vectors.
const XC::Matrix &XC::ShellMecLoad::getLocalForces(void) const
  {
    static Matrix retval;
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented." << std::endl;
    return retval;
  }

//! @brief Returns the components of the vector moments.
const XC::Matrix &XC::ShellMecLoad::getLocalMoments(void) const
  {
    static Matrix retval;
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented." << std::endl;
    return retval;
  }

//! @brief Returns the vectors expressed in global coordinates.
const XC::Matrix &XC::ShellMecLoad::getGlobalVectors(const Matrix &localVectors) const
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
            if(const ShellMITC4Base *ptrShell= dynamic_cast<const ShellMITC4Base *>(ptrElem))
              {
                const ShellCrdTransf3dBase *ptrTransf= ptrShell->getCoordTransf();
                if(ptrTransf)
                  retval= ptrTransf->getVectorGlobalCoordFromLocal(localVectors);
                else
		  std::cerr << getClassName() << "::" << __FUNCTION__
			    << ": the element: " << elemTag
			    << " has not coordinate transformation." << std::endl;
              }
            else if(const ShellMITC9 *ptrShell= dynamic_cast<const ShellMITC9 *>(ptrElem))
              {
                const ShellCrdTransf3dBase *ptrTransf= ptrShell->getCoordTransf();
                if(ptrTransf)
                  retval= ptrTransf->getVectorGlobalCoordFromLocal(localVectors);
                else
		  std::cerr << getClassName() << "::" << __FUNCTION__
			    << ": the element: " << elemTag
			    << " has not coordinate transformation." << std::endl;
              }
            else if(const ShellNLDKGQ *ptrShell= dynamic_cast<const ShellNLDKGQ *>(ptrElem))
              {
                const ShellCrdTransf3dBase *ptrTransf= ptrShell->getCoordTransf();
                if(ptrTransf)
                  retval= ptrTransf->getVectorGlobalCoordFromLocal(localVectors);
                else
		  std::cerr << getClassName() << "::" << __FUNCTION__
			    << ": the element: " << elemTag
			    << " has not coordinate transformation." << std::endl;
              }
            else
	      std::cerr << getClassName() << "::" << __FUNCTION__ << ": the element: "
                        << elemTag << " is not a shell element." << std::endl;
          }
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< ": pointer to domain is NULL." << std::endl;
    return retval;
  }

//! @brief Return the force expressed in global coordinates.
const XC::Matrix &XC::ShellMecLoad::getGlobalForces(void) const
  { return getGlobalVectors(getLocalForces()); }

//! @brief Returns the bending moment expressed in global coordinates.
const XC::Matrix &XC::ShellMecLoad::getGlobalMoments(void) const
  { return getGlobalVectors(getLocalMoments()); }

//! brief Returns load resultant (force and moment integration over the elements).
SlidingVectorsSystem3d XC::ShellMecLoad::getResultant(const Pos3d &centro, bool initialGeometry) const
  {
    SlidingVectorsSystem3d retval(centro);
    Matrix forces= getGlobalForces();
    const Domain *ptrDom= getDomain();
    if(ptrDom)
      {
        const size_t sz= forces.noRows();
        for(size_t i=0; i<sz; i++)
          {
            const size_t elemTag= getElementTags()(i);
            const Element *ptrElem= ptrDom->getElement(elemTag);
            if(const Shell4NBase *ptrShell= dynamic_cast<const Shell4NBase *>(ptrElem))
	      {
  	        const double area= ptrShell->getArea();
		const Vector3d force(area*forces(i,0),area*forces(i,1),area*forces(i,2));
		retval+= SlidingVector3d(ptrShell->getCenterOfMassPosition(),force);
              }
            else
	      std::cerr << getClassName() << "::" << __FUNCTION__
			<< "; the element: "
                        << elemTag << " is not a shell element." << std::endl;
          }
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; pointer to domain is NULL." << std::endl;
    return retval;
  }
