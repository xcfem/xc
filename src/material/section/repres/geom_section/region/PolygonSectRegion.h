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
//PolygonSectRegion.h

#ifndef PolygonSectRegion_h 
#define PolygonSectRegion_h 

#include <material/section/repres/geom_section/region/QuadCellRegion.h>
#include "utility/matrix/Matrix.h"
#include "utility/geom/d2/2d_polygons/Polygon2d.h"

class Polygon2d;

namespace XC {

//! @ingroup MATSCCRegions
//
//! Polygonal patch for cross-section discretization.
class PolygonSectRegion: public QuadCellRegion
  {
  private:
    Polygon2d plg;//!< Polygon that defines the patch.
  protected:

  public:
    PolygonSectRegion(Material *);
    PolygonSectRegion(Material *, int numSubdivIJ, int numSubdivJK, const Polygon2d &);
    
    const Grid2d &getMesh(void) const;
    const VectorCells &getCells(void) const;
    SectRegion *getCopy(void) const;

    double getMaxY(void) const;
    double getMaxZ(void) const;
    double getMinY(void) const;
    double getMinZ(void) const;
    Polygon2d getPolygon(void) const;


    void Print(std::ostream &s, int flag =0) const;   
    friend std::ostream &operator<<(std::ostream &, PolygonSectRegion &);    
  };

std::ostream &operator<<(std::ostream &, PolygonSectRegion &);    
} // end of XC namespace


#endif

 
