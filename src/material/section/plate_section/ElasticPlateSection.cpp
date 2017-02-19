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
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/ElasticPlateSection.cpp,v $

// Ed "C++" Love
//
//  Elastic Plate Section
//


#include <material/section/plate_section/ElasticPlateSection.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include <cstdio> 
#include <cstdlib> 
#include <cmath> 
#include "material/section/ResponseId.h"

//! @brief Constructor
XC::ElasticPlateSection::ElasticPlateSection(int tag)
  : XC::ElasticPlateProto<5>(tag, SEC_TAG_ElasticPlateSection) {}

//! @brief Default constructor
XC::ElasticPlateSection::ElasticPlateSection(void)
  : XC::ElasticPlateProto<5>(0,SEC_TAG_ElasticPlateSection) {}



//! @brief Constructor
XC::ElasticPlateSection::ElasticPlateSection(  int    tag, double young,double poisson, double thickness)
  : XC::ElasticPlateProto<5>(tag, SEC_TAG_ElasticPlateSection,young,poisson,thickness) {}

//! @brief Virtual constructor
XC::SectionForceDeformation *XC::ElasticPlateSection::getCopy(void) const
  { return new ElasticPlateSection(*this); }



//! @brief Return the resultante de las tensiones en la sección.
const XC::Vector& XC::ElasticPlateSection::getStressResultant(void) const
  {
    const Vector &strain= getSectionDeformation();
    double D=  E * (h*h*h) / 12.0 / ( 1.0 - nu*nu ) ; //bending modulus
    double G=  0.5 * E / ( 1.0 + nu ) ; //shear modulus
    G *= five6 ;
    G *= h ;
    stress(0) = -( D*strain(0) + nu*D*strain(1) ) ;
    stress(1) = -( nu*D*strain(0) + D*strain(1) ) ;
    stress(2) = -0.5*D*( 1.0 - nu )*strain(2) ;
    stress(3) = G*strain(3) ;
    stress(4) = G*strain(4) ;
    return this->stress ;
  }


//! @brief Return the tangent stiffness matrix.
const XC::Matrix& XC::ElasticPlateSection::getSectionTangent(void) const
  {
    const double D= E * (h*h*h) / 12.0 / ( 1.0 - nu*nu ) ;
    const double G= 0.5 * E / ( 1.0 + nu ) ;
    tangent.Zero() ;
    tangent(0,0) = -D ;
    tangent(1,1) = -D ;
    tangent(0,1) = -nu*D ;
    tangent(1,0) = tangent(0,1) ;
    tangent(2,2) = -0.5 * D * ( 1.0 - nu ) ;
    tangent(3,3) = five6*G*h ;
    tangent(4,4) = tangent(3,3) ;
    return tangent;
  }

//! @brief Return the initial tangent stiffness matrix.
const XC::Matrix& XC::ElasticPlateSection::getInitialTangent(void) const
  { return getSectionTangent(); }

//! @brief Returns the labels of the DOFs for wich the element
//! adds (assembles) stiffness.
const XC::ResponseId &XC::ElasticPlateSection::getType(void) const 
  { return RespPlateMat; }

//! @brief print out data
void  XC::ElasticPlateSection::Print( std::ostream &s, int flag )
  {
    s << "ElasticPlateSection: \n " ;
    s <<  "  Young's Modulus E  = "  <<  E  <<  std::endl; ;
    s <<  "  Poisson's Ratio nu = " <<  nu <<  std::endl; ;
    s <<  "  Thickness h = "        <<  h  <<  std::endl; ;
    return ;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::ElasticPlateSection::sendSelf(CommParameters &cp) 
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(8);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << nombre_clase() << "sendSelf() - failed to send data\n";
    return res;
  }


//! @brief Receives object through the channel being passed as parameter.
int XC::ElasticPlateSection::recvSelf(const CommParameters &cp)
  {
    inicComm(8);
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
