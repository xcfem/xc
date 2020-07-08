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
// $Date: 2003/08/14 20:23:50 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/FatigueMaterial.h,v $
                                                      
// Written: Patxi
// Created: Aug 2003
//
// Description: This file contains the class definition for 
// FatigueMaterial.  FatigueMaterial wraps a UniaxialMaterial
// and imposes fatigue limits.

#ifndef FatigueMaterial_h
#define FatigueMaterial_h

#include <material/uniaxial/UniaxialMaterial.h>

namespace XC {
//! @ingroup MatUnx
//
//! @brief FatigueMaterial wraps a UniaxialMaterial
//! and imposes fatigue limits.
class FatigueMaterial: public UniaxialMaterial
  {
  private:
    UniaxialMaterial *theMaterial;

    double energy, CStress; //SAJalali
    double DI; //Damage index
    double  X; //Range in consideration
    double  Y; //Previous Adjacent Range
    double  A; //Peak or valley 1
    double  B; //Peak or valley 2
    double  C; //Peak or valley 2
    double  D; //Peak or valley 4
    int   PCC; /*Previous Cycle counter flag if >1 then previous 'n' 
		 cycles did not flag a complete cycle */
    int   R1F; //Flag for first  peak count
    int   R2F; //Flag for second peak count
    double cSlope; //Current Slope
    double PS; //Previous slope
    double EP; //Previous Strain
    int    SF; /*Start Flag = 0 if very first strain, 
		 (i.e. when initializing)    = 1 otherwise */
    double DL; //Damage if current strain was last peak.

    double Dmax;
    double E0;
    double m;

    double minStrain;
    double maxStrain;

    bool Cfailed;
    double trialStrain;

    // added 6/9/2006
    // For recording strain ranges (SRXX) and Number of Cycles (NCXX)
    double SR1;  // Committed strain range at peak
    double NC1;  // Committed number of cycles at SR1 (i.e. 1.0 or 0.5)
    double SR2;  // Committed strain range 2 at PSEUDO peak - there are potentially two ranges
    double NC2;  // Committed number of cycles at SR2 2 (at PSEUDO peak) - there are potentially two ranges
    double SR3;  // Committed strain range 3 at PSEUDO peak - there are potentially two ranges
    double NC3;  // Committed number of cycles at SR2 3 (at PSEUDO peak) - there are potentially two ranges

    void alloc(const UniaxialMaterial &);
    void free(void); 
  public:
    FatigueMaterial(int tag, const UniaxialMaterial &material, 
		    double Dmax    =  1.0,
		    double E0      =  0.191,
		    double m       = -0.458,
		    double minStrain = -1.0e16,
		    double maxStrain =  1.0e16 );
    FatigueMaterial(int tag= 0);
    FatigueMaterial(const FatigueMaterial &);
    FatigueMaterial &operator=(const FatigueMaterial &);
    ~FatigueMaterial(void);
    UniaxialMaterial *getCopy(void) const;

    int setTrialStrain(double strain, double strainRate = 0.0); 
    double getStrain(void) const;          
    double getStrainRate(void) const;
    double getStress(void) const;
    double getTangent(void) const;
    double getDampTangent(void) const;
    inline double getInitialTangent(void) const
      { return theMaterial->getInitialTangent();}

    int commitState(void);
    int revertToLastCommit(void);    
    int revertToStart(void);        

    
    int sendSelf(Communicator &);  
    int recvSelf(const Communicator &);
    
    void Print(std::ostream &s, int flag =0) const;
    
    Response *setResponse(const std::vector<std::string> &, Information &);
    int getResponse(int responseID, Information &matInformation);
    bool hasFailed(void);  

    //by SAJalali
    inline virtual double getEnergy(void)
      { return energy; }
  };
} // end of XC namespace


#endif

