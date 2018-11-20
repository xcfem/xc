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
                                                                        
// $Revision: 1.2 $
// $Date: 2003/02/14 23:00:48 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/LoadPath.cpp,v $
                                                                        
                                                                        
// File: ~/analysis/integrator/LoadPath.h
// 
// Written: fmk 
// Created: 07/98
// Revision: A
//
// Description: This file contains the class definition for XC::LoadPath.
// LoadPath is an algorithmic class for performing a static analysis
// using a load control integration scheme.
//
// What: "@(#) LoadPath.h, revA"


#include <solution/analysis/integrator/static/LoadPath.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <utility/matrix/ID.h>
#include <cstdlib>
#include "utility/actor/actor/ArrayCommMetaData.h"

//! @brief Constructor.
//! 
//! The integer INTEGRATOR\_TAGS\_LoadPath (defined in
//! \f$<\f$classTags.h\f$>\f$) is passed to the StaticIntegrator classes
//! constructor. A vector object \p path is created which is a copy of
//! \p loadPath and an index into this vector is set to \f$0\f$.
XC::LoadPath::LoadPath(AnalysisAggregation *owr,const Vector &theLoadPath)
  :StaticIntegrator(owr,INTEGRATOR_TAGS_LoadPath), loadPath(theLoadPath),
   currentStep(0) {}

//! @brief Constructor.
//!
//! The integer INTEGRATOR\_TAGS\_LoadPath (defined in \f$<\f$classTags.h\f$>\f$) is
//! passed to the StaticIntegrator classes constructor. No vector object
//! is created. Provided for the FEM\_ObjectBroker to create a blank
//! object, recvSelf() should be invoked on this object.
XC::LoadPath::LoadPath(AnalysisAggregation *owr)
  :StaticIntegrator(owr,INTEGRATOR_TAGS_LoadPath), currentStep(0) {}

//! @brief Virtual constructor.
XC::Integrator *XC::LoadPath::getCopy(void) const
  { return new LoadPath(*this); }

//! The object obtains the current value of \f$\lambda\f$ from the path
//! vector using the current index. The index is then incremented by
//! \f$1\f$. If the index is greater than the size of path, \f$\lambda\f$ is
//! set to \f$0\f$ and an error message is printed. It will then invoke
//! {\em applyLoadDomain(0.0, \f$\lambda\f$)} on the AnalysisModel
//! object. Returns \f$0\f$ if successful. A warning message is printed and a
//! \f$-1\f$ is returned if no AnalysisModel is associated with the object.
int XC::LoadPath::newStep(void)
  {
    AnalysisModel *theModel = this->getAnalysisModelPtr();    
    if(theModel == 0)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; no associated AnalysisModel.\n";
	return -1;
      }
    
    const double modelLambda= getCurrentModelTime();

    double currentLambda= 0.0;
    if(currentStep < loadPath.Size())
      {
      
        if(currentStep > 0)
          {
	    if(modelLambda == loadPath(currentStep-1))
	      currentLambda= loadPath(currentStep);  
            else
	      currentLambda= loadPath(currentStep-1);  
          }
        else
	  currentLambda= loadPath(currentStep);  
      }      
    else
      {
	currentLambda= 0.0;
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; reached end of specified load path"
		  << " - setting lambda = 0.0 \n";
      }
    currentStep++;
    applyLoadModel(currentLambda);
    return 0;
  }
    
//! Invoked this causes the object to first increment the DOF\_Group
//! displacements with \f$\Delta U\f$, by invoking
//! {\em incrDisp(\f$\Delta U)\f$}! on the AnalysisModel, and then to update
//! the domain, by invoking {\em updateDomain()} on the AnalysisModel. Returns
//! \f$0\f$ if successful, a warning message and a \f$-1\f$ is returned if
//! no AnalysisModel is associated with the object.
int XC::LoadPath::update(const Vector &deltaU)
  {
    AnalysisModel *myModel = this->getAnalysisModelPtr();
    if(myModel == 0)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING no AnalysisModel has been set.\n";
	return -1;
      }

    myModel->incrDisp(deltaU);    
    updateModel();
    return 0;
  }

//! @brief Send object members through the channel being passed as parameter.
int XC::LoadPath::sendData(CommParameters &cp)
  {
    int res= StaticIntegrator::sendData(cp);
    res+= cp.sendInt(currentStep,getDbTagData(),CommMetaData(1));
    res+= cp.sendVector(loadPath,getDbTagData(),CommMetaData(2));
    return res;
  }

//! @brief Receives object members through the channel being passed as
//! parameter.
int XC::LoadPath::recvData(const CommParameters &cp)
  {
    int res= StaticIntegrator::recvData(cp);
    res+= cp.receiveInt(currentStep,getDbTagData(),CommMetaData(1));
    res+= cp.receiveVector(loadPath,getDbTagData(),CommMetaData(2));
    return res;
  }

//! Places the size if \p path and the index in an ID of size 5 and send this
//! ID. Then sends the Vector \p path. Returns \f$0\f$ if successful, a
//! warning message is printed and a \f$-1\f$ is returned if \p theChannel
//! fails to send the ID or the Vector. 
int XC::LoadPath::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(5);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to send data.\n";
    return res;
  }


//! Receives in a ID of size 2 the size of the vector and current index.
//! Creates a new Vector and receives the Vector from the Channel. 
//! Returns \f$0\f$ if successful, a warning message is printed and a \f$-1\f$
//! is returned if \p theChannel fails to receive the Vector or the ID.
int XC::LoadPath::recvSelf(const CommParameters &cp)
  {
    inicComm(5);
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


