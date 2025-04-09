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
//BeamColumns.h

#ifndef BeamColumn_h
#define BeamColumn_h

#include <domain/mesh/element/Element1D.h>
#include "material/section/PrismaticBarCrossSectionsVector.h"
#include "SectionMatrices.h"


namespace XC {

class PrismaticBarCrossSection;
  
//! @ingroup BeamColumnElemGrp
//
//! @brief Base class for Beam-column elements with PrismaticBarCrossSection material
class BeamColumn: public Element1D
  {
  public:
    BeamColumn(int tag, int classTag);
    BeamColumn(int tag, int classTag, int Nd1, int Nd2);

    virtual const PrismaticBarCrossSection *getSectionPtr(const size_t &) const;

    virtual const Matrix &getExtrapolationMatrix(void) const;
    Matrix getExtrapolatedValues(const Matrix &) const;
  };

} //end of XC namespace
#endif
