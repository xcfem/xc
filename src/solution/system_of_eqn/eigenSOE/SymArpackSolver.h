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
// File: ~/system_of_eqn/linearSOE/LawSolver/SymArpackSolver.h
//
// Written: Jun Peng
// Created: 2/1999
// Revision: A
//
// Description: This file contains the class definition for 
// SymArpackSolver. It solves the SymArpackSOE object by calling
// some "C" functions. The solver used here is generalized sparse
// solver. The user can choose three different ordering schema.
//
// What: "@(#) SymArpackSolver.h, revA"

#ifndef SymArpackSolver_h
#define SymArpackSolver_h

#include <solution/system_of_eqn/eigenSOE/EigenSolver.h>
#include "utility/matrix/Vector.h"

namespace XC {
class SymArpackSOE;

//! @ingroup LinearSolver
//
//! @brief Objetos que se encargan de obtener la solución
//! de sistemas de ecuaciones lineales con matriz simétrica
//! empleando la biblioteca arcpack
//! (http://www.caam.rice.edu/software/ARPACK).
class SymArpackSolver : public EigenSolver
  {
  private:
    SymArpackSOE *theSOE;
    bool factored;

    Vector value;
    Vector vector;
    mutable Vector eigenV;
    
    void myMv(int n, double *v, double *result);
    void myCopy(int n, double *v, double *result);
    int getNCV(int n, int nev);

    friend class EigenSOE;
    SymArpackSolver(int numE = 0);     
    virtual EigenSolver *getCopy(void) const;
    bool setEigenSOE(EigenSOE *theSOE);
  public:
    virtual int solve(void);
    virtual int solve(int numModes) {return this->solve();};    
    virtual int setSize(void);
    const int &getSize(void) const;

    virtual bool setEigenSOE(SymArpackSOE &theSOE); 
	
    virtual const Vector &getEigenvector(int mode) const;
    virtual const double &getEigenvalue(int mode) const;
    
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
  };

inline EigenSolver *SymArpackSolver::getCopy(void) const
   { return new SymArpackSolver(*this); }
} // end of XC namespace

#endif

