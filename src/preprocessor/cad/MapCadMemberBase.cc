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
//MapCadMemberBase.cc

#include "MapCadMemberBase.h"

#include "preprocessor/cad/Cad.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"

//! @brief Constructor.
XC::MapCadMemberBase::MapCadMemberBase(Cad *cad)
  : EntCmd(cad), tag(0) {}

//! @brief Devuelve un apuntador al objeto Cad.
const XC::Cad *XC::MapCadMemberBase::getCad(void) const
  {
    const Cad *retval= dynamic_cast<const Cad *>(Owner());
    return retval;
  }

//! @brief Devuelve un apuntador al objeto Cad.
XC::Cad *XC::MapCadMemberBase::getCad(void)
  { 
    Cad *retval= const_cast<Cad *>(dynamic_cast<const Cad *>(Owner()));
    return retval;
  }

//! @brief Devuelve un apuntador al preprocesador.
const XC::Preprocessor *XC::MapCadMemberBase::getPreprocessor(void) const
  { return getCad()->getPreprocessor(); }

//! @brief Devuelve un apuntador al preprocesador.
XC::Preprocessor *XC::MapCadMemberBase::getPreprocessor(void)
  { return getCad()->getPreprocessor(); }

