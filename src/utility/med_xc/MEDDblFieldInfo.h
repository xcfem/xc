//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
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
//MEDDblFieldInfo.h
//Información sobre las celdas de la malla (para exportar archivos a «salome»).

#ifndef MEDDBLFIELDINFO_H
#define MEDDBLFIELDINFO_H

#include "MEDTFieldInfo.h"
#include "xc_basic/src/med_xc/MEDMEM_ArrayInterface.hxx"

namespace XC {
class Set;
class MEDGroupInfo;

//! @ingroup MED
//
//! @brief Información sobre un campo definido sobre un
//! subconjunto de la malla.
class MEDDblFieldInfo: public MEDTFieldInfo<double>
  {
  protected:
    friend class MEDMeshing;
    MEDDblFieldInfo(const FieldInfo &,MEDGroupInfo *);


    void populateOnNodes(const Set &,const FieldInfo &);
    void populateOnElements(const Set &,const FieldInfo &);
    void populateOnGaussPoints(const Set &,const FieldInfo &);
  };
} // end of XC namespace
#endif
