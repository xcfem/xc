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
** file 'COPYRIGHT'  in XC::main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.1.1.1 $
// $Date: 2000/09/15 08:23:28 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/bandGEN/BandGenLinSolver.cpp,v $
                                                                        
                                                                        
// File: ~/system_of_eqn/linearSOE/bandGEN/BandGenLinSolver.C
//
// Written: fmk 
// Created: Tue Sep 26 16:27:47: 1996
// Revision: A
//
// Description: This file contains the class definition for XC::BandGenLinSolver.
// BandGenLinSolver is an abstract base class and thus no objects of it's type
// can be instantiated. It has pure virtual functions which must be
// implemented in it's derived classes.  Instances of XC::BandGenLinSolver 
// are used to solve a system of equations of type XC::BandGenLinSOE.
//
// What: "@(#) BandGenLinSolver.C, revA"

#include <solution/system_of_eqn/linearSOE/bandGEN/BandGenLinSolver.h>
#include <solution/system_of_eqn/linearSOE/bandGEN/BandGenLinSOE.h>

XC::BandGenLinSolver::BandGenLinSolver(int classTags)    
  : LinearSOESolver(classTags), theSOE(nullptr) {}    

bool XC::BandGenLinSolver::setLinearSOE(LinearSOE *soe)
  {
    bool retval= false;
    BandGenLinSOE *tmp= dynamic_cast<BandGenLinSOE *>(soe);
    if(tmp)
      {
        theSOE= tmp;
        retval= true;
      }
    else
      std::cerr  << nombre_clase() << "::setLinearSOE: el sistema de ecuaciones no es del tipo adecuado para este solver." << std::endl;
    return retval;
  }

bool XC::BandGenLinSolver::setLinearSOE(BandGenLinSOE &theBandGenSOE)
  { return setLinearSOE(&theBandGenSOE); }

