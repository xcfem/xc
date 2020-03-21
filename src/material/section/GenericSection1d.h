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
// $Date: 2003/02/14 23:01:34 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/GenericSection1d.h,v $
                                                                        
                                                                        
#ifndef GenericSection1d_h
#define GenericSection1d_h

// File: ~/material/GenericSection1d.h
//
// Written: MHS
// Created: Apr 2000
// Revision: A
//
// Description: This file contains the class definition for GenericSection1d.
//
// What: "@(#) GenericSection1d.h, revA"

#include <material/section/PrismaticBarCrossSection.h>
#include <utility/matrix/Matrix.h>

namespace XC {
class Information;
class UniaxialMaterial;

//! @ingroup MATSCC
//
//! @brief Wrapper for an uniaxial material to behave like a section material.
//!
//! Provides a wrapper around a UniaxialMaterial
//! so that any UniaxialMaterial may be used to model section response. 
//! The design of this class follows the Object Adapter pattern in 
//! "Design Patterns" by Gamma et al.
class GenericSection1d: public PrismaticBarCrossSection
  {
    //XXX Add copy constructor and assignment operator.
  protected:
    UniaxialMaterial *theModel;//!< Material model.
    int code;

    static Vector s;
    static Matrix ks;
    static ResponseId c;

    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    GenericSection1d(int tag, UniaxialMaterial& m, int code);
    GenericSection1d(int tag);
    GenericSection1d(void);
    ~GenericSection1d(void);

    int setInitialSectionDeformation (const Vector&);
    int setTrialSectionDeformation (const Vector&);
    void zeroInitialSectionDeformation(void);
    const Vector &getInitialSectionDeformation(void) const;
    const Vector &getSectionDeformation(void) const;

    const Vector &getStressResultant(void) const;
    const Matrix &getSectionTangent(void) const;
    const Matrix &getInitialTangent(void) const;
    const Matrix &getSectionFlexibility(void) const;
    const Matrix &getInitialFlexibility(void) const;
    
    int commitState(void);
    int revertToLastCommit(void);
    int revertToStart(void);
    
    SectionForceDeformation *getCopy(void) const;
    const ResponseId &getType(void) const;
    int getOrder(void) const;
    
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    
    void Print(std::ostream &s, int flag = 0) const;
  };
} // end of XC namespace


#endif
