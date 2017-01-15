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
//EqualDOF.h

// Este objeto construye un objeto MFreedom_Constraint que impone
// igualdad de movimientos en los grados de libertad que
// se le indiquen.

#ifndef EqualDOF_h
#define EqualDOF_h

#include "MFreedom_Constraint.h"

namespace XC {
class Domain;
class ID;

//! @ingroup CContMP
//
//! @brief Impone la igualdad de desplazamientos a varios nodos.
class EqualDOF: public MFreedom_Constraint
  {
    void setup_matrix(void);
  public:
    EqualDOF(int tag);
    EqualDOF(int tag,const int &,const int &,const ID &);

    void setDofs(const ID &);
    void setup(Domain *theDomain);
  };
} // end of XC namespace

#endif
