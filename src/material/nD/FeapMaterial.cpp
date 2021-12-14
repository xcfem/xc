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
// $Date: 2003/04/02 22:02:40 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/FeapMaterial.cpp,v $
                                                                        
// Written: MHS
// Created: June 2001
//
// Description: This file contains the class definition for 
// FeapMaterial. FeapMaterial wraps a Feap material subroutine.


#include <material/nD/FeapMaterial.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/ID.h>

#include <cstring>
#include <cstdlib>
#include <cfloat>
#include "material/nD/NDMaterialType.h"

double XC::FeapMaterial::d[200];
double XC::FeapMaterial::sig[6];
double XC::FeapMaterial::dd[36];

XC::Vector XC::FeapMaterial::strain3(3);
XC::Vector XC::FeapMaterial::strain4(4);
XC::Vector XC::FeapMaterial::strain6(6);

XC::Vector XC::FeapMaterial::sigma3(3);
XC::Vector XC::FeapMaterial::sigma4(4);
XC::Vector XC::FeapMaterial::sigma6(sig,6);

XC::Matrix XC::FeapMaterial::tangent3(3,3);
XC::Matrix XC::FeapMaterial::tangent4(4,4);
XC::Matrix XC::FeapMaterial::tangent6(dd,6,6);

XC::FeapMaterial::FeapMaterial(int tag, int classTag, int nhv, int ndata, double r)
  :XC::NDMaterial(tag,classTag), ud(0), hstv(0), rho(r),
   numHV(nhv), numData(ndata), myFormulation(ThreeDimensional)
{
  if(numHV < 0)
    numHV = 0;
  
  if(numHV > 0) {
    // Allocate history array
    hstv = new double[2*numHV];
    if(hstv == 0) {
      std::cerr <<  "FeapMaterial::FeapMaterial -- failed to allocate history array -- type: " << classTag << std::endl;
      exit(-1);
    }
                        
    
    // Zero out the history variables
    for(int i = 0; i < 2*numHV; i++)
      hstv[i] = 0.0;
  }
  
  if(numData < 0)
    numData = 0;
  
  if(numData > 0) {
    // Allocate material parameter array
    ud = new double[numData];
    if(ud == 0) {
      std::cerr << "FeapMaterial::FeapMaterial -- failed to allocate ud array -- type: " << classTag << std::endl;
      exit(-1);
    }
  }
  
  // Zero the strain vector
  for(int i = 0; i < 6; i++)
    eps[i] = 0.0;
}

XC::FeapMaterial::FeapMaterial(int classTag)
  :XC::NDMaterial(0,classTag), ud(0), hstv(0), rho(0.0),
   numHV(0), numData(0), myFormulation(Unknown)
{
  // Zero the strain vector
  for(int i = 0; i < 6; i++)
    eps[i] = 0.0;
}

XC::FeapMaterial::~FeapMaterial()
{
  if(ud != 0)
    delete [] ud;
  
  if(hstv != 0)
    delete [] hstv;
}

// Here we are assuming the following order on strains
// \epsilon = {11, 22, 33, 12, 23, 31}
int
XC::FeapMaterial::setTrialStrain(const XC::Vector &strain)
{
  switch(myFormulation) {
  case ThreeDimensional:
    eps[0] = strain(0);
    eps[1] = strain(1);
    eps[2] = strain(2);
    eps[3] = strain(3);
    eps[4] = strain(4);
    eps[5] = strain(5);
    break;
  case PlaneStrain:
    eps[0] = strain(0);
    eps[1] = strain(1);
    eps[3] = strain(2);
    break;
  case AxiSymmetric:
    eps[0] = strain(0);
    eps[1] = strain(1);
    eps[2] = strain(2);
    eps[3] = strain(3);
    break;
  default:
    std::cerr << "FeapMaterial::FeapMaterial -- unknown material formulation\n";
    exit(-1);
    break;
  }

  return 0;
}

// Here we are assuming the following order on strains
// \epsilon = {11, 22, 33, 12, 23, 31}
const XC::Vector &XC::FeapMaterial::getStrain(void) const
{
  switch(myFormulation) {
  case ThreeDimensional:
    strain6(0) = eps[0];
    strain6(1) = eps[1];
    strain6(2) = eps[2];
    strain6(3) = eps[3];
    strain6(4) = eps[4];
    strain6(5) = eps[5];
    return strain6;
  case PlaneStrain:
    strain3(0) = eps[0];
    strain3(1) = eps[1];
    strain3(2) = eps[3];
    return strain3;
  case AxiSymmetric:
    strain4(0) = eps[0];
    strain4(1) = eps[1];
    strain4(2) = eps[2];
    strain4(3) = eps[3];
    return strain4;
  default:
    std::cerr << "FeapMaterial::getSTrain -- unknown material formulation\n";
    exit(-1);
      
    return strain6;
  }
}

