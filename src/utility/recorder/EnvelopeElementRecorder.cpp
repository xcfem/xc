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
                                                                        
// $Revision: 1.15 $
// $Date: 2006/01/18 19:43:42 $
// $Source: /usr/local/cvs/OpenSees/SRC/recorder/EnvelopeElementRecorder.cpp,v $
                                                                        
// Written: fmk 
//
// Description: This file contains the class implementatation of 
// EnvelopeElementRecorder.
//
// What: "@(#) EnvelopeElementRecorder.C, revA"

#include <utility/recorder/EnvelopeElementRecorder.h>
#include <domain/domain/Domain.h>
#include <domain/mesh/element/Element.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/ID.h>
#include <cstring>
#include <utility/recorder/response/Response.h>
#include <utility/database/FE_Datastore.h>
#include <domain/mesh/element/utils/Information.h>
#include <utility/handler/DataOutputHandler.h>
#include <cstdlib>

#include <utility/actor/message/Message.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include "xc_basic/src/texto/cadena_carac.h"
#include "boost/lexical_cast.hpp"
#include "utility/actor/actor/MatrixCommMetaData.h"

//! @brief Constructor.
XC::EnvelopeElementRecorder::EnvelopeElementRecorder(void)
  :ElementRecorderBase(RECORDER_TAGS_EnvelopeElementRecorder),
   envelope() {}

//! @brief Constructor.
XC::EnvelopeElementRecorder::EnvelopeElementRecorder(const XC::ID &theEleID, 
                                                     const std::vector<std::string> &argv, 
                                                     Domain &theDom, 
                                                     DataOutputHandler &theOutputHandler,
                                                     double dT, bool echoTime)
  :ElementRecorderBase(RECORDER_TAGS_EnvelopeElementRecorder,theEleID,argv,echoTime,theDom,theOutputHandler,dT), envelope() {}

//! @brief Destructor.
XC::EnvelopeElementRecorder::~EnvelopeElementRecorder(void)
  {
    //
    // write the data
    //
    Vector *currentData= envelope.getCurrentData();
    Matrix *data= envelope.getData();
    if(theHandler && currentData != 0)
      {
        for(int i=0;i<3;i++)
          {
            int size = currentData->Size();
            for(int j=0; j<size; j++)
              (*currentData)(j) = (*data)(i,j);
            theHandler->write(*currentData);
          }
      }
  }


int XC::EnvelopeElementRecorder::record(int commitTag, double timeStamp)
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

    int result = 0;
    Matrix* data= envelope.getData(); assert(data);
    Vector* currentData= envelope.getCurrentData(); assert(currentData);
    if(deltaT == 0.0 || timeStamp >= nextTimeStampToRecord)
      {
        if(deltaT != 0.0) 
          nextTimeStampToRecord = timeStamp + deltaT;
      
        int loc = 0;
        // for each element do a getResponse() & put the result in current data
        const int numEle= eleID.Size();
        for(int i=0; i< numEle; i++)
          {
            if(theResponses[i] != 0)
              {
                // ask the element for the reponse
                int res;
                if(( res = theResponses[i]->getResponse()) < 0)
                  result += res;
                else
                  {
                    // from the response determine no of cols for each
                    Information &eleInfo= theResponses[i]->getInformation();
                    const Vector &eleData= eleInfo.getData();
                    for(int j=0; j<eleData.Size(); j++) 
                      (*currentData)(loc++) = eleData(j);
                  }
              }
          }
        if(echoTimeFlag == false) {
        // check if max or min
        // check if currentData modifies the saved data
	int size= currentData->Size();
        if(envelope.First()) {
          for(int i=0; i<size; i++) {
            (*data)(0,i) = (*currentData)(i);
            (*data)(1,i) = (*currentData)(i);
            (*data)(2,i) = fabs((*currentData)(i));
            envelope.First() = false;
          } 
        } else {
          for(int i=0; i<size; i++) {
            double value = (*currentData)(i);
            if((*data)(0,i) > value) {
              (*data)(0,i) = value;
              double absValue = fabs(value);
              if((*data)(2,i) < absValue) 
                (*data)(2,i) = absValue;
            } else if((*data)(1,i) < value) {
              (*data)(1,i) = value;
              double absValue = fabs(value);
              if((*data)(2,i) < absValue) 
                (*data)(2,i) = absValue;
            }
          }
        }
      } else {
        // check if max or min
        // check if currentData modifies the saved data
        int size = currentData->Size();
        size /= 2;
        if(envelope.First()) {
          for(int i=0; i<size; i++) {
            (*data)(0,i*2) = timeStamp;
            (*data)(1,i*2) = timeStamp;
            (*data)(2,i*2) = timeStamp;
            (*data)(0,i*2+1) = (*currentData)(i);
            (*data)(1,i*2+1) = (*currentData)(i);
            (*data)(2,i*2+1) = fabs((*currentData)(i));
            envelope.First() = false;
          } 
        } else {
          for(int i=0; i<size; i++) {
            double value = (*currentData)(i);
            if((*data)(0,2*i+1) > value) {
              (*data)(0,2*i) = timeStamp;
              (*data)(0,2*i+1) = value;
              double absValue = fabs(value);
              if((*data)(2,2*i+1) < absValue) {
                (*data)(2,2*i) = timeStamp;
                (*data)(2,2*i+1) = absValue;
              }
            } else if((*data)(1,2*i+1) < value) {
              (*data)(1,2*i) = timeStamp;
              (*data)(1,2*i+1) = value;
              double absValue = fabs(value);
              if((*data)(2,2*i+1) < absValue) {
                (*data)(2,2*i) = timeStamp;
                (*data)(2,2*i+1) = absValue;
              }
            }
          }
        }
      }
  }
    // succesfull completion - return 0
    return result;
}

