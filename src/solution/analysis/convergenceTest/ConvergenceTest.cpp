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
                                                                        
// $Revision: 1.1.1.1 $
// $Date: 2000/09/15 08:23:17 $
// $Source: /usr/local/cvs/OpenSees/SRC/convergenceTest/ConvergenceTest.cpp,v $
                                                                        
                                                                        
// File: ~/convergenceTest/ConvergenceTest.C
//
// Written: fmk 
// Date: 09/98
// Revised:
//
// Purpose: This file contains the class definition for XC::ConvergenceTest,
// which is an abstract class. Objects of concrete subclasses can be used 
// to test the convergence of an algorithm. 

#include <solution/analysis/convergenceTest/ConvergenceTest.h>
#include <solution/analysis/algorithm/equiSolnAlgo/EquiSolnAlgo.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>

#include "solution/SoluMethod.h"

XC::ConvergenceTest::ConvergenceTest(EntCmd *owr,int clasTag)
  :MovableObject(clasTag), EntWOwner(owr), currentIter(0), maxNumIter(0),
   printFlag(0), nType(2), norms(1), lastRatio(0.0), calculatedNormX(0.0), calculatedNormB(0.0) {}

XC::ConvergenceTest::ConvergenceTest(EntCmd *owr,int clasTag,int maxIter,int prtFlg, int normType, int sz_norms)
  :MovableObject(clasTag), EntWOwner(owr), currentIter(0), maxNumIter(maxIter),
   printFlag(prtFlg), nType(normType), norms(sz_norms), lastRatio(0.0), calculatedNormX(0.0), calculatedNormB(0.0) {}

XC::ConvergenceTest* XC::ConvergenceTest::getCopy(int iterations) const
  {
    ConvergenceTest *theCopy= getCopy();
    theCopy->maxNumIter= iterations;
    return theCopy;
  }

int XC::ConvergenceTest::getNumTests(void) const
  { return currentIter; }


int XC::ConvergenceTest::getMaxNumTests(void) const
  { return maxNumIter; }

const XC::Vector& XC::ConvergenceTest::getNorms(void) const 
  { return norms; }

double XC::ConvergenceTest::getRatioNumToMax(void) const
  {
    double div= maxNumIter;
    return currentIter/div;
  }

int XC::ConvergenceTest::getMaxNumIter(void) const
  { return maxNumIter; }
void XC::ConvergenceTest::setMaxNumIter(const int &i)
  {
    maxNumIter= i;
    norms= Vector(maxNumIter);
  }
int XC::ConvergenceTest::getCurrentIter(void) const
  { return currentIter; }
void XC::ConvergenceTest::setCurrentIter(const int &i)
  { currentIter= i; }
int XC::ConvergenceTest::getPrintFlag(void) const
  { return printFlag; }
void XC::ConvergenceTest::setPrintFlag(const int &i)
  { printFlag= i; }
int XC::ConvergenceTest::getNormType(void) const
  { return nType; }
void XC::ConvergenceTest::setNormType(const int &i)
  { nType= i; }


int XC::ConvergenceTest::start(void)
  {
    int retval= 0;
    if(hasLinearSOE())
      {
        // set iteration count = 1
        currentIter = 1;
        norms.Zero();
      }
    else
      {
        std::cerr << "WARNING: ConvergenceTest::start() - no SOE returning true\n";
        retval= -1;
      }
    return retval;
  }

//! @brief Devuelve un puntero al modelo de solución al que pertenece.
XC::SoluMethod *XC::ConvergenceTest::getSoluMethod(void)
  { return dynamic_cast<SoluMethod *>(Owner()); }

//! @brief Devuelve un puntero al modelo de solución al que pertenece.
const XC::SoluMethod *XC::ConvergenceTest::getSoluMethod(void) const
  { return dynamic_cast<const SoluMethod *>(Owner()); }

//! @brief Devuelve verdadero si puede acceder al sistema de ecuaciones.
bool XC::ConvergenceTest::hasLinearSOE(void) const
  { return (getLinearSOEPtr()!=nullptr); }

