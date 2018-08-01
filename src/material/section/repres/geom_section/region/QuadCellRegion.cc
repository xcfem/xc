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
//QuadCellRegion.cpp

#include <material/section/repres/geom_section/region/QuadCellRegion.h> 


#include "xc_utils/src/geom/d2/Rejilla2d.h"
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>

void XC::QuadCellRegion::free_mem(void) const
  {
    if(rejilla) delete rejilla;
    rejilla= nullptr;
  }

const Rejilla2d &XC::QuadCellRegion::alloc(const Rejilla2d &ptos) const
  {
    free_mem();
    rejilla= new Rejilla2d(ptos);
    return *rejilla;
  }

XC::QuadCellRegion::QuadCellRegion(Material *mat)
  : SectRegion(mat), rejilla(nullptr), nDivIJ(1), nDivJK(1) {}

XC::QuadCellRegion::QuadCellRegion(Material *mat, int numSubdivIJ, int numSubdivJK)
  : SectRegion(mat), rejilla(nullptr), nDivIJ(numSubdivIJ), nDivJK(numSubdivJK) {}

//! @brief Copy constructor.
XC::QuadCellRegion::QuadCellRegion(const QuadCellRegion &other)
  : SectRegion(other), rejilla(nullptr), nDivIJ(other.nDivIJ), nDivJK(other.nDivJK) 
  {
    if(other.rejilla)
      alloc(*other.rejilla);
  }

//! @brief Assignment operator.
XC::QuadCellRegion &XC::QuadCellRegion::operator=(const QuadCellRegion &other)
  {
    SectRegion::operator=(other);
    if(other.rejilla)
      alloc(*other.rejilla);
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


//! @brief Return the centro de gravedad de la rejilla.
XC::Vector XC::QuadCellRegion::getCenterOfMassRejilla(void) const
  {
    Vector retval(2);
    assert(rejilla);
    Pos2d c= rejilla->getCenterOfMass();
    retval[0]= c.x();
    retval[1]= c.y();
    return retval;
  }

//! @brief Return the coordenadas of the point i,j.
XC::Vector XC::QuadCellRegion::getVertCoords(const size_t &i,const size_t &j) const
  {
    Vector retval(2);
    assert(rejilla);
    retval[0]= (*rejilla)(i,j).x();
    retval[1]= (*rejilla)(i,j).y();
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
    assert(rejilla);
    retval(0,0)= (*rejilla)(i,j).x();
    retval(0,1)= (*rejilla)(i,j).y();
    retval(1,0)= (*rejilla)(i,j+1).x();
    retval(1,1)= (*rejilla)(i,j+1).y();
    retval(2,0)= (*rejilla)(i+1,j+1).x();
    retval(2,1)= (*rejilla)(i+1,j+1).y();
    retval(3,0)= (*rejilla)(i+1,j).x();
    retval(3,1)= (*rejilla)(i+1,j).y();
    return retval;
  }
