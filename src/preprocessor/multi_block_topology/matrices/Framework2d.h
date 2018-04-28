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
//Framework2d.h

#ifndef FRAMEWORK2D_H
#define FRAMEWORK2D_H

#include "preprocessor/multi_block_topology/ModelComponentContainer.h"
#include "boost/lexical_cast.hpp"


namespace XC {
class MatrizPtrPnt;

//! @ingroup MultiBlockTopology
//
//! @brief Bidimensional framework container.
class Framework2d: public ModelComponentContainer<MatrizPtrPnt>
  {
  protected:
    MatrizPtrPnt *Crea(void);
  public:
    Framework2d(MultiBlockTopology *mbt= nullptr);

    MatrizPtrPnt *makeNew(void);
  };

} //end of XC namespace
#endif
