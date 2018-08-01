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
//PolygonSectRegion.cpp                                                                        

#include <material/section/repres/geom_section/region/PolygonSectRegion.h>
#include <material/section/repres/cell/QuadCell.h>

#include "xc_basic/src/text/text_string.h"
#include "xc_utils/src/geom/d2/Rejilla2d.h"
#include "xc_utils/src/geom/d2/2d_polygons/Polygon2d.h"
#include <utility/matrix/Matrix.h>

XC::PolygonSectRegion::PolygonSectRegion(Material *mat)
  : QuadCellRegion(mat), plg() {}


XC::PolygonSectRegion::PolygonSectRegion(Material *mat, int numSubdivIJ, int numSubdivJK,const Polygon2d &p)
  : QuadCellRegion(mat,numSubdivIJ,numSubdivJK), plg(p)
  {}  

double XC::PolygonSectRegion::getMaxY(void) const
  { return plg.GetXMax(); }

double XC::PolygonSectRegion::getMaxZ(void) const
  { return plg.GetYMax(); }

double XC::PolygonSectRegion::getMinY(void) const
  { return plg.GetXMin(); }

double XC::PolygonSectRegion::getMinZ(void) const
  { return plg.GetYMin(); }

Polygon2d XC::PolygonSectRegion::getPolygon(void) const
  { return plg; }

const Rejilla2d &XC::PolygonSectRegion::getMesh(void) const
  {
    std::cerr << "PolygonSectRegion::getMesh not implemented." << std::endl;
    static Rejilla2d retval;
    return retval;
  }

const XC::VectorCells &XC::PolygonSectRegion::getCells(void) const
  {
    std::cerr << "PolygonSectRegion::getCells not implemented." << std::endl;
    return cells;
  }


XC::SectRegion *XC::PolygonSectRegion::getCopy (void) const
  { return new PolygonSectRegion(*this); }
 
void XC::PolygonSectRegion::Print(std::ostream &s, int flag) const
  {
    s << "\nQuadCellRegion Type: PolygonSectRegion";
    //s << "\nMaterial Id: " << getMaterialID();
    s << "\nNumber of subdivisions in the IJ direction: " << nDivIJ;
    s << "\nNumber of subdivisions in the JK direction: " << nDivJK;
    s << "\nVertex Polígono: " << plg;
  }


std::ostream &XC::operator<<(std::ostream &s, XC::PolygonSectRegion &rg_scc_cuad)
  {
    rg_scc_cuad.Print(s);
    return s;
  }
