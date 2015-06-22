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
//LinearBucklingAlgo.cpp

#include <solution/analysis/algorithm/eigenAlgo/LinearBucklingAlgo.h>
#include <solution/analysis/integrator/eigen/LinearBucklingIntegrator.h>
#include <solution/system_of_eqn/eigenSOE/EigenSOE.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <utility/matrix/Vector.h>

//! @brief Devuelve, si puede, un puntero al integrador adecuado.
XC::LinearBucklingIntegrator *XC::LinearBucklingAlgo::getLinearBucklingIntegrator(void)
  {
    LinearBucklingIntegrator *retval= nullptr;
    Integrator *theIntegrator= getIntegratorPtr();
    if(theIntegrator)
      {
        retval= dynamic_cast<LinearBucklingIntegrator *>(theIntegrator);
        if(!retval)
	  std::cerr << "LinearBucklingAlgo::getLinearBucklingIntegrator; el integrador"
                    << " no es del tipo adecuado." << std::endl;
      }
    return retval;
  }

//! @brief Constructor.
XC::LinearBucklingAlgo::LinearBucklingAlgo(SoluMethod *owr)
  :EigenAlgorithm(owr,EigenALGORITHM_TAGS_LinearBuckling) {}

//! @brief Obtiene los autovalores para el paso actual.
int XC::LinearBucklingAlgo::solveCurrentStep(int numModes)
  {
    AnalysisModel *theModel= getAnalysisModelPtr();
    LinearBucklingIntegrator *theIntegrator= getLinearBucklingIntegrator();
    EigenSOE *theSOE = getEigenSOEPtr();
    if((theModel == 0) || (theIntegrator == 0) || (theSOE == 0))
      {
        std::cerr << "WARNING LinearBucklingAlgo::solverCurrentStep() - ";
        std::cerr << " no se ha asignado dominio, modelo o integrador.\n";
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

    eigen_to_model(numModes); //Envia autovectores (modos) y autovalores al modelo.
    return 0;
  }

//! @brief Vuelca los autovalores en el modelo (ver Finite Element Procedures. Klaus Jurgen Bathe página 632).
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

