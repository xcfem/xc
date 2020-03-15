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
// $Date: 2005/09/23 22:51:27 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/Concrete04.h,v $
                                                                        
                                                                        
#ifndef Concrete04_h
#define Concrete04_h

// File: Concrete04.h
//
// Written: N.Mitra (nmitra@u.washington.edu)
// Created: 09/04
// Revision: A
//
// Description: This file contains the class definition for 
// Concrete04.h 
//   - No tension 
//   - Linear unloading/reloading
//
// What: "@(#) Concrete04.h, revA"
// Revision: 1. Adding in Exponential tension part (05-16-05)

#include "material/uniaxial/concrete/ConcreteBase.h"

namespace XC {
//! @ingroup MatUnx
//
//! @brief Uniaxial Popovics concrete material object with degraded
//! linear unloading/reloading stiffness according to the work of
//! Karsan-Jirsa and tensile strength with exponential decay.
class Concrete04: public ConcreteBase
  {
   private:
      /*** Material Properties ***/
    double fpc; //!< Compressive strength
    double epsc0; //!< Strain at compressive strength
    double epscu; //!< Strain at crushing strength
    double Ec0; //!< initial tangent
    double fct; //!< Concrete tensile strength
    double etu; //!< ultimate tensile strain              
    double beta; //!< exponential curve parameter, residual stress (as a factor of ft) at etu. 

      /*** CONVERGED History Variables ***/
    double CmaxStrain;  
    double CcompStrain; //!< strain value at which the compression unloading intersects the   
                        // zero stress value or the strain value at which tensile reloading starts.                                  
    double CUtenStress; //!< tensile stress value at which unloading begins
    double CUtenSlope;  //!< unloading tensile slope value

      /*** TRIAL History Variables ***/
    double TmaxStrain;
    double TcompStrain;
    double TUtenStress;
    double TUtenSlope;

    void CompReload(void);
    void CompEnvelope(void);
    void setCompUnloadEnv(void);
    void TensReload(void);
    void TensEnvelope(void);
    void setTenUnload(void);
    void setup_parameters(void);
  public:
    Concrete04(int tag, double fpc, double eco, double ecu, double Ec0, double fct, double etu);  
    Concrete04(int tag, double fpc, double eco, double ecu, double Ec0, double fct, double etu, double beta);
    Concrete04(int tag, double fpc, double eco, double ecu, double Ec0);
    Concrete04(int tag= 0);

    int setTrialStrain(double strain, double strainRate = 0.0); 
    inline double getInitialTangent(void) const
      { return Ec0; }

    int commitState(void);
    int revertToLastCommit(void);
    int revertToStart(void);        

    UniaxialMaterial *getCopy(void) const;
    
    int sendSelf(CommParameters &);  
    int recvSelf(const CommParameters &);
    
    void Print(std::ostream &s, int flag =0) const;

    // LOWES: add function for use with variable hinge lnegth model
    int getMaterialType(void);
    // LOWES: end
  };
} // end of XC namespace

#endif
