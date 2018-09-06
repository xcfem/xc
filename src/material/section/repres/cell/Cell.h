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
                                                                        
// $Revision: 1.2 $
// $Date: 2003/02/14 23:01:36 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/repres/cell/Cell.h,v $
                                                                        
                                                                        
// File: Cell.h
//
// Written by Remo M. de Souza
// December 1998


#ifndef Cell_h 
#define Cell_h 

#include <iostream>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>

namespace XC {

//! @ingroup MATSCCRepres
//!
//! @defgroup MATSCCCell Cells for cross-section discretization.
//
//! @ingroup MATSCCCell
//
//! @brief Base class for cells (cross-section discretization).
class Cell
  {
  protected:
    Matrix vertCoord;
    Vector Centroid;
    void calcCentroidPosition(void);

  public:
    Cell(int numVert);
    explicit Cell(const Matrix &);
    virtual ~Cell(void) {}
    virtual Cell *getCopy(void) const= 0;

    void setVertCoords(const Matrix &vertexCoords);
    const Matrix &getVertCoords(void) const;

    virtual double getArea(void) const = 0;
    virtual const Vector &getCentroidPosition(void);
    virtual void Print(std::ostream &s, int flag =0) const = 0;   
    friend std::ostream &operator<<(std::ostream &, const Cell &);    
  };

std::ostream &operator<<(std::ostream &, const Cell &);

} // end of XC namespace


#endif

