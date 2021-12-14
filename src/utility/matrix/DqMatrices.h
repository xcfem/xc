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
//DqMatrices.h

#ifndef DqMatrices_h
#define DqMatrices_h

#include <deque>
#include <iostream>


namespace XC {

class Matrix;

//! @ingroup Matrix
//
//! @brief Dynamically allocated matrices deque container.
class DqMatrices: public std::deque<Matrix *>
  {
  private:
    DqMatrices(const DqMatrices &);
    DqMatrices &operator=(const DqMatrices &);
  protected:
    typedef std::deque<Matrix *> dq_ptr_Matrices;
    typedef dq_ptr_Matrices::const_reference const_reference;
    typedef dq_ptr_Matrices::reference reference;
    typedef dq_ptr_Matrices::iterator iterator;
    typedef dq_ptr_Matrices::const_iterator const_iterator;


  public:
    DqMatrices(const size_t &num= 0);
    ~DqMatrices(void);

    void clear(void);

    void Print(std::ostream &s,const int &flag) const;
  };
} // end of XC namespace

#endif
