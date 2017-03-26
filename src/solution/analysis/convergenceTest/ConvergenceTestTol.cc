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
XC::ConvergenceTestTol::ConvergenceTestTol(EntCmd *owr,int clasTag)	    	
  : ConvergenceTest(owr,clasTag), tol(0.0) {}


//! @brief Constructor.
XC::ConvergenceTestTol::ConvergenceTestTol(EntCmd *owr,int clasTag,double theTol, int maxIter, int printIt, int normType,int sz_norms)
  : ConvergenceTest(owr,clasTag,maxIter,printIt,normType,sz_norms), tol(theTol) {}

void XC::ConvergenceTestTol::setTolerance(double newTol)
  { tol = newTol; }

double XC::ConvergenceTestTol::getTolerance(void) const
  { return tol; }


//! @brief Send object members through the channel being passed as parameter.
int XC::ConvergenceTestTol::sendData(CommParameters &cp)
  {
    int res= ConvergenceTest::sendData(cp);
    res+= cp.sendDouble(tol,getDbTagData(),CommMetaData(3));
    return res;
  }

//! @brief Receives object members through the channel being passed as parameter.
int XC::ConvergenceTestTol::recvData(const CommParameters &cp)
  {
    int res= ConvergenceTest::recvData(cp);
    res+= cp.receiveDouble(tol,getDbTagData(),CommMetaData(3));
    return res;
  }

int XC::ConvergenceTestTol::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(4);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << nombre_clase() << "sendSelf() - failed to send data\n";
    return res;
  }


int XC::ConvergenceTestTol::recvSelf(const CommParameters &cp)
  {
    inicComm(4);
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
