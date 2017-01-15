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
//UnbalAndTangentStorage.cpp


#include "UnbalAndTangentStorage.h"

XC::UnbalAndTangentStorage::UnbalAndTangentStorage(const size_t &n)
  : theMatrices(n), theVectors(n) {}

const XC::Matrix &XC::UnbalAndTangentStorage::getTangent(const size_t &i) const
  { return theMatrices[i]; }

XC::Matrix &XC::UnbalAndTangentStorage::getTangent(const size_t &i)
  { return theMatrices[i]; }

const XC::Vector &XC::UnbalAndTangentStorage::getUnbalance(const size_t &i) const
  { return theVectors[i]; }

XC::Vector &XC::UnbalAndTangentStorage::getUnbalance(const size_t &i)
  { return theVectors[i]; }

XC::Vector *XC::UnbalAndTangentStorage::setUnbalance(const size_t &i)
  {
    if(theVectors[i].Nulo())
      { theVectors[i]= Vector(i); }
    return &theVectors[i];
  }

XC::Matrix *XC::UnbalAndTangentStorage::setTangent(const size_t &i)
  {
    if(theMatrices[i].Nula())
      { theMatrices[i]= Matrix(i,i); }
    return &theMatrices[i];
  }




