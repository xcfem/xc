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
// $Date: 2005/11/29 23:36:47 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/analysis/SubstructuringAnalysis.cpp,v $
                                                                        
                                                                        
// File: ~/analysis/Analysis/SubstructuringAnalysis.C
// 
// Written: fmk 
// Created: Tue Sept 17 16:34:47: 1996
// Revision: A
//
// Description: This file contains the class definition for 
// SubstructuringAnalysis. SubstructuringAnalysis is a subclass 
// of XC::DomainDecompositionAnalysis, it is used to perform the operations
// required of a domain decomposition substructuring method.
//
// What: "@(#) SubstructuringAnalysis.C, revA"

#include <solution/analysis/analysis/SubstructuringAnalysis.h>
#include <solution/analysis/handler/ConstraintHandler.h>
#include <solution/analysis/numberer/DOF_Numberer.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <solution/system_of_eqn/linearSOE/DomainSolver.h>
#include "solution/SoluMethod.h"
#include <solution/analysis/integrator/IncrementalIntegrator.h>
#include "domain/domain/subdomain/Subdomain.h"

//! @brief Constructor.
XC::SubstructuringAnalysis::SubstructuringAnalysis(Subdomain &the_Domain,DomainSolver &theSolver,SoluMethod *s)
  :DomainDecompositionAnalysis(the_Domain,theSolver,s)
  {}

//! @brief Ejecuta el análisis.
int XC::SubstructuringAnalysis::analyze(void)
  {
    std::cerr << "SubstructuringAnalysis::analyze(void)";
    std::cerr << "does nothing and should not have been called\n";
    return -1;
  }

