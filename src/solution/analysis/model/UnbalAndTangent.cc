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
//UnbalAndTangent.cpp

#include "UnbalAndTangent.h"


void XC::UnbalAndTangent::alloc(void)
  {
    unbalAndTangentArray->alloc(nDOF);
  }

//! @brief Constructor.
XC::UnbalAndTangent::UnbalAndTangent(const size_t &n,UnbalAndTangentStorage &a)
  :nDOF(n), unbalAndTangentArray(&a) 
  { alloc(); }

//! @brief destructor.
XC::UnbalAndTangent::~UnbalAndTangent(void)
  {
    unbalAndTangentArray= nullptr;
  }

//! @brief Return the tangent stiffness matrix.
const XC::Matrix &XC::UnbalAndTangent::getTangent(void) const
  {
    return unbalAndTangentArray->getTangent(this->nDOF);
  }

//! @brief Return the tangent stiffness matrix.
XC::Matrix &XC::UnbalAndTangent::getTangent(void)
  {
    return unbalAndTangentArray->getTangent(this->nDOF);
  }

//! @brief Returns the residual vector.
const XC::Vector &XC::UnbalAndTangent::getResidual(void) const
  {
    return unbalAndTangentArray->getUnbalance(this->nDOF);
  }

//! @brief Return the residual vector.
XC::Vector &XC::UnbalAndTangent::getResidual(void)
  {
    return unbalAndTangentArray->getUnbalance(this->nDOF);
  }
