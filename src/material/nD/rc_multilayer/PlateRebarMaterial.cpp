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
                                                                        
// $Revision: 1.0 $
// $Date: 2012-05-26 22:03:16 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/PlateRebarMaterial.cpp,v $

//
// Yuli Huang (yulihuang@gmail.com) & Xinzheng Lu (luxz@tsinghua.edu.cn)
//
// Generic Plate Rebar Material
//
/* Ref: Lu X, Lu XZ, Guan H, Ye LP, Collapse simulation of reinforced 
concrete high-rise building induced by extreme earthquakes, 
Earthquake Engineering & Structural Dynamics, 2013, 42(5): 705-723*/

#include "PlateRebarMaterial.h"
#include "material/uniaxial/UniaxialMaterial.h"
#include <cmath>

//static vector and matrices
XC::Vector XC::PlateRebarMaterial::stress(5);
XC::Matrix XC::PlateRebarMaterial::tangent(5,5);


void XC::PlateRebarMaterial::setAngle(const double &angle)
  {
    this->angle= angle;
    c= cos(angle);
    s= sin(angle);
  }

//null constructor
XC::PlateRebarMaterial::PlateRebarMaterial(int tag)
  : PlateAdaptorMaterial<UniaxialMaterialWrapper>(tag, ND_TAG_PlateRebarMaterial ), 
   angle(0.0), c(0.0), s(0.0), strain(5) 
  { }


//full constructor
XC::PlateRebarMaterial::PlateRebarMaterial(int tag, const UniaxialMaterial &uniMat, const double &ang):
  PlateAdaptorMaterial<UniaxialMaterialWrapper>(tag, ND_TAG_PlateRebarMaterial, uniMat),
  angle(ang), c(0.0), s(0.0), strain(5)
  {
    this->setAngle(angle);
  }

//! @brief Virtual constructor.
XC::NDMaterial *XC::PlateRebarMaterial::getCopy(void) const 
  { return new PlateRebarMaterial(*this); }


//! @brief make a clone of this material
XC::NDMaterial *XC::PlateRebarMaterial::getCopy(const std::string &type) const 
  {
    if(type==this->getType())
      return this->getCopy( );
    else
      return nullptr;
  }


//swap history variables
int XC::PlateRebarMaterial::commitState(void) 
  { return this->getMaterial()->commitState( ); }



//revert to last saved state
int XC::PlateRebarMaterial::revertToLastCommit(void)
  { return this->getMaterial()->revertToLastCommit( ); }


//revert to start
int XC::PlateRebarMaterial::revertToStart(void)
  {
    strain.Zero();
    return this->getMaterial()->revertToStart( );
  }

//receive the strain
int XC::PlateRebarMaterial::setTrialStrain(const Vector &strainFromElement)
  {
    strain(0) = strainFromElement(0);
    strain(1) = strainFromElement(1);
    strain(2) = strainFromElement(2);
    strain(3) = strainFromElement(3);
    strain(4) = strainFromElement(4);

    UniaxialMaterial *tmp= this->getMaterial();
    if(angle == 0.0)
      return tmp->setTrialStrain(strain(0));
    else if (angle == M_PI/2.0)
      return tmp->setTrialStrain(strain(1));

    return tmp->setTrialStrain(   strain(0) * c * c
				     + strain(1) * s * s
				     + strain(2) * c * s,
				     0);
  }


//send back the strain
const XC::Vector &XC::PlateRebarMaterial::getStrain(void) const
  { return strain; }


//send back the stress 
const XC::Vector &XC::PlateRebarMaterial::getStress(void) const
  {
    double sig = this->getMaterial()->getStress();

    stress.Zero();
    if (angle == 0) 
      stress(0) = sig; 
    else if (angle == M_PI/2.0)
      stress(1)= sig;
    else {
      stress(0) = sig * c * c;
      stress(1) = sig * s * s;
      stress(2) = sig * c * s;
    }

    return stress;
  }


//send back the tangent 
const XC::Matrix &XC::PlateRebarMaterial::getTangent(void) const
  {
    const double tan = this->getMaterial()->getTangent( );

    tangent.Zero();
    if(angle == 0.0) 
      tangent(0,0)= tan;
    else if (angle == M_PI/2.0)
      tangent(1,1) = tan;
    else
      {
        tangent(0,0)= tan * c * c * c * c;
        tangent(0,1)= tan * c * c * s * s;
        tangent(0,2)= tan * c * c * c * s;
        tangent(1,0)= tangent(0,1);
        tangent(1,1)= tan * s * s * s * s;
        tangent(1,2)= tan * c * s * s * s;
        tangent(2,0)= tangent(0,2);
        tangent(2,1)= tangent(1,2);
        tangent(2,2)= tangent(0,1);
      }
    return tangent;
  }

const XC::Matrix &XC::PlateRebarMaterial::getInitialTangent(void) const
  {
    const double tan = this->getMaterial()->getInitialTangent( );
    tangent.Zero();
    if (angle == 0) 
      tangent(0,0) = tan;
    else if (angle == M_PI/2.0)
      tangent(1,1) = tan;
    else
      {
        tangent(0,0) = tan * c * c * c * c;
        tangent(0,1) = tan * c * c * s * s;
        tangent(0,2) = tan * c * c * c * s;
        tangent(1,0) = tangent(0,1);
        tangent(1,1) = tan * s * s * s * s;
        tangent(1,2) = tan * c * s * s * s;
        tangent(2,0) = tangent(0,2);
        tangent(2,1) = tangent(1,2);
        tangent(2,2) = tangent(0,1);
      }
    return tangent;
  }


//! @brief print out data
void XC::PlateRebarMaterial::Print(std::ostream &s, int flag) const
  {
    PlateAdaptorMaterial<UniaxialMaterialWrapper>::Print(s, flag);
    s << "angle: " << angle << std::endl;
  }

//! @brief Send material data.
int XC::PlateRebarMaterial::sendData(Communicator &comm)
  {
    int res= PlateAdaptorMaterial<UniaxialMaterialWrapper>::sendData(comm);
    res+= comm.sendDoubles(angle, c, s,getDbTagData(),CommMetaData(2));
    res+= comm.sendVector(strain, getDbTagData(),CommMetaData(3));
    return res;
  }

//! @brief Receive material data.
int XC::PlateRebarMaterial::recvData(const Communicator &comm)
  {
    int res= PlateAdaptorMaterial<UniaxialMaterialWrapper>::recvData(comm);
    res+= comm.receiveDoubles(angle, c, s,getDbTagData(),CommMetaData(2));
    res+= comm.receiveVector(strain, getDbTagData(),CommMetaData(3));
    return res;
  }

int XC::PlateRebarMaterial::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(4);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to send data\n";
    return res;
  }

int XC::PlateRebarMaterial::recvSelf(const Communicator &comm)
  {
    inicComm(4);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; failed to receive ids.\n";
    else
      {
        setTag(getDbTagDataPos(0));
        res+= recvData(comm);
        if(res<0)
          std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; failed to receive data.\n";
      }
    return res;
  }
 
