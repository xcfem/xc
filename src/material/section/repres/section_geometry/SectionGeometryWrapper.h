// -*-c++-*-
//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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
//SectionGeometryWrapper.h

#ifndef ENTGEOMSECTION_H
#define ENTGEOMSECTION_H

#include "utility/kernel/NamedEntity.h"

namespace XC {
class SectionGeometry;
class SectionReferenceFrame;
class Spot;
class Axis;

//! @ingroup MATSCCRepresGeom
//!
//! @defgroup MATSCCEntGeom Geometric entities.
//
//! @ingroup MATSCCEntGeom
//
//! @brief Base class of section geometry representation classes.
class SectionGeometryWrapper: public NamedEntity
  {
  private:
    SectionGeometry *sccGeom; //!< section geometry.
  public:
    SectionGeometryWrapper(const std::string &nmb="",SectionGeometry *sccGeom= nullptr);
    SectionGeometryWrapper(const SectionGeometryWrapper &);
    SectionGeometryWrapper &operator=(const SectionGeometryWrapper &);
    const SectionGeometry *getSectionGeometry(void) const;
    SectionGeometry *getSectionGeometry(void);
    int getVtkCellType(void) const;


    SectionReferenceFrame *SectionReferenceFrameActual(void);    
    const SectionReferenceFrame *SectionReferenceFrameActual(void) const;

    Spot *BuscaSpot(const size_t &);    
    const Spot *BuscaSpot(const size_t &) const;
    Axis *findAxis(const size_t &);    
    const Axis *findAxis(const size_t &) const;
  };

} //end of XC namespace
#endif
