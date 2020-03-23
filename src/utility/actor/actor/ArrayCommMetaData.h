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
                                                                        
                                                                        
#ifndef ArrayCommMetaData_h
#define ArrayCommMetaData_h

#include "PtrCommMetaData.h"
#include <cstddef>

namespace XC {

//! @ingroup IPComm
//
//! @brief Data about the position, size, ... of the
//! array to transmit.
class ArrayCommMetaData: public PtrCommMetaData
  {
  protected:
    std::size_t posSz; //!< Index of the array size.
  public:
    ArrayCommMetaData(const int &,const int &,const size_t &);

    //! @brief Return the index of the array size.
    inline const std::size_t &getPosSize(void) const
      { return posSz; }

    //! @brief Returns the array size.
    inline const std::size_t &getSize(void) const
      { return posSz; }
  };

} // end of XC namespace

#endif
