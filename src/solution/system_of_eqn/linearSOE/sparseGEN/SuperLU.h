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
// $Date: 2005/03/17 20:47:09 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/sparseGEN/SuperLU.h,v $
                                                                        
                                                                        
#ifndef SuperLU_h
#define SuperLU_h

// File: ~/system_of_eqn/linearSOE/sparseGEN/SuperLU.h
//
// Written: fmk 
// Created: 11/96
//
// Description: This file contains the class definition for SuperLU.
// A SuperLU object can be constructed to solve a SparseGenColLinSOE
// object. It obtains the solution by making calls on the
// the SuperLU library developed at UC Berkeley by Prof. James Demmel, 
// Xiaoye S. Li and John R. Gilbert.
// The SuperLU library contains a set of subroutines to solve a sparse
// linear system  $AX=B$. It uses Gaussian elimination with partial
// pivoting (GEPP). The columns of A may be preordered before
// factorization; the preordering for sparsity is completely separate
// from the factorization and a number of ordering schemes are provided. 
//
// What: "@(#) SuperLU.h, revA"

#include <solution/system_of_eqn/linearSOE/sparseGEN/SparseGenColLinSolver.h>
//#include "superlu/slu_cdefs.h"
#include "superlu/slu_ddefs.h"
#include "superlu/supermatrix.h"

namespace XC {

//! @ingroup LinearSolver
//
//! @brief Objetos que se encargan de obtener la solución
//! de sistemas de ecuaciones lineales
//! empleando la biblioteca SuperLU (http://crd.lbl.gov/~xiaoye/SuperLU).
class SuperLU : public SparseGenColLinSolver
  {
  private:
    SuperMatrix A,B,AC;
    SuperMatrix L,U;
    ID perm_r;
    ID perm_c;
    ID etree;
    int relax, permSpec, panelSize;
    double drop_tol;
    char symmetric;
    superlu_options_t options;
    void libera_matricesLU(void);
    void libera_matricesABAC(void);
    void libera_matrices(void);
    void libera(void);
    void inic_permutation_vectors(const size_t &n);
    void alloc_matrices(const size_t &n);
    void alloc(const size_t &n);
    int factoriza(void);

    friend class LinearSOE;
    friend class FEM_ObjectBroker;
    SuperLU(int permSpec = 0, double drop_tol = 0.0, int panelSize = 6, int relax = 6,char symmetric = 'N');
    SuperLU(const SuperLU &otro);
    SuperLU &operator=(const SuperLU &otro);
    virtual LinearSOESolver *getCopy(void) const;
  public:
    ~SuperLU(void);

    int solve(void);
    int setSize(void);

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);

    void Print(std::ostream &os) const;
  };
inline LinearSOESolver *SuperLU::getCopy(void) const
   { return new SuperLU(*this); }
} // end of XC namespace

#endif

