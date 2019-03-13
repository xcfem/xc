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
// $Date: 2005/06/16 21:41:03 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/EPPGapMaterial.h,v $

// File: ~/material/EPPGapMaterial.h
//
// Written: krm
// Created: 07/2000
// Revision: A
//
// Description: This file contains the class definition for 
// ElasticMaterial. EPPGapMaterial provides the abstraction
// of an elastic perfectly plastic (tension only) path dependent uniaxial 
// material, with an initial gap offset (force-displacement units)
// For compression only behavior, enter negative gap and ep
// Damage can accumulate through specification of damage = 1 switch,
// otherwise damage = 0
//
//                  gap ep
//                  |<>|>|
//          stress  |    +-----+ fy
//                  |   /E    /
//         ---------+--+-+---+----strain
//                  |       
//                  |   
//                  |      
//
// What: "@(#) EPPGapMaterial.h, revA"

#ifndef EPPGapMaterial_h
#define EPPGapMaterial_h

#include <material/uniaxial/EPPBaseMaterial.h>

namespace XC {
//! @ingroup MatUnx
//
//! @brief Elastic perfectly plastic material with initial "gap".
//! provides the abstraction of an elastic perfectly
//! plastic (tension only) path dependent uniaxial 
//! material, with an initial gap offset (force-displacement units)
//! For compression only behavior, enter negative gap and ep
//! Damage can accumulate through specification of damage = 1 switch,
//! otherwise damage = 0
class EPPGapMaterial : public EPPBaseMaterial
  {
  private:
    double fy;
    double gap;
    double eta;
    double maxElasticYieldStrain;
    double minElasticYieldStrain;
    int damage;
  protected:

  public:
    EPPGapMaterial(int tag, double E, double fy, double gap, double eta, int damage = 0);
    EPPGapMaterial(int tag);  
    EPPGapMaterial(void);  

    int setTrialStrain(double strain, double strainRate = 0.0); 
    double getStrain(void) const;          
    double getInitialTangent(void) const;

    int commitState(void);
    int revertToLastCommit(void);    
    int revertToStart(void);    

    UniaxialMaterial *getCopy(void) const;
    
    int sendSelf(CommParameters &);  
    int recvSelf(const CommParameters &);
    
    void Print(std::ostream &s, int flag =0);

  };
} // end of XC namespace


#endif
