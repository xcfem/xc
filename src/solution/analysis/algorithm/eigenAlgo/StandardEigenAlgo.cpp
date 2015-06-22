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
                                                                        
// $Revision: 1.2 $
// $Date: 2003/02/14 23:00:41 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/algorithm/eigenAlgo/StandardEigenAlgo.cpp,v $
                                                                        
// Written: MHS
// Created: Oct 2001
//
// Description: This file contains the class definition of XC::StandardEigenAlgo.
// StandardEigenAlgo is a class which performs a eigen solution algorithm
// to solve standard eigenvalue equations. It is not expected that 
// this class will have subclasses.

#include <solution/analysis/algorithm/eigenAlgo/StandardEigenAlgo.h>
#include <solution/analysis/analysis/EigenAnalysis.h>
#include <solution/analysis/integrator/EigenIntegrator.h>
#include <solution/system_of_eqn/eigenSOE/EigenSOE.h>


//! @brief Constructor.
XC::StandardEigenAlgo::StandardEigenAlgo(SoluMethod *owr)
  :EigenAlgorithm(owr,EigenALGORITHM_TAGS_Standard) {}

//! @brief Obtiene los autovalores para el paso actual.
int XC::StandardEigenAlgo::solveCurrentStep(int numModes)
  {
    AnalysisModel *theModel= getAnalysisModelPtr();
    EigenIntegrator *theIntegrator= getEigenIntegratorPtr();
    EigenSOE *theSOE = getEigenSOEPtr();
    if((theModel == 0) || (theIntegrator == 0) || (theSOE == 0))
      {
        std::cerr << "StandardEigenAlgo::solverCurrentStep() -- no se ha asignado dominio, modelo o integrador.\n";
        return -1;
      }
  
    if(theIntegrator->formK() < 0)
      {
        std::cerr << "XC::StandardEigenAlgo::solverCurrentStep() -- the Integrator failed in formK()\n";
        return -2;
      }
  
    if(theSOE->solve(numModes) < 0)
      {
        std::cerr << "XC::StandardEigenAlgo::solverCurrentStep() -- the EigenSOE failed in solve()\n";
        return -4;
      }
  
    // now set the eigenvalues and eigenvectors in the model
    eigen_to_model(numModes);
    return 0;
  }

//! @brief Envía.
int XC::StandardEigenAlgo::sendSelf(CommParameters &cp)
  { return 0; }

//! @brief Recibe.
int XC::StandardEigenAlgo::recvSelf(const CommParameters &cp)
  { return 0; }

//! @brief Imprime el objeto.
void XC::StandardEigenAlgo::Print(std::ostream &s, int flag)
  {
    s << "\tStandardEigenAlgo\n";
  }
