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
//BeamMecLoad.cpp

#include <domain/load/beam_loads/BeamMecLoad.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include "utility/matrix/ID.h"

#include "domain/mesh/element/Element1D.h"
#include "domain/mesh/element/utils/coordTransformation/CrdTransf.h"
#include "utility/actor/actor/MovableID.h"
#include "utility/actor/actor/MovableVector.h"
#include "xc_utils/src/geom/pos_vec/SlidingVectorsSystem3d.h"

XC::BeamMecLoad::BeamMecLoad(int tag,int classTag,const double &wt,const double &wa,const ID &theElementTags)
  :BeamLoad(tag, classTag, theElementTags), Trans(wt), Axial(wa) {}

XC::BeamMecLoad::BeamMecLoad(int tag,int classTag)
  :BeamLoad(tag, classTag), Trans(0.0), Axial(0.0) {}

//! @brief Returns applied section forces due to this load (called in element's addLoad method). 
const XC::Matrix &XC::BeamMecLoad::getAppliedSectionForces(const double &L,const Matrix &xi,const double &loadFactor) const
  {
    static Matrix retval;
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; not implemented yet." << std::endl;
    return retval;
  }

//! @brief Adds the load al consistent load vector (see page 108 libro Eugenio Oñate).
//! @param L Length of the element.
//! @param loadFactor Load factor.
//! @param p0 element load vector.
void XC::BeamMecLoad::addReactionsInBasicSystem(const double &,const double &,FVector &) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; not implemented yet." << std::endl;
  }

//! @brief Returns the consistent load vector (see page 108 Eugenio Oñate book).
//! @param L Length of the element.
//! @param loadFactor Load factor.
//! @param q0 consistent load vector.
//! @param release Moment release: 0=none, 1=I, 2=J, 3=I,J
void XC::BeamMecLoad::addFixedEndForcesInBasicSystem(const double &L,const double &loadFactor,FVector &, int release) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; not implemented yet." << std::endl;
  }

void XC::BeamMecLoad::addElasticDeformations(const double &L,const CrossSectionProperties2d &ctes_scc,const double &lpI,const double &lpJ,const double &loadFactor,FVector &v0)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << " not defined for 2D sections." << std::endl;
  }

void XC::BeamMecLoad::addElasticDeformations(const double &L,const CrossSectionProperties3d &ctes_scc,const double &lpI,const double &lpJ,const double &loadFactor,FVector &v0)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << " not defined for 3D sections." << std::endl;
  }

//! @brief Return the dimension of the force vector.
size_t XC::BeamMecLoad::getForceVectorDimension(void) const
  { return 2; }

//! @brief Return the dimension of the moment vector.
size_t XC::BeamMecLoad::getMomentVectorDimension(void) const
  { return 1; }

//! @brief Returns force expressed in local coordinates.
XC::Vector XC::BeamMecLoad::getLocalForce(void) const
  {
    Vector retval(2);
    retval(0)= Axial;
    retval(1)= Trans;
    return retval;
  }

//! @brief Returns moment expressed in local coordinates.
XC::Vector XC::BeamMecLoad::getLocalMoment(void) const
  {
    Vector retval(1);
    retval(0)= 0.0;
    return retval;
  }

//! @brief Returns punctual/distributed force vectors (one for each element) expressed in local coordinates.
const XC::Matrix &XC::BeamMecLoad::getLocalForces(void) const
  {
    static Matrix retval;
    const size_t sz= numElements();
    retval= Matrix(sz,2);
    for(size_t i=0; i<sz; i++)
      {
        retval(i,0)= Trans;
        retval(i,1)= Axial;
      }
    return retval;
  }

//! @brief Returns punctual/distributed force moments (one for each element) expressed in local coordinates.
const XC::Matrix &XC::BeamMecLoad::getLocalMoments(void) const
  {
    static Matrix retval;
    const size_t sz= numElements();
    retval= Matrix(sz,1);
    for(size_t i=0; i<sz; i++)
      retval(i,0)= 0.0;
    return retval;
  }

const XC::Matrix &XC::BeamMecLoad::getGlobalVectors(const Matrix &localVectors) const
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
            const Element1D *ptrElem= dynamic_cast<const Element1D *>(ptrDom->getElement(elemTag));
            if(ptrElem)
              {
                const CrdTransf *ptrTransf= ptrElem->getCoordTransf();
                if(ptrTransf)
                  retval= ptrTransf->getVectorGlobalCoordFromLocal(localVectors);
                else
		  std::cerr << "BeamMecLoad::getGlobalVectors; the element: "
                        << elemTag << " no tiene coordinate transformation." << std::endl;
              }
            else
	      std::cerr << "BeamMecLoad::getGlobalVectors; the element: "
                        << elemTag << " is not unidimensional." << std::endl;
          }
      }
    else
      std::cerr << "BeamMecLoad::getGlobalVectors; pointer to domain is NULL." << std::endl;
    return retval;
  }

//! @brief Returns punctual/distributed force vectors (one for each element) expressed in global coordinates.
const XC::Matrix &XC::BeamMecLoad::getGlobalForces(void) const
  { return getGlobalVectors(getLocalForces()); }

//! @brief Returns punctual/distributed moment vectors (one for each element) expressed in global coordinates.
const XC::Matrix &XC::BeamMecLoad::getGlobalMoments(void) const
  { return getGlobalVectors(getLocalMoments()); }

//! brief Returns load resultant (force and moment integration over the elements).
SlidingVectorsSystem3d XC::BeamMecLoad::getResultant(const Pos3d &centro, bool initialGeometry) const
  {
    std::cerr << getClassName()
              << "::getResultant not yet implemented." << std::endl;
    return SlidingVectorsSystem3d(centro);
  }

void XC::BeamMecLoad::Print(std::ostream &s, int flag) const
  {
    s << "BeamMecLoad - Reference load" << std::endl;
    s << "  Transverse: " << Trans << std::endl;
    s << "  Axial:      " << Axial << std::endl;
    BeamLoad::Print(s,flag);
  }

//! @brief Send data through the channel being passed as parameter.
int XC::BeamMecLoad::sendData(Communicator &comm)
  {
    int res= BeamLoad::sendData(comm);
    res+= comm.sendDoubles(Trans,Axial,getDbTagData(),CommMetaData(5));
    return res;
  }

//! @brief Receive data through the channel being passed as parameter.
int XC::BeamMecLoad::recvData(const Communicator &comm)
  {
    int res= BeamLoad::recvData(comm);
    res+= comm.receiveDoubles(Trans,Axial,getDbTagData(),CommMetaData(5));
    return res;
  }
