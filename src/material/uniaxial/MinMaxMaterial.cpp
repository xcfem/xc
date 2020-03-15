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
                                                                        
// $Revision: 1.5 $
// $Date: 2003/04/02 22:02:42 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/MinMaxMaterial.cpp,v $

// Written: MHS
// Created: Aug 2001
//
// Description: This file contains the class definition for 
// MinMaxMaterial.  MinMaxMaterial wraps a UniaxialMaterial
// and imposes min and max strain limits.

#include <cstdlib>

#include <material/uniaxial/MinMaxMaterial.h>
#include <utility/matrix/ID.h>
#include "utility/matrix/Vector.h"

XC::MinMaxMaterial::MinMaxMaterial(int tag, UniaxialMaterial &material, double min, double max)
  :EncapsulatedMaterial(tag,MAT_TAG_MinMax,material),
   minStrain(min), maxStrain(max), Tfailed(false), Cfailed(false)
  {}

XC::MinMaxMaterial::MinMaxMaterial(int tag)
  :EncapsulatedMaterial(tag,MAT_TAG_MinMax),
   minStrain(0.0), maxStrain(0.0), Tfailed(false), Cfailed(false) {}

XC::MinMaxMaterial::MinMaxMaterial(void)
  :EncapsulatedMaterial(0,MAT_TAG_MinMax),
   minStrain(0.0), maxStrain(0.0), Tfailed(false), Cfailed(false) {}

int XC::MinMaxMaterial::setTrialStrain(double strain, double strainRate)
  {
    if(Cfailed)
      return 0;
    if(strain >= maxStrain || strain <= minStrain)
      {
        Tfailed = true;
        return 0;
      }
    else
      {
        Tfailed = false;
        return theMaterial->setTrialStrain(strain, strainRate);
      }
  }

double XC::MinMaxMaterial::getStress(void) const
  {
    if(Tfailed)
      return 0.0;
    else
      return theMaterial->getStress();
  }

double XC::MinMaxMaterial::getTangent(void) const
  {
    if(Tfailed)
      //return 0.0;
      return 1.0e-8*theMaterial->getInitialTangent();
    else
      return theMaterial->getTangent();
  }

double XC::MinMaxMaterial::getDampTangent(void) const
  {
    if(Tfailed)
      return 0.0;
    else
      return theMaterial->getDampTangent();
  }

int XC::MinMaxMaterial::commitState(void)
  {        
    Cfailed = Tfailed;

    // Check if failed at current step
    if(Tfailed)
      return 0;
    else
      return theMaterial->commitState();
  }

int XC::MinMaxMaterial::revertToLastCommit(void)
  {
    // Check if failed at last step
    if(Cfailed)
      return 0;
    else
      return theMaterial->revertToLastCommit();
  }

int XC::MinMaxMaterial::revertToStart(void)
  {
    Cfailed = false;
    Tfailed = false;
    return theMaterial->revertToStart();
  }

XC::UniaxialMaterial *XC::MinMaxMaterial::getCopy(void) const
  { return new MinMaxMaterial(*this); }

int XC::MinMaxMaterial::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(6); 
    int res= EncapsulatedMaterial::sendData(cp);
    res+= cp.sendDoubles(minStrain,maxStrain,getDbTagData(),CommMetaData(4));
    setDbTagDataPos(5,Cfailed);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "MinMaxMaterial::sendSelf() - failed to send the XC::ID\n";
    return res;
  }

int XC::MinMaxMaterial::recvSelf(const CommParameters &cp)
  {
    inicComm(6);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "MinMaxMaterial::recvSelf() - failed to get the ID\n";
    else
      {
        res+= EncapsulatedMaterial::recvData(cp);
        res+= cp.receiveDoubles(minStrain,maxStrain,getDbTagData(),CommMetaData(4));
        Cfailed= getDbTagDataPos(5);
        Tfailed = Cfailed;

      }
    return res;
  }

void XC::MinMaxMaterial::Print(std::ostream &s, int flag) const
  {
    s << "MinMaxMaterial tag: " << this->getTag() << std::endl;
    s << "\tMaterial: " << theMaterial->getTag() << std::endl;
    s << "\tMin strain: " << minStrain << std::endl;
    s << "\tMax strain: " << maxStrain << std::endl;
  }
