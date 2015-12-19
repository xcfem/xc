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
// File: ~/system_of_eqn/linearSOE/LawSolver/SymSparseLinSOE.h
//
// Written: Jun Peng  (junpeng@stanford.edu)
//          Prof. Kincho H. Law
//          Stanford University
// Created: 12/98
// Revision: A
//
// Description: This file contains the class definition for 
// SymSparseLinSOE.h. It stores the sparse matrix A in a fashion
// that only store the none zero entries.
//
// What: "@(#) SymSparseLinSOE.h, revA"
//
// Almost all the information (Matrix A and Vector B) is stored as 
// global variables in the file "symbolic.h".


#ifndef SymSparseLinSOE_h
#define SymSparseLinSOE_h

#include <solution/system_of_eqn/linearSOE/SparseSOEBase.h>

extern "C" {
   #include <solution/system_of_eqn/linearSOE/sparseSYM/FeStructs.h>
}

namespace XC {
class SymSparseLinSolver;

//! @ingroup SOE
//
//! @brief Sistemas de ecuaciones con matriz dispersa simétrica.
class SymSparseLinSOE : public SparseSOEBase
  {
  private:
    ID colA, rowStartA;  //These are (ADJNCY, XADJ) pair.

    int      LSPARSE;
    int      nblks;
    int      *xblk,  *invp;
    double   *diag, **penv;
    int      *rowblks;
    OFFDBLK  **begblk;
    OFFDBLK  *first;
  protected:
    virtual bool setSolver(LinearSOESolver *);

    friend class SoluMethod;
    SymSparseLinSOE(SoluMethod *,int lSparse= 0);
    SystemOfEqn *getCopy(void) const;
  public:
    ~SymSparseLinSOE(void);

    int setSize(Graph &theGraph);
    int addA(const Matrix &, const ID &, double fact = 1.0);
    int addB(const Vector &, const ID &,const double &fact= 1.0);    
    
    void zeroA(void);

    int setSymSparseLinSolver(SymSparseLinSolver *);    

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);

    friend class SymSparseLinSolver;
  };
inline SystemOfEqn *SymSparseLinSOE::getCopy(void) const
  { return new SymSparseLinSOE(*this); }
} // end of XC namespace

#endif

