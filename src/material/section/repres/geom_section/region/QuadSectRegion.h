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
//QuadSectRegion.h
// Written by Remo M. de Souza
// December 1998

#ifndef QuadSectRegion_h 
#define QuadSectRegion_h

#include <material/section/repres/geom_section/region/QuadCellRegion.h>
#include "utility/matrix/Matrix.h"

class Quadrilateral2d;

namespace XC {

// Vertices order: I->J->K->L.
//
// L +---+---+---+---+ K
//   |   |   |   |   |
//   +---+---+---+---+ nDivJK= 2
//   |   |   |   |   |
// I +---+---+---+---+ J
//         nDivIJ= 4

//! @ingroup MATSCCRegions
//
//! @brief Quad that discretizes in quad cells.
class QuadSectRegion: public QuadCellRegion
  {
  private:
    Matrix vertCoord; //!< Coordinates of the vertices.
  protected:

  public:
    QuadSectRegion(Material *);
    QuadSectRegion(Material *, int numSubdivIJ, int numSubdivJK, const Matrix &vertexCoords);

    // edition functions
    void setVertCoords(const Matrix &vertexCoords);

    int setTileSizeIJ(const double &size);
    int setTileSizeJK(const double &size);
    int setTileSize(const double &sizeIJ, const double &sizeJK);
    
    const Grid2d &getMesh(void) const;
    const VectorCells &getCells(void) const;
    SectRegion *getCopy(void) const;

    const Matrix &getVertCoords(void) const;
    double getMaxY(void) const;
    double getMaxZ(void) const;
    Pos2d getPMax(void) const;
    void setPMax(const Pos2d &);
    double getMinY(void) const;
    double getMinZ(void) const;
    Pos2d getPMin(void) const;
    void setPMin(const Pos2d &);
    Pos2d getIVertex(void) const;
    Pos2d getJVertex(void) const;
    Pos2d getKVertex(void) const;
    Pos2d getLVertex(void) const;
    Quadrilateral2d getQuad(void) const;
    void setQuad(const Quadrilateral2d &);
    Polygon2d getPolygon(void) const;
    void swap(void);


    void Print(std::ostream &s, int flag =0) const;   
    friend std::ostream &operator<<(std::ostream &, QuadSectRegion &);    
  };
 
std::ostream &operator<<(std::ostream &, QuadSectRegion &);
} // end of XC namespace


#endif

 
