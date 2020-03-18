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
//NMPointCloud.cc

#include "NMPointCloud.h"


//! @brief Constructor.
XC::NMPointCloud::NMPointCloud(const double &u)
  : NMPointCloudBase(u), GeomObj::list_Pos2d()
  {
    lastInserted= nullptr;
  }

//! @brief Erases objects members.
void XC::NMPointCloud::clear(void)
  {
    GeomObj::list_Pos2d::clear();
    NMPointCloudBase::clear();
    lastInserted= nullptr;
  }

//! @brief Appends the cross-section normal stresses resultant
//! being passed as parameter.
const Pos2d *XC::NMPointCloud::append(const Pos2d &NM)
  {
    if(lastInserted)
      {
        if(dist(NM,*lastInserted)>threshold)
          lastInserted= Agrega(NM);
      }
    else
      lastInserted= Agrega(NM);
    return lastInserted;  
  }
