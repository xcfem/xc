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
//UnbalAndTangentStorage.cpp


#include "UnbalAndTangentStorage.h"

XC::UnbalAndTangentStorage::UnbalAndTangentStorage(const size_t &n)
  : theMatrices(n), theVectors(n) {}

const XC::Matrix &XC::UnbalAndTangentStorage::getTangent(const size_t &i) const
  {
    if(i<theMatrices.size())
      return theMatrices[i];
    else
      return theMatrixMap.at(i);
  }

XC::Matrix &XC::UnbalAndTangentStorage::getTangent(const size_t &i)
  {
    if(i<theMatrices.size())
      return theMatrices[i];
    else
      return theMatrixMap.at(i);
  }

const XC::Vector &XC::UnbalAndTangentStorage::getUnbalance(const size_t &i) const
  {
    if(i<theVectors.size())
      return theVectors[i];
    else
      return theVectorMap.at(i);
  }

XC::Vector &XC::UnbalAndTangentStorage::getUnbalance(const size_t &i)
  {
    if(i<theVectors.size())
      return theVectors[i];
    else
      return theVectorMap.at(i);
  }

//! @brief Initializes the i-th unbalance vector.
void XC::UnbalAndTangentStorage::setUnbalance(const size_t &i)
  {
    if(i>=theVectors.size())
      { theVectorMap[i]= Vector(i); }
    else
      {
        if(theVectors[i].isEmpty())
          { theVectors[i]= Vector(i); }
      }
  }

//! @brief Initializes the i-th tangent matrix.
void XC::UnbalAndTangentStorage::setTangent(const size_t &i)
  {
    if(i>=theMatrices.size())
      {	theMatrixMap[i]= Matrix(i, i); }
    else
      {
        if(theMatrices[i].isEmpty())
          { theMatrices[i]= Matrix(i,i); }
      }
  }

//! @brief Initializes the i-th tangent matrix and unbalance vector.
void XC::UnbalAndTangentStorage::alloc(const size_t &i)
  {
    this->setUnbalance(i);
    this->setTangent(i);    
  }



