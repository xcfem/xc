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
//InteractionDiagram.h

#ifndef INTERACTION_DIAGRAM2D_H
#define INTERACTION_DIAGRAM2D_H

#include "xc_utils/src/geom/d2/2d_polygons/Polygon2d.h"

namespace XC {

class Vector;
class FiberSectionBase;
class InteractionDiagramData;

//! \@ingroup MATSCCDiagInt
//
//! @brief Interaction diagram (N,My) for an RC section
//! of a two-dimensional problem.
class InteractionDiagram2d: public Polygon2d
  {
  protected:
    Pos2d get_intersection(const Pos2d &p) const;
  public:
    InteractionDiagram2d(void);
    InteractionDiagram2d(const Polygon2d &);
    virtual InteractionDiagram2d *clon(void) const;

    void Simplify(void);
    Pos2d getIntersection(const Pos2d &) const;
    double getCapacityFactor(const Pos2d &esf_d) const;
    Vector getCapacityFactor(const GeomObj::list_Pos2d &lp) const;

    void Print(std::ostream &os) const;
  };

InteractionDiagram2d calcPlaneInteractionDiagram(const FiberSectionBase &scc,const InteractionDiagramData &, const double &);
InteractionDiagram2d calcNMyInteractionDiagram(const FiberSectionBase &scc,const InteractionDiagramData &);
InteractionDiagram2d calcNMzInteractionDiagram(const FiberSectionBase &scc,const InteractionDiagramData &);

} // end of XC namespace

#endif
