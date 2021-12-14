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

// $Revision: 1.4 $
// $Date: 2003/02/14 23:01:24 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/BeamFiberMaterial.cpp,v $

// Written: MHS
// Created: Aug 2001
//
// Description: This file contains the class definition of BeamFiberMaterial.
// The BeamFiberMaterial class is a wrapper class that performs static
// condensation on a three-dimensional material model to give the 11, 12, and 13
// stress components which can then be integrated over an area to model a
// shear flexible 3D beam.

#include <material/nD/nd_adaptor/BeamFiberMaterial.h>
#include "utility/matrix/ID.h"
#include "material/nD/NDMaterialType.h"


XC::Vector XC::BeamFiberMaterial::stress(3);
XC::Matrix XC::BeamFiberMaterial::tangent(3,3);

XC::BeamFiberMaterial::BeamFiberMaterial(void)
  : XC::NDAdaptorMaterial(ND_TAG_BeamFiberMaterial,3),
Tstrain33(0.0), Tgamma23(0.0),
Cstrain33(0.0), Cgamma23(0.0)
  {}

XC::BeamFiberMaterial::BeamFiberMaterial(int tag, XC::NDMaterial &theMat)
  : XC::NDAdaptorMaterial(ND_TAG_BeamFiberMaterial,tag,theMat,3),
    Tstrain33(0.0), Tgamma23(0.0),
    Cstrain33(0.0), Cgamma23(0.0)
  {}

XC::NDMaterial *XC::BeamFiberMaterial::getCopy(void) const
  { return new BeamFiberMaterial(*this); }

XC::NDMaterial* XC::BeamFiberMaterial::getCopy(const std::string &type) const
  {
    if((type==strTypeBeamFiber))
      return this->getCopy();
    else
      return nullptr;
  }

int XC::BeamFiberMaterial::getOrder(void) const
  { return 3; }

const std::string &XC::BeamFiberMaterial::getType(void) const 
  { return strTypeBeamFiber; }

//! @brief Commit the state of the material.
int XC::BeamFiberMaterial::commitState(void)
  {
    Cstrain33 = Tstrain33;
    Cgamma23 = Tgamma23;
    return NDAdaptorMaterial::commitState();
  }

int XC::BeamFiberMaterial::revertToLastCommit(void)
  {
    Tstrain33 = Cstrain33;
    Tgamma23 = Cgamma23;
    return NDAdaptorMaterial::revertToLastCommit();
  }

int XC::BeamFiberMaterial::revertToStart()
  {
    int retval= NDAdaptorMaterial::revertToStart();
    Tstrain33 = 0.0;
    Tgamma23  = 0.0;
    Cstrain33 = 0.0;
    Cgamma23  = 0.0;
    return retval;
  }

//! @brief Set trial strain
//NDmaterial strain order        = 11, 22, 33, 12, 23, 31  
//BeamFiberMaterial strain order = 11, 12, 31, 22, 33, 23
int XC::BeamFiberMaterial::setTrialStrain(const Vector &strainFromElement)
  {
    static const double tolerance = 1.0e-08;

    this->strain(0) = strainFromElement(0);
    this->strain(1) = strainFromElement(1);
    this->strain(2) = strainFromElement(2);

    //newton loop to solve for out-of-plane strains

    double norm;
    static Vector condensedStress(3);
    static Vector strainIncrement(3);
    static Vector threeDstress(6);
    static Vector threeDstrain(6);
    static Matrix threeDtangent(6,6);
    static Vector threeDstressCopy(6); 
    static Matrix threeDtangentCopy(6,6);
    static Matrix dd22(3,3);

    int i, j;
    int ii, jj;

    do {
      //set three dimensional strain
      threeDstrain(0) = this->strain(0);
      threeDstrain(1) = this->Tstrain22;
      threeDstrain(2) = this->Tstrain33;
      threeDstrain(3) = this->strain(1); 
      threeDstrain(4) = this->Tgamma23;
      threeDstrain(5) = this->strain(2);

      if(theMaterial->setTrialStrain(threeDstrain) < 0) {
	std::cerr << "BeamFiberMaterial::setTrialStrain - setStrain failed in material with strain " << threeDstrain;
	return -1;   
      }

      //three dimensional stress
      threeDstress = theMaterial->getStress();

      //three dimensional tangent 
      threeDtangent = theMaterial->getTangent();

      //NDmaterial strain order        = 11, 22, 33, 12, 23, 31  
      //BeamFiberMaterial strain order = 11, 12, 31, 22, 33, 23

      //swap matrix indices to sort out-of-plane components 
      for(i=0; i<6; i++) {

	ii = this->indexMap(i);

	threeDstressCopy(ii) = threeDstress(i);

	for(j=0; j<6; j++) {

      jj = this->indexMap(j);

      threeDtangentCopy(ii,jj) = threeDtangent(i,j);

	}//end for j

      }//end for i


      //out of plane stress and tangents
      for(i=0; i<3; i++) {

	condensedStress(i) = threeDstressCopy(i+3);

	for(j=0; j<3; j++) 
      dd22(i,j) = threeDtangentCopy(i+3,j+3);

      }//end for i

      //set norm
      norm = condensedStress.Norm();

      //condensation 
      dd22.Solve(condensedStress, strainIncrement);

      //update out of plane strains
      this->Tstrain22 -= strainIncrement(0);
      this->Tstrain33 -= strainIncrement(1);
      this->Tgamma23  -= strainIncrement(2);

    } while(norm > tolerance);

    return 0;
  }

