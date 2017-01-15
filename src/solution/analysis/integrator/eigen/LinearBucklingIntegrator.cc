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
