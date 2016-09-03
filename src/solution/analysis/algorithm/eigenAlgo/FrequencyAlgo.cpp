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
// $Date: 2003/02/14 23:00:41 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/algorithm/eigenAlgo/FrequencyAlgo.cpp,v $
                                                                        
                                                                        
// File: ~/analysis/algorithm/eigenAlgo/FrequencyAlgo.C
//
// Written: Jun Peng
// Created: Mon Feb. 8, 1999
// Revision: A
//
// Description: This file contains the class definition of XC::FrequencyAlgo.
// FrequencyAlgo is a class which performs a eigen solution algorithm
// to solve the Generalized eigen equations. It is not expected that 
// this class will have subclasses.
//
// This class is inheritanted from the base class of XC::SolutionAlgorithm
// which was created by fmk (Frank).


#include <solution/analysis/algorithm/eigenAlgo/FrequencyAlgo.h>
#include <solution/analysis/analysis/EigenAnalysis.h>
#include <solution/analysis/integrator/EigenIntegrator.h>
#include <solution/system_of_eqn/eigenSOE/EigenSOE.h>

//! @brief Constructor.
XC::FrequencyAlgo::FrequencyAlgo(SoluMethod *owr)
  :EigenAlgorithm(owr,EigenALGORITHM_TAGS_Frequency) {}

//! @brief Obtiene los autovalores para el paso actual.
int XC::FrequencyAlgo::solveCurrentStep(int nModes)
  {
    AnalysisModel *theModel= getAnalysisModelPtr();
    EigenIntegrator *theIntegrator= getEigenIntegratorPtr();
    EigenSOE *theSOE = getEigenSOEPtr();

    if((theModel == 0) || (theIntegrator == 0) || (theSOE == 0))
      {
        std::cerr << "WARNING FrequencyAlgo::solverCurrentStep() - ";
        std::cerr << " no se ha asignado dominio, modelo o integrador.\n";
        return -1;
      }

    if(theIntegrator->formK()<0) //Forma la matriz de rigidez tangente.
      {
        std::cerr << "WARNING FrequencyAlgo::solverCurrentStep() - ";
        std::cerr << "the Integrator failed in formK().\n";
        return -2;
      }

    if(theIntegrator->formM()<0) //Forma la matriz de masas.
      {
        std::cerr << "WARNING FrequencyAlgo::solverCurrentStep() - ";
        std::cerr << "the Integrator failed in formM().\n";
        return -3;
      }

    if(theSOE->solve(nModes) < 0) //Calcula los modos propios.
      {
        std::cerr << "Warning XC::FrequencyAlgo::solveCurrentStep() - ";
        std::cerr << "the XC::EigenSOE failed in solve().\n";
        return -4;
      }

    eigen_to_model(nModes); //Envia autovectores (modos) y autovalores al modelo.
    return 0;
  }

//! @brief Envía.
int XC::FrequencyAlgo::sendSelf(CommParameters &cp)
  { return 0; }

//! @breif Recibe.
int XC::FrequencyAlgo::recvSelf(const CommParameters &cp)
  { return 0; }

//! @brief Imprime el objeto.
void XC::FrequencyAlgo::Print(std::ostream &s, int flag)
  { s << "\t Eigen Algorithm \n"; }


