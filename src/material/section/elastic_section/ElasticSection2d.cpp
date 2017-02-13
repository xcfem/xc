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
                                                                        
// $Revision: 1.8 $
// $Date: 2003/02/14 23:01:33 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/fiber_section/ElasticSection2d.cpp,v $
                                                                        
                                                                        
///////////////////////////////////////////////////////
// File:  ~/Src/element/hinge/fiber_section/ElasticSection2d.cpp
//
// Written by Matthew Peavy
//
// Written:  Feb 13, 2000
// Debugged: Feb 14, 2000
// Revised:  May 2000 -- MHS (to include elastic shear)
//
//
// Purpose:  This file contains the function definitions
// for the XC::ElasticSection2d class.

#include <material/section/elastic_section/ElasticSection2d.h>
#include "material/section/repres/geom_section/GeomSection.h"
#include "preprocessor/loaders/MaterialLoader.h"
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>


#include "material/section/ResponseId.h"

XC::Vector XC::ElasticSection2d::s(2);

XC::ElasticSection2d::ElasticSection2d(int tag, MaterialLoader *mat_ldr)
  : BaseElasticSection2d(tag,SEC_TAG_Elastic2d,2,mat_ldr) {}

XC::ElasticSection2d::ElasticSection2d(void)
  : BaseElasticSection2d(0,SEC_TAG_Elastic2d,2) {}


XC::ElasticSection2d::ElasticSection2d(int tag, double E, double A, double I)
  : BaseElasticSection2d(tag,SEC_TAG_Elastic2d,2,E,A,I,0.0,0.0)
  {}

XC::ElasticSection2d::ElasticSection2d(int tag, double EA, double EI)
  :BaseElasticSection2d(tag, SEC_TAG_Elastic2d,2,1,EA,EI,0.0,0.0)
  {}

//! @brief Return the resultante de las tensiones en la sección.
const XC::Vector &XC::ElasticSection2d::getStressResultant(void) const
  {
    const Vector &e= getSectionDeformation();
    s(0)= ctes_scc.EA()*e(0);
    s(1)= ctes_scc.EI()*e(1);
    return s;
  }

//! @brief Return the tangent stiffness matrix.
const XC::Matrix &XC::ElasticSection2d::getSectionTangent(void) const
  { return ctes_scc.getSectionTangent2x2(); }


//! @brief Return the matriz de rigidez noval.
const XC::Matrix &XC::ElasticSection2d::getInitialTangent(void) const
  { return getSectionTangent(); }

//! @brief Return the matriz de flexibilidad.
const XC::Matrix &XC::ElasticSection2d::getSectionFlexibility(void) const
  { return ctes_scc.getSectionFlexibility2x2(); }

//! @brief Return the matriz de flexibilidad noval.
const XC::Matrix &XC::ElasticSection2d::getInitialFlexibility(void) const
  { return getSectionFlexibility(); }


XC::SectionForceDeformation *XC::ElasticSection2d::getCopy(void) const
  { return new XC::ElasticSection2d(*this); }

//! @brief Returns the códigos de los esfuerzos para los que la
//! sección aporta rigidez.
const XC::ResponseId &XC::ElasticSection2d::getType(void) const
  { return RespElasticSection2d; }

int XC::ElasticSection2d::getOrder(void) const
  { return 2; }

int XC::ElasticSection2d::sendSelf(CommParameters &cp)
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

int XC::ElasticSection2d::recvSelf(const CommParameters &cp)
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

void XC::ElasticSection2d::Print(std::ostream &s, int flag) const
  {
    s << "ElasticSection2d, tag: " << this->getTag() << std::endl;
    ctes_scc.Print(s,flag);
  }

