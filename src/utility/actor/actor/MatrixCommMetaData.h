// -*-c++-*-
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
                                                                        
                                                                        
#ifndef MatrixCommMetaData_h
#define MatrixCommMetaData_h

#include "ArrayCommMetaData.h"

namespace XC {

//! @ingroup IPComm
//
//! @brief Data about the position, size,... of 
//! the object to transmit.
class MatrixCommMetaData: public ArrayCommMetaData
  {
  private:
    std::size_t posNc; //!< Number of matrix columns.
  public:
    MatrixCommMetaData(const int &pos,const int &flag,const size_t &nf,const size_t &nc);

    //! @brief Return a const reference to the the index of the row number.
    inline const size_t &getPositionNumberOfRows(void) const
      { return posSz; }
    //! @brief Return a const reference to the the index of the column number.
    inline const size_t &getPositionNumberOfColumns(void) const
      { return posNc; }
    //! @brief Return a reference to the the index of the row number.
    inline const size_t &getNumberOfRows(void) const
      { return posSz; }
    //! @brief Return a reference to the the index of the column number.
    inline const size_t &getNumberOfColumns(void) const
      { return posNc; }
  };

} // end of XC namespace

#endif
