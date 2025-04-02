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
//QuadMecLoad.cpp

#include "QuadMecLoad.h"
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include "utility/matrix/ID.h"

#include "domain/mesh/element/plane/SolidMech4N.h"
//#include "domain/mesh/element/utils/coordTransformation/CrdTransf.h"
#include "domain/mesh/node/Node.h"
#include "utility/geom/pos_vec/SlidingVectorsSystem3d.h"
#include "utility/geom/pos_vec/SlidingVector3d.h"

XC::QuadMecLoad::QuadMecLoad(int tag,int classTag,const ID &theElementTags)
  : BidimMecLoad(tag, classTag, theElementTags) {}

XC::QuadMecLoad::QuadMecLoad(int tag,int classTag)
  : BidimMecLoad(tag, classTag) {}

//! @brief Adds the load the consistent load vector (see page 108 libro Eugenio Oñate).
//! @param areas tributary areas for each node.
//! @param loadFactor Load factor.
//! @param p0 element load vector.
void XC::QuadMecLoad::addReactionsInBasicSystem(const std::vector<double> &,const double &,FVectorQuad &) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented." << std::endl;
  }

//! @brief ??
//! @param areas tributary areas for each node.
//! @param loadFactor Load factor.
//! @param q0 
void XC::QuadMecLoad::addFixedEndForcesInBasicSystem(const std::vector<double> &,const double &loadFactor,FVectorQuad &) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented." << std::endl;
  }

//! @brief Returns the vectors expressed in global coordinates.
const XC::Matrix &XC::QuadMecLoad::getGlobalVectors(const Matrix &localVectors) const
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
	    const SolidMech4N *ptrQuad= dynamic_cast<const SolidMech4N *>(ptrElem);
            if(ptrQuad)
              {
                retval= localVectors; // No transformation needed.
              }
            else
	      std::cerr << getClassName() << "::" << __FUNCTION__ << ": the element: "
                        << elemTag << " is not a quad element." << std::endl;
          }
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< ": pointer to domain is NULL." << std::endl;
    return retval;
  }

//! brief Returns load resultant (force and moment integration over the elements).
SlidingVectorsSystem3d XC::QuadMecLoad::getResultant(const Pos3d &centro, bool initialGeometry) const
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
            if(const SolidMech4N *ptrQuad= dynamic_cast<const SolidMech4N *>(ptrElem))
	      {
  	        const double area= ptrQuad->getArea();
		const Vector3d force(area*forces(i,0),area*forces(i,1),0.0);
		retval+= SlidingVector3d(ptrQuad->getCenterOfMassPosition(),force);
              }
            else
	      std::cerr << getClassName() << "::" << __FUNCTION__
			<< "; the element: "
                        << elemTag << " is not a quad element." << std::endl;
          }
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; pointer to domain is NULL." << std::endl;
    return retval;
  }
