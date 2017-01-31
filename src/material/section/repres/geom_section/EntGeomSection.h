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
//EntGeomSection.h

#ifndef ENTGEOMSECTION_H
#define ENTGEOMSECTION_H

#include "xc_utils/src/nucleo/EntConNmb.h"

namespace XC {
class GeomSection;
class SisRefScc;
class Spot;
class Eje;

//! @ingroup MATSCCRepresGeom
//
//! @defgroup MATSCCEntGeom Geometric entities.
//
//! @ingroup MATSCCEntGeom
//
//! @brief Base class of section geometry representation classes.
class EntGeomSection: public EntConNmb
  {
  private:
    GeomSection *sccGeom; //!< GeomSection.
  protected:
    const GeomSection *getGeomSection(void) const;
    GeomSection *getGeomSection(void);

  public:
    EntGeomSection(const std::string &nmb="",GeomSection *sccGeom= NULL);
    EntGeomSection(const EntGeomSection &otro);
    EntGeomSection &operator=(const EntGeomSection &otro);
    inline const GeomSection *GetGeomSection(void) const
      { return getGeomSection(); }
    inline GeomSection *GetGeomSection(void)
      { return getGeomSection(); }
    int getVtkCellType(void) const;
    virtual int getMEDCellType(void) const;


    SisRefScc *SisRefSccActual(void);    
    const SisRefScc *SisRefSccActual(void) const;

    Spot *BuscaSpot(const size_t &id_punto);    
    const Spot *BuscaSpot(const size_t &id_punto) const;
    Eje *BuscaEje(const size_t &id_edge);    
    const Eje *BuscaEje(const size_t &id_edge) const;
  };

} //end of XC namespace
#endif
