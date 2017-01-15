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
//DqMatrices.cc

#include "DqMatrices.h"
#include "Matrix.h"

//! @brief Constructor.
XC::DqMatrices::DqMatrices(const size_t &num)
  : dq_ptr_Matrices(num,static_cast<Matrix *>(nullptr)) {}

// //! @brief Constructor de copia.
// XC::DqMatrices::DqMatrices(const DqMatrices &otro)
//   : dq_ptr_Matrices(otro) {}

// //! @brief Operador asignación.
// XC::DqMatrices &XC::DqMatrices::operator=(const DqMatrices &otro)
//   {
//     dq_ptr_Matrices::operator=(otro);
//     return *this;
//   }

void XC::DqMatrices::clear(void)
  {
    iterator i= begin();
    for(;i!= end();i++)
      if(*i)
        delete (*i);
    erase(begin(),end());
  }

//! @brief Destructor:
XC::DqMatrices::~DqMatrices(void)
  { clear(); }

void XC::DqMatrices::Print(std::ostream &s,const int &flag)
  {
    s << "\tNumber of Matrices: " << size() << std::endl;
  }
