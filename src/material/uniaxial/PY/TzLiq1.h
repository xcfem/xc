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
/* *********************************************************************
**    Module:	TzLiq1.h 
**
**    Purpose:	Provide a t-z material that gets pore pressure from a
**				specified element that contains a PorousFluidSolid.
**              
**
**    Developed by Ross W. Boulanger
**    (C) Copyright 2002, All Rights Reserved.
**
** ****************************************************************** */

// $Revision: 1.0
// $Date: 2002/2/5
// $Source: /OpenSees/SRC/material/uniaxial/TzLiq1.h

#ifndef TZLIQ1_H
#define TZLIQ1_H

// Written: RWB
// Created: Feb 2002
//
// Description: This file contains the class definition for TzLiq1.
// 

#include "TzSimple1.h"

namespace XC {
class FourNodeQuad;

//! @ingroup PYMat
//
//! @brief Uniaxial t-z material that incorporates liquefaction effects.
class TzLiq1 : public TzSimple1
  {
  private:
    // Committed and trial values for t, z, and ru
    double Tz;
    double Cz;
    double Tt;
    double Ct;
    double Tangent;
    double maxTangent;
    double Tru;
    double Cru;
    double Hru;

    // Solid element from which pore pressures are obtained, domain pointer
    // and stage information to get the initial vertical effective stress.
    int solidElem1;
    int solidElem2;

    double meanConsolStress;
    double ru;

    static int loadStage;
    int    lastLoadStage;
    std::string elemFlag; 
    Domain *theDomain;
    FourNodeQuad *theQuad1;
    FourNodeQuad *theQuad2;

    // Initial tangent
    double initialTangent;
    
    // Function for obtaining effective stresses from adjoining solid soil elements
    double getEffectiveStress(void);
    static Vector stressV3;
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    TzLiq1(int tag, int classtag, int tzType, double tult, double z50,
    	      double dashpot, int solidElem1, int solidElem2, Domain *theDomain);
    TzLiq1(int tag, int classtag= MAT_TAG_TzLiq1);
    TzLiq1(void);

    int setTrialStrain(double y, double yRate); 
    double getStrain(void) const;          
    double getStress(void) const;
    double getTangent(void) const;
    double getStrainRate(void) const;
    double getDampTangent(void) const;
    double getInitialTangent(void) const;

    int commitState(void);
    int revertToLastCommit(void);    
    int revertToStart(void);        

    UniaxialMaterial *getCopy(void) const;
    
    int sendSelf(CommParameters &);  
    int recvSelf(const CommParameters &);

    //  Command for initiating vertConsolStress from TclUpdateMaterialStageCommand
    int updateParameter(int snum, Information &eleInformation);
    
    void Print(std::ostream &s, int flag =0) const;
  };
} // end of XC namespace

#endif // TZLIQ1_H
