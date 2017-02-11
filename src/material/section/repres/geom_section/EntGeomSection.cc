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
//EntGeomSection.cc

#include "EntGeomSection.h"
#include "GeomSection.h"
#include "Spot.h"
#include "Eje.h"

#include "med.h"
#include "vtkCellType.h"

//! @brief Devuelve un puntero al GeomSection.
const XC::GeomSection *XC::EntGeomSection::getGeomSection(void) const
  { return sccGeom; }
//! @brief Devuelve un puntero al GeomSection.
XC::GeomSection *XC::EntGeomSection::getGeomSection(void)
  { return sccGeom; }

//! @brief Constructor.
XC::EntGeomSection::EntGeomSection(const std::string &nmb,GeomSection *s)
  : EntConNmb(nmb),sccGeom(s) {}

//! @brief Constructor de copia.
XC::EntGeomSection::EntGeomSection(const EntGeomSection &otro)
  : EntConNmb(otro), sccGeom(otro.sccGeom)
  {}

//! @brief Operator asignación.
XC::EntGeomSection &XC::EntGeomSection::operator=(const EntGeomSection &otro)
  {
    EntConNmb::operator=(otro);
    sccGeom= otro.sccGeom;
    return *this;
  }

//! @brief Returns current reference system.
XC::SisRefScc *XC::EntGeomSection::SisRefSccActual(void)
  {
    SisRefScc *retval= nullptr;
    if(sccGeom)
      retval= getGeomSection()->current_reference_system();
    return retval;
  }

//! @brief Returns current reference system.
const XC::SisRefScc *XC::EntGeomSection::SisRefSccActual(void) const
  {
    const SisRefScc *retval= nullptr;
    if(sccGeom)
      retval= getGeomSection()->current_reference_system();
    return retval;
  }

//! @brief Returns a pointer to the point with the identifier being passed as parameter.
XC::Spot *XC::EntGeomSection::BuscaSpot(const size_t &id_punto)
  {
    Spot *retval= nullptr;
    if(sccGeom)
      {
        retval= getGeomSection()->busca_spot(id_punto);
      }
    else
      std::cerr << "XC::EntGeomSection::BuscaSpot: el objeto: '"
                << GetNombre() << "' no tiene asignado GeomSection."
                << std::endl;
    return retval;
  }

//! @brief Returns a const pointer to the point with the identifier being passed as parameter.
const XC::Spot *XC::EntGeomSection::BuscaSpot(const size_t &id_punto) const
  {
    const Spot *retval= nullptr;
    if(sccGeom)
      {
        retval= getGeomSection()->busca_spot(id_punto);
      }
    else
      std::cerr << "XC::EntGeomSection::BuscaSpot: el objeto: '" 
                << GetNombre() << "' no tiene asignado GeomSection."
                << std::endl;
    return retval;
  }

//! @brief Devuelve un puntero al Eje cuyo índice se pasa como parámetro.
XC::Eje *XC::EntGeomSection::BuscaEje(const size_t &id_Eje)
  {
    Eje *retval= nullptr;
    if(sccGeom)
      {
        retval= getGeomSection()->busca_eje(id_Eje);
      }
    else
      std::cerr << "XC::EntGeomSection::BuscaEje: el objeto: '"
                << GetNombre() << "' no tiene asignado GeomSection."
                << std::endl;
    return retval;
  }

//! @brief Devuelve un puntero al Eje cuyo índice se pasa como parámetro
const XC::Eje *XC::EntGeomSection::BuscaEje(const size_t &id_Eje) const
  {
    const Eje *retval= nullptr;
    if(sccGeom)
      {
        retval= getGeomSection()->busca_eje(id_Eje);
      }
    else
      std::cerr << "XC::EntGeomSection::BuscaEje: el objeto: '" 
                << GetNombre() << "' no tiene asignado GeomSection."
                << std::endl;
    return retval;
  }

//! @brief Interfaz con VTK.
int XC::EntGeomSection::getVtkCellType(void) const
  {
    std::cerr << "EntGeomSection::getVtkCellType: la función getVtkCellType debe definirse en las clases derivadas." << std::endl;
    return VTK_EMPTY_CELL;
  } 

//! @brief Interfaz con el formato MED de Salome.
int XC::EntGeomSection::getMEDCellType(void) const
  { return MED_NONE; }