const XC::Vector &XC::BeamFiberMaterial::getStress(void) const
  {
    const Vector &threeDstress= theMaterial->getStress();
    static Vector threeDstressCopy(6);

    int i, ii;
    //swap matrix indices to sort out-of-plane components 
    for(i=0; i<6; i++)
      {
	ii = this->indexMap(i);
	threeDstressCopy(ii) = threeDstress(i);
      }

    for(i=0; i<3; i++) 
      this->stress(i)     = threeDstressCopy(i);

    return this->stress;
  }

//! @brief Return the material tangent stiffness.
const XC::Matrix &XC::BeamFiberMaterial::getTangent(void) const
{
  static Matrix dd11(3,3);
  static Matrix dd12(3,3);
  static Matrix dd21(3,3);
  static Matrix dd22(3,3);
  static Matrix dd22invdd21(3,3);
  static Matrix threeDtangentCopy(6,6);

  const XC::Matrix &threeDtangent = theMaterial->getTangent();

  //swap matrix indices to sort out-of-plane components 
  int i, j , ii, jj;
  for(i=0; i<6; i++) {

    ii = this->indexMap(i);

    for(j=0; j<6; j++) {
      
      jj = this->indexMap(j);
      
      threeDtangentCopy(ii,jj) = threeDtangent(i,j);
      
    }//end for j
       
  }//end for i


  for(i=0; i<3; i++) {
    for(j=0; j<3; j++) {
      dd11(i,j) = threeDtangentCopy(i,  j );
      dd12(i,j) = threeDtangentCopy(i,  j+3);
      dd21(i,j) = threeDtangentCopy(i+3,j );
      dd22(i,j) = threeDtangentCopy(i+3,j+3);
      
    }
  }

  //int Solve(const XC::Vector &V, XC::Vector &res) const;
  //int Solve(const XC::Matrix &M, XC::Matrix &res) const;
  //condensation 
  dd22.Solve(dd21, dd22invdd21);
  this->tangent   = dd11; 
  this->tangent  -= (dd12*dd22invdd21);

  return this->tangent;
}

//! @brief Return the material initial stiffness.
const XC::Matrix &XC::BeamFiberMaterial::getInitialTangent(void) const
  {
    static Matrix dd11(3,3);
    static Matrix dd12(3,3);
    static Matrix dd21(3,3);
    static Matrix dd22(3,3);
    static Matrix dd22invdd21(3,3);
    static Matrix threeDtangentCopy(6,6);

    const XC::Matrix &threeDtangent = theMaterial->getInitialTangent();

    //swap matrix indices to sort out-of-plane components 
    int i, j , ii, jj;
    for(i=0; i<6; i++) {

      ii = this->indexMap(i);

      for(j=0; j<6; j++) {

	jj = this->indexMap(j);

	threeDtangentCopy(ii,jj) = threeDtangent(i,j);

      }//end for j

    }//end for i


    for(i=0; i<3; i++) {
      for(j=0; j<3; j++) {
	dd11(i,j) = threeDtangentCopy(i,  j );
	dd12(i,j) = threeDtangentCopy(i,  j+3);
	dd21(i,j) = threeDtangentCopy(i+3,j );
	dd22(i,j) = threeDtangentCopy(i+3,j+3);

      }
    }

    //int Solve(const XC::Vector &V, XC::Vector &res) const;
    //int Solve(const XC::Matrix &M, XC::Matrix &res) const;
    //condensation 
    dd22.Solve(dd21, dd22invdd21);
    this->tangent   = dd11; 
    this->tangent  -= (dd12*dd22invdd21);

    return this->tangent;
  }

//NDmaterial strain order        = 11, 22, 33, 12, 23, 31 
//BeamFiberMaterial strain order = 11, 12, 31, 22, 33, 23
int XC::BeamFiberMaterial::indexMap(int i) const
  {
    int retval;

    if(i == 3) 
	retval = 1;
    else if(i == 5)
	retval = 2;
    else if(i == 1)
	retval = 3;
    else if(i == 2)
	retval = 4;
    else if(i == 4)
	retval = 5;
    else 
	retval = i;
    return retval;
  }

void XC::BeamFiberMaterial::Print(std::ostream &s, int flag) const
  {
    s << "BeamFiberMaterial, tag: " << this->getTag() << std::endl;
    s << "\tWrapped material: "<< theMaterial->getTag() << std::endl;

    theMaterial->Print(s, flag);
}

//! @brief Send object members through the communicator argument.
int XC::BeamFiberMaterial::sendData(Communicator &comm)
  {
    int res= NDAdaptorMaterial::sendData(comm);
    res+= comm.sendDoubles(Tstrain33,Tgamma23,Cstrain33,Cgamma23,getDbTagData(),CommMetaData(6));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::BeamFiberMaterial::recvData(const Communicator &comm)
  {
    int res= NDAdaptorMaterial::recvData(comm);
    res+= comm.receiveDoubles(Tstrain33,Tgamma23,Cstrain33,Cgamma23,getDbTagData(),CommMetaData(6));
    return res;
  }

//! @brief Sends object through the communicator argument.
int XC::BeamFiberMaterial::sendSelf(Communicator &comm) 
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(7);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Receives object through the communicator argument.
int XC::BeamFiberMaterial::recvSelf(const Communicator &comm)
  {
    inicComm(7);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << getClassName() << "::recvSelf - failed to receive ids.\n";
    else
      {
        setTag(getDbTagDataPos(0));
        res+= recvData(comm);
        if(res<0)
          std::cerr << getClassName() << "::recvSelf - failed to receive data.\n";
      }
    return res;
  }
