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
//ConvergenceTestTol.cc


#include <solution/analysis/convergenceTest/ConvergenceTestTol.h>


//! @brief Constructor.
//!
//! @param owr: object that owns this one.
//! @param classTag: identifier of the object class.
XC::ConvergenceTestTol::ConvergenceTestTol(CommandEntity *owr,int classTag)	    	
  : ConvergenceTest(owr,classTag), tol(0.0) {}


//! @brief Constructor.
//!
//! @param owr: object that owns this one.
//! @param classTag: identifier of the object class.
//! @param theTol: the tolerance used in test().
//! @param maxIter the max number of iterations to be performed.
//! @param printFlag: what, if anything, is printed on each test.
//! @param normType: type of norm to use (1-norm, 2-norm, p-norm, max-norm).
XC::ConvergenceTestTol::ConvergenceTestTol(CommandEntity *owr,int classTag,double theTol, int maxIter, int printFlag, int normType,int sz_norms)
  : ConvergenceTest(owr,classTag,maxIter,printFlag,normType,sz_norms), tol(theTol) {}

//! Sets the tolerance used in test() to be \p newTol.
void XC::ConvergenceTestTol::setTolerance(double newTol)
  { tol = newTol; }

//! Gets the tolerance used in test().
double XC::ConvergenceTestTol::getTolerance(void) const
  { return tol; }


//! @brief Send object members through the communicator argument.
int XC::ConvergenceTestTol::sendData(Communicator &comm)
  {
    int res= ConvergenceTest::sendData(comm);
    res+= comm.sendDouble(tol,getDbTagData(),CommMetaData(3));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::ConvergenceTestTol::recvData(const Communicator &comm)
  {
    int res= ConvergenceTest::recvData(comm);
    res+= comm.receiveDouble(tol,getDbTagData(),CommMetaData(3));
    return res;
  }

int XC::ConvergenceTestTol::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(4);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "sendSelf() - failed to send data\n";
    return res;
  }


int XC::ConvergenceTestTol::recvSelf(const Communicator &comm)
  {
    inicComm(4);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << getClassName() << "::recvSelf - failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(comm);
        if(res<0)
          std::cerr << getClassName() << "::recvSelf - failed to receive data.\n";
      }
    return res;
  }

//! @brief Display current ratio and tolerance.
std::string XC::ConvergenceTestTol::getRatioMessage(const std::string &ratioExpression) const
  {
    std::ostringstream retval; 
    retval << " current ratio " << ratioExpression
	   <<": " << lastRatio << " (max: " << tol
	   << ", Norm deltaX: " << calculatedNormX << ")";
    return retval.str();
  }

//! @brief Display current displacement increment and tolerance.
std::string XC::ConvergenceTestTol::getDispIncrMessage(void) const
  {
    std::ostringstream retval; 
    retval << " current |x|: " << calculatedNormX
	   << " (max: " << tol << ", Norm deltaR: "
	   << calculatedNormB << ")";
    return retval.str();
  }

//! @brief Display current unbalance and tolerance.
std::string XC::ConvergenceTestTol::getUnbalanceMessage(void) const
  {
    std::ostringstream retval; 
    retval << " current |b|: " << calculatedNormB
	   << " (max: " << tol << ", Norm deltaX: "
	   << calculatedNormX << ")";
    return retval.str();
  }

//! @brief Display current energy product and tolerance.
std::string XC::ConvergenceTestTol::getEnergyProductMessage(void) const
  {
    std::ostringstream retval; 
    calculatedNormX= getNormX(); //Update values.
    calculatedNormB= getNormB();
    retval << " current EnergyIncr: " << calculatedEnergyProduct
	   << " (max: " << tol << getDeltaXRNormsMessage() << ")";
    return retval.str();
  }
