// -*-c++-*-
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
                                                                        
// $Revision: 1.1 $
// $Date: 2010-09-11 00:45:21 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/InitStrainBaseMaterial.h,v $
                                                      
// Written: fmk
// Created: Sep 2010
//
// Description: This file contains the class definition for 
// InitStrainBaseMaterial.  Base class for InintStrainMaterial and
// InitStressMaterial.

#ifndef InitStrainBaseMaterial_h
#define InitStrainBaseMaterial_h

#include <material/uniaxial/EncapsulatedMaterial.h>

namespace XC {
//! @ingroup MatUnx
//
//! @brief InitStrainBaseMaterial wraps a UniaxialMaterial
//! and imposes an initial strain
class InitStrainBaseMaterial: public EncapsulatedMaterial
  {
  protected:
    double epsInit;

    int sendData(Communicator &);  
    int recvData(const Communicator &);
  public:
    InitStrainBaseMaterial(int tag, int classTag);
    InitStrainBaseMaterial(int tag, int classTag, const UniaxialMaterial &material, double epsInit);

    
    virtual int setInitialStrain(const double &);
    virtual int incrementInitialStrain(const double &);
    virtual void zeroInitialStrain(void);
    double getInitialStrain(void) const;
    
    double getStrainRate(void) const;
    double getStress(void) const;
    double getTangent(void) const;
    double getDampTangent(void) const;
    inline double getInitialTangent(void) const
      { return this->getMaterial()->getInitialTangent();}

    int commitState(void);
    int revertToLastCommit(void);    
    int revertToStart(void);        

    // AddingSensitivity:BEGIN //////////////////////////////////////////
    double getStressSensitivity(int gradIndex, bool conditional);
    double getInitialTangentSensitivity(int gradIndex);
    int commitSensitivity(double strainGradient, int gradIndex, int numGrads);
    // AddingSensitivity:END ///////////////////////////////////////////
  };
} // end of XC namespace


#endif

