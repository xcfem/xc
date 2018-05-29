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
// $Date: 2003/02/25 23:33:33 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/Bidirectional.h,v $
                                                                        
                                                                        
#ifndef Bidirectional_h
#define Bidirectional_h

// File: ~/material/Bidirectional.h
//
// Written: MHS
// Created: Feb 2000
// Revision: A
//
// Description: 
//
// What: "@(#) Bidirectional.h, revA"

#include <material/section/SectionForceDeformation.h>

namespace XC {
//! @ingroup MATSCC
//
//! @brief ??
class Bidirectional: public SectionForceDeformation
  {
  private:
    double E;
    double sigY;
    double Hiso;
    double Hkin;

    double e_n1Trial[2]; //!< Trial strain.
    double e_n1Inic[2]; //!< Initial strain.
    double eP_n[2];
    mutable double eP_n1[2];

    double q_n[2];
    mutable double q_n1[2];

    double alpha_n;
    mutable double alpha_n1;
	
    static Vector s;
    static Matrix ks;
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    Bidirectional(int tag, double E, double sigY, double Hiso, double Hkin);
    Bidirectional(int tag= 0);

    int setInitialSectionDeformation(const Vector &v);
    int setTrialSectionDeformation(const Vector &v);
    const Matrix &getSectionTangent(void) const;
    const Matrix &getInitialTangent(void) const;
    const Vector &getStressResultant(void) const;
    void zeroInitialSectionDeformation(void);
    const Vector &getInitialSectionDeformation(void) const;
    const Vector &getSectionDeformation(void) const;
    virtual double getStrain(const double &,const double &) const;

    int commitState(void);
    int revertToLastCommit(void);
    int revertToStart(void);
    
    SectionForceDeformation *getCopy(void) const;
    const ResponseId &getType(void) const;
    int getOrder(void) const;
    
    int sendSelf(CommParameters &);  
    int recvSelf(const CommParameters &);

    void Print(std::ostream &s, int flag = 0);
  };
} // end of XC namespace


#endif
