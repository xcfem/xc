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
// $Date: 2003/02/25 23:33:39 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/ParallelMaterial.h,v $
                                                                        
                                                                        
#ifndef ParallelMaterial_h
#define ParallelMaterial_h

// File: ~/material/ParallelMaterial.h
//
// Written: fmk 
// Created: 07/98
// Revision: A
//
// Description: This file contains the class definition for 
// ParallelMaterial. ParallelMaterial is an aggregation
// of UniaxialMaterial objects all considered acting in parallel.
//
// What: "@(#) ParallelMaterial.h, revA"

#include <material/uniaxial/connected/ConnectedMaterial.h>

namespace XC{
//! @ingroup MatUnx
//
//! @brief Parallel connected materials.
class ParallelMaterial: public ConnectedMaterial
  {
  private:
    double trialStrain;
    double trialStrainRate;
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    ParallelMaterial(int tag, const DqUniaxialMaterial &theMaterials);
    ParallelMaterial(int tag);
    ParallelMaterial(void);

    int setTrialStrain(double strain, double strainRate = 0.0); 
    double getStrain(void) const;          
    double getStrainRate(void) const;
    double getStress(void) const;
    double getTangent(void) const;
    double getDampTangent(void) const;
    double getInitialTangent(void) const;

    int commitState(void);
    int revertToLastCommit(void);    
    int revertToStart(void);        

    UniaxialMaterial *getCopy(void) const;
    
    int sendSelf(CommParameters &);  
    int recvSelf(const CommParameters &);
    
    void Print(std::ostream &s, int flag =0);

    Response *setResponse(const std::vector<std::string> &argv, Information &matInformation);
    int getResponse(int responseID, Information &matInformation);
  };
} // end of XC namespace


#endif

