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
//
#ifndef GAUSSMODEL_H
#define GAUSSMODEL_H

#include "GaussPoint.h"
#include "xc_utils/src/geom/pos_vec/Pos2d.h"
#include "xc_utils/src/geom/pos_vec/Pos3d.h"
#include <deque>

namespace XC {

//! @ingroup FEMisc
//
//! @brief Base class for Gauss integration models.
class GaussModel
  {
  protected:
    std::deque<Pos3d> coo_ref_nodes;//!< Coordinates of the reference nodes.
    std::deque<GaussPoint> gauss_points;//!< Gauss points.
  public:
    GaussModel(void);
    GaussModel(const Pos2d &n1,const Pos2d &n2,const Pos2d &n3,
               const GaussPoint &p);
    GaussModel(const Pos2d &n1,const Pos2d &n2,const Pos2d &n3,
               const GaussPoint &p1,const GaussPoint &p2,const GaussPoint &p3);
    GaussModel(const Pos2d &n1,const Pos2d &n2,const Pos2d &n3,const Pos2d &n4,
               const GaussPoint &p1,const GaussPoint &p2,const GaussPoint &p3,const GaussPoint &p4);
    GaussModel(const Pos2d &n1,const Pos2d &n2,const Pos2d &n3,const Pos2d &n4,
               const Pos2d &n5,const Pos2d &n6,const Pos2d &n7,const Pos2d &n8,const Pos2d &n9,
               const GaussPoint &p1,const GaussPoint &p2,const GaussPoint &p3,const GaussPoint &p4,
               const GaussPoint &p5,const GaussPoint &p6,const GaussPoint &p7,const GaussPoint &p8,const GaussPoint &p9);

    inline size_t getNumberOfReferenceNodes(void) const
      { return coo_ref_nodes.size(); }
    const std::deque<Pos3d> &getReferenceNodesPositions(void) const
      { return coo_ref_nodes; }
    inline size_t getNumGaussPoints(void) const
      { return gauss_points.size(); }
    const std::deque<GaussPoint> &getPuntosGauss(void) const
      { return gauss_points; }

  };

const GaussModel gauss_model_empty;
const GaussModel gauss_model_tria1(Pos2d(0,0),Pos2d(1,0),Pos2d(0,1),
                                   GaussPoint(Pos2d(0.333333333333333,0.333333333333333),0.5));

const GaussModel gauss_model_tria3(Pos2d(0,0),Pos2d(1,0),Pos2d(0,1),
                                   GaussPoint(Pos2d(0.2,0.2),0.3334),
                                   GaussPoint(Pos2d(0.8,0.1),0.3334),
                                   GaussPoint(Pos2d(0.1,0.8),0.3334));
const double root3= sqrt(3.0);
const double one_over_root3= 1.0/root3;
const GaussModel gauss_model_quad4(Pos2d(-1,-1),Pos2d(1,-1),Pos2d(1,1),Pos2d(-1,1),
                                    GaussPoint(Pos2d(-one_over_root3,-one_over_root3),1),
                                    GaussPoint(Pos2d(one_over_root3,-one_over_root3),1),
                                    GaussPoint(Pos2d(one_over_root3,one_over_root3),1),
                                    GaussPoint(Pos2d(-one_over_root3,one_over_root3),1));

const double root5= sqrt(5.0);
const double root3_over_root5= root3/root5;

//Gauss model for nine node quads.
//
//  6----5----4
//  |         |
//  7    8    3
//  |         |
//  0----1----2
//
const GaussModel gauss_model_quad9(Pos2d(-1,-1),Pos2d(0,-1),Pos2d(1,-1),Pos2d(1,0),
                                    Pos2d(1,1),Pos2d(0,1),Pos2d(-1,1),Pos2d(-1,0),Pos2d(0,0),
                                    GaussPoint(Pos2d(-root3_over_root5,-root3_over_root5),25.0/81.0),
                                    GaussPoint(Pos2d(0,-root3_over_root5),40.0/81.0),
                                    GaussPoint(Pos2d(root3_over_root5,-root3_over_root5),25.0/81.0),
 				    GaussPoint(Pos2d(root3_over_root5,0),40.0 / 81.0),
				    GaussPoint(Pos2d(root3_over_root5,root3_over_root5),25.0 / 81.0),
				    GaussPoint(Pos2d(0,root3_over_root5),40.0 / 81.0),
 				    GaussPoint(Pos2d(-root3_over_root5,root3_over_root5),25.0 / 81.0),
				    GaussPoint(Pos2d(-root3_over_root5,0),40.0 / 81.0),
                                    GaussPoint(Pos2d(0,0),64.0 / 81.0));
} // end of XC namespace

#endif 
//
