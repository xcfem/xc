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
#include "domain/mesh/element/coordTransformation/CrdTransf.h"
#include "utility/actor/actor/MovableID.h"
#include "utility/actor/actor/MovableVector.h"
#include "xc_utils/src/geom/pos_vec/SVD3d.h"

XC::BeamMecLoad::BeamMecLoad(int tag,int classTag,const double &wt,const double &wa,const ID &theElementTags)
  :BeamLoad(tag, classTag, theElementTags), Trans(wt), Axial(wa) {}

XC::BeamMecLoad::BeamMecLoad(int tag,int classTag)
  :BeamLoad(tag, classTag), Trans(0.0), Axial(0.0) {}

//! @brief Returns applied section forces due to this load (called in element's addLoad method). 
const XC::Matrix &XC::BeamMecLoad::getAppliedSectionForces(const double &L,const Matrix &xi,const double &loadFactor)
  {
    static Matrix retval;
    std::cerr << "getAppliedSectionForces no definida." << std::endl;
    return retval;
  }

//! @brief Añade la carga al vector de cargas consistentes (ver página 108 libro Eugenio Oñate).
//! @param L Longitud del elemento.
//! @param loadFactor Ponderación de la carga.
//! @param p0 Vector de cargas del elemento.
void XC::BeamMecLoad::addReactionsInBasicSystem(const double &,const double &,FVector &)
  {
    std::cerr << "addReactionsInBasicSystem no definida." << std::endl;
  }

//! @brief ??
//! @param L Longitud del elemento.
//! @param loadFactor Ponderación de la carga.
//! @param q0 ??
void XC::BeamMecLoad::addFixedEndForcesInBasicSystem(const double &L,const double &loadFactor,FVector &)
  {
    std::cerr << "addFixedEndForcesInBasicSystem no definida." << std::endl;
  }

void XC::BeamMecLoad::addElasticDeformations(const double &L,const CrossSectionProperties2d &ctes_scc,const double &lpI,const double &lpJ,const double &loadFactor,FVector &v0)
  {
    std::cerr << "addElasticDeformations no definida para secciones 2d." << std::endl;
  }

void XC::BeamMecLoad::addElasticDeformations(const double &L,const CrossSectionProperties3d &ctes_scc,const double &lpI,const double &lpJ,const double &loadFactor,FVector &v0)
  {
    std::cerr << "addElasticDeformations no definida para secciones 3d." << std::endl;
  }

//! @brief Devuelve la dimension del vector fuerza.
size_t XC::BeamMecLoad::getDimVectorFuerza(void) const
  { return 2; }

//! @brief Devuelve la dimension del vector momento.
size_t XC::BeamMecLoad::getDimVectorMomento(void) const
  { return 1; }

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

//! @brief Returns puntual/distributed force moments (one for each element) expressed in local coordinates.
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
		  std::cerr << "BeamMecLoad::getGlobalVectors; el elemento: "
                        << elemTag << " no tiene coordinate transformation." << std::endl;
              }
            else
	      std::cerr << "BeamMecLoad::getGlobalVectors; el elemento: "
                        << elemTag << " no es unidimensional." << std::endl;
          }
      }
    else
      std::cerr << "BeamMecLoad::getGlobalVectors; no existe apuntador al dominio." << std::endl;
    return retval;
  }

//! @brief Returns punctual/distributed force vectors (one for each element) expressed in global coordinates.
const XC::Matrix &XC::BeamMecLoad::getGlobalForces(void) const
  { return getGlobalVectors(getLocalForces()); }

//! @brief Returns punctual/distributed moment vectors (one for each element) expressed in global coordinates.
const XC::Matrix &XC::BeamMecLoad::getGlobalMoments(void) const
  { return getGlobalVectors(getLocalMoments()); }

//! brief Returns load resultant (force and moment integration over the elements).
SVD3d XC::BeamMecLoad::getResultant(const Pos3d &centro, bool initialGeometry) const
  {
    std::cerr << nombre_clase()
              << "::getResultant not yet implemented." << std::endl;
    return SVD3d(centro);
  }

void XC::BeamMecLoad::Print(std::ostream &s, int flag) const
  {
    s << "BeamMecLoad - Reference load" << std::endl;
    s << "  Transverse: " << Trans << std::endl;
    s << "  Axial:      " << Axial << std::endl;
    BeamLoad::Print(s,flag);
  }

//! @brief Envía los datos through the channel being passed as parameter.
int XC::BeamMecLoad::sendData(CommParameters &cp)
  {
    int res= BeamLoad::sendData(cp);
    res+= cp.sendDoubles(Trans,Axial,getDbTagData(),CommMetaData(5));
    return res;
  }

//! @brief Recibe los datos through the channel being passed as parameter.
int XC::BeamMecLoad::recvData(const CommParameters &cp)
  {
    int res= BeamLoad::recvData(cp);
    res+= cp.receiveDoubles(Trans,Axial,getDbTagData(),CommMetaData(5));
    return res;
  }
