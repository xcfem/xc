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
//MEDFieldInfo.h

#ifndef MEDFIELDINFO_H
#define MEDFIELDINFO_H

#include "MEDGroupInfo.h"
#include "post_process/FieldInfo.h"

namespace XC {

class MEDGaussModel;
class GaussModel;

//! @ingroup MED
//
//! @brief Information about a field defined over a mesh subset.
class MEDFieldInfo: public MEDObject
  {
    const FieldInfo &field_info;
  protected:
    friend class MEDMeshing;
    MEDFieldInfo(const FieldInfo &,MEDGroupInfo *);

    const MEDGroupInfo &getGrupo(void) const;
    MEDMEM::GROUP *getGrupoMED(void) const;

  public:
    inline virtual ~MEDFieldInfo(void)
      {}
    inline const FieldInfo &getXCFieldInfo(void) const
      { return field_info; }
    inline const std::string *getComponentNamesPtr(void) const
      { return &(field_info.getComponentNames()[0]); }
    inline const std::string *getComponentDescriptionsPtr(void) const
      { return &(field_info.getComponentDescriptions()[0]); }
    inline const std::string *getComponentUnitsPtr(void) const
      { return &(field_info.getComponentUnits()[0]); }

    virtual void defineGaussModels(const Set &)= 0;

    virtual void write(const std::string &) const= 0;
  };
} // end of XC namespace
#endif
