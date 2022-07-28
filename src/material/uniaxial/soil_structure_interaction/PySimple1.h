// -*-c++-*-
//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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
**    Module:        PySimple1.h
**
**    Purpose:        Provide a simple p-y spring for OpenSees
**
**
**    Developed by Ross W. Boulanger
**    (C) Copyright 2001, All Rights Reserved.
**
** ****************************************************************** */

// $Revision: 1.0
// $Date: 2001/10/15
// $Source: /OpenSees/SRC/material/uniaxial/PySimple1.h

#ifndef PYSIMPLE1_H
#define PYSIMPLE1_H

// Written: RWB
// Created: Oct 2001
// tested and checked: Boris Jeremic (jeremic@ucdavis.edu) Spring 2002
//
// Description: This file contains the class definition for PySimple1.
//

#include "PQyzBase.h"

namespace XC {
//! @ingroup PYMat
//
//! @brief Simple uniaxial p-y material for use with a zeroLength
//! element.
class PySimple1: public PQyzBase
  {
  protected:
    // Material parameters
    double drag; //!< ratio of max gap drag force to spring capacity
  private:
    InternalParamsIn CDrag; //!< Committed internal parameters for the Drag component
    InternalParamsIn TDrag; //!< Trial internal parameters for the Drag component

    InternalParamsLR CClose; //!< Committed internal parameters for the Closure component
    InternalParamsLR TClose; //!< Trial internal parameters for the Closure component

    // Functions to get p & y for each component individually
    void getGap(double ylast, double dy, double dy_old);
    void getClosure(double ylast, double dy);
    void getDrag(double ylast, double dy);
    void getNearField(double ylast, double dy, double dy_old);
    
  protected:
    int sendData(Communicator &);
    int recvData(const Communicator &);

  public:
    PySimple1(int tag, int classtag, int soilType, double pult, double y50,double drag, double dashpot);
    PySimple1(int tag, int classtag= MAT_TAG_PySimple1);
    PySimple1(void);

    int setTrialStrain(double y, double yRate);
    double getStress(void) const;
    double getDampTangent(void) const;
    void setDragResistanceFactor(const double &);
    double getDragResistanceFactor(void) const;

    int commitState(void);
    int revertToLastCommit(void);
    int revertToStart(void);

    UniaxialMaterial *getCopy(void) const;

    void initialize(void);

    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);

    void Print(std::ostream &s, int flag =0) const;

  };
} // end of XC namespace


#endif
