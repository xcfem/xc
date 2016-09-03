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
XC::Cell::Cell(const XC::Matrix &vertexCoords)
  : vertCoord(vertexCoords), Centroid(2) {}

//! @brief Devuelve las coordenadas de los vértices.
const XC::Matrix &XC::Cell::getVertCoords(void) const
  { return vertCoord; }

//! @brief Asigna las coordenadas de los vértics.
void XC::Cell::setVertCoords(const XC::Matrix &vertexCoords)
  { vertCoord = vertexCoords; }

//! @brief Calcula la posición del centroide de la celda.
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

const XC::Vector &XC::Cell::getCentroidPosition(void)
  {
    calcCentroidPosition();
    return Centroid;
  }

std::ostream &XC::operator<<(std::ostream &s, const XC::Cell &cell)
  {
    cell.Print(s);
    return s;
  }
