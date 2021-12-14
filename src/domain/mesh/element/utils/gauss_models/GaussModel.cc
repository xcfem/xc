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
//
#include "GaussModel.h"

//! @brief  Constructor
XC::GaussModel::GaussModel(void)
  {}

//! @brief  Constructor: 3 node triangle 1 Gauss point. 
XC::GaussModel::GaussModel(const Pos2d &n1,const Pos2d &n2,const Pos2d &n3,const GaussPoint &p1)
  {
    coo_ref_nodes.push_back(Pos3d(n1.x(),n1.y(),0));
    coo_ref_nodes.push_back(Pos3d(n2.x(),n2.y(),0));
    coo_ref_nodes.push_back(Pos3d(n3.x(),n3.y(),0));
    gauss_points.push_back(p1);
  }

//! @brief  Constructor: three node triangle. 
XC::GaussModel::GaussModel(const Pos2d &n1,const Pos2d &n2,const Pos2d &n3,const GaussPoint &p1,const GaussPoint &p2,const GaussPoint &p3)
  {
    coo_ref_nodes.push_back(Pos3d(n1.x(),n1.y(),0));
    coo_ref_nodes.push_back(Pos3d(n2.x(),n2.y(),0));
    coo_ref_nodes.push_back(Pos3d(n3.x(),n3.y(),0));
    gauss_points.push_back(p1);
    gauss_points.push_back(p2);
    gauss_points.push_back(p3);
  }

//! @brief  Constructor: four node quad. 
XC::GaussModel::GaussModel(const Pos2d &n1,const Pos2d &n2,const Pos2d &n3,const Pos2d &n4,const GaussPoint &p1,const GaussPoint &p2,const GaussPoint &p3,const GaussPoint &p4)
  {
    coo_ref_nodes.push_back(Pos3d(n1.x(),n1.y(),0));
    coo_ref_nodes.push_back(Pos3d(n2.x(),n2.y(),0));
    coo_ref_nodes.push_back(Pos3d(n3.x(),n3.y(),0));
    coo_ref_nodes.push_back(Pos3d(n4.x(),n4.y(),0));
    gauss_points.push_back(p1);
    gauss_points.push_back(p2);
    gauss_points.push_back(p3);
    gauss_points.push_back(p4);
  }

//! @brief  Constructor: nine node quad. 
XC::GaussModel::GaussModel(const Pos2d &n1,const Pos2d &n2,const Pos2d &n3,const Pos2d &n4,
               const Pos2d &n5,const Pos2d &n6,const Pos2d &n7,const Pos2d &n8,const Pos2d &n9,
               const GaussPoint &p1,const GaussPoint &p2,const GaussPoint &p3,const GaussPoint &p4,
               const GaussPoint &p5,const GaussPoint &p6,const GaussPoint &p7,const GaussPoint &p8,const GaussPoint &p9)

  {
    coo_ref_nodes.push_back(Pos3d(n1.x(),n1.y(),0));
    coo_ref_nodes.push_back(Pos3d(n2.x(),n2.y(),0));
    coo_ref_nodes.push_back(Pos3d(n3.x(),n3.y(),0));
    coo_ref_nodes.push_back(Pos3d(n4.x(),n4.y(),0));
    coo_ref_nodes.push_back(Pos3d(n5.x(),n5.y(),0));
    coo_ref_nodes.push_back(Pos3d(n6.x(),n6.y(),0));
    coo_ref_nodes.push_back(Pos3d(n7.x(),n7.y(),0));
    coo_ref_nodes.push_back(Pos3d(n8.x(),n8.y(),0));
    coo_ref_nodes.push_back(Pos3d(n9.x(),n9.y(),0));
    gauss_points.push_back(p1);
    gauss_points.push_back(p2);
    gauss_points.push_back(p3);
    gauss_points.push_back(p4);
    gauss_points.push_back(p5);
    gauss_points.push_back(p6);
    gauss_points.push_back(p7);
    gauss_points.push_back(p8);
    gauss_points.push_back(p9);
  }
