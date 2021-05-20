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
//TrfGeom.cc

#include "TrfGeom.h"
#include "utility/geom/pos_vec/Pos3dArray.h"
#include "utility/geom/pos_vec/Pos3dArray3d.h"

const Pos3dArray &XC::TrfGeom::Transform(const Pos3dArray &m) const
  {
    static Pos3dArray retval;
    retval= m;
    const size_t numberOfRows= retval.getNumberOfRows();
    const size_t numberOfColumns= retval.getNumberOfColumns();
    for(size_t j= 1;j<=numberOfRows;j++)
      for(size_t k= 1;k<=numberOfColumns;k++)
        retval(j,k)= Transform(retval(j,k));
    return retval;
  }
const Pos3dArray3d &XC::TrfGeom::Transform(const Pos3dArray3d &m) const
  {
    static Pos3dArray3d retval;
    retval= m;
    const size_t numberOfLayers= retval.getNumberOfLayers();
    const size_t numberOfRows= retval.getNumberOfRows();
    const size_t numberOfColumns= retval.getNumberOfColumns();
    for(size_t i= 1;i<=numberOfLayers;i++)
      for(size_t j= 1;j<=numberOfRows;j++)
        for(size_t k= 1;k<=numberOfColumns;k++)
          retval(i,j,k)= Transform(retval(i,j,k));
    return retval;
  }
