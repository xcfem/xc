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
//Aux_Mallado.h
                                                                        
#ifndef Aux_Mallado_h
#define Aux_Mallado_h

#include "preprocessor/ParamMallado.h"

namespace XC {

class Element;
class TritrizPtrNod;
class TritrizPtrElem;

//void malla_quad4N_bidimensional(const Element &e,const TritrizPtrNod &nodos,TritrizPtrElem &elementos);
TritrizPtrElem coloca_quad4N_en_malla(const Element &e,const TritrizPtrNod &,dir_mallado dm);
TritrizPtrElem coloca_quad9N_en_malla(const Element &e,const TritrizPtrNod &,dir_mallado dm);

} // end of XC namespace
#endif
