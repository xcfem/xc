// -*-c++-*-
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
//NMPointCloud.h
//Internal forces point cloud container.


#ifndef NMPOINTCLOUD_H
#define NMPOINTCLOUD_H

#include "NMPointCloudBase.h"
#include "utility/geom/pos_vec/Pos2d.h"
#include "utility/geom/pos_vec/Pos2dList.h"

namespace XC {


//! @ingroup MATSCCDiagInt
//
//! @brief (N,M) point set
class NMPointCloud: public NMPointCloudBase, public GeomObj::list_Pos2d
  {
    const Pos2d *lastInserted;
  public:
    NMPointCloud(const double &u= 0.0);
    void clear(void);
    const Pos2d *append(const Pos2d &);
  };

} // end of XC namespace

#endif
