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
// $Date: 2003/02/14 23:00:43 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/algorithm/equiSolnAlgo/SecantLineSearch.h,v $

// Written: fmk 
// Created: 11/01

// Description: This file contains the class definition for SecantLineSearch.
// This performs the search for U(i+1) = U(i) + eta * deltaU(i) by using the 
// secant method to find the best solution.
//
//                eta(j+1) = eta(j) -  s(j) * (eta(j-1)-eta(j))
//                                     ------------------------
//                                           s(j-1) - s(j)
//
// where     s(j) = U(i+1,j) ^ R(U(i+1, j))
//
//  and      U(i+1,j) = U(i) + eta(j)*deltaU(i)
// 
// What: "@(#)NewtonLineSearch.h, revA"

#ifndef SecantLineSearch_h
#define SecantLineSearch_h

#include <solution/analysis/algorithm/equiSolnAlgo/lineSearch/LineSearch.h>
namespace XC {
class Vector;

//! @ingroup EQSolAlgo
//
//! @brief performs the search for U(i+1) = U(i) + eta * deltaU(i) by
//! using the secant method to find the best solution.
class SecantLineSearch: public LineSearch
  {
    friend class NewtonLineSearch;
    friend class FEM_ObjectBroker;
    SecantLineSearch(void);
    virtual LineSearch *getCopy(void) const;
  public:
    int search(double s0, 
	       double s1, 
	       LinearSOE &theSOE, 
	       IncrementalIntegrator &theIntegrator);
  };
inline LineSearch *SecantLineSearch::getCopy(void) const
  { return new SecantLineSearch(*this); }
} // fin namespace XC

#endif


