// -*-c++-*-
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

//! @ingroup Quadrature
//
//! @brief One-dimensional Gauss quadrature.
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