// Here we are assuming the following order on stresses
// \sigma = {11, 22, 33, 12, 23, 31}
const XC::Vector &XC::FeapMaterial::getStress(void) const
{
  int isw = 3;

  // Invoke Feap subroutine
  this->invokeSubroutine(isw);
  
  switch(myFormulation) {
  case ThreeDimensional:
    return sigma6;
  case PlaneStrain:
    sigma3(0) = sig[0];
    sigma3(1) = sig[1];
    sigma3(2) = sig[3];
    return sigma3;
  case AxiSymmetric:
    sigma4(0) = sig[0];
    sigma4(1) = sig[1];
    sigma4(2) = sig[2];
    sigma4(3) = sig[3];
    return sigma4;
  default:
    std::cerr << "FeapMaterial::getStress -- unknown material formulation\n";
      exit(-1);
    return sigma6;
  }
}

//! @brief Return the material tangent stiffness.
const XC::Matrix &XC::FeapMaterial::getTangent(void) const
{
  int isw = 6;

  // Invoke Feap subroutine
  this->invokeSubroutine(isw);

  int i,j;

  switch(myFormulation) {
  case ThreeDimensional:
    return tangent6;
  case PlaneStrain:
    tangent3(0,0) = tangent6(0,0);
    tangent3(0,1) = tangent6(0,1);
    tangent3(0,2) = tangent6(0,3);
    tangent3(1,0) = tangent6(1,0);
    tangent3(1,1) = tangent6(1,1);
    tangent3(1,2) = tangent6(1,3);
    tangent3(2,0) = tangent6(3,0);
    tangent3(2,1) = tangent6(3,1);
    tangent3(2,2) = tangent6(3,3);
    return tangent3;
  case AxiSymmetric:
    for(i = 0; i < 4; i++)
      for(j = 0; j < 4; j++)
    tangent4(i,j) = tangent6(i,j);
    return tangent4;
  default:
    std::cerr << "FeapMaterial::getTangent -- unknown material formulation\n";
    exit(-1);
    return tangent6;
  }
}

//! @brief Commit the state of the material.
int XC::FeapMaterial::commitState(void)
{
  // Set committed values equal to corresponding trial values
  for(int i = 0; i < numHV; i++)
    hstv[i] = hstv[i+numHV];
  
  return 0;
}

int
XC::FeapMaterial::revertToLastCommit(void)
{
  // Set trial values equal to corresponding committed values
  for(int i = 0; i < numHV; i++)
    hstv[i+numHV] = hstv[i];
  
  return 0;
}

//! @brief Revert the material to its initial state.
int XC::FeapMaterial::revertToStart(void)
  {
    int retval= NDMaterial::revertToStart();
    // Set all trial and committed values to zero
    for(int i = 0; i < 2*numHV; i++)
      hstv[i] = 0.0;
    return retval;
  }

XC::NDMaterial *XC::FeapMaterial::getCopy(void) const
  { return new FeapMaterial(*this); }

XC::NDMaterial *XC::FeapMaterial::getCopy(const std::string &type) const
  {
    FeapMaterial *theCopy = dynamic_cast<FeapMaterial *>(this->getCopy());
  
    if((type==strTypeThreeDimensional) || (type==strType3D))
      theCopy->myFormulation = ThreeDimensional;
    else if((type==strTypePlaneStrain) || (type==strTypePlaneStrain2D))
      theCopy->myFormulation = PlaneStrain;
    else if((type==strTypeAxiSymmetric) || (type==strTypeAxiSymmetric2D))
      theCopy->myFormulation = AxiSymmetric;
    else
      {
        std::cerr << "FeapMaterial::getCopy -- Invalid type(" << type << ") for XC::FeapMaterial\n";
        return 0;
      }
    return theCopy;
  }

const std::string &XC::FeapMaterial::getType(void) const
  {
    switch(myFormulation)
      {
      case ThreeDimensional:
        return strTypeThreeDimensional;
      case PlaneStrain:
        return strTypePlaneStrain;
      case AxiSymmetric:
        return strTypeAxiSymmetric;
      default:
        std::cerr << "FeapMaterial::getTYpe -- unknown material formulation\n";
        return "Unknown";
      }
  }

int XC::FeapMaterial::getOrder(void) const
{
  switch(myFormulation) {
  case ThreeDimensional:
    return 6;
  case PlaneStrain:
    return 3;
  case AxiSymmetric:
    return 4;
  default:
    std::cerr << "FeapMaterial::getOrder -- unknown material formulation\n";
    return 0;
  }
}

int XC::FeapMaterial::sendSelf(Communicator &comm)
  {
    int res = 0;
  
    static ID idData(4);
  
    idData(0) = this->getTag();
    idData(1) = numHV;
    idData(2) = numData;
    idData(3) = myFormulation;
  
    res+= comm.sendIdData(this->getDbTag());
    if(res < 0) 
      std::cerr << "FeapMaterial::sendSelf() - failed to send XC::ID data\n";
  
    Vector vecData(numHV+numData);
  
    int i, j;
    // Copy history variables into vector
    for(i = 0; i < numHV; i++)
      vecData(i) = hstv[i];
  
    // Copy material properties into vector
    for(i = 0, j = numHV; i < numData; i++, j++)
      vecData(j) = ud[i];
  
    res+= theChannel.sendVector(this->getDbTag(), commitTag, vecData);
    if(res < 0) 
      std::cerr << "FeapMaterial::sendSelf() - failed to send XC::Vector data\n";
    return res;
  }

