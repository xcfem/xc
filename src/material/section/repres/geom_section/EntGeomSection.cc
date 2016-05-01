//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
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

//! @brief Devuelve el sistema de coordenadas activo.
XC::SisRefScc *XC::EntGeomSection::SisRefSccActual(void)
  {
    SisRefScc *retval= nullptr;
    if(sccGeom)
      retval= getGeomSection()->sistema_referencia_actual();
    return retval;
  }

//! @brief Devuelve el sistema de coordenadas activo.
const XC::SisRefScc *XC::EntGeomSection::SisRefSccActual(void) const
  {
    const SisRefScc *retval= nullptr;
    if(sccGeom)
      retval= getGeomSection()->sistema_referencia_actual();
    return retval;
  }

//! @brief Devuelve un puntero al punto cuyo índice se pasa como parámetro.
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

//! @brief Devuelve un puntero al punto cuyo índice se pasa como parámetro
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
