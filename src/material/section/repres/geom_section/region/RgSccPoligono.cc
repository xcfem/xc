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
//RgSccPoligono.cpp                                                                        

#include <material/section/repres/geom_section/region/RgSccPoligono.h>
#include <material/section/repres/cell/QuadCell.h>

#include "xc_basic/src/text/text_string.h"
#include "xc_utils/src/geom/d2/Rejilla2d.h"
#include "xc_utils/src/geom/d2/poligonos2d/Poligono2d.h"
#include <utility/matrix/Matrix.h>

XC::RgSccPoligono::RgSccPoligono(Material *mat)
  : RgQuadCell(mat), plg() {}


XC::RgSccPoligono::RgSccPoligono(Material *mat, int numSubdivIJ, int numSubdivJK,const Poligono2d &p)
  : RgQuadCell(mat,numSubdivIJ,numSubdivJK), plg(p)
  {}  

double XC::RgSccPoligono::getMaxY(void) const
  { return plg.GetXMax(); }

double XC::RgSccPoligono::getMaxZ(void) const
  { return plg.GetYMax(); }

double XC::RgSccPoligono::getMinY(void) const
  { return plg.GetXMin(); }

double XC::RgSccPoligono::getMinZ(void) const
  { return plg.GetYMin(); }

Poligono2d XC::RgSccPoligono::getPolygon(void) const
  { return plg; }

const Rejilla2d &XC::RgSccPoligono::getMesh(void) const
  {
    std::cerr << "RgSccPoligono::getMesh not implemented." << std::endl;
    static Rejilla2d retval;
    return retval;
  }

const XC::VectorCells &XC::RgSccPoligono::getCells(void) const
  {
    std::cerr << "RgSccPoligono::getCells not implemented." << std::endl;
    return cells;
  }


XC::RegionSecc *XC::RgSccPoligono::getCopy (void) const
  { return new RgSccPoligono(*this); }
 
void XC::RgSccPoligono::Print(std::ostream &s, int flag) const
  {
    s << "\nRgQuadCell Type: RgSccPoligono";
    //s << "\nMaterial Id: " << getMaterialID();
    s << "\nNumber of subdivisions in the IJ direction: " << nDivIJ;
    s << "\nNumber of subdivisions in the JK direction: " << nDivJK;
    s << "\nVertex Polígono: " << plg;
  }


std::ostream &XC::operator<<(std::ostream &s, XC::RgSccPoligono &rg_scc_cuad)
  {
    rg_scc_cuad.Print(s);
    return s;
  }