int XC::FeapMaterial::recvSelf(const Communicator &comm)
{
  int res = 0;
  
  static ID idData(4);
  
  res += comm.receiveIdData(this->getDbTag());
  if(res < 0) {
    std::cerr << "FeapMaterial::recvSelf() - failed to receive XC::ID data\n";
    return res;
  }
  
  this->setTag(idData(0));
  numHV   = idData(1);
  numData = idData(2);
  myFormulation  = idData(3);
  
  XC::Vector vecData(numHV+numData);
  
  res += theChannel.recvVector(this->getDbTag(), commitTag, vecData);
  if(res < 0) {
    std::cerr << "FeapMaterial::recvSelf() - failed to receive XC::Vector data\n";
    return res;
  }
  
  int i, j;
  // Copy history variables from vector
  for(i = 0; i < numHV; i++)
    hstv[i] = vecData(i);
  
  // Copy material properties from vector
  for(i = 0, j = numHV; i < numData; i++, j++)
    ud[i] = vecData(j);
  
  return res;
}
    
void
XC::FeapMaterial::Print(std::ostream &s, int flag) const
{
  s << "FeapMaterial, tag: " << this->getTag() << std::endl;
  s << "Material formulation: " << this->getType() << std::endl;
  s << "Material subroutine: ";
  
  switch(this->getClassTag()) {
  case ND_TAG_FeapMaterial01:
    s << "matl01" << std::endl;
    break;

  case ND_TAG_FeapMaterial02:
    s << "matl02" << std::endl;
    break;

  case ND_TAG_FeapMaterial03:
    s << "matl03" << std::endl;
    break;
    
    // Add more cases as needed
  default:
    s << this->getClassTag() << std::endl;
    break;
  }
  
  s << "Material density: " << rho << std::endl;
}

#ifdef _WIN32

extern "C" int FEAPCOMMON(double *dt, int *niter);

extern "C" int MATL01(double *eps, double *trace, double *td, double *d,
                           double *ud, double *hn, double *h1, int *nh,
                           double *sig, double *dd, int *isw);

extern "C" int MATL02(double *eps, double *trace, double *td, double *d,
                           double *ud, double *hn, double *h1, int *nh,
                           double *sig, double *dd, int *isw);

extern "C" int MATL03(double *eps, double *trace, double *td, double *d,
                           double *ud, double *hn, double *h1, int *nh,
                           double *sig, double *dd, int *isw);

#define feapcommon_ FEAPCOMMON
#define matl01_ MATL01
#define matl02_ MATL02
#define matl03_ MATL03

#else

extern "C" int feapcommon_(double *dt, int *niter);

extern "C" int matl01_(double *eps, double *trace, double *td, double *d,
                   double *ud, double *hn, double *h1, int *nh,
                   double *sig, double *dd, int *isw);

extern "C" int matl02_(double *eps, double *trace, double *td, double *d,
                   double *ud, double *hn, double *h1, int *nh,
                   double *sig, double *dd, int *isw);

extern "C" int matl03_(double *eps, double *trace, double *td, double *d,
                   double *ud, double *hn, double *h1, int *nh,
                   double *sig, double *dd, int *isw);

#endif

int
XC::FeapMaterial::invokeSubroutine(int isw)
{
  // Trace of strain vector
  double trace = eps[0] + eps[1] + eps[2];

  // Temperature change(currently not used)
  double td = 0.0;

  // Zero out stress and tangent arrays as required to do so
  int i;
  for(i = 0; i < 6; i++) {
    sig[i] = 0.0;
    dd[i] = 0.0;
  }
  for( ; i < 36; i++)
    dd[i] = 0.0;

  // Populate the FEAP material array for this model
  this->fillDArray();

  // Fill in the common blocks
  const double dt= FEProblem::theActiveDomain->getTimeTracker().getDt();  // From G3Globals.h
  int niter = 1;    // Need to count the number of global iterations!
  feapcommon_(&dt, &niter);

  switch(this->getClassTag()) {
  case ND_TAG_FeapMaterial01:
    matl01_(eps, &trace, &td, d, ud, hstv, &hstv[numHV], &numHV, sig, dd, &isw);
    break;

  case ND_TAG_FeapMaterial02:
    matl02_(eps, &trace, &td, d, ud, hstv, &hstv[numHV], &numHV, sig, dd, &isw);
    break;

  case ND_TAG_FeapMaterial03:
    matl03_(eps, &trace, &td, d, ud, hstv, &hstv[numHV], &numHV, sig, dd, &isw);
    break;
    
    // Add more cases as needed
  default:
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; unknown material type\n";
    return -1;
  }
  
  return 0;
}

int
XC::FeapMaterial::fillDArray(void)
{
  // Must be done by subclasses!
  return 0;
}
