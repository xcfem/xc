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
                                                                        
#ifndef NormalizedPeak_h
#define NormalizedPeak_h         

// $Revision: 1.1 $
// $Date: 2004/09/01 03:54:28 $
// $Source: /usr/local/cvs/OpenSees/SRC/damage/NormalizedPeak.h,v $

// Written: Arash Altoontash, Gregory Deierlein
// Created: 08/02
// Revision: AA
//
// Description: This file contains the class definition for NormalizedPeak
// Damage model. This is a maximum based, non hysteretic damage model. 
// It gets the maximum positive and maximum negative values as initial
// parameters and calculates the damage index based on the maximum and minimum
// values occurred.


#include <material/damage/DamageModel.h>

namespace XC {
class DamageResponse;
class Element;


class NormalizedPeak : public DamageModel
  {
  private:
    std::string damagename;
    DamageType damagetype;
  
    // Model parameters
    double MaxValue, MinValue;
  
    // Trial step
    double TrialScalar;
    double TrialDmg;
    Vector TrialVector;
  
    // Committed state
    double CommitScalar;
    double CommitDmg;
    Vector CommitVector;
  
    // Last commit
    double LCommitScalar;
    double LCommitDmg;
    Vector LCommitVector;
  public:
    NormalizedPeak(int tag, double maxVal, double minVal , const char *argv);
    NormalizedPeak(void);  
  
    int setTrial(const Vector &trialVector);
    int setTrial(void) { return -1; }
  
    double getDamage(void);
    double getPosDamage(void);
    double getNegDamage(void);
  
    int commitState(void);
    int revertToLastCommit (void);
    int revertToStart (void);
  
    DamageModel *getCopy(void) const;
  
    int setParameter(const std::vector<std::string> &argv, Information &eleInformation) { return -1; }
    int updateParameter(int responseID, Information &eleInformation) { return -1; }
  
    Response *setResponse(const std::vector<std::string> &argv, Information &eleInformation);
    int getResponse(int responseID, Information &info);
  
    int sendSelf(CommParameters &);  
    int recvSelf(const CommParameters &);
    void Print(std::ostream &s, int flag =0) const;
  };
} // end of XC namespace

#endif
