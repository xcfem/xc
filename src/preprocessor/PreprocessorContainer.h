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
//PreprocessorContainer.h

#ifndef PREPROCESSORCONTAINER_H
#define PREPROCESSORCONTAINER_H

#include "utility/kernel/CommandEntity.h"

namespace XC {
class Domain;
class Preprocessor;

//! @ingroup Preprocessor
//
//! @brief Base class for preprocessor containers i. e.
//! objects that manage model entities: sets, geometric entities,
//! mesh entities and so on.
class PreprocessorContainer: public CommandEntity
  {
    friend class Preprocessor;
    Preprocessor *preprocessor;
  public:
    PreprocessorContainer(Preprocessor *owr);
    const Domain *getDomain(void) const;
    Domain *getDomain(void);

    //! @brief Returns a pointer to the object preprocessor.
    inline Preprocessor *getPreprocessor(void)
      { return preprocessor; }
    //! @brief Returns a pointer to the object preprocessor.
    inline const Preprocessor *getPreprocessor(void) const
      { return preprocessor; }
    void checkPreprocessor(void);
  };

} // end of XC namespace

#endif