//! @brief Devuelve un puntero al modelo de solución al que pertenece.
XC::LinearSOE *XC::ConvergenceTest::getLinearSOEPtr(void)
  {
    SoluMethod *sm= getSoluMethod();
    assert(sm);
    return sm->getLinearSOEPtr();
  }

//! @brief Devuelve un puntero al modelo de solución al que pertenece.
const XC::LinearSOE *XC::ConvergenceTest::getLinearSOEPtr(void) const
  {
    const SoluMethod *sm= getSoluMethod();
    assert(sm);
    return sm->getLinearSOEPtr();
  }

//! @brief Devuelve el vector de incógnitas.
const XC::Vector &XC::ConvergenceTest::getX(void) const
  { return getLinearSOEPtr()->getX(); }

//! @brief Devuelve la norma del vector de incógnitas.
double XC::ConvergenceTest::getNormX(void) const
  { return getLinearSOEPtr()->getX().pNorm(nType); }

//! @brief Devuelve el vector de términos independientes.
const XC::Vector &XC::ConvergenceTest::getB(void) const
  { return getLinearSOEPtr()->getB(); }

//! @brief Devuelve la norma del vector de términos independientes.
double XC::ConvergenceTest::getNormB(void) const
 { return getLinearSOEPtr()->getB().pNorm(nType); }

//! @brief Returns the the energy increment,
//! which is 0.5 times the absolute value of the product of the rhs and 
//! the solution vector of the LinearSOE: |0.5*(x ^ b)|.
double XC::ConvergenceTest::getEnergyProduct(void) const
  {
    double retval= getX() ^ getB();
    if(retval < 0.0)
      retval*= -0.5;
    else
      retval*= 0.5;
    return retval;
  }

//! @brief Send members del objeto through the channel being passed as parameter.
int XC::ConvergenceTest::sendData(CommParameters &cp)
  {
    //setDbTagDataPos(0,getTag());
    int res= cp.sendInts(currentIter,maxNumIter,printFlag,nType,getDbTagData(),CommMetaData(1));
    res+= cp.sendVector(norms,getDbTagData(),CommMetaData(2));
    return res;
  }

//! @brief Receives members del objeto through the channel being passed as parameter.
int XC::ConvergenceTest::recvData(const CommParameters &cp)
  {
    //setTag(getDbTagDataPos(0));
    int res= cp.receiveInts(currentIter,maxNumIter,printFlag,nType,getDbTagData(),CommMetaData(1));
    res+= cp.receiveVector(norms,getDbTagData(),CommMetaData(2));
    return res;
  }

int XC::ConvergenceTest::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(3);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << nombre_clase() << "sendSelf() - failed to send data\n";
    return res;
  }


int XC::ConvergenceTest::recvSelf(const CommParameters &cp)
  {
    inicComm(3);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << nombre_clase() << "::recvSelf - failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
          std::cerr << nombre_clase() << "::recvSelf - failed to receive data.\n";
      }
    return res;
  }

//! @brief Returns a string with the name of the class and the iteration number.
std::string XC::ConvergenceTest::getTestIterationMessage(void) const
  {
    return nombre_clase() + "::test() - iteration: "+std::to_string(currentIter);
  }

//! @brief Returns a string with the proper failed to converge message.
std::string XC::ConvergenceTest::getFailedToConvergeMessage(void) const
  {
    std::ostringstream retval; 
    retval << "WARNING: " << nombre_clase()
	   << "::test() - failed to converge \n"
           << "after: " << currentIter << " iterations\n";	
    return retval.str();
  }

//! @brief Returns a string with the values of x and b vectors.
std::string XC::ConvergenceTest::getDeltaXRMessage(void) const
  {
    std::ostringstream retval; 
    retval << "\tdeltaX: " << getX() << "\tdeltaR: " << getB();
    return retval.str();    
  }

//! @brief Returns a string with the values of x and b vector norms.
std::string XC::ConvergenceTest::getDeltaXRNormsMessage(void) const
  {
    std::ostringstream retval; 
    retval << "\tNorm deltaX: " << calculatedNormX << "\tNorm deltaR: " << calculatedNormB;
    return retval.str();    
  }


