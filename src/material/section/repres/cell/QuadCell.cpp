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
/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 1999, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.3 $
// $Date: 2003/05/02 18:34:56 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/repres/cell/QuadCell.cpp,v $
                                                                        
                                                                        
// File: QuadCell.C
// Written by Remo M. de Souza
// December 1998

#include <material/section/repres/cell/QuadCell.h>

//! @briet Constructor
XC::QuadCell::QuadCell(void)
  : Cell(4) {}

//! @briet Constructor
XC::QuadCell::QuadCell(const XC::Matrix &vertexCoords)
  : Cell(vertexCoords) {}

//! @brief Constructor de copia.
XC::Cell *XC::QuadCell::getCopy(void) const
  { return new QuadCell(*this); }

//! @brief Return the area of the cell.
double XC::QuadCell::getArea(void) const
  {
    const double x0= vertCoord(0,0);
    const double y0= vertCoord(0,1);
    const double x1= vertCoord(1,0);
    const double y1= vertCoord(1,1);
    const double x2= vertCoord(2,0);
    const double y2= vertCoord(2,1);
    const double x3= vertCoord(3,0);
    const double y3= vertCoord(3,1);

    return (((x2-x1)*(y0-y1) - (x0-x1)*(y2-y1) +
	     (x0-x3)*(y2-y3) - (x2-x3)*(y0-y3)) / 2.0);
  }

void XC::QuadCell::Print(std::ostream &s, int flag) const
  {
    s << "\nCell Type: QuadCell";
    s << "\nVertex Coordinates: " << vertCoord;
  }
