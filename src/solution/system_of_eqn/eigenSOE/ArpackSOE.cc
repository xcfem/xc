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
// File: ~/system_of_eqn/eigenSOE/ArpackSOE.C
//
// Written: Jun Peng 
// Created: Febuary 1999
// Revision: A
//
// Description: This file contains the class definition for XC::ArpackSOE
// ArpackSOE is a subclass of XC::EigenSOE. It uses the LAPACK storage
// scheme to store the components of the K, M matrix, which is a full matrix.
// It uses the ARPACK to do eigenvalue analysis.

#include <solution/system_of_eqn/eigenSOE/ArpackSOE.h>


//! @brief Constructor.
XC::ArpackSOE::ArpackSOE(SoluMethod *owr,int classTag, double theShift)
  :EigenSOE(owr,classTag), shift(theShift) {}


//! @brief Returns the valor del parámetro shift.
const double &XC::ArpackSOE::getShift(void) const
  { return shift; }

void XC::ArpackSOE::setShift(const double &s)
  { shift= s; }
