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
//LinearBucklingIntegrator.cpp


#include "solution/analysis/integrator/eigen/LinearBucklingIntegrator.h"
#include <solution/analysis/model/fe_ele/FE_Element.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <solution/system_of_eqn/eigenSOE/EigenSOE.h>
#include <solution/analysis/model/FE_EleIter.h>

//! @brief Constructor.
XC::LinearBucklingIntegrator::LinearBucklingIntegrator(SoluMethod *owr)
  :EigenIntegrator(owr) {}

//! @brief Ensambla la matriz de rigidez en el paso siguiente a formKt()
//! y la guarda en la matriz A del SOE.
int XC::LinearBucklingIntegrator::formKtplusDt(void)
  { return EigenIntegrator::formK(); }

//! @brief Ensambla la matriz de rigidez en el paso previo
//! y la guarda en la matriz M del SOE.
int XC::LinearBucklingIntegrator::formKt(void)
  {
    EigenSOE *theSOE= getEigenSOEPtr();
    AnalysisModel *mdl= getAnalysisModelPtr();
    if(!mdl || !theSOE)
      {
	std::cerr << "WARNING XC::LinearBucklingIntegrator::formKtplusDt -";
	std::cerr << " no AnalysisModel or EigenSOE has been set\n";
	return -1;
      }
    
    // the loops to form and add the tangents are broken into two for 
    // efficiency when performing parallel computations

    // loop through the FE_Elements getting them to form the tangent
    // FE_EleIter &theEles1 = mdl->getFEs();
    FE_Element *elePtr= nullptr;
    flagK= 0; //matriz de masas (ver formEleTangent)
    theSOE->zeroM(); //hace nula la matriz M.

    // while((elePtr = theEles1()) != 0) 
    //     elePtr->formTangent(this);
   
     // loop through the FE_Elements getting them to add the tangent    
    int result = 0;
    FE_EleIter &theEles2= mdl->getFEs();    
    while((elePtr= theEles2()) != 0)
      {
	if(theSOE->addM(elePtr->getTangent(this), elePtr->getID()) < 0) //guarda en M.
          {
	    std::cerr << "WARNING XC::LinearBucklingIntegrator::formKt;";
	    std::cerr << " failed in addM for ID " << elePtr->getID();	    
	    result = -2;
	  }
      }
    return result;    
  }


void XC::LinearBucklingIntegrator::Print(std::ostream &s, int flag)
  {
    s << "\t XC::LinearBucklingIntegrator: \n";
  }
