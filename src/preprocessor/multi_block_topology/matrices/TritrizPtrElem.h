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
//TritrizPtrElem.h
//Pointers to element in an array of matrices

#ifndef TRITRIZPTRELEM_H
#define TRITRIZPTRELEM_H

#include "xc_utils/src/nucleo/EntCmd.h"
#include "MatrizPtrElem.h"
#include "TritrizPtrBase.h"


namespace XC{

//! @ingroup MultiBlockTopologyMR
//! 
//! @brief "Tritriz" of pointers to elements.
class TritrizPtrElem: public TritrizPtrBase<MatrizPtrElem>
  {
  protected:

  public:

    TritrizPtrElem(const size_t n_layers= 0,const MatrizPtrElem &m= MatrizPtrElem());
    TritrizPtrElem(const size_t ,const size_t ,const size_t );

    Element *findElement(const int &);
    const Element *findElement(const int &) const;
    Element *getNearestElement(const Pos3d &p);
    const Element *getNearestElement(const Pos3d &p) const;


  };

} //end of XC namespace.

#endif
