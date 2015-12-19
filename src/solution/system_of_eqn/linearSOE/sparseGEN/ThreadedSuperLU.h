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
// $Date: 2002/01/25 20:27:11 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/sparseGEN/ThreadedSuperLU.h,v $
                                                                        
                                                                        
#ifndef ThreadedSuperLU_h
#define ThreadedSuperLU_h

// File: ~/system_of_eqn/linearSOE/sparseGEN/ThreadedSuperLU.h
//
// Written: fmk 
// Created: 11/96
// Revision: A
//
// Description: This file contains the class interface for ThreadedSuperLU.
// This is a class that uses the threads version of SuperLU
//
// What: "@(#) ThreadedSuperLU.h, revA"

#include <solution/system_of_eqn/linearSOE/sparseGEN/SparseGenColLinSolver.h>
#include <solution/system_of_eqn/linearSOE/sparseGEN/pdsp_defs.h>
#include <superlu/supermatrix.h>

namespace XC {

//! @ingroup LinearSolver
//
//! @brief Objeto encargado de obtener la solución
//! de un sistemas de ecuaciones lineal con matriz
//! dispersa no simétrica, empleando programación multihilo.
class ThreadedSuperLU : public SparseGenColLinSolver
  {
  private:
    SuperMatrix A,L,U,B,AC;
    ID perm_r;
    ID perm_c;
    ID etree;
    int sizePerm;
    int relax, permSpec, panelSize;
    float thresh;

    int numThreads;
    pdgstrf_options_t pdgstrf_options;
    yes_no_t refact, usepr;
    fact_t fact;
    trans_t trans;
    void *work;
    int lwork;
    Gstat_t gStat;
  public:
    ThreadedSuperLU(int numThreads = 2,
		    int permSpec = 0, int panelSize = 6, 
		    int relax = 6, double thresh = 0.0);     

    ~ThreadedSuperLU();

    int solve(void);
    int setSize(void);

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);        
  };
} // end of XC namespace

#endif