int XC::EnvelopeElementRecorder::restart(void)
  { return envelope.restart(); }

//! @brief Envía el objeto a través del comunicador que
//! se pasa como parámetro.
int XC::EnvelopeElementRecorder::sendData(CommParameters &cp)
  {
    int res= ElementRecorderBase::sendData(cp);
    res+= cp.sendMovable(envelope,getDbTagData(),CommMetaData(14));
    return res;
  }

//! @brief Recibe el objeto a través del comunicador que
//! se pasa como parámetro.
int XC::EnvelopeElementRecorder::receiveData(const CommParameters &cp)
  {
    int res= ElementRecorderBase::receiveData(cp);
    res+= cp.receiveMovable(envelope,getDbTagData(),CommMetaData(14));
    return res;
  }

//! @brief Envía el objeto a través del comunicador que
//! se pasa como parámetro.
int XC::EnvelopeElementRecorder::sendSelf(CommParameters &cp)
  {
    int res= 0;
    if(cp.isDatastore() == 1)
      std::cerr << "EnvelopeElementRecorder::sendSelf() - does not send data to a datastore\n";
    else
      {
        setDbTag(cp);
        const int dataTag= getDbTag();
        inicComm(15);
        res= sendData(cp);
        if(cp.sendIdData(getDbTagData(),dataTag)< 0)
          {
            std::cerr << "EnvelopeElementRecorder::sendSelf() "
                      << "- failed to send idData\n";
            return -1;
          }
      }
    return res;
  }


//! @brief Recibe el objeto a través del comunicador que
//! se pasa como parámetro.
int XC::EnvelopeElementRecorder::recvSelf(const CommParameters &cp)
  {
    int res= 0;
    if(cp.isDatastore() == 1)
      std::cerr << "EnvelopeElementRecorder::recvSelf() - does not recv data to a datastore\n";
    else
      {
        inicComm(15);
        const int dataTag= getDbTag();
        res= cp.receiveIdData(getDbTagData(),dataTag);
        if(res < 0)
          {
            std::cerr << "EnvelopeElementRecorder::recvSelf() - failed to recv idData\n";
            return res;
          }
        res= receiveData(cp);
      }
    return res;
  }


int XC::EnvelopeElementRecorder::initialize(void) 
  {
    const int numEle= eleID.Size();
    if(numEle == 0 || theDomain == 0)
      return -1;

    // Set the response objects:
    //   1. create an array of pointers for them
    //   2. iterate over the elements invoking setResponse() to get the new objects & determine size of data
    //

    theResponses= std::vector<Response *>(numEle,static_cast<Response *>(nullptr));

    Information eleInfo(1.0);
    int numDbColumns = 0;

    for(int ii=0; ii<numEle; ii++) {
      Element *theEle = theDomain->getElement(eleID(ii));
      if(theEle == 0) {
        theResponses[ii] = 0;
      } else {
        theResponses[ii]= theEle->setResponse(responseArgs, eleInfo);
        if(theResponses[ii] != 0)
          {
            // from the response type determine no of cols for each      
            Information &eleInfo = theResponses[ii]->getInformation();
            const Vector &eleData = eleInfo.getData();
            numDbColumns += eleData.Size();
          }
      }
    }

    //
    // create the matrix & vector that holds the data
    //

    if(echoTimeFlag == true)
      numDbColumns *= 2;

    envelope.alloc(numDbColumns);

    //
    // now create the columns strings for the database
    // for each element do a getResponse() & print the result
    //

    std::vector<std::string> dbColumns(numDbColumns);
    static std::string aColumn; 
    
    std::string dataToStore= "";
    const size_t numArgs= getNumArgs();
    for(size_t j=0; j<numArgs; j++)
      {
        dataToStore+= responseArgs[j];
        if(j<(numArgs-1))
          dataToStore+= " ";
      }

    int counter = 0;
    for(int i=0; i<eleID.Size(); i++)
      {
        int eleTag = eleID(i);
        int numVariables = 0;
        if(theResponses[i]!= 0)
          {
            const XC::Information &eleInfo = theResponses[i]->getInformation();
        
            if(eleInfo.theType == IntType || eleInfo.theType == DoubleType)
              {
                // create column heading for single data item for element
                if(echoTimeFlag == true)
                  {
                    dbColumns[counter] = "time";
                    counter++;
                  }

                numVariables = 0;
                //sprintf(aColumn, "Element%d_%s", eleTag, dataToStore);
                aColumn= "Element" + boost::lexical_cast<std::string>(eleTag) + "_" 
		+ dataToStore;
                dbColumns[counter]= aColumn;
                counter++;
              }
            else if(eleInfo.theType == VectorType) 
              numVariables = eleInfo.theVector->Size();
            else if(eleInfo.theType == IdType) 
              numVariables = eleInfo.theID->Size();

            // create the column headings for multiple data for the element
            for(int j=1; j<=numVariables; j++)
              {
                if(echoTimeFlag == true)
                  {
                    dbColumns[counter]= "time";
                    counter++;
                  }
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

    initializationDone = true;  
    return 0;
  }
  
