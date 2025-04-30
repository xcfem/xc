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
// $Date: 2012-05-28 22:03:16 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/PlateFromPlaneStressMaterial.cpp,v $

//
// Yuli Huang (yulihuang@gmail.com) & Xinzheng Lu (luxz@tsinghua.edu.cn)
//
// Generic Plate Material from Plane Stress Material
//
/* Ref: Lu X, Lu XZ, Guan H, Ye LP, Collapse simulation of reinforced 
concrete high-rise building induced by extreme earthquakes, 
Earthquake Engineering & Structural Dynamics, 2013, 42(5): 705-723*/


#include "PlateFromPlaneStressMaterial.h"
#include "utility/recorder/response/MaterialResponse.h"
#include "material/ResponseId.h"

//static vector and matrices
XC::Vector  XC::PlateFromPlaneStressMaterial::stress(5);
XC::Matrix  XC::PlateFromPlaneStressMaterial::tangent(5,5);

//null constructor
XC::PlateFromPlaneStressMaterial::PlateFromPlaneStressMaterial(int tag)
  : PlateAdaptorMaterial<NDMaterialWrapper>(tag, ND_TAG_PlateFromPlaneStressMaterial), strain(5), gmod(0.0)
  {}


//full constructor
XC::PlateFromPlaneStressMaterial::PlateFromPlaneStressMaterial(int tag, NDMaterial &ndMat, const double &g )
  :PlateAdaptorMaterial<NDMaterialWrapper>(tag, ND_TAG_PlateFromPlaneStressMaterial, ndMat), strain(5), gmod(g)
 {}


//make a clone of this material
XC::NDMaterial *XC::PlateFromPlaneStressMaterial::getCopy(void) const
  { return new PlateFromPlaneStressMaterial(*this); }


//make a clone of this material
XC::NDMaterial *XC::PlateFromPlaneStressMaterial::getCopy(const std::string &type) const 
  {
    if(type == this->getType())
      return this->getCopy();
    else
      return nullptr;
  }

void XC::PlateFromPlaneStressMaterial::setGmod(const double &d)
  { this->gmod= d; }

//swap history variables
int XC::PlateFromPlaneStressMaterial::commitState(void) 
  { return this->getMaterial()->commitState(); }



//revert to last saved state
int XC::PlateFromPlaneStressMaterial::revertToLastCommit(void)
  {
  return this->getMaterial()->revertToLastCommit() ;
}


//revert to start
int XC::PlateFromPlaneStressMaterial::revertToStart(void)
  {
    strain.Zero();
    return this->getMaterial()->revertToStart();
  }


//receive the strain
int XC::PlateFromPlaneStressMaterial::setTrialStrain(const Vector &strainFromElement )
  {
    strain(0) = strainFromElement(0);
    strain(1) = strainFromElement(1);
    strain(2) = strainFromElement(2);
    strain(3) = strainFromElement(3);
    strain(4) = strainFromElement(4);

    static Vector PSStrain(3);

    PSStrain(0) = strain(0);
    PSStrain(1) = strain(1);
    PSStrain(2) = strain(2);

    return this->getMaterial()->setTrialStrain(PSStrain);
  }


//send back the strain
const XC::Vector &XC::PlateFromPlaneStressMaterial::getStrain(void) const
  {
    return strain;
  }


//send back the stress 
const XC::Vector &XC::PlateFromPlaneStressMaterial::getStress(void) const
  {
    //three dimensional stress
    const Vector &PSStress = this->getMaterial()->getStress();

    stress(0) = PSStress(0);
    stress(1) = PSStress(1);
    stress(2) = PSStress(2);

    stress(3) = gmod * strain(3);
    stress(4) = gmod * strain(4);

    return stress;
  }


//send back the tangent 
const XC::Matrix &XC::PlateFromPlaneStressMaterial::getTangent(void) const
  {
    const NDMaterial *mat= this->getMaterial();
    const Matrix &PSTangent= mat->getTangent();

    tangent.Zero();

    tangent(0,0) = PSTangent(0,0);
    tangent(0,1) = PSTangent(0,1);
    tangent(0,2) = PSTangent(0,2);
    tangent(1,0) = PSTangent(1,0);
    tangent(1,1) = PSTangent(1,1);
    tangent(1,2) = PSTangent(1,2);
    tangent(2,0) = PSTangent(2,0);
    tangent(2,1) = PSTangent(2,1);
    tangent(2,2) = PSTangent(2,2);

    tangent(3,3) = gmod;
    tangent(4,4) = gmod;

    return tangent;
  }

//send back the tangent 
const XC::Matrix &XC::PlateFromPlaneStressMaterial::getInitialTangent(void) const
  {
    const NDMaterial *mat= this->getMaterial();
    const Matrix &PSTangent= mat->getInitialTangent();

    tangent.Zero();

    tangent(0,0) = PSTangent(0,0);
    tangent(0,1) = PSTangent(0,1);
    tangent(0,2) = PSTangent(0,2);
    tangent(1,0) = PSTangent(1,0);
    tangent(1,1) = PSTangent(1,1);
    tangent(1,2) = PSTangent(1,2);
    tangent(2,0) = PSTangent(2,0);
    tangent(2,1) = PSTangent(2,1);
    tangent(2,2) = PSTangent(2,2);
    tangent(3,3) = gmod;
    tangent(4,4) = gmod;

    return tangent;
  }


//print out data
void XC::PlateFromPlaneStressMaterial::Print(std::ostream &s, int flag ) const
  {
    PlateAdaptorMaterial<NDMaterialWrapper>::Print(s, flag);
    s << "G: " << gmod << std::endl;
  }

//! @brief Send material data.
int XC::PlateFromPlaneStressMaterial::sendData(Communicator &comm)
  {
    int res= PlateAdaptorMaterial<NDMaterialWrapper>::sendData(comm);
    res+= comm.sendDouble(gmod, getDbTagData(), CommMetaData(2));
    res+= comm.sendVector(strain, getDbTagData(),CommMetaData(3));
    return res;
  }

//! @brief Receive material data.
int XC::PlateFromPlaneStressMaterial::recvData(const Communicator &comm)
  {
    int res= PlateAdaptorMaterial<NDMaterialWrapper>::recvData(comm);
    res+= comm.receiveDouble(gmod, getDbTagData(), CommMetaData(2));
    res+= comm.receiveVector(strain, getDbTagData(),CommMetaData(3));
    return res;
  }
int XC::PlateFromPlaneStressMaterial::sendSelf(Communicator &comm)
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

int XC::PlateFromPlaneStressMaterial::recvSelf(const Communicator &comm)
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

//setResponse - added by V.K. Papanikolaou [AUTh] - start
XC::Response* XC::PlateFromPlaneStressMaterial::setResponse(const std::vector<std::string> &argv, Information &matInfo)
  {
    if ((argv[0]== "Tangent") || (argv[0]== "tangent") ||
        (argv[0] == "stress") || (argv[0] == "stresses") ||
        (argv[0] == "strain") || (argv[0] == "strains"))
      {
        
        return NDMaterial::setResponse(argv, matInfo);  // for stresses/strains, get response from NDMaterial
      }

    // for material-specific output

    Response *theResponse = getMaterial()->setResponse(argv, matInfo);

    if(!theResponse) 
        return NDMaterial::setResponse(argv, matInfo);  // not implemented, get default (zero) response from NDMaterial

    return theResponse;  // implemented, get damage response from theMat
  }
//setResponse - added by V.K. Papanikolaou [AUTh] - end
