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
                                                                        
// $Revision: 1.3 $
// $Date: 2005/11/29 22:42:41 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/algorithm/equiSolnAlgo/EquiSolnAlgo.cpp,v $
                                                                        
                                                                        
// File: ~/OOP/analysis/algorithm/EquiSolnAlgo.C 
// 
// Written: fmk 
// Created: 11/96 
// Revision: A 
//

// Description: This file contains the class implementation for 
// EquiSolnAlgo. EquiSolnAlgo is an abstract base class, 
// i.e. no objects of it's type can be created.  Its subclasses deifine
// the sequence of operations to be performed in the analysis by static
// equilibrium of a finite element model.  
// 
// What: "@(#)EquiSolnAlgo.C, revA"


#include <solution/analysis/algorithm/equiSolnAlgo/EquiSolnAlgo.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <solution/analysis/integrator/IncrementalIntegrator.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include "solution/SoluMethod.h"


XC::EquiSolnAlgo::EquiSolnAlgo(SoluMethod *owr,int clasTag)
  :SolutionAlgorithm(owr,clasTag) {}

//! @brief Devuelve un puntero al test de convergencia.
const XC::ConvergenceTest *XC::EquiSolnAlgo::getConvergenceTestPtr(void) const
  {
    const SoluMethod *sm= getSoluMethod();
    assert(sm);
    return sm->getConvergenceTestPtr();
  }

//! @brief Devuelve un puntero al test de convergencia.
XC::ConvergenceTest *XC::EquiSolnAlgo::getConvergenceTestPtr(void)
  {
    SoluMethod *sm= getSoluMethod();
    assert(sm);
    return sm->getConvergenceTestPtr();
  }

XC::IncrementalIntegrator *XC::EquiSolnAlgo::getIncrementalIntegratorPtr(void)
  { return dynamic_cast<IncrementalIntegrator *>(getIntegratorPtr()); }
