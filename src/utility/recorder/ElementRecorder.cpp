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
                                                                        
// $Revision: 1.25 $
// $Date: 2006/01/18 19:43:42 $
// $Source: /usr/local/cvs/OpenSees/SRC/recorder/ElementRecorder.cpp,v $
                                                                        
// Written: fmk 
// Created: 09/99
//
// Description: This file contains the class implementatation of XC::ElementRecorder.
//
// What: "@(#) ElementRecorder.C, revA"

#include <utility/recorder/ElementRecorder.h>
#include "domain/mesh/element/utils/Information.h"
#include <domain/domain/Domain.h>
#include <domain/mesh/element/Element.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/ID.h>
#include <cstring>
#include <utility/recorder/response/Response.h>
#include <utility/database/FE_Datastore.h>
#include <utility/handler/DataOutputHandler.h>
#include <utility/actor/message/Message.h>
#include <utility/actor/channel/Channel.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include "xc_basic/src/text/text_string.h"
#include "boost/lexical_cast.hpp"

XC::ElementRecorder::ElementRecorder()
  :ElementRecorderBase(RECORDER_TAGS_ElementRecorder),data()
  {}

XC::ElementRecorder::ElementRecorder(const ID &ele,const std::vector<std::string> &argv, bool echoTime, 
                                     Domain &theDom, DataOutputHandler &theOutputHandler, double dT)
  :ElementRecorderBase(RECORDER_TAGS_ElementRecorder,ele,argv,echoTime,theDom,theOutputHandler,dT),data()
  {}

XC::ElementRecorder::~ElementRecorder(void)
  {}


int XC::ElementRecorder::record(int commitTag, double timeStamp)
  {
    // 
    // check that initialization has been done
    //

    if(initializationDone == false)
      {
        if(this->initialize() != 0)
          {
            std::cerr << "XC::ElementRecorder::record() - failed to initialize\n";
            return -1;
          }
      }

    assert(theHandler);
    int result = 0;
    if(deltaT == 0.0 || timeStamp >= nextTimeStampToRecord)
      {
        if(deltaT != 0.0) 
          nextTimeStampToRecord = timeStamp + deltaT;
        int loc = 0;
        if(echoTimeFlag == true) 
          data(loc++) = timeStamp;
    
        //
        // for each element if responses exist, put them in response vector
        //
        const size_t numEle= eleID.Size();
        assert(theResponses.size()>=numEle);
        for(size_t i=0; i< numEle; i++)
          { 
            if(theResponses[i])
              {
	        // ask the element for the reponse
	        int res;
	        if(( res = theResponses[i]->getResponse()) < 0)
	          result += res;
	        else
                  {
	            Information &eleInfo = theResponses[i]->getInformation();
	            const Vector &eleData = eleInfo.getData();
	            for(int j=0; j<eleData.Size(); j++)
	              data(loc++) = eleData(j);
	          }
              } 
          }
        //
        // send the response vector to the output handler for o/p
        //

        theHandler->write(data);
      }
    // succesfull completion - return 0
    return result;
  }

int XC::ElementRecorder::restart(void)
  {
    data.Zero();
    return 0;
  }


int XC::ElementRecorder::initialize(void)
  {
    const int numEle= eleID.Size();
    if(numEle == 0 || theDomain == 0)
      {
        if(!theDomain)
	  std::cerr << getClassName() << "::" << __FUNCTION__
	            << "; undefined domain." << std::endl;
        if(numEle == 0)
	  std::cerr << getClassName() << "::" << __FUNCTION__
	            << "; there is no elements." << std::endl;
        return -1;
      }

    // Set the response objects:
    //   1. create an array of pointers for them
    //   2. iterate over the elements invoking setResponse() to get the new objects & determine size of data
    //

    int numDbColumns = 0;
    if(echoTimeFlag == true) 
      numDbColumns = 1;  // 1 for the pseudo-time

    theResponses= std::vector<Response *>(numEle,static_cast<Response *>(nullptr));

    Information eleInfo(1.0);
    int i;
    for(i=0; i<numEle; i++)
      {
        Element *theEle = theDomain->getElement(eleID(i));
        if(theEle == 0)
          { theResponses[i]= 0; }
        else
          {
            theResponses[i] = theEle->setResponse(responseArgs, eleInfo);
            if(theResponses[i] != 0)
              {
	        // from the response type determine no of cols for each
	        Information &eleInfo = theResponses[i]->getInformation();
	        const Vector &eleData = eleInfo.getData();
	        numDbColumns += eleData.Size();
              }
          }
      }

    //
    // now create the columns strings for the data description
    // for each element do a getResponse() 
    //
    std::vector<std::string> dbColumns(numDbColumns);
    static std::string aColumn;

    int counter = 0;
    if(echoTimeFlag == true)
      {
        dbColumns[0] = "time";
        counter = 1;
      }
    
    std::string dataToStore= "";
    const size_t numArgs= getNumArgs();
    for(size_t j=0; j<numArgs; j++)
      {
        dataToStore+= responseArgs[j];
        if(j<(numArgs-1))
          dataToStore+= " ";
      }
    
    for(i=0; i<eleID.Size(); i++)
      {
        int eleTag = eleID(i);
        int numVariables = 0;
        if(theResponses[i]!=nullptr)
          {
            const XC::Information &eleInfo = theResponses[i]->getInformation();
            if(eleInfo.theType == IntType || eleInfo.theType == DoubleType)
              {
	      // create column heading for single data item for element
	      numVariables = 0;
	      //sprintf(aColumn, "Element%d_%s", eleTag, dataToStore);
                aColumn= "Element" + boost::lexical_cast<std::string>(eleTag) + "_" 
		+ dataToStore;
                dbColumns[counter] = aColumn;
	      counter++;
              }
            else if(eleInfo.theType == VectorType) 
	    numVariables = eleInfo.theVector->Size();
            else if(eleInfo.theType == IdType) 
	    numVariables = eleInfo.theID->Size();
            else if(eleInfo.theType == MatrixType) 
	    numVariables = eleInfo.theMatrix->noRows()* eleInfo.theMatrix->noCols();

            // create the column headings for multiple data for the element
            for(int j=1; j<=numVariables; j++)
              {
	      //sprintf(aColumn, "Element%d_%s_%d",eleTag, dataToStore, j);
                aColumn= "Element" + boost::lexical_cast<std::string>(eleTag) + "_" 
		                 + dataToStore + "_"
                                   + boost::lexical_cast<std::string>(j);
                dbColumns[counter] = aColumn;
	      counter++;
              }
          }
      }

    // replace spaces with undescore for tables
    for(int kk=0; kk<numDbColumns; kk++)
      replace(dbColumns[kk],' ','_');

    //
    // call open in the handler with the data description
    //

    theHandler->open(dbColumns);

    // create the vector to hold the data
    data= Vector(numDbColumns);

    initializationDone = true;
    return 0;
  }


int XC::ElementRecorder::sendSelf(CommParameters &cp)
  { return ElementRecorderBase::sendSelf(cp); }

int XC::ElementRecorder::recvSelf(const CommParameters &cp)
  { return ElementRecorderBase::recvSelf(cp); }

