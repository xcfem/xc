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
                                                                        
// $Revision: 1.5 $
// $Date: 2008-04-14 21:26:50 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/HalfDiagramMaterial.h,v $
                                                      
// Created: Aug 2022
//
// Description: This file contains the class definition for 
// HalfDiagramMaterial.  HalfDiagramMaterial wraps a UniaxialMaterial
// and removes the positive or negative part of its diagram
// (see derived classes).

#ifndef HalfDiagramMaterial_h
#define HalfDiagramMaterial_h

#include "material/uniaxial/EncapsulatedMaterial.h"

namespace XC {
//! @brief Removes negative part of the stress-strain diagram.
//! @ingroup MatUnx
class HalfDiagramMaterial : public EncapsulatedMaterial
  {
  protected:
    const double factor= 0.0001;
  protected:
    DbTagData &getDbTagData(void) const;
  public:
    HalfDiagramMaterial(int tag, int classTag, UniaxialMaterial &material); 
    HalfDiagramMaterial(int tag, int classTag);

    int setTrialStrain(double strain, double strainRate = 0.0); 
    //int setTrialStrain(double strain, double FiberTemperature, double strainRate); 

    inline double getInitialTangent(void) const
      { return theMaterial->getInitialTangent();}

    int revertToLastCommit(void);    
    int revertToStart(void);        

    
    int sendSelf(Communicator &);  
    int recvSelf(const Communicator &);
    
    void Print(std::ostream &s, int flag =0) const;

    // AddingSensitivity:BEGIN //////////////////////////////////////////
    int setParameter(const std::vector<std::string> &, Parameter &);
    int updateParameter(int parameterID, Information &info);
    double getStrainSensitivity     (int gradIndex);
    double getInitialTangentSensitivity(int gradIndex);
    double getRhoSensitivity        (int gradIndex);
    // AddingSensitivity:END ///////////////////////////////////////////
  };
} // end of XC namespace

#endif

