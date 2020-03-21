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
// $Date: 2005/08/08 21:54:39 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/Steel03.h,v $
                                                                        
                                                                        
#ifndef Steel03_h
#define Steel03_h

// File: Steel03.h
//
// Written: mackie
// Created: 06/2005
// Revision: A
//
// Description: This file contains the class definition for 
// Steel03.h. Steel03 is Steel01 verbatim but with added Giuffre-Menegotto-Pinto 
// transitions on the loading and unloading loops.  
// references:
// 1.) 	Menegotto, M., and Pinto, P.E. (1973). Method of analysis of cyclically loaded 
//	RC plane frames including changes in geometry and non-elastic behavior of 
//	elements under normal force and bending. Preliminary Report IABSE, vol 13. 
// 2.)	Dhakal, R.J., and Maekawa, K. (2002). Path-dependent cyclic stress-strain relationship
//	of reinforcing bar including buckling. Engineering Structures, 24(11): 1383-96.
// 3.)	Gomes, A., and Appleton, J. (1997). Nonlinear cyclic stress-strain relationship of 
//	reinforcing bars including buckling. Engineering Structures, 19(10): 822-6.

// 
//
//
// What: "@(#) Steel03.h, revA"


#include "material/uniaxial/steel/SteelBase0103.h"

namespace XC {
//! @ingroup MatUnx
//
//! @brief Steel 03 uniaxial material.
class Steel03: public SteelBase0103
  {
  private:
    /*** Material Properties ***/
    double r;	// radius of rounded corners
    double cR1;
    double cR2;

    double CbStrain;
    double CbStress;
    double CrStrain;
    double CrStress;
    double Cplastic;

    /*** CONVERGED State Variables ***/    
    double CcurR;

    /*** TRIAL State Variables ***/
    double TcurR;

    double TbStrain;
    double TbStress;
    double TrStrain;
    double TrStress;
    double Tplastic;
    

    void determineTrialState(double dStrain);
    double getR(double x_in);
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
    int setup_parameters(void);
  public:
    Steel03(int tag, double fy, double E0, double b, double r, double cR1, double cR2,
       double a1 = STEEL_0103_DEFAULT_A1, double a2 = STEEL_0103_DEFAULT_A2,
       double a3 = STEEL_0103_DEFAULT_A3, double a4 = STEEL_0103_DEFAULT_A4);
    Steel03(int tag);
    Steel03(void);

    int setTrialStrain(double strain, double strainRate = 0.0); 
    int setTrial(double strain, double &stress, double &tangent, double strainRate = 0.0);

    int commitState(void);
    int revertToLastCommit(void);    

    UniaxialMaterial *getCopy(void) const;
    
    int sendSelf(CommParameters &);  
    int recvSelf(const CommParameters &);
    
    void Print(std::ostream &s, int flag =0) const;
  };
} // end of XC namespace

#endif
