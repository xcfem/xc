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

#ifndef PolylineReinfLayer_h 
#define PolylineReinfLayer_h 

#include <deque>
#include <material/section/repres/section_geometry/reinfLayer/ReinfLayer.h>
#include <utility/matrix/Vector.h>
#include "utility/geom/d1/Polyline2d.h"

namespace XC {
class ReinfBar;

//! @ingroup MATSCCReinforcement
//
//! @brief Set of rebars distributed along a segment.
class PolylineReinfLayer: public ReinfLayer
  {
  private:
    std::deque<Vector> positions; //!< positions defining the polyline.
  protected:

    friend class ListReinfLayer;
    PolylineReinfLayer(ListReinfLayer *,Material *mat);
    PolylineReinfLayer(ListReinfLayer *,Material *mat, int numReinfBars,
		       double  reinfBarArea,
                       const std::deque<Vector> &);
    ReinfLayer *getCopy(void) const;
    PolylineReinfLayer _reinforce_mid_points(const double &) const;
  public:
    // edition functions
    void setPositions(const std::deque<Vector> &);
    const std::deque<Vector> &getPositions(void) const;
    void setPolyline(const Polyline2d &);
    Polyline2d getPolyline(void) const;

    // inquiring functions
    const VectorReinfBar &getReinfBars(void) const;
  
    double getMaxY(void) const;
    double getMaxZ(void) const;
    double getMinY(void) const;
    double getMinZ(void) const;
    // Segment2d getLineSegment(void) const;
    double getLength(void) const;
    double getSpacement(void) const;
    int setSpacement(const double &);

    void Print(std::ostream &s, int flag =0) const;   
  };
} // end of XC namespace


#endif

