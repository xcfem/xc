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
//NewtonBased.cpp

#include <solution/analysis/algorithm/equiSolnAlgo/NewtonBased.h>

//! @brief Constructor
XC::NewtonBased::NewtonBased(AnalysisAggregation *owr,int classTag,int theTangentToUse)
  :EquiSolnAlgo(owr,classTag), tangent(theTangentToUse) {}

//! @brief Send object members through the channel being passed as parameter.
int XC::NewtonBased::sendData(CommParameters &cp)
  {
    int res= EquiSolnAlgo::sendData(cp);
    res+= cp.sendDouble(tangent,getDbTagData(),CommMetaData(2));
    return res;
  }

//! @brief Receives object members through the channel being passed as parameter.
int XC::NewtonBased::recvData(const CommParameters &cp)
  {
    int res= EquiSolnAlgo::recvData(cp);
    double tmp= tangent;
    res+= cp.receiveDouble(tmp,getDbTagData(),CommMetaData(2));
    tangent= tmp;
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::NewtonBased::sendSelf(CommParameters &cp)
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

//! @brief Receives object through the channel being passed as parameter.
int XC::NewtonBased::recvSelf(const CommParameters &cp)
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
          std::cerr << getClassName()  << "::" << __FUNCTION__
		    << "; failed to receive data.\n";
      }
    return res;
  }
