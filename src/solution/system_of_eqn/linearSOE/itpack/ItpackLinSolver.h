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
// $Date: 2002/06/08 16:17:26 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/itpack/ItpackLinSolver.h,v $

#ifndef ItpackLinSolver_h
#define ItpackLinSolver_h

// Written: MHS
// Created: Sept 2001
//
// Description: This file contains the class definition for ItpackLinSolver.
// ItpackLinSolver is a concrete subclass of LinearSOE. It stores full
// unsymmetric linear system of equations using 1d arrays in Fortan style

#include <solution/system_of_eqn/linearSOE/LinearSOESolver.h>

// Adaptive methods
#define ItpackJCG         1
#define ItpackJSI         2
#define ItpackSOR         3
#define ItpackSSORCG      4
#define ItpackSSORSI      5
#define ItpackRSCG        6
#define ItpackRSSI        7

// Textbook methods
#define ItpackJ           8
#define ItpackGS          9
#define ItpackSORFixed   10
#define ItpackSSORFixed  11
#define ItpackRS         12

namespace XC {
class ItpackLinSOE;

//! @ingroup Solver
//
//! @brief Objeto encargado de la solución de
//! sistema de ecuaciones basado en la biblioteca
//! ITPACK (http://rene.ma.utexas.edu/CNA/ITPACK).
class ItpackLinSolver : public LinearSOESolver
  {
  private:
    
    ItpackLinSOE *theSOE; //!< Sparse Ax=b represntation 
  
    ID IA;
    ID JA;
     
    int n; // Size of system, i.e., the number of equations
      // Parameter arrays sent to ITPACK subroutines
    int iparm[12];
    double rparm[12];
    // Workspace arrays sent to ITPACK subroutines
    ID iwksp;
    Vector wksp;
     
    int nwksp; //!< Length of wksp array
    int maxIter; //!< Maximum number of iterations
    int method; //!< Integer indicating which method to use
    double omega; //!< Parameter for SOR and SSOR fixed omega methods

    ItpackLinSolver(int method, int maxIter = 100, double omega = 1.0);
    ItpackLinSolver(void);
    virtual LinearSOESolver *getCopy(void) const;
    virtual bool setLinearSOE(LinearSOE *theSOE);
  public:
  
    int solve(void);
    int setSize(void);
    bool setLinearSOE(ItpackLinSOE &theSOE);
  
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
  };

inline LinearSOESolver *ItpackLinSolver::getCopy(void) const
   { return new ItpackLinSolver(*this); }
} // end of XC namespace

#endif
