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
/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 1999, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.7 $
// $Date: 2003/12/12 18:41:09 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/fiber_section/ElasticSection3d.cpp,v $
                                                                        
                                                                        
///////////////////////////////////////////////////////
// File:  ~/Src/element/hinge/fiber_section/ElasticSection3d.cpp
//
// Written: MHS
// Date: May 2000
//
//
// Purpose:  This file contains the function definitions
// for the XC::ElasticSection3d class.

#include "material/section/elastic_section/ElasticSection3d.h"
#include "preprocessor/loaders/MaterialLoader.h"
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>


#include "material/section/ResponseId.h"

XC::Vector XC::ElasticSection3d::s(4);

//! @brief Constructor.
XC::ElasticSection3d::ElasticSection3d(int tag, MaterialLoader *mat_ldr,const CrossSectionProperties3d &ctes)
  :BaseElasticSection3d(tag, SEC_TAG_Elastic3d,4,ctes,mat_ldr) {}

//! @brief Constructor.
XC::ElasticSection3d::ElasticSection3d(void)
  : BaseElasticSection3d(0, SEC_TAG_Elastic3d,4) {}

//! @brief Constructor.
XC::ElasticSection3d::ElasticSection3d(int tag, double E_in, double A_in, double Iz_in, double Iy_in, double G_in, double J_in)
  :BaseElasticSection3d(tag, SEC_TAG_Elastic3d,4,CrossSectionProperties3d(E_in,A_in,Iz_in,Iy_in,G_in,J_in)) {}

//! @brief Constructor.
XC::ElasticSection3d::ElasticSection3d(int tag, double EA_in, double EIz_in, double EIy_in, double GJ_in)
  :BaseElasticSection3d(tag, SEC_TAG_Elastic3d,4,CrossSectionProperties3d(1,EA_in,EIz_in,EIy_in,1,GJ_in)) {}


//! @brief Devuelve el valor de la resultante de tensiones.
const XC::Vector &XC::ElasticSection3d::getStressResultant(void) const
  {
    const Vector &e= getSectionDeformation();
    s(0) = ctes_scc.EA()*e(0); //Esfuerzo axil.
    s(1) = ctes_scc.EIz()*e(1); //Momento flector según z.
    s(2) = ctes_scc.EIy()*e(2); //Momento flector según y.
    s(3) = ctes_scc.GJ()*e(3); //Momento torsor.
    return s;
  }

//! @brief Devuelve la matriz de rigidez tangente.
const XC::Matrix &XC::ElasticSection3d::getSectionTangent(void) const
  { return ctes_scc.getSectionTangent4x4(); }

//! @brief Devuelve la matriz de rigidez tangente en el estado inicial.
const XC::Matrix &XC::ElasticSection3d::getInitialTangent(void) const
  { return ctes_scc.getInitialTangent4x4(); }

//! @brief Devuelve la matriz de flexibilidad.
const XC::Matrix &XC::ElasticSection3d::getSectionFlexibility(void) const
  { return ctes_scc.getSectionFlexibility4x4(); }

//! @brief Devuelve la matriz de flexibilidad en el estado inicial.
const XC::Matrix &XC::ElasticSection3d::getInitialFlexibility(void) const
  { return ctes_scc.getInitialFlexibility4x4(); }

//! @brief Constructor virtual.
XC::SectionForceDeformation *XC::ElasticSection3d::getCopy(void) const
  { return new ElasticSection3d(*this); }

//! @brief Devuelve los GDL a los que la sección aporta rigidez.
const XC::ResponseId &XC::ElasticSection3d::getType(void) const
  { return RespElasticSection3d; }

int XC::ElasticSection3d::getOrder(void) const
  { return 4; }

//! @brief Sends object through the channel being passed as parameter.
int XC::ElasticSection3d::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(3);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << nombre_clase() << "sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::ElasticSection3d::recvSelf(const CommParameters &cp)
  {
    inicComm(3);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << nombre_clase() << "::recvSelf - failed to receive ids.\n";
    else
      {
        setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
          std::cerr << nombre_clase() << "::recvSelf - failed to receive data.\n";
      }
    return res;
  }

//! @brief Imprime.
void XC::ElasticSection3d::Print(std::ostream &s, int flag) const
  {
    if(flag == 2)
      {}
    else
      {
        s << "ElasticSection3d, tag: " << this->getTag() << std::endl;
        ctes_scc.Print(s);
      }
  }
