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
// $Date: 2005/05/25 23:33:46 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/diagonal/DistributedDiagonalSolver.h,v $

// Written: fmk 
// Created: 05/05
//
// Description: This file contains the class definition for DistributedDiagonalSolver.
// DistributedDiagonalSolver is a concrete class for solving a system stored using
// a DistributedDiagonalSOE. The solve() method is overwritten to allow subclassing.
//
// What: "@(#) DistributedDiagonalSolver.h, revA"

#ifndef DistributedDiagonalSolver_h
#define DistributedDiagonalSolver_h

#include <solution/system_of_eqn/linearSOE/LinearSOESolver.h>
namespace XC {
class DistributedDiagonalSOE;

//! @ingroup Solver
//
//! @brief Objetos que se encargan de la solución
//! de sistemas de ecuaciones con matriz diagonal distribuída.
class DistributedDiagonalSolver : public LinearSOESolver
  {
  private:
    double minDiagTol;
  protected:
    DistributedDiagonalSOE *theSOE;

    friend class LinearSOE;
    DistributedDiagonalSolver(int classTag);    
    DistributedDiagonalSolver(double minDiagTol=1.0e-18);    
    virtual LinearSOESolver *getCopy(void) const;
    virtual bool setLinearSOE(LinearSOE *theSOE);
  public:
    virtual int solve(void);
    virtual int setSize(void);
    virtual bool setLinearSOE(DistributedDiagonalSOE &theSOE);
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);    
  };

inline LinearSOESolver *DistributedDiagonalSolver::getCopy(void) const
   { return new DistributedDiagonalSolver(*this); }
} // end of XC namespace

#endif

