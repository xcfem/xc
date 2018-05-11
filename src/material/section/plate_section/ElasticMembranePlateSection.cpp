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
                                                                        
// $Revision: 1.9 $
// $Date: 2003/02/14 23:01:33 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/ElasticMembranePlateSection.cpp,v $

// Ed "C++" Love
//
//  Elastic Plate Section with membrane
//


#include <material/section/plate_section/ElasticMembranePlateSection.h>

#include <cstdio> 
#include <cstdlib>
#include <cmath> 
#include "material/section/ResponseId.h"

//Strains.

// Membrane.
// strain(0): epsilon_1 (Longitudinal axis 1)
// strain(1): epsilon_2 (Longitudinal axis 2)
// strain(2): epsilon_12 (in-plane shear on plane 12)

// Plate.
// strain(3): m_xx bending first axis.
// strain(4): m_yy bending second axis.
// strain(4): m_xy
// strain(6): shear first axis.
// strain(7): shear 2nd axis. 


//! @brief Constructor
XC::ElasticMembranePlateSection::ElasticMembranePlateSection(int tag)
  : ElasticPlateProto<8>(tag, SEC_TAG_ElasticMembranePlateSection) {}

//! @brief Constructor.
XC::ElasticMembranePlateSection::ElasticMembranePlateSection(void)
  : ElasticPlateProto<8>(0,SEC_TAG_ElasticMembranePlateSection) {}


//! @brief Constructor
XC::ElasticMembranePlateSection::ElasticMembranePlateSection(int tag,double young,double poisson,double thickness,double r )
  : ElasticPlateProto<8>(tag, SEC_TAG_ElasticMembranePlateSection,young,poisson,thickness), rhoH(r*thickness)
  {}

//! @brief Make a clone of this material
XC::SectionForceDeformation*  XC::ElasticMembranePlateSection::getCopy(void) const
  { return new ElasticMembranePlateSection(*this); }

//! @brief Density per unit area
double XC::ElasticMembranePlateSection::getRho(void) const
  { return rhoH; }

//! @brief Asigns density per unit area
void XC::ElasticMembranePlateSection::setRho(const double &r)
  { rhoH= r; }


//! @brief Returns the abels of the DOFs for which the elements
//! adds stiffness.
const XC::ResponseId &XC::ElasticMembranePlateSection::getType(void) const 
  { return RespShellMat; }

//! @brief Return the cross-section stress resultant.
const XC::Vector &XC::ElasticMembranePlateSection::getStressResultant(void) const
  {
    const double M= membraneModulus(); //membrane modulus
    const Vector &strain= getSectionDeformation();
    double G= shearModulus(); //shear modulus
    //membrane resultants
    stress(0)=  M*strain(0) + (nu*M)*strain(1);
    stress(1)=  (nu*M)*strain(0) +  M*strain(1);
    stress(2)=  G*strain(2);
    G*= five6;  //multiply by shear correction factor
    const double D=  bendingModulus();  //bending modulus

    //bending resultants
    stress(3)= -( D*strain(3) + nu*D*strain(4) );
    stress(4)= -( nu*D*strain(3) + D*strain(4) );
    stress(5)= -0.5*D*( 1.0 - nu )*strain(5);
    stress(6)= G*strain(6);
    stress(7)= G*strain(7);
    return stress;
  }


//! @brief Return the tangent stiffness matrix.
const XC::Matrix &XC::ElasticMembranePlateSection::getSectionTangent(void) const
  {
    const double M= membraneModulus(); //membrane modulus
    double G= shearModulus(); //shear modulus
    tangent.Zero();

    //membrane tangent terms

    tangent(0,0)= M;
    tangent(1,1)= M;

    tangent(0,1)= nu*M;
    tangent(1,0)= tangent(0,1);

    tangent(2,2)= G;

    G*= five6;  //multiply by shear correction factor

    const double D= bendingModulus();  //bending modulus

    //bending tangent terms

    tangent(3,3)= -D;
    tangent(4,4)= -D;

    tangent(3,4)= -nu*D;
    tangent(4,3)= tangent(3,4);

    tangent(5,5)= -0.5 * D * ( 1.0 - nu );
    tangent(6,6)= G;

    tangent(7,7)= G;

    return tangent;
  }


//! @brief Return the initial tangent stiffness matrix.
const XC::Matrix &XC::ElasticMembranePlateSection::getInitialTangent(void) const
  {
    const double M= membraneModulus(); //membrane modulus
    double G= shearModulus(); //shear modulus
    tangent.Zero();

    //membrane tangent terms
    tangent(0,0)= M;
    tangent(1,1)= M;

    tangent(0,1)= nu*M;
    tangent(1,0)= tangent(0,1);

    tangent(2,2)= G;

    G*= five6;  //multiply by shear correction factor

    const double D=bendingModulus(); //bending modulus

    //bending tangent terms
    tangent(3,3)= -D;
    tangent(4,4)= -D;

    tangent(3,4)= -nu*D;
    tangent(4,3)= tangent(3,4);
    tangent(5,5)= -0.5*D*(1.0-nu);
    tangent(6,6)= G;
    tangent(7,7) = G;
    return this->tangent;
  }


//print out data
void  XC::ElasticMembranePlateSection::Print( std::ostream &s, int flag )
  {
    s << "ElasticMembranePlateSection: \n ";
    s <<  "  Young's Modulus E = "  <<  E  <<  std::endl;;
    s <<  "  Poisson's Ratio nu = " <<  nu <<  std::endl;;
    s <<  "  Thickness h = "        <<  h  <<  std::endl;;
    s <<  "  Density rho = "        <<  (rhoH/h)  <<  std::endl;;
    return;
  }

//! @brief Returns a vector to store the dbTags
//! of the class members.
XC::DbTagData &XC::ElasticMembranePlateSection::getDbTagData(void) const
  {
    static DbTagData retval(9);
    return retval;
  }

//! @brief Send data through the channel being passed as parameter.
int XC::ElasticMembranePlateSection::sendData(CommParameters &cp)
  {
    int res= ElasticPlateProto<8>::sendData(cp);
    res+= cp.sendDouble(rhoH,getDbTagData(),CommMetaData(8));
    return res;
  }

//! @brief Receive data through the channel being passed as parameter.
int XC::ElasticMembranePlateSection::recvData(const CommParameters &cp)
  {
    int res= ElasticPlateProto<8>::recvData(cp);
    res+= cp.receiveDouble(rhoH,getDbTagData(),CommMetaData(8));
    return res;
  }

//! @brief Send the object itself through the communicator argument.
int XC::ElasticMembranePlateSection::sendSelf(CommParameters &cp) 
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(9);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; failed to send data.\n";
    return res;
  }

//! @brief Receive the object itself through the communicator argument.
int XC::ElasticMembranePlateSection::recvSelf(const CommParameters &cp)
  {
    inicComm(9);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to receive ids.\n";
    else
      {
        res+= recvData(cp);
        if(res<0)
          std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; failed to receive data.\n";
      }
    return res;
  }
