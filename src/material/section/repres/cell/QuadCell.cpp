//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
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
