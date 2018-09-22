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
#include "Axis.h"
#include "vtkCellType.h"

//! @brief Returns a pointer to the GeomSection.
const XC::GeomSection *XC::EntGeomSection::getGeomSection(void) const
  { return sccGeom; }
//! @brief Returns a pointer to the GeomSection.
XC::GeomSection *XC::EntGeomSection::getGeomSection(void)
  { return sccGeom; }

//! @brief Constructor.
XC::EntGeomSection::EntGeomSection(const std::string &nmb,GeomSection *s)
  : NamedEntity(nmb),sccGeom(s) {}

//! @brief Copy constructor.
XC::EntGeomSection::EntGeomSection(const EntGeomSection &other)
  : NamedEntity(other), sccGeom(other.sccGeom)
  {}

//! @brief Assignment operator.
XC::EntGeomSection &XC::EntGeomSection::operator=(const EntGeomSection &other)
  {
    NamedEntity::operator=(other);
    sccGeom= other.sccGeom;
    return *this;
  }

//! @brief Returns current reference system.
XC::SectionReferenceFrame *XC::EntGeomSection::SectionReferenceFrameActual(void)
  {
    SectionReferenceFrame *retval= nullptr;
    if(sccGeom)
      retval= getGeomSection()->current_reference_system();
    return retval;
  }

//! @brief Returns current reference system.
const XC::SectionReferenceFrame *XC::EntGeomSection::SectionReferenceFrameActual(void) const
  {
    const SectionReferenceFrame *retval= nullptr;
    if(sccGeom)
      retval= getGeomSection()->current_reference_system();
    return retval;
  }

//! @brief Returns a pointer to the point with the identifier being passed as parameter.
XC::Spot *XC::EntGeomSection::BuscaSpot(const size_t &id_point)
  {
    Spot *retval= nullptr;
    if(sccGeom)
      {
        retval= getGeomSection()->busca_spot(id_point);
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; section geometry not set for the object: '"
                << getName() << std::endl;
    return retval;
  }

//! @brief Returns a const pointer to the point with the identifier being passed as parameter.
const XC::Spot *XC::EntGeomSection::BuscaSpot(const size_t &id_point) const
  {
    const Spot *retval= nullptr;
    if(sccGeom)
      {
        retval= getGeomSection()->busca_spot(id_point);
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; geometry of the section not set for the object: '" 
                << getName() << "'."
                << std::endl;
    return retval;
  }

//! @brief Returns a pointer to the axis indentified by the tag being passed
//! as parameter.
XC::Axis *XC::EntGeomSection::findAxis(const size_t &id_Axis)
  {
    Axis *retval= nullptr;
    if(sccGeom)
      {
        retval= getGeomSection()->find_axis(id_Axis);
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; geometry of the section not set for the object: '" 
                << getName() << "'."
                << std::endl;
    return retval;
  }

//! @brief Returns a pointer to the Axis indentified by the tag being passed as parameter.
const XC::Axis *XC::EntGeomSection::findAxis(const size_t &id_Axis) const
  {
    const Axis *retval= nullptr;
    if(sccGeom)
      {
        retval= getGeomSection()->find_axis(id_Axis);
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; geometry of the section not set for the object: '" 
                << getName() << "'."
                << std::endl;
    return retval;
  }

//! @brief VTK interface.
int XC::EntGeomSection::getVtkCellType(void) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << " must be overloaded on derived classes." << std::endl;
    return VTK_EMPTY_CELL;
  } 

