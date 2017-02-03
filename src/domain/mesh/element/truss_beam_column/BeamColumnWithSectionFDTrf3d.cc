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
//BeamColumnWithSectionFDTrf3d.cpp

#include <domain/mesh/element/truss_beam_column/BeamColumnWithSectionFDTrf3d.h>
#include <domain/mesh/element/coordTransformation/CrdTransf3d.h>
#include "material/section/SeccionBarraPrismatica.h"


#include "xc_utils/src/geom/pos_vec/Vector2d.h"


void XC::BeamColumnWithSectionFDTrf3d::set_transf(const CrdTransf *trf)
  {
    if(theCoordTransf)
      {
        delete theCoordTransf;
        theCoordTransf= nullptr;
      }
    if(trf)
      {
        const CrdTransf3d *tmp= dynamic_cast<const CrdTransf3d *>(trf);
        if(tmp)
          theCoordTransf = tmp->getCopy();
        else
          {
            std::cerr << "BeamColumnWithSectionFDTrf3d::set_transf -- failed to get copy of coordinate transformation\n";
            exit(1);
          }
      }
    else
      std::cerr << "BeamColumnWithSectionFDTrf3d::set_transf; se pasó un puntero nulo." << std::endl;
  }

XC::BeamColumnWithSectionFDTrf3d::BeamColumnWithSectionFDTrf3d(int tag, int classTag,int numSec)
  :BeamColumnWithSectionFD(tag, classTag,numSec), theCoordTransf(nullptr)
  {}

XC::BeamColumnWithSectionFDTrf3d::BeamColumnWithSectionFDTrf3d(int tag, int classTag,int numSec,const Material *mat,const CrdTransf *coordTransf)
  :BeamColumnWithSectionFD(tag, classTag,numSec,mat), theCoordTransf(nullptr)
  { set_transf(coordTransf); }

XC::BeamColumnWithSectionFDTrf3d::BeamColumnWithSectionFDTrf3d(int tag, int classTag,int numSec, int nodeI, int nodeJ,CrdTransf3d &coordTransf)
  :BeamColumnWithSectionFD(tag, classTag,numSec,nullptr,nodeI,nodeJ), theCoordTransf(nullptr)
  { set_transf(&coordTransf); }

//! @brief Constructor de copia.
XC::BeamColumnWithSectionFDTrf3d::BeamColumnWithSectionFDTrf3d(const XC::BeamColumnWithSectionFDTrf3d &otro)
  :BeamColumnWithSectionFD(otro), theCoordTransf(nullptr)
  { set_transf(otro.theCoordTransf); }

//! @brief Operador asignación.
XC::BeamColumnWithSectionFDTrf3d &XC::BeamColumnWithSectionFDTrf3d::operator=(const XC::BeamColumnWithSectionFDTrf3d &otro)
  {
    //BeamColumnWithSectionFD::operator=(otro);
    //set_transf(otro.theCoordTransf);
    std::cerr << "BeamColumnWithSectionFDTrf3d; operador asignación no implementado." << std::endl;
    return *this;
  }

//! @brief Destructor.
XC::BeamColumnWithSectionFDTrf3d::~BeamColumnWithSectionFDTrf3d(void)
  {
    if(theCoordTransf) delete theCoordTransf;
    theCoordTransf= nullptr;
  }

void XC::BeamColumnWithSectionFDTrf3d::initialize_trf(void)
  {
    if(theCoordTransf->initialize(theNodes[0], theNodes[1]))
      {
        std::cerr << "XC::BeamColumnWithSectionFDTrf3d::initialize_trf() -- failed to initialize coordinate transformation\n";
        exit(-1);
      }
  }

//! @brief Devuelve (si puede) un apuntador a la coordinate transformation.
XC::CrdTransf *XC::BeamColumnWithSectionFDTrf3d::getCoordTransf(void)
  { return theCoordTransf; }

//! @brief Devuelve (si puede) un apuntador a la coordinate transformation.
const XC::CrdTransf *XC::BeamColumnWithSectionFDTrf3d::getCoordTransf(void) const
  { return theCoordTransf; }

