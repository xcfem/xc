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
// $Date: 2000/09/15 08:23:29 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/bandSPD/BandSPDLinSolver.h,v $
                                                                        
                                                                        
#ifndef BandSPDLinSolver_h
#define BandSPDLinSolver_h

// File: ~/system_of_eqn/linearSOE/bandGEN/BandSPDLinSolver.h
//
// Written: fmk 
// Created: Tue Sep 26 16:27:47: 1996
// Revision: A
//
// Description: This file contains the class definition for BandSPDLinSolver.
// BandSPDLinSolver is an abstract base class and thus no objects of it's type
// can be instantiated. It has pure virtual functions which must be
// implemented in it's derived classes.  Instances of BandSPDLinSolver 
// are used to solve a system of equations of type BandSPDLinSOE.
//
// What: "@(#) BandSPDLinSolver.h, revA"

#include <solution/system_of_eqn/linearSOE/LinearSOESolver.h>
namespace XC {
class BandSPDLinSOE;

//! @ingroup Solver
//
//! @brief Objetos que se encargan de la solución
//! de sistemas de ecuaciones con matriz en banda.
class BandSPDLinSolver : public LinearSOESolver
  {
  protected:
    BandSPDLinSOE *theSOE;

    BandSPDLinSolver(int classTag);    
    virtual bool setLinearSOE(LinearSOE *theSOE);
  public:
    virtual int solve(void) = 0;
    virtual bool setLinearSOE(BandSPDLinSOE &theSOE);
    
  };
} // fin namespace XC

#endif

