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
                                                                        
// $Revision: 1.6 $
// $Date: 2003/04/02 22:02:42 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/PathIndependentMaterial.cpp,v $

// Written: MHS
// Created: Aug 2000
//
// Description: This file contains the class definition for 
// PathIndependentMaterial.  PathIndependentMaterial uses a XC::UniaxialMaterial
// object to represent a path-independent uniaxial material.  Since
// it is path-independent, no state information is stored by
// PathIndependentMaterial.

#include <cstdlib>
#include <material/uniaxial/PathIndependentMaterial.h>
#include <utility/matrix/ID.h>

XC::PathIndependentMaterial::PathIndependentMaterial(int tag, UniaxialMaterial &material)
:EncapsulatedMaterial(tag,MAT_TAG_PathIndependent)
  {
    theMaterial = material.getCopy();
    if(theMaterial == 0)
      {
        std::cerr <<  "XC::PathIndependentMaterial::PathIndependentMaterial -- failed to get copy of material\n";
        exit(-1);
      }
  }

XC::PathIndependentMaterial::PathIndependentMaterial(int tag)
  : EncapsulatedMaterial(tag,MAT_TAG_PathIndependent) {}

XC::PathIndependentMaterial::PathIndependentMaterial(void)
  :EncapsulatedMaterial(0,MAT_TAG_PathIndependent) {}

int XC::PathIndependentMaterial::setTrialStrain(double strain, double strainRate)
  {
    return theMaterial->setTrialStrain(strain, strainRate);
  }

double XC::PathIndependentMaterial::getStress(void) const
  { return theMaterial->getStress(); }


double XC::PathIndependentMaterial::getTangent(void) const
  { return theMaterial->getTangent(); }

double XC::PathIndependentMaterial::getDampTangent(void) const
  { return theMaterial->getDampTangent(); }

double XC::PathIndependentMaterial::getInitialTangent(void) const
  { return theMaterial->getInitialTangent(); }

int XC::PathIndependentMaterial::commitState(void)
  { return 0; }

int XC::PathIndependentMaterial::revertToLastCommit(void)
  { return 0; }

int XC::PathIndependentMaterial::revertToStart(void)
  { return theMaterial->revertToStart(); }

XC::UniaxialMaterial *XC::PathIndependentMaterial::getCopy(void) const
  { return new PathIndependentMaterial(*this); }


int XC::PathIndependentMaterial::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(4); 
    int res= EncapsulatedMaterial::sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "PathIndependentMaterial::sendSelf() - failed to send the ID.\n";
    return res;

  }

int XC::PathIndependentMaterial::recvSelf(const CommParameters &cp)
  {
    inicComm(4);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "PathIndependentMaterial::recvSelf() - failed to get the ID\n";
    else
      res+= EncapsulatedMaterial::recvData(cp);
    return res;
  }

void XC::PathIndependentMaterial::Print(std::ostream &s, int flag) const
  {
    s << "PathIndependentMaterial tag: " << this->getTag() << std::endl;
    s << "\tmaterial: " << theMaterial->getTag() << std::endl;
  }
