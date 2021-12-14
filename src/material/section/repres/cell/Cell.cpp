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
                                                                        
// $Revision: 1.1.1.1 $
// $Date: 2000/09/15 08:23:22 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/repres/cell/Cell.cpp,v $
                                                                        
                                                                        
// File: Cell.C
// Written by Remo M. de Souza
// December 1998

#include "Cell.h" 

//! @brief Constructor
XC::Cell::Cell(int numVert)
  : vertCoord(numVert,2), Centroid(2) {}

//! @brief Constructor
XC::Cell::Cell(const Matrix &vertexCoords)
  : vertCoord(vertexCoords), Centroid(2) {}

//! @brief Returns the coordinates of the vertices.
const XC::Matrix &XC::Cell::getVertCoords(void) const
  { return vertCoord; }

//! @brief Sets vertices coordinates.
void XC::Cell::setVertCoords(const Matrix &vertexCoords)
  { vertCoord = vertexCoords; }

//! @brief Computes the centroid position.
void XC::Cell::calcCentroidPosition(void)
  {
    const size_t sz= vertCoord.noRows();
    int i1;
    double yi, zi, yi1, zi1, dyi, dzi;
    double area= 0.0, integ;
    double CGy = 0.0, CGz = 0.0;

    for(size_t i= 0;i<sz;i++)
      {
        i1= (i+1)%4;
        yi= vertCoord(i,0);
        zi= vertCoord(i,1);
        yi1= vertCoord(i1,0);
        zi1= vertCoord(i1,1);
        area+= (zi1 - zi) * (yi1 + yi);

        dyi= yi1 - yi;
        dzi= zi1 - zi;
   
        integ= yi*zi + (yi*dzi + zi*dyi)/2.0 + dyi*dzi/3.0;
        CGy-= dyi * integ;
        CGz+= dzi * integ;
      }
    area/= 2.0;
    CGy/= area;
    CGz/= area;
    Centroid(0)= CGy;
    Centroid(1)= CGz;
  }

//! @brief Return the coordinates of the centroid.
const XC::Vector &XC::Cell::getCentroidPosition(void)
  {
    calcCentroidPosition();
    return Centroid;
  }

//! @brief Insertion on an output stream.
std::ostream &XC::operator<<(std::ostream &s, const XC::Cell &cell)
  {
    cell.Print(s);
    return s;
  }
