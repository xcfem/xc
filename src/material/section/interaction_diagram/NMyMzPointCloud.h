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
//NMyMzPointCloud.h
//Internal forces point cloud container.


#ifndef NMYMZPOINTCLOUD_H
#define NMYMZPOINTCLOUD_H

#include "NMPointCloudBase.h"
#include "NMPointCloud.h"
#include "xc_utils/src/geom/pos_vec/Pos3d.h"
#include "xc_utils/src/geom/pos_vec/ListaPos3d.h"


namespace XC {


//! @ingroup MATSCCDiagInt
//
//! @brief (N,My,Mz) point set
class NMyMzPointCloud: public NMPointCloudBase, public GeomObj::list_Pos3d
  {
    const Pos3d *lastInserted;
  public:
    NMyMzPointCloud(const double &u=0.0);
    void clear(void);
    const Pos3d *append(const Pos3d &);
    NMPointCloud getNMy(void) const;
    NMPointCloud getNMz(void) const;
    NMPointCloud getNM(const double &) const;
  };

} // end of XC namespace

#endif
