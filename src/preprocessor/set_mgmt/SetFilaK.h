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
//SetFilaK.h

#ifndef SETFILAK_H
#define SETFILAK_H

#include "SetFila.h"
#include "preprocessor/cad/matrices/TritrizPtrNod.h"
#include "preprocessor/cad/matrices/TritrizPtrElem.h"

class RangoIndice;

namespace XC {

class EntMdlr;

//!  @ingroup Set
//! 
//!  @brief Set of objects in a row.
//! 
//!  A SetFilaK object contains zero o more:
//!  - Nodes.
//!  - Finite elements.
//!  that correspond to a fila_k of an EntMdlr object.
class SetFilaK: public SetFila<TritrizPtrNod::var_ref_fila_k,TritrizPtrElem::var_ref_fila_k>
  {
  public:
    typedef TritrizPtrNod::var_ref_fila_k tfilanod;
    typedef TritrizPtrElem::var_ref_fila_k tfilaelem;
    SetFilaK(EntMdlr &e,const size_t &f=1,const size_t &c=1,const std::string &nmb="",Preprocessor *preprocessor= nullptr);
    SetFilaK(EntMdlr &e,const size_t &capa,const size_t &f,const RangoIndice &,const std::string &nmb="",Preprocessor *preprocessor= nullptr);
  };
} //end of XC namespace
#endif
