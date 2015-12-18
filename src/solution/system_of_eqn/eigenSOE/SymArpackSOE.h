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
// File: ~/system_of_eqn/linearSOE/LawSolver/SymArpackSOE.h
//
// Written: Jun Peng
// Created: 12/98
// Revision: A
//
// Description: This file contains the class definition for 
// SymArpackSOE.h. It stores the sparse matrix A in a fashion
// that only store the none zero.
//
// What: "@(#) SymArpackSOE.h, revA"
//
// Almost all the information (Matrix A and Vector B) is stored as 
// global variables in the file "symbolic.h".


#ifndef SymArpackSOE_h
#define SymArpackSOE_h

#include <solution/system_of_eqn/eigenSOE/ArpackSOE.h>

extern "C" {
   #include <solution/system_of_eqn/linearSOE/sparseSYM/FeStructs.h>
}



namespace XC {
class SymArpackSolver;

//! @ingroup EigenSOE
//
//! @brief Sistema de ecuaciones correspondiente
//! a un problema de autovalores con matriz simétrica
//! en cuya solución se empleará la biblioteca
//! ArPack (http://www.caam.rice.edu/software/ARPACK)
class SymArpackSOE : public ArpackSOE
  {
  private:
    int nnz; // number of non-zeros in A
    ID colA;
    ID rowStartA;  //These are (ADJNCY, XADJ) pair.

    int      nblks;
    int      *xblk,  *invp;
    double   *diag, **penv;
    int      *rowblks;
    OFFDBLK  **begblk;
    OFFDBLK  *first;
  protected:
    bool setSolver(EigenSolver *);
    bool procesa_comando(CmdStatus &status);

    friend class SoluMethod;
    friend class FEM_ObjectBroker;
    SymArpackSOE(SoluMethod *,double shift = 0.0);        
    SystemOfEqn *getCopy(void) const;
  public:
    virtual ~SymArpackSOE(void);

    virtual int setSize(Graph &theGraph);
    
    virtual int addA(const Matrix &, const ID &, double fact = 1.0);
    virtual int addM(const Matrix &, const ID &, double fact = 1.0);    
      
    virtual void zeroA(void);
    virtual void zeroM(void);
    
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);

    friend class SymArpackSolver;
  };
inline SystemOfEqn *SymArpackSOE::getCopy(void) const
  { return new SymArpackSOE(*this); }
} // end of XC namespace

#endif

