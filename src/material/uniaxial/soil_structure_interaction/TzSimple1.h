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
**    Module:        TzSimple1.h 
**
**    Purpose:        Provide a simple t-z spring for OpenSees
** 
**    Developed by Ross W. Boulanger
**    (C) Copyright 2002, All Rights Reserved.
**
** ****************************************************************** */

// $Revision: 1.0
// $Date: 2002/1/19
// $Source: /OpenSees/SRC/material/uniaxial/TzSimple1.h

#ifndef TZSIMPLE1_H
#define TZSIMPLE1_H

// Written: RWB
// Created: Jan 2002
// tested and checked: Boris Jeremic (jeremic@ucdavis.edu) Spring 2002
//
// Description: This file contains the class definition for TzSimple1.
// 

#include "PYBase.h"

namespace XC {
//! @ingroup PYMat
//
//! @brief Simple uniaxial t-z material for use with a zeroLength element.
class TzSimple1: public PYBase
  {
  private:
    InternalParamsIn CNF; //!< Committed internal parameters for the NearField plastic component
    InternalParamsIn TNF; //!< Trial internal parameters for the NearField plastic component

    // Functions to get t & z for elastic & plastic components
    void getNearField(double zlast, double dz, double dz_old);
  protected:
    int sendData(Communicator &);
    int recvData(const Communicator &);

    
  public:
    TzSimple1(void);
    TzSimple1(int tag, int classtag= MAT_TAG_TzSimple1);
    TzSimple1(int tag, int classtag, int tzType, double tult, double z50, double dashpot);
    UniaxialMaterial *getCopy(void) const;

    int setTrialStrain(double y, double yRate); 
    double getStress(void) const;
    double getDampTangent(void) const;

    //! @brief Set t-z backbone relation (= 1 for Reese & O’Neill relation;
    //! = 2 for Mosher relation.
    inline void setTzType(const int &i)
      { PYBase::setSoilType(i); }
    //! @brief Get t-z backbone relation (= 1 for Reese & O’Neill relation;
    //! = 2 for Mosher relation.
    inline int getTzType(void) const
      { return PYBase::getSoilType(); }
    //! @brief Set the displacement at which 50% of tult is mobilized in
    //! monotonic loading. 
    inline void setZ50(const double &d)
      { PYBase::setY50(d); }
    //! @brief Get the displacement at which 50% of tult is mobilized in
    //! monotonic loading. 
    inline double getZ50(void) const
      { return PYBase::getY50(); }
    
    int commitState(void);
    int revertToLastCommit(void);    
    int revertToStart(void);        

    void initialize();
    
    int sendSelf(Communicator &);  
    int recvSelf(const Communicator &);
    
    void Print(std::ostream &s, int flag =0) const;

   
  };
} // end of XC namespace


#endif
