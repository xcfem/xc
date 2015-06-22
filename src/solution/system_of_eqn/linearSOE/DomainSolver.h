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
                                                                        
// $Revision: 1.1.1.1 $
// $Date: 2000/09/15 08:23:27 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/DomainSolver.h,v $
                                                                        
                                                                        
// File: ~/system_of_eqn/linearSOE/DomainSolver.h
//
// Written: fmk 
// Created: Tue Sep 26 16:27:47: 1996
// Revision: A
//
// Description: This file contains the class definition for DomainSolver.
// DomainSolver is an abstract base class and thus no objects of it's type
// can be instantiated. It has pure virtual functions which must be
// implemented in it's derived classes.  Instances of DomainSolver are used
// to solve a system of equations and to do static condensation operations on the
// linear system of equations.
//
// What: "@(#) DomainSolver.h, revA"

#ifndef DomainSolver_h
#define DomainSolver_h

#include <solution/system_of_eqn/linearSOE/LinearSOESolver.h>
namespace XC {
class Matrix;
class Vector;

//! @ingroup LinearSolver
//
//! @brief Objetos que se encargan de obtener la solución
//! de sistemas de ecuaciones lineales.
class DomainSolver: public virtual LinearSOESolver
  {
  protected:
    DomainSolver(int classTag);
  public:

    virtual int condenseA(int numInt) =0;
    virtual int condenseRHS(int numInt, Vector *u= NULL) =0;
    virtual int computeCondensedMatVect(int numInt, const Vector &u) =0;
    virtual const Matrix &getCondensedA(void) =0;
    virtual const Vector &getCondensedRHS(void) =0;
    virtual const Vector &getCondensedMatVect(void) =0;

    virtual int setComputedXext(const Vector &) =0;
    virtual int solveXint(void) =0;
  };
} // fin namespace XC

#endif

