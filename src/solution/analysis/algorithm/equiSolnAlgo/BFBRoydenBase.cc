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
//BFBRoydenBase.cc
                                                                        
#include <solution/analysis/algorithm/equiSolnAlgo/BFBRoydenBase.h>
#include <solution/analysis/convergenceTest/ConvergenceTest.h>

void XC::BFBRoydenBase::libera(void)
  {
    if(localTest)
      {
        delete localTest;
        localTest= nullptr;
      }
  }

void XC::BFBRoydenBase::alloc(const ConvergenceTest *theT)
  {
    libera();
    localTest= theT->getCopy(numberLoops);
    if(!localTest)
      std::cerr << "BFBRoydenBase::alloc() - could not get a copy\n";
  }

//! @brief Constructor
XC::BFBRoydenBase::BFBRoydenBase(SoluMethod *owr,int classTag,int theTangentToUse, int n)
  :EquiSolnConvAlgo(owr,classTag,nullptr), localTest(nullptr) , tangent(theTangentToUse), numberLoops(n),s(n+3),z(n+3)
  {}

//! @brief Constructor
XC::BFBRoydenBase::BFBRoydenBase(SoluMethod *owr,int classTag,ConvergenceTest &theT, int theTangentToUse, int n)
  :EquiSolnConvAlgo(owr,classTag,&theT), localTest(nullptr), tangent(theTangentToUse), numberLoops(n),s(n+3),z(n+3)
  {
    alloc(theTest);
  }

//! @brief Destructor
XC::BFBRoydenBase::~BFBRoydenBase(void)
  { libera(); }

//! @brief Asigna el test de convergencia que empleará el algoritmo.
int XC::BFBRoydenBase::setConvergenceTest(ConvergenceTest *nwTest)
  {
    theTest= nwTest;
    alloc(nwTest);
    return 0;
  }
