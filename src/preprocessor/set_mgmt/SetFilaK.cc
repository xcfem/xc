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
//SetFilaK.cc

#include "SetFilaK.h"
#include "preprocessor/multi_block_topology/entities/EntMdlr.h"
#include <boost/any.hpp>
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"

XC::SetFilaK::SetFilaK(EntMdlr &e,const size_t &f,const size_t &c,const std::string &nmb,XC::Preprocessor *preprocessor)
  : SetFila<tfilanod,tfilaelem>(e.getTtzNodes().GetVarRefFilaK(f,c),e.getTtzElements().GetVarRefFilaK(f,c),nmb,preprocessor) {}

XC::SetFilaK::SetFilaK(EntMdlr &e,const size_t &capa,const size_t &f,const RangoIndice &rango_cols,const std::string &nmb,Preprocessor *preprocessor)
  : SetFila<tfilanod,tfilaelem>(e.getTtzNodes().GetVarRefFilaK(capa,f,rango_cols),e.getTtzElements().GetVarRefFilaK(capa,f,rango_cols),nmb,preprocessor) {}
