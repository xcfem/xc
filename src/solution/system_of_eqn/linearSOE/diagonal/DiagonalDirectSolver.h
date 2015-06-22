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
                                                                        
// $Revision: 1.1 $
// $Date: 2005/01/27 22:22:50 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/diagonal/DiagonalDirectSolver.h,v $

// Written: fmk 
// Created: Jan 2005
// Revision: A
//
// Description: This file contains the class definition for 
// DiagonalDirectSolver. DiagonalDirectSolver is a subclass 
// of LinearSOESOlver. It solves diagonal system directly!

// What: "@(#) DiagonalDirectSolver.h, revA"

#ifndef DiagonalDirectSolver_h
#define DiagonalDirectSolver_h

#include <solution/system_of_eqn/linearSOE/diagonal/DiagonalSolver.h>
namespace XC {
class DiagonalSOE;

//! @ingroup Solver
//
//! @brief Objetos que se encargan de la solución
//! de sistemas de ecuaciones con matriz diagonal.
class DiagonalDirectSolver : public DiagonalSolver
  {
  protected:
    double minDiagTol;

    friend class LinearSOE;
    DiagonalDirectSolver(double tol=1.0e-18);    
    virtual LinearSOESolver *getCopy(void) const;
  public:
    virtual int solve(void);        
    virtual int setSize(void);    
    double getDeterminant(void);
    
    virtual int setDiagonalSOE(DiagonalSOE &theSOE);

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
  };

inline LinearSOESolver *DiagonalDirectSolver::getCopy(void) const
   { return new DiagonalDirectSolver(*this); }
} // fin namespace XC


#endif

