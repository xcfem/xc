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
// File: ~/system_of_eqn/linearSOE/LawSolver/SymSparseLinSolver.h
//
// Written: Jun Peng  (junpeng@stanford.edu)
//          Prof. Kincho H. Law
//          Stanford University
// Created: 12/98
// Revision: A
//
// Description: This file contains the class definition for 
// SymSparseinSolver. It solves the SymSparseLinSOEobject by calling
// some "C" functions. The solver used here is generalized sparse
// solver. The user can choose three different ordering schema.
//
// What: "@(#) SymSparseLinSolver.h, revA"


#ifndef SymSparseLinSolver_h
#define SymSparseLinSolver_h

#include <solution/system_of_eqn/linearSOE/LinearSOESolver.h>


namespace XC {
class SymSparseLinSOE;

//! @ingroup Solver
//
//! @brief Objetos que se encargan de la solución
//! de sistemas de ecuaciones con matriz dispersa
//! simétrica.
class SymSparseLinSolver : public LinearSOESolver
  {
  private:
    SymSparseLinSOE *theSOE;

    friend class LinearSOE;
    SymSparseLinSolver();     
    virtual LinearSOESolver *getCopy(void) const;
    virtual bool setLinearSOE(LinearSOE *theSOE);
  public:

    int solve(void);
    int setSize(void);

    bool setLinearSOE(SymSparseLinSOE &theSOE); 
	
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    
  };

inline LinearSOESolver *SymSparseLinSolver::getCopy(void) const
   { return new SymSparseLinSolver(*this); }
} // end of XC namespace

#endif

