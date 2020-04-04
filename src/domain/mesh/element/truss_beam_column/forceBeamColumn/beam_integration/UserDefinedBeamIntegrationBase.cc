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

// $Revision: 1.4 $
// $Date: 2003/06/10 00:36:09 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/forceBeamColumn/UserDefinedBeamIntegrationBase.cpp,v $

#include "UserDefinedBeamIntegrationBase.h"
#include <utility/matrix/Vector.h>
#include <domain/mesh/element/utils/Information.h>
#include "domain/component/Parameter.h"

XC::UserDefinedBeamIntegrationBase::UserDefinedBeamIntegrationBase(int classTag,
						       const Vector &pt,
						       const Vector &wt)
  : BeamIntegration(classTag), pts(pt), wts(wt)
  {
    const size_t sz= pt.Size();
    for(size_t i= 0; i < sz; i++)
      {
        if(pt(i) < 0.0 || pt(i) > 1.0)
          std::cerr << "UserDefinedBeamIntegrationBase::UserDefinedBeamIntegrationBase -- point lies outside [0,1]" << std::endl;
        if(wt(i) < 0.0 || wt(i) > 1.0)
          std::cerr << "UserDefinedBeamIntegrationBase::UserDefinedBeamIntegrationBase -- weight lies outside [0,1]" << std::endl;
      }
  }

XC::UserDefinedBeamIntegrationBase::UserDefinedBeamIntegrationBase(int classTag,
						       const Vector &pt)
  : BeamIntegration(classTag), pts(pt), wts(pt.Size())
  {
    const size_t sz= pt.Size();
    for(size_t i= 0; i < sz; i++)
      {
        if(pt(i) < 0.0 || pt(i) > 1.0)
          std::cerr << "UserDefinedBeamIntegrationBase::UserDefinedBeamIntegrationBase -- point lies outside [0,1]" << std::endl;
      }
  }

XC::UserDefinedBeamIntegrationBase::UserDefinedBeamIntegrationBase(int classTag):
  BeamIntegration(classTag)
  {}

void XC::UserDefinedBeamIntegrationBase::getSectionLocations(int numSections,double L, double *xi) const
  {
    const int nIP = pts.Size();

    int i;
    for(i = 0; i < nIP; i++)
      xi[i] = pts(i);
    for( ; i < numSections; i++)
      xi[i] = 0.0;
  }

void XC::UserDefinedBeamIntegrationBase::getSectionWeights(int numSections,double L, double *wt) const
  {
    const int nIP = wts.Size();

    int i;
    for(i = 0; i < nIP; i++)
      wt[i] = wts(i);
    for(;i<numSections;i++)
      wt[i] = 1.0;
  }

//! @brief Send object members through the communicator argument.
int XC::UserDefinedBeamIntegrationBase::sendData(Communicator &comm)
  {
    //setDbTagDataPos(0,getTag()); Not tagged.
    int res= comm.sendVector(pts,getDbTagData(),CommMetaData(1));
    res+= comm.sendVector(wts,getDbTagData(),CommMetaData(2));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::UserDefinedBeamIntegrationBase::recvData(const Communicator &comm)
  {
    //setTag(getDbTagDataPos(0)); Not tagged.
    int res= comm.receiveVector(pts,getDbTagData(),CommMetaData(1));
    res+= comm.receiveVector(wts,getDbTagData(),CommMetaData(2));
    return res;
  }

int XC::UserDefinedBeamIntegrationBase::sendSelf(Communicator &comm)
  { return -1; }

int XC::UserDefinedBeamIntegrationBase::recvSelf(const Communicator &comm)
  { return -1; }

//! @brief Assigns to the param the value identified by the string in argv.
int XC::UserDefinedBeamIntegrationBase::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
    int retval= -1;
    const size_t sz= argv.size();
    if(sz>2)
      {
	const int point= atoi(argv[1]);
	if(point>=1)
	  {
  	    const int Np= wts.Size();
	    if(argv[0]=="pt")
	      {
	        param.setValue(pts(point-1));
	        retval= param.addObject(point, this);
	      }
	    else if((argv[0]=="wt") && (point <= Np))
	      {
	        param.setValue(wts(point-1));
	        retval= param.addObject(10+point, this);
	      }
          }
      }
    return retval;
  }

//! @brief Update the value of the parameter.
int XC::UserDefinedBeamIntegrationBase::updateParameter(int parameterID,Information &info)
  {
    if(parameterID <= 10)
      { // pt
        pts(parameterID-1) = info.theDouble;
        return 0;
      }
    else if(parameterID <= 20)
      { // wt
        wts(parameterID-10-1) = info.theDouble;
        return 0;
      }
    else
      return -1;
  }

//! @brief Print stuff.
void XC::UserDefinedBeamIntegrationBase::Print(std::ostream &s, int flag) const
  {
    s << " Points: " << pts;
    s << " Weights: " << wts;
  }
