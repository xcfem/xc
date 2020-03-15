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
//DqVectors.cc

#include "DqVectors.h"
#include "Vector.h"

//! @brief Constructor.
XC::DqVectors::DqVectors(const size_t &num)
  : dq_ptr_Vectors(num,static_cast<Vector *>(nullptr)) {}

// //! @brief Copy constructor.
// XC::DqVectors::DqVectors(const DqVectors &other)
//   : dq_ptr_Vectors(other) {}

// //! @brief Assignment operator.
// XC::DqVectors &XC::DqVectors::operator=(const DqVectors &other)
//   {
//     dq_ptr_Vectors::operator=(other);
//     return *this;
//   }

void XC::DqVectors::clear(void)
  {
    iterator i= begin();
    for(;i!= end();i++)
      if(*i)
        delete (*i);
    erase(begin(),end());
  }

//! @brief Destructor:
XC::DqVectors::~DqVectors(void)
  { clear(); }

void XC::DqVectors::Print(std::ostream &s,const int &flag) const
  {
    s << "\tNumber of Vectors: " << size() << std::endl;
  }
