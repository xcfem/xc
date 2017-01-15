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
//MapEsquemas2d.cc

#include "MapEsquemas2d.h"
#include "MatrizPtrPnt.h"
#include "preprocessor/Preprocessor.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"

#include "preprocessor/cad/entidades/Pnt.h"
#include "preprocessor/set_mgmt/Set.h"



//! @brief Constructor.
XC::MapEsquemas2d::MapEsquemas2d(Cad *cad)
  : MapCadMember<MatrizPtrPnt>(cad) {}

//! @brief Crea un nuevo punto.
XC::MatrizPtrPnt *XC::MapEsquemas2d::Crea(void)
  {
    MatrizPtrPnt *retval= new MatrizPtrPnt();
    retval->set_owner(this);
    (*this)[getTag()]= retval;
    //UpdateSets(retval);
    tag++;
    return retval;
  }

//! @brief Crea un nuevo esquema bidimensional.
XC::MatrizPtrPnt *XC::MapEsquemas2d::makeNew(void)
  {
    MatrizPtrPnt *retval= busca(getTag());
    if(!retval) //El esquema es nuevo.
      retval= Crea();
    return retval;
  }
