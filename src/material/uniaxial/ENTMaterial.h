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
                                                                        
// $Revision: 1.4 $
// $Date: 2003/02/25 23:33:38 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/ENTMaterial.h,v $
                                                                        
                                                                        
#ifndef ENTMaterial_h
#define ENTMaterial_h

// File: ~/material/ENTMaterial.h
//
// Written: fmk 
// Created: 07/98
// Revision: A
//
// What: "@(#) ENTMaterial.h, revA"

#include <material/uniaxial/ElasticBaseMaterial.h>

namespace XC {
//! @ingroup MatUnx
//
//! @brief Elastic no traction material. ENTMaterial
//! provides the abstraction of an elastic uniaxial
//! material under compression i.e. stress = E*strain
//! under tension however it exhbits the following
//!     stress = a*(tanh(strain*b))
//!     tangent = a*(1-tanh(strain*b)*tanh(strain*b));
class ENTMaterial : public ElasticBaseMaterial
  {
    double a;
    double b;
    int parameterID;
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);


  public:
    ENTMaterial(int tag= 0, const double &E=0.0,const double &a=0.0,const double &b=1.0);    

    int setTrialStrain(double strain, double strainRate = 0.0); 
    double getStress(void) const;
    double getTangent(void) const;

    int commitState(void);
    int revertToLastCommit(void);    
    int revertToStart(void);        

    UniaxialMaterial *getCopy(void) const;
        
    int setParameter(const std::vector<std::string> &argv, Parameter &param);
    int updateParameter(int parameterID, Information &info);
    // AddingSensitivity:BEGIN //////////////////////////////////////////
    int activateParameter(int parameterID);
    double getStressSensitivity(int gradIndex, bool conditional);
    double getInitialTangentSensitivity(int gradIndex);
    int commitSensitivity(double strainGradient, int gradIndex, int numGrads);
    // AddingSensitivity:END ///////////////////////////////////////////

    int sendSelf(CommParameters &);  
    int recvSelf(const CommParameters &);


    void Print(std::ostream &s, int flag =0) const;
  };
} // end of XC namespace


#endif

