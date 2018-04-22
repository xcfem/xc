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
//TriangCell.cpp

#include <material/section/repres/cell/TriangCell.h>

//! @brief Constructor
XC::TriangCell::TriangCell(void)
  : Cell(3) {}


//! @brief Virtual constructor.
XC::TriangCell::TriangCell(const XC::Matrix &vertexCoords)
  : Cell(vertexCoords) {}

//! @brief Constructor de copia.
XC::Cell *XC::TriangCell::getCopy(void) const
  { return new TriangCell(*this); }

//! @brief Returns the area of the cell.
double XC::TriangCell::getArea(void) const
  {
    const double x0= vertCoord(0,0);
    const double y0= vertCoord(0,1);
    const double x1= vertCoord(1,0);
    const double y1= vertCoord(1,1);
    const double x2= vertCoord(2,0);
    const double y2= vertCoord(2,1);

    return (((x2-x1)*(y0-y1) - (x0-x1)*(y2-y1)) / 2.0);
  }

//! @brief Print stuff.
void XC::TriangCell::Print(std::ostream &s, int flag) const
  {
    s << "\nCell Type: TriangCell";
    s << "\nVertex Coordinates: " << vertCoord;
  }



