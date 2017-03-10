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
//MapCadMemberBase.h

#ifndef MAPCADMEMBERBASE_H
#define MAPCADMEMBERBASE_H

#include "xc_utils/src/nucleo/EntCmd.h"

namespace XC {

class Preprocessor;
class Cad;

//! @ingroup Cad
//
//! @brief Base para los contenedores de entidades del modelo.
class MapCadMemberBase: public EntCmd
  {
  public:
    typedef size_t Indice;

  protected:
    Indice tag; //! Default identifier for new point.



  public:
    MapCadMemberBase(Cad *cad= nullptr);

    const Cad *getCad(void) const;
    Cad *getCad(void);
    const Preprocessor *getPreprocessor(void) const;
    Preprocessor *getPreprocessor(void);

    inline size_t getTag(void) const
      { return tag; }
    void setTag(const size_t &t)
      { tag= t; }


  };

} //end of XC namespace
#endif
