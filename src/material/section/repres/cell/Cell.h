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
//
//! @defgroup MATSCCCell Teselas para discretización de secciones.
//
//! @ingroup MATSCCCell
//
//! @brief Clase base para las teselas.
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
    friend std::ostream &operator<<(std::ostream &s, const Cell &Cell);    
  };

std::ostream &operator<<(std::ostream &s, const Cell &cell);

} // end of XC namespace


#endif

