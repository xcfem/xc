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
                                                                        
// $Revision: 1.7 $
// $Date: 2004/07/15 21:34:10 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/FedeasMaterial.h,v $
                                                                      
// Written: MHS
// Created: Jan 2001
//
// Description: This file contains the class definition for 
// FedeasMaterial. FedeasMaterial provides a FORTRAN interface
// for programming uniaxial material models, using the subroutine
// interface from the FEDEAS ML1D library, developed by F.C. Filippou.
//
// For more information visit the FEDEAS web page:
//    http://www.ce.berkeley.edu/~filippou/Research/fedeas.htm

#ifndef FedeasMaterial_h
#define FedeasMaterial_h

#include "UniaxialMaterial.h"
#include "UniaxialStateVars.h"

namespace XC {
//! @ingroup MatUnx
//
//! @brief FedeasMaterial provides a FORTRAN interface
//! for programming uniaxial material models, using the subroutine
//! interface from the FEDEAS ML1D library, developed by F.C. Filippou.
class FedeasMaterial : public UniaxialMaterial
  {
  protected:
    // Invokes the FORTRAN subroutine
    virtual int invokeSubroutine(int ist);
  
    double *data; //!< Material parameters array
    double *hstv; //!< History array: first half is committed, second half is trial
  
    int numData; //!< Number of material parameters
    int numHstv; //!< Number of history variables
  
    UniaxialStateVars converged; //!< Committed state
    UniaxialStateVars trial; //!< Trial state

    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    FedeasMaterial(int tag, int classTag, int numHV, int numData);
    virtual ~FedeasMaterial(void);
  
    virtual int setTrialStrain(double strain, double strainRate = 0.0);
    virtual int setTrial(double strain, double &stress, double &tangent, double strainRate= 0.0);
    virtual double getStrain(void) const;
    virtual double getStress(void) const;
    virtual double getTangent(void) const;
  
    virtual int commitState(void);
    virtual int revertToLastCommit(void);    
    virtual int revertToStart(void);
  
    virtual int sendSelf(CommParameters &);  
    virtual int recvSelf(const CommParameters &);
  
    virtual void Print(std::ostream &s, int flag = 0);
  };
} // end of XC namespace

#endif
