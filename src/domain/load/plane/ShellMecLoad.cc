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
#include "domain/mesh/element/plane/shell/ShellNL.h"
#include "domain/mesh/element/utils/coordTransformation/CrdTransf.h"
#include "domain/mesh/node/Node.h"

XC::ShellMecLoad::ShellMecLoad(int tag,int classTag,const double &wt,const double &wa1,const double &wa2,const ID &theElementTags)
  :BidimMecLoad(tag, classTag, wt, wa1, wa2, theElementTags) {}

XC::ShellMecLoad::ShellMecLoad(int tag,int classTag)
  :BidimMecLoad(tag, classTag) {}

//! @brief Añade the load al consistent load vector (ver página 108 libro Eugenio Oñate).
//! @param area Area del elemento.
//! @param loadFactor Load factor.
//! @param p0 element load vector.
void XC::ShellMecLoad::addReactionsInBasicSystem(const double &,const double &,FVectorShell &) const
  {
    std::cerr << "addReactionsInBasicSystem no definida." << std::endl;
  }

//! @brief ??
//! @param area Area del elemento.
//! @param loadFactor Load factor.
//! @param q0 
void XC::ShellMecLoad::addFixedEndForcesInBasicSystem(const double &,const double &loadFactor,FVectorShell &) const
  {
    std::cerr << "addFixedEndForcesInBasicSystem no definida." << std::endl;
  }

//! @brief Return the dimension del vector fuerza.
size_t XC::ShellMecLoad::getDimVectorFuerza(void) const
  { return 3; }

//! @brief Return the dimension del vector momento.
size_t XC::ShellMecLoad::getDimVectorMomento(void) const
  { return 3; }

//! @brief Returns force expressed in local coordinates.
XC::Vector XC::ShellMecLoad::getLocalForce(void) const
  {
    Vector retval(3);
    retval(0)= Axial1;
    retval(1)= Axial2;
    retval(2)= Trans;
    return retval;
  }

//! @brief Returns moment expressed in local coordinates.
XC::Vector XC::ShellMecLoad::getLocalMoment(void) const
  {
    Vector retval(3);
    retval(0)= 0.0;
    retval(1)= 0.0;
    retval(2)= 0.0;
    return retval;
  }

//! @brief Returns the componentes de los vectores fuerza.
const XC::Matrix &XC::ShellMecLoad::getLocalForces(void) const
  {
    static Matrix retval;
    const size_t sz= numElements();
    retval= Matrix(sz,3);
    for(size_t i=0; i<sz; i++)
      {
        retval(i,0)= Trans;
        retval(i,1)= Axial1;
        retval(i,2)= Axial2;
      }
    return retval;
  }

//! @brief Returns the componentes de los vectores momento.
const XC::Matrix &XC::ShellMecLoad::getLocalMoments(void) const
  {
    static Matrix retval;
    const size_t sz= numElements();
    retval= Matrix(sz,3);
    for(size_t i=0; i<sz; i++)
      {
        retval(i,0)= 0.0;
        retval(i,1)= 0.0;
        retval(i,2)= 0.0;
      }
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
		  std::cerr << "ShellMecLoad::getGlobalVectors; el elemento: "
                        << elemTag << " no tiene coordinate transformation." << std::endl;
              }
            else if(const ShellNL *ptrShell= dynamic_cast<const ShellNL *>(ptrElem))
              {
                const ShellCrdTransf3dBase *ptrTransf= ptrShell->getCoordTransf();
                if(ptrTransf)
                  retval= ptrTransf->getVectorGlobalCoordFromLocal(localVectors);
                else
		  std::cerr << "ShellMecLoad::getGlobalVectors; el elemento: "
                        << elemTag << " no tiene coordinate transformation." << std::endl;
              }
            else
	      std::cerr << "ShellMecLoad::getGlobalVectors; the element: "
                        << elemTag << " is not a shell element." << std::endl;
          }
      }
    else
      std::cerr << "ShellMecLoad::getGlobalVectors; pointer to domain is NULL." << std::endl;
    return retval;
  }

//! @brief Return the fuerza expresada en coordenadas globales.
const XC::Matrix &XC::ShellMecLoad::getGlobalForces(void) const
  { return getGlobalVectors(getLocalForces()); }

//! @brief Returns the momento expresado en coordenadas globales.
const XC::Matrix &XC::ShellMecLoad::getGlobalMoments(void) const
  { return getGlobalVectors(getLocalMoments()); }
