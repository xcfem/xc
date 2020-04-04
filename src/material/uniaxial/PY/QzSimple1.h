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
/* *********************************************************************
**    Module:	QzSimple1.h 
**
**    Purpose:	Provide a simple Q-z material for OpenSees
**
**
**    Developed by Ross W. Boulanger
**    (C) Copyright 2002, All Rights Reserved.
**
** ****************************************************************** */

// $Revision: 1.0
// $Date: 2002/1/22
// $Source: /OpenSees/SRC/material/uniaxial/QzSimple1.h

#ifndef QZSIMPLE1_H
#define QZSIMPLE1_H

// Written: RWB
// Created: Jan 2002
// tested and checked: Boris Jeremic (jeremic@ucdavis.edu) Spring 2002
//
// Description: This file contains the class definition for QzSimple1.
// 

#include "PQyzBase.h"


namespace XC {
//! @ingroup PYMat
//
//! @brief Simple uniaxial q-z material for use with a zeroLength
//! element.
class QzSimple1: public PQyzBase
  {
  private:
    // Material parameters
    double suction; //!< ratio of max suction force to Qult
    double maxElast; //!< max size of elastic range (in terms of dQ/Qult)

    InternalParamsIn CSuction; //!< Committed internal parameters for the Suction component
    InternalParamsIn TSuction; //!< Trial internal parameters for the Suction component

    InternalParamsA CClose; //!< Committed internal parameters for the Closure component
    InternalParamsA TClose; //!< Trial internal parameters for the Closure component

    // Functions to get Q & z for each component individually
    void getGap(double zlast, double dz, double dz_old);
    void getClosure(double zlast, double dz);
    void getSuction(double zlast, double zy);
    void getNearField(double zlast, double dz, double dz_old);
  protected:
    int sendData(Communicator &);
    int recvData(const Communicator &);

  public:
    QzSimple1(int tag, int qzType, double Qult, double z50, double suction,double dashpot);
    QzSimple1(int tag, int classtag= MAT_TAG_QzSimple1);
    QzSimple1(void);

    void set_suction(const double &);
    double get_suction(void) const;
    
    int setTrialStrain(double z, double zRate); 
    double getStress(void) const;
    double getDampTangent(void) const;

    int commitState(void);
    int revertToLastCommit(void);    
    int revertToStart(void);        

    UniaxialMaterial *getCopy(void) const;
    
    int sendSelf(Communicator &);  
    int recvSelf(const Communicator &);
    
    void Print(std::ostream &s, int flag =0) const;

   
    

  };
} // end of XC namespace

#endif
