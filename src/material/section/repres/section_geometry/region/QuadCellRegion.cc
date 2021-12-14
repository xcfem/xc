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
//QuadCellRegion.cpp

#include <material/section/repres/section_geometry/region/QuadCellRegion.h> 


#include "utility/geom/d2/Grid2d.h"
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>

void XC::QuadCellRegion::free_mem(void) const
  {
    if(grid) delete grid;
    grid= nullptr;
  }

const Grid2d &XC::QuadCellRegion::alloc(const Grid2d &ptos) const
  {
    free_mem();
    grid= new Grid2d(ptos);
    return *grid;
  }

XC::QuadCellRegion::QuadCellRegion(Material *mat)
  : SectRegion(mat), grid(nullptr), nDivIJ(1), nDivJK(1) {}

XC::QuadCellRegion::QuadCellRegion(Material *mat, int numSubdivIJ, int numSubdivJK)
  : SectRegion(mat), grid(nullptr), nDivIJ(numSubdivIJ), nDivJK(numSubdivJK) {}

//! @brief Copy constructor.
XC::QuadCellRegion::QuadCellRegion(const QuadCellRegion &other)
  : SectRegion(other), grid(nullptr), nDivIJ(other.nDivIJ), nDivJK(other.nDivJK) 
  {
    if(other.grid)
      alloc(*other.grid);
  }

//! @brief Assignment operator.
XC::QuadCellRegion &XC::QuadCellRegion::operator=(const QuadCellRegion &other)
  {
    SectRegion::operator=(other);
    if(other.grid)
      alloc(*other.grid);
    nDivIJ= other.nDivIJ;
    nDivJK= other.nDivJK;
    return *this;
  }

XC::QuadCellRegion::~QuadCellRegion(void)
  { free_mem(); }

//! @brief Sets the number of divisions on each direction.
void XC::QuadCellRegion::setDiscretization(int numSubdivIJ, int numSubdivJK)
  {
    nDivIJ = numSubdivIJ;
    nDivJK = numSubdivJK;
  }

//! @brief Return the number of divisions on each direction.
void XC::QuadCellRegion::getDiscretization(int &numSubdivIJ, int &numSubdivJK) const
  {
     numSubdivIJ = nDivIJ;
     numSubdivJK = nDivJK;
  }

//! @brief Return the number of celdas.
int XC::QuadCellRegion::getNumCells (void) const
  { return nDivIJ * nDivJK; }


//! @brief Return the center of mass of the grid.
XC::Vector XC::QuadCellRegion::getCenterOfMassGrid(void) const
  {
    Vector retval(2);
    assert(grid);
    Pos2d c= grid->getCenterOfMass();
    retval[0]= c.x();
    retval[1]= c.y();
    return retval;
  }

//! @brief Return the coordinates of the point i,j.
XC::Vector XC::QuadCellRegion::getVertCoords(const size_t &i,const size_t &j) const
  {
    Vector retval(2);
    assert(grid);
    retval[0]= (*grid)(i,j).x();
    retval[1]= (*grid)(i,j).y();
    return retval;
  }

//! @brief Return the coordinates of the vertices of the (i,j) cell.
//                                                                             i+1,j +---+ i+1,j+1
//                                                                                   |   |
//                                                                                   |   |
//                                                                                   |   |
//                                                                               i,j +---+ i,j+1
XC::Matrix XC::QuadCellRegion::getCellVertCoords(const size_t &i,const size_t &j) const
  {
    Matrix retval(4,2);
    assert(grid);
    retval(0,0)= (*grid)(i,j).x();
    retval(0,1)= (*grid)(i,j).y();
    retval(1,0)= (*grid)(i,j+1).x();
    retval(1,1)= (*grid)(i,j+1).y();
    retval(2,0)= (*grid)(i+1,j+1).x();
    retval(2,1)= (*grid)(i+1,j+1).y();
    retval(3,0)= (*grid)(i+1,j).x();
    retval(3,1)= (*grid)(i+1,j).y();
    return retval;
  }
