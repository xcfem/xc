//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
//  XC is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//LinearBucklingAlgo.cpp

#include <solution/analysis/algorithm/eigenAlgo/LinearBucklingAlgo.h>
#include <solution/analysis/integrator/eigen/LinearBucklingIntegrator.h>
#include <solution/system_of_eqn/eigenSOE/EigenSOE.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <utility/matrix/Vector.h>

//! @brief Returns, si puede, a pointer al integrator adecuado.
XC::LinearBucklingIntegrator *XC::LinearBucklingAlgo::getLinearBucklingIntegrator(void)
  {
    LinearBucklingIntegrator *retval= nullptr;
    Integrator *theIntegrator= getIntegratorPtr();
    if(theIntegrator)
      {
        retval= dynamic_cast<LinearBucklingIntegrator *>(theIntegrator);
        if(!retval)
	  std::cerr << "LinearBucklingAlgo::getLinearBucklingIntegrator; el integrator"
                    << " no es del tipo adecuado." << std::endl;
      }
    return retval;
  }

//! @brief Constructor.
XC::LinearBucklingAlgo::LinearBucklingAlgo(SoluMethod *owr)
  :EigenAlgorithm(owr,EigenALGORITHM_TAGS_LinearBuckling) {}

//! @brief Obtiene los eigenvalues para el paso actual.
int XC::LinearBucklingAlgo::solveCurrentStep(int numModes)
  {
    AnalysisModel *theModel= getAnalysisModelPtr();
    LinearBucklingIntegrator *theIntegrator= getLinearBucklingIntegrator();
    EigenSOE *theSOE = getEigenSOEPtr();
    if((theModel == 0) || (theIntegrator == 0) || (theSOE == 0))
      {
        std::cerr << "WARNING LinearBucklingAlgo::solverCurrentStep() - ";
        std::cerr << " no se ha asignado domain, modelo o integrator.\n";
        return -1;
      }

    if(theIntegrator->formKtplusDt()<0) //Forma la matriz de rigidez.
      {
        std::cerr << "WARNING LinearBucklingAlgo::solverCurrentStep() - ";
        std::cerr << "the Integrator failed in formKtplusDt().\n";
        return -3;
      }

    if(theSOE->solve(numModes) < 0) //Calcula los modos propios.
      {
        std::cerr << "Warning LinearBucklingAlgo::solveCurrentStep() - ";
        std::cerr << "the EigenSOE failed in solve().\n";
        return -4;
      }

    eigen_to_model(numModes); //Envia eigenvectors (modos) y eigenvalues al modelo.
    return 0;
  }

//! @brief Vuelca los eigenvalues en el modelo (ver Finite Element Procedures. Klaus Jurgen Bathe página 632).
void XC::LinearBucklingAlgo::eigen_to_model(int numModes)
  {
    AnalysisModel *theModel= getAnalysisModelPtr();
    theModel->setNumEigenvectors(numModes);
    Vector theEigenvalues(numModes);
    EigenSOE *theSOE = getEigenSOEPtr();
    for(int i= 1;i<=numModes;i++)
      {
        const double ev= theSOE->getEigenvalue(i);
        const double denom= 1.0-ev;
        if(denom!=0.0)
          theEigenvalues[i-1]= 1.0/denom;
        else
          {
	    std::cerr << "Error en LinearBucklingAlgo::eigen_to_model;" 
                      << "theSOE.eigenvalue(" << i << ")= "
                      << ev << std::endl;
	    theEigenvalues[i-1]= 1e99;
          }
        theModel->setEigenvector(i, theSOE->getEigenvector(i));
      }
    theModel->setEigenvalues(theEigenvalues);
  }

//! @brief Imprime el objeto.
void XC::LinearBucklingAlgo::Print(std::ostream &s, int flag)
  { s << "\t Linear Buckling Algorithm \n"; }

//! @brief Envía.
int XC::LinearBucklingAlgo::sendSelf(CommParameters &cp)
  { return 0; }

//! @breif Recibe.
int XC::LinearBucklingAlgo::recvSelf(const CommParameters &cp)
  { return 0; }

