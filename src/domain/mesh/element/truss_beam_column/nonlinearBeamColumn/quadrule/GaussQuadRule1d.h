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
// $Date: 2001/10/02 20:20:12 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/nonlinearBeamColumn/quadrule/GaussQuadRule1d.h,v $
                                                                        
// Written: rms
// Created: 12/98
//
// Description: This file contains the class definition for 
// GaussQuadRule1d (Quadrature Rule).

#ifndef GaussQuadRule1d_h
#define GaussQuadRule1d_h

#include <domain/mesh/element/truss_beam_column/nonlinearBeamColumn/quadrule/QuadRule1d.h>

namespace XC {
class Vector;
class Matrix;

//! \ingroup Cuadratura
//
//! @brief Cuadratura de Gauss para una dimensión.
class GaussQuadRule1d: public QuadRule1d
  {
  private:
    int order;

    Matrix *myPts;
    Vector *myWts;

    enum {maxOrder = 10};

    static bool dataSet;

    static double ptsArray[];
    static double wtsArray[];

    static Matrix pts1;
    static Matrix pts2;
    static Matrix pts3;
    static Matrix pts4;
    static Matrix pts5;
    static Matrix pts6;
    static Matrix pts7;
    static Matrix pts8;
    static Matrix pts9;
    static Matrix pts10;

    static Vector wts1;
    static Vector wts2;
    static Vector wts3;
    static Vector wts4;
    static Vector wts5;
    static Vector wts6;
    static Vector wts7;
    static Vector wts8;
    static Vector wts9;
    static Vector wts10;
  public:
    GaussQuadRule1d(void);

    int            setOrder              (int quadOrder);
    int            getOrder              (void) const;
    int            getNumIntegrPoints    (void) const;
    const Matrix & getIntegrPointCoords  (void) const;
    const Vector & getIntegrPointWeights (void) const; 
    const Matrix & getIntegrPointCoords  (int quadOrder);
    const Vector & getIntegrPointWeights (int quadOrder); 
  };
} // end of XC namespace

#endif
