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
//Loader.h

#ifndef LOADER_H
#define LOADER_H

#include "preprocessor/PreprocessorContainer.h"
#include "utility/actor/actor/MovableObject.h"
#include <map>

namespace XC {

//! @ingroup Preprocessor
//
//! @defgroup Ldrs Utilities for model definition.
//
//! \ingroup Ldrs
//! 
//! @brief Base class for the preprocessor objects that
//! create model entities: nodes, elements, loads, etc.
class Loader: public PreprocessorContainer, public MovableObject
  {
  public:
    Loader(Preprocessor *owr);

    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);
  };

} // end of XC namespace

#endif
