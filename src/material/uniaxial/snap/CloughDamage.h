// -*-c++-*-
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
                                                                        
// $Revision: 1.1 $
// $Date: 2004/09/01 03:53:13 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/snap/CloughDamage.h,v $
//
//
// CloughDamage.h: implementation of the CloughDamage class from Fortran version.
// Originally from SNAP PROGRAM by Prof H.K. Krawinkler
//
// Written: A. Altoontash & Prof. G. Deierlein 12/01
// Revised: 03/02
//
// Purpose: This file contains the implementation for the CloughDamage class.
//
//////////////////////////////////////////////////////////////////////

// CloughDamage.h: interface for the CloughDamage class.
//
//////////////////////////////////////////////////////////////////////

#ifndef CloughDamage_H
#define CloughDamage_H

#include <material/uniaxial/UniaxialMaterial.h>
#include <utility/matrix/Vector.h>
#include <stdio.h>

namespace XC {
class DamageModel;

//! @ingroup MatUnx
//
//! @brief Clough type hysteretic model with degradation.
class CloughDamage: public UniaxialMaterial
  {
  protected:
    void envelPosCap(double fy, double alphaPos, double alphaCap,
    	double cpDsp, double d, double *f, double *ek );
    
    void envelNegCap(double fy, double alphaNeg, double alphaCap,
    	double cpDsp, double d, double *f, double *ek);
    
    void recordInfo(int cond =0);
    
    
  private:
    // Input parameters
    double elstk,fyieldPos,fyieldNeg,alpha,Resfac;		//	Properties
    double capSlope,capDispPos,capDispNeg;	 // Cap

    // Parameters calculated from input data
    double dyieldPos,dyieldNeg;
    DamageModel *StrDamage;
    DamageModel *StfDamage;
    DamageModel *AccDamage;
    DamageModel *CapDamage;

    double hsTrial[24], hsCommit[24], hsLastCommit[24];
    
    FILE *OutputFile;		// For debugging
  public:
    CloughDamage();
    CloughDamage(int tag);
    CloughDamage(int tag, Vector inputParam, DamageModel *strength, DamageModel *stiffness,DamageModel *accelerated, DamageModel *capping);
    virtual ~CloughDamage();
    
    int setTrialStrain(double d, double strainRate = 0.0);

    double getStrain(void) const;
    double getStress(void) const;
    double getTangent(void) const;
    double getInitialTangent(void) const;

    int commitState(void);
    int revertToLastCommit(void);    
    int revertToStart(void);  
    
    //virtual
    UniaxialMaterial *getCopy(void) const;
    
    int sendSelf(CommParameters &);  
    int recvSelf(const CommParameters &);
    
    void Print(std::ostream &s, int flag=0) const;
  };
} // end of XC namespace

#endif