//! @brief Devuelve el vector dirección del eje fuerte (mayor inercia) de la sección 
//! del elemento cuyo índice se pasa como parámetro expresado en coordenadas locales.
XC::Vector XC::BeamColumnWithSectionFDTrf3d::getVDirEjeFuerteLocales(const size_t &i) const
  {
    Vector retval(3);
    retval(2)= 1;
    const SeccionBarraPrismatica *tmp= dynamic_cast<const SeccionBarraPrismatica *>(theSections[i]);
    if(tmp)
      {
        const Vector2d ejeFuerteSeccion= tmp->getVDirEjeFuerte();
        retval(0)= 0.0; retval(1)= ejeFuerteSeccion.x(); retval(2)= ejeFuerteSeccion.y();
      }
    else
      {
        std::cerr << "BeamColumnWithSectionFDTrf3d::getVDirEjeFuerte; la sección: "
                  << i << " no devuelve ejes principales." << std::endl;
      }
    return retval;
  }

//! @brief Devuelve el vector dirección del eje débil (menor inercia) de la sección 
//! del elemento cuyo índice se pasa como parámetro expresado en coordenadas locales.
XC::Vector XC::BeamColumnWithSectionFDTrf3d::getVDirEjeDebilLocales(const size_t &i) const
  {
    Vector retval(3);
    retval(2)= 1;
    const SeccionBarraPrismatica *tmp= dynamic_cast<const SeccionBarraPrismatica *>(theSections[i]);
    if(tmp)
      {
        const Vector2d ejeDebilSeccion= tmp->getVDirEjeDebil();
        retval(0)= 0.0; retval(1)= ejeDebilSeccion.x(); retval(2)= ejeDebilSeccion.y();
      }
    else
      {
        std::cerr << "BeamColumnWithSectionFDTrf3d::getVDirEjeDebil; la sección: "
                  << i << " no devuelve ejes principales." << std::endl;
        return retval;
      }
    return retval;
  }

//! @brief Devuelve el ángulo del eje fuerte (mayor inercia) del elemento
//! con su plano XZ.
double XC::BeamColumnWithSectionFDTrf3d::getAnguloEjeFuerte(const size_t &i) const
  {
    Vector eF= getVDirEjeFuerteLocales(i);
    return atan2(eF(2),eF(1));
  }

//! @brief Devuelve el ángulo del eje débil (menor inercia) del elemento
//! con su plano XZ.
double XC::BeamColumnWithSectionFDTrf3d::getAnguloEjeDebil(const size_t &i) const
  {
    Vector eD= getVDirEjeDebilLocales(i);
    return atan2(eD(2),eD(1));
  }

//! @brief Devuelve el vector dirección del eje fuerte (mayor inercia) de la sección 
//! del elemento cuyo índice se pasa como parámetro.
const XC::Vector &XC::BeamColumnWithSectionFDTrf3d::getVDirEjeFuerteGlobales(const size_t &i) const
  {
    if(theCoordTransf)
      {
        const Vector eF= getVDirEjeFuerteLocales(i);
        return theCoordTransf->getVectorGlobalCoordFromLocal(eF);
      }
    else
      {
        std::cerr << "BeamColumnWithSectionFDTrf3d::getVDirEjeFuerte; no se ha asignado una coordinate transformation."
                  << std::endl;
        static Vector tmp(3);
        return tmp;
      }
  }

//! @brief Devuelve el vector dirección del eje débil (menor inercia) de la sección 
//! del elemento cuyo índice se pasa como parámetro.
const XC::Vector &XC::BeamColumnWithSectionFDTrf3d::getVDirEjeDebilGlobales(const size_t &i) const
  {
    if(theCoordTransf)
      {
        const Vector eD= getVDirEjeDebilLocales(i);
        return theCoordTransf->getVectorGlobalCoordFromLocal(eD);
      }
    else
      {
        std::cerr << "XC::BeamColumnWithSectionFDTrf3d::getVDirEjeDebil; no se ha asignado una coordinate transformation."
                  << std::endl;
        static Vector tmp(3);
        return tmp;
      }
  }

//! @brief Send members through the channel being passed as parameter.
int XC::BeamColumnWithSectionFDTrf3d::sendData(CommParameters &cp)
  {
    int res= BeamColumnWithSectionFD::sendData(cp);
    res+= sendCoordTransf(9,10,11,cp);
    return res;
  }

//! @brief Receives members through the channel being passed as parameter.
int XC::BeamColumnWithSectionFDTrf3d::recvData(const CommParameters &cp)
  {
    int res= BeamColumnWithSectionFD::recvData(cp);
    theCoordTransf= recvCoordTransf3d(9,10,11,cp);
    return res;
  }

