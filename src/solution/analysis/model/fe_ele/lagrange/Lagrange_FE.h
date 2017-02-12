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

// File: ~/analysis/model/fe_ele/lagrange/Lagrange_FE.h
// 
// Written: lcpt 
// Created: 03/2015
// Revision: A
//
// Description: This file contains the class definition for Lagrange_FE.
// Lagrange_FE is a subclass of FE_Element which handles _Constraints
// using the Lagrange method.
//
// What: "@(#) Lagrange_FE.h, revA"


#ifndef Lagrange_FE_h
#define Lagrange_FE_h

namespace XC {
class DOF_Group;
class ID;

//! @ingroup AnalysisFE
//
//! @brief Lagrange_FE is a subclass of FE_Element
//! which handles _Constraints using the Lagrange method.
class Lagrange_FE
  {
  private:
    DOF_Group *theDofGroup; //!< Lagrange DOF group.

  public:
    Lagrange_FE(DOF_Group &);
    DOF_Group *getLagrangeDOFGroup(void)
      { return theDofGroup; } 
    inline const DOF_Group *getLagrangeDOFGroup(void) const
      { return theDofGroup; } 
    int getLagrangeDOFGroupTag(void) const;

  };
} // end of XC namespace

#endif


