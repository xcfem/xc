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
// $Date: 2006/01/17 20:44:32 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/Isolator2spring.h,v $
                                                                        
                                                                        
// Written: K. Ryan
// Created: September 2003
// Updates: November 2005
//
// Description: This file contains the class implementation for a "two-spring isolator" 
// material.

#ifndef Isolator2spring_h
#define Isolator2spring_h
#include <material/section/SectionForceDeformation.h>

#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>

namespace XC {
//! @ingroup MATSCC
//
//! @brief "two-spring isolator" material.

//! This material is based on the two-spring model originally developed
//! by Koh and Kelly to represent the buckling behavior of an elastomeric
//! bearing. The material model has been modified to include material
//! nonlinearity and optional strength degradation.
class Isolator2spring: public SectionForceDeformation
  {
  private:
    double tol;
    double k1;
    double Fyo;
    double kbo;
    double kvo;
    double h;
    double Pe;
    double po;

    double utptTrial[2]; //!< Trial strains.
    double utptInic[2]; //!< Initial strains.
    double sP_n;
    mutable double sP_n1;

    double q_n;
    mutable double q_n1;

    double H;
    double pcr;

    mutable Vector x0;
    mutable Matrix ks;
    static Vector f0;
    static Matrix df;
    static Vector s;
    static Vector s3;
  protected:
    int sendData(Communicator &);
    int recvData(const Communicator &);
  public:
    Isolator2spring(int tag, double tol_in, double k1_in, double Fy_in, double kb_in, double kvo_in, double hb_in, double Pe_in, double po_in);
    Isolator2spring(int tag);
    Isolator2spring(void);

    int setInitialSectionDeformation(const Vector &v); 
    int setTrialSectionDeformation(const Vector &v);
    const Matrix &getSectionTangent(void) const;
    const Matrix &getInitialTangent(void) const;
    const Vector &getStressResultant(void) const;
    void zeroInitialSectionDeformation(void);
    const Vector &getInitialSectionDeformation(void) const;
    const Vector &getSectionDeformation(void) const;
    double getStrain(const double &,const double &) const;

    int commitState(void);
    int revertToLastCommit(void);
    int revertToStart(void);
    
    SectionForceDeformation *getCopy(void) const;
    const ResponseId &getType(void) const;
    int getOrder(void) const;
    
    int sendSelf(Communicator &);  
    int recvSelf(const Communicator &);

    void Print(std::ostream &s, int flag = 0) const;
  };
} // end of XC namespace

#endif
