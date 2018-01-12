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

#include "solution/AnalysisAggregation.h"

//! @brief Default constructor.
//!
//! @param owr: object that owns this one.
//! @param classTag: class identifier.
XC::ConvergenceTest::ConvergenceTest(EntCmd *owr,int classTag)
  :MovableObject(classTag), EntWOwner(owr), currentIter(0), maxNumIter(0),
   printFlag(0), nType(2), norms(1), lastRatio(0.0), calculatedNormX(0.0), calculatedNormB(0.0) {}

//! @brief Constructor.
//!
//! @param owr: object that owns this one.
//! @param classTag: class identifier.
//! @param theTol: the tolerance used in test().
//! @param maxIter the max number of iterations to be performed.
//! @param printFlag: what, if anything, is printed on each test.
//! @param normType: type of norm to use (1-norm, 2-norm, p-norm, max-norm).
//! @param sz_norms: size of the vector that contains computed norms.
XC::ConvergenceTest::ConvergenceTest(EntCmd *owr,int classTag,int maxIter,int prtFlg, int normType, int sz_norms)
  :MovableObject(classTag), EntWOwner(owr), currentIter(0), maxNumIter(maxIter),
   printFlag(prtFlg), nType(normType), norms(sz_norms), lastRatio(0.0), calculatedNormX(0.0), calculatedNormB(0.0) {}

//! @brief Virtual constructor.
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

//! @brief Return the current iteration number.
int XC::ConvergenceTest::getCurrentIter(void) const
  { return currentIter; }
//! @brief Set the current iteration number.
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

//! This is invoked at the start of each iteration. To return
//! {\em 0} if sucessfull, i.e that testing can proceed, a
//! negative number if not.
//! Sets an integer indicating the current number of iterations, {\em
//! currentNumIter} to \f$1\f$. returns \f$0\f$ if successfull, an error message
//! and \f$-1\f$ are returned if no LinearSOE object has been set.
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
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; WARNING: - no SOE returning true\n";
        retval= -1;
      }
    return retval;
  }

//! @brief Returns a pointer to the solution method that owns this object.
XC::AnalysisAggregation *XC::ConvergenceTest::getAnalysisAggregation(void)
  { return dynamic_cast<AnalysisAggregation *>(Owner()); }

//! @brief Returns a const pointer to the solution method that owns this object.
const XC::AnalysisAggregation *XC::ConvergenceTest::getAnalysisAggregation(void) const
  { return dynamic_cast<const AnalysisAggregation *>(Owner()); }

//! @brief Returns true ifpuede acceder al system of equations.
bool XC::ConvergenceTest::hasLinearSOE(void) const
  { return (getLinearSOEPtr()!=nullptr); }

//! @brief Returns a pointer to the system of equations.
XC::LinearSOE *XC::ConvergenceTest::getLinearSOEPtr(void)
  {
    AnalysisAggregation *sm= getAnalysisAggregation();
    assert(sm);
    return sm->getLinearSOEPtr();
  }

//! @brief Returns a const pointer to the system of equations.
const XC::LinearSOE *XC::ConvergenceTest::getLinearSOEPtr(void) const
  {
    const AnalysisAggregation *sm= getAnalysisAggregation();
    assert(sm);
    return sm->getLinearSOEPtr();
  }

//! @brief Returns the vector de incógnitas.
const XC::Vector &XC::ConvergenceTest::getX(void) const
  { return getLinearSOEPtr()->getX(); }

//! @brief Return the norma del vector de incógnitas.
double XC::ConvergenceTest::getNormX(void) const
  { return getLinearSOEPtr()->getX().pNorm(nType); }

//! @brief Return the vector at the right hand side of the equation.
const XC::Vector &XC::ConvergenceTest::getB(void) const
  { return getLinearSOEPtr()->getB(); }

//! @brief Return the norm of the vector at the right hand side of
//! the equation.
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

//! @brief Send object members through the channel being passed as parameter.
int XC::ConvergenceTest::sendData(CommParameters &cp)
  {
    //setDbTagDataPos(0,getTag());
    int res= cp.sendInts(currentIter,maxNumIter,printFlag,nType,getDbTagData(),CommMetaData(1));
    res+= cp.sendVector(norms,getDbTagData(),CommMetaData(2));
    return res;
  }

//! @brief Receives object members through the channel being passed as parameter.
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
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to send data\n";
    return res;
  }


int XC::ConvergenceTest::recvSelf(const CommParameters &cp)
  {
    inicComm(3);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
          std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to receive data.\n";
      }
    return res;
  }

//! @brief Returns a string with the name of the class and the iteration number.
std::string XC::ConvergenceTest::getTestIterationMessage(void) const
  {
    return getClassName() + "::" + __FUNCTION__ + " - iteration: "+std::to_string(currentIter);
  }

//! @brief Returns a string with the proper failed to converge message.
std::string XC::ConvergenceTest::getFailedToConvergeMessage(void) const
  {
    std::ostringstream retval; 
    retval << "WARNING: " << getClassName()
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


