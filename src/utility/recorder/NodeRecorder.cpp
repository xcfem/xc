//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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

// $Revision: 1.24 $
// $Date: 2006/01/18 19:43:42 $
// $Source: /usr/local/cvs/OpenSees/SRC/recorder/NodeRecorder.cpp,v $

// Written: fmk 
//
// Description: This file contains the class definition for XC::NodeRecorder.
// A XC::NodeRecorder is used to record the specified dof responses 
// at a collection of nodes over an analysis. (between commitTag of 0 and
// last commitTag).
//
// What: "@(#) NodeRecorder.C, revA"

#include <utility/recorder/NodeRecorder.h>
#include <domain/domain/Domain.h>
#include <domain/mesh/node/Node.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/ID.h>
#include <utility/matrix/Matrix.h>
#include <utility/database/FE_Datastore.h>
#include <utility/handler/DataOutputHandler.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <deque>
#include "utility/utils/text/text_string.h"
#include "boost/lexical_cast.hpp"
#include "domain/component/Parameter.h"

//! @brief store copy of dof's to be recorder, verifying dof are valid, i.e. >= 0
void XC::NodeRecorder::setup_dofs(const ID &dofs)
  {
    const int numDOF = dofs.Size();
    if(numDOF != 0)
      {
        theDofs = new ID(numDOF);
        int count = 0;
        int i;
        for(i=0; i<numDOF; i++)
          {
            int dof = dofs(i);
            if(dof >= 0)
              {
                (*theDofs)[count] = dof;
                count++;
              }
            else
              {
                std::cerr << getClassName() << "::" << __FUNCTION__
			  << "; invalid dof  " << dof
			  << " will be ignored\n";
              }
          }
      }
  }

//! @brief create memory to hold nodal XC::ID's (need parallel).
void XC::NodeRecorder::setup_nodes(const ID &nodes)
  {
    const int numNode = nodes.Size();
    if(numNode != 0)
      {
        theNodalTags = new ID(nodes);
        if(theNodalTags == 0)
          std::cerr << getClassName() << "::" << __FUNCTION__
	            << "; out of memory\n";
      }
  }

//! @brief set the data flag used as a switch to get the response in a record
void XC::NodeRecorder::setupDataFlag(const std::string &dataToStore)
  {
    std::deque<std::string> fields= separa_cadena(dataToStore," ");
    int intValue= -1;
    if(fields.size()>1) intValue= boost::lexical_cast<int>(fields[1]);

    if(dataToStore.size() == 0 || (fields[0]== "disp"))
      { dataFlag = 0; }
    else if((fields[0]== "vel"))
      { dataFlag = 1; }
    else if((fields[0]== "accel"))
      { dataFlag = 2; }
    else if((fields[0]== "incrDisp"))
      { dataFlag = 3; }
    else if((fields[0]== "incrDeltaDisp"))
      { dataFlag = 4; }
    else if((fields[0]== "unbalance"))
      { dataFlag = 5; }
    else if((fields[0]== "unbalanceInclInertia"))
      { dataFlag = 6; }
    else if((fields[0]== "reaction"))
      { dataFlag = 7; }
    else if( (fields[0]== "reactionIncInertia") || 
              (fields[0]== "reactionIncludingInertia"))
      { dataFlag = 8; }
    else if(fields[0] == "eigen")
      {
        int mode= intValue;
        if(mode > 0)
          dataFlag = 10 + mode;
        else
          dataFlag = 6;
      }
    else if(fields[0] == "sensitivity")
      {
       if(fields.size()>11)
	  {
	    const int paramTag= boost::lexical_cast<int>(fields[11]);
	    Parameter *theParameter= theDomain->getParameter(paramTag);
	    int grad= -1;
	    if(theParameter)
	      grad= theParameter->getGradIndex();
	    if(grad > 0)
	      dataFlag= 1000 + grad;
	    else
	      dataFlag= 10;
	  }
      }
    else if(fields[0] == "velSensitivity")
      {
        if(fields.size()>14)
	  {
	    const int paramTag= boost::lexical_cast<int>(fields[14]);
	    Parameter *theParameter= theDomain->getParameter(paramTag);
	    int grad= -1;
	    if(theParameter)
	      grad= theParameter->getGradIndex();
	    if(grad > 0)
	      dataFlag= 2000 + grad;
	    else
	      dataFlag= 10;
	  }
      }
    else if(fields[0] == "accSensitivity")
      {	
        if(fields.size()>14)
	  {
	    const int paramTag= boost::lexical_cast<int>(fields[14]);
	    Parameter *theParameter= theDomain->getParameter(paramTag);
	    int grad= -1;
	    if(theParameter)
	      grad= theParameter->getGradIndex();
	    if(grad > 0)
	      dataFlag = 3000 + grad;
	    else
	      dataFlag = 6;
	  }
      }
    else
      {
        dataFlag = 6;
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; dataToStore " << dataToStore
		  << "not recognized (disp, vel, accel, incrDisp, incrDeltaDisp)\n";
      }
  }

XC::NodeRecorder::NodeRecorder(void)
  :NodeRecorderBase(RECORDER_TAGS_NodeRecorder),
   response(0),gradIndex(-1)
  {}

XC::NodeRecorder::NodeRecorder(const ID &dofs, const ID &nodes, 
			       int pgradIndex, const std::string &dataToStore,
                               Domain &theDom, DataOutputHandler &theOutputHandler,
                               double dT, bool timeFlag)
  :NodeRecorderBase(RECORDER_TAGS_NodeRecorder,dofs,nodes,theDom,theOutputHandler,dT,timeFlag),
   response(1 + nodes.Size()*dofs.Size()), 
   gradIndex(pgradIndex)
  {
    setup_dofs(dofs);
    setup_nodes(nodes);
    setupDataFlag(dataToStore);
  }

int XC::NodeRecorder::record(int commitTag, double timeStamp)
  {
    if(theDomain == 0 || theNodalTags == 0 || theDofs == 0)
      { return 0; }
    if(theHandler == 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; no DataOutputHandler has been set.\n";
        return -1;
      }
    if(initializationDone != true) 
    if(this->initialize() != 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; failed in initialize().\n";
        return -1;
      }

    int numDOF = theDofs->Size();
    if(deltaT == 0.0 || timeStamp >= nextTimeStampToRecord)
      {
        if(deltaT != 0.0) 
          nextTimeStampToRecord= timeStamp + deltaT;

        //
        // if need nodal reactions get the domain to calculate them
        // before we iterate over the nodes
        //
        if(dataFlag == 7)
          theDomain->calculateNodalReactions(false,1e-4);
        else if(dataFlag == 8)
          theDomain->calculateNodalReactions(true,1e-4);

        //
        // add time information if requested
        //

        int timeOffset = 0;
        if(echoTimeFlag == true)
          {
            timeOffset = 1;
            response(0)= timeStamp;
          }

        //
        // now we go get the responses from the nodes & place them in disp vector
        //
        for(int i=0; i<numValidNodes; i++)
          {
            int cnt = i*numDOF + timeOffset; 
            const Node *theNode = theNodes[i];
            if(dataFlag == 0)
              {
                // AddingSensitivity:BEGIN ///////////////////////////////////
                if(gradIndex<0)
                  {
                    const Vector &theResponse = theNode->getTrialDisp();
                    for(int j=0; j<numDOF; j++)
                      {
                        const int dof= (*theDofs)(j);
                        if(theResponse.Size() > dof)
                          { response(cnt) = theResponse(dof); }
                        else
                          { response(cnt) = 0.0; }
                        cnt++;
                      }
                  }
                else
                  {
                    for(int j=0;j<numDOF;j++)
                      {
                        const int dof= (*theDofs)(j);
                        response(cnt) = theNode->getDispSensitivity(dof+1, gradIndex);
                        cnt++;
                      }
                  }
               // AddingSensitivity:END /////////////////////////////////////
             }
           else if(dataFlag == 1)
             {
               const Vector &theResponse= theNode->getTrialVel();
               for(int j=0; j<numDOF; j++)
                 {
                   const int dof= (*theDofs)(j);
                   if(theResponse.Size() > dof)
                     { response(cnt) = theResponse(dof); }
                   else 
                     response(cnt) = 0.0;
                   cnt++;
                 }
             }
           else if(dataFlag == 2)
             {
               const Vector &theResponse= theNode->getTrialAccel();
               for(int j=0;j<numDOF;j++)
                 {
                   const int dof= (*theDofs)(j);
                   if(theResponse.Size() > dof)
                     { response(cnt) = theResponse(dof); }
                   else 
                     response(cnt) = 0.0;
                  cnt++;
                 }
             }
           else if(dataFlag == 3)
             {
               const Vector &theResponse= theNode->getIncrDisp();
               for(int j=0; j<numDOF; j++)
                 {
                   const int dof= (*theDofs)(j);
                   if(theResponse.Size() > dof)
                     { response(cnt) = theResponse(dof); }
                   else 
                     response(cnt) = 0.0;
                   cnt++;
                 }
             }
           else if(dataFlag == 4)
             {
               const Vector &theResponse= theNode->getIncrDeltaDisp();
               for(int j=0; j<numDOF; j++)
                 {
                   const int dof= (*theDofs)(j);
                   if(theResponse.Size() > dof)
                     { response(cnt) = theResponse(dof); }
                   else 
                     response(cnt) = 0.0;
                   cnt++;
                 }
             }
           else if(dataFlag == 5)
             {
               const Vector &theResponse= theNode->getUnbalancedLoad();
               for(int j=0; j<numDOF; j++)
                 {
                   const int dof= (*theDofs)(j);
                   if(theResponse.Size() > dof)
                     { response(cnt) = theResponse(dof); }
                   else 
                     response(cnt) = 0.0;
                   cnt++;
                 }

             }
           else if(dataFlag == 6)
             {
               const Vector &theResponse= theNode->getUnbalancedLoadIncInertia();
               for(int j=0;j<numDOF;j++)
                 {
                   const int dof= (*theDofs)(j);
                   if(theResponse.Size() > dof)
                     { response(cnt) = theResponse(dof); }
                   else 
                     response(cnt) = 0.0;
                   cnt++;
                 }
             }
           else if(dataFlag == 7)
             {
               const Vector &theResponse = theNode->getReaction();
               for(int j=0; j<numDOF; j++)
                 {
                   const int dof= (*theDofs)(j);
                   if(theResponse.Size() > dof)
                     { response(cnt) = theResponse(dof); }
                   else 
                     response(cnt) = 0.0;
                   cnt++;
                 }
             }
           else if(dataFlag == 8)
             {
               const Vector &theResponse = theNode->getReaction();
               for(int j=0; j<numDOF; j++)
                 {
                   const int dof= (*theDofs)(j);
                   if(theResponse.Size() > dof)
                     { response(cnt) = theResponse(dof); }
                   else 
                     response(cnt) = 0.0;
                   cnt++;
                 }
             }
           else if(10 <= dataFlag  && dataFlag < 1000)
             {
               int mode= dataFlag - 10;
               int column = mode - 1;
               const Matrix &theEigenvectors = theNode->getEigenvectors();
               if(theEigenvectors.noCols() > column)
                 {
                   int noRows = theEigenvectors.noRows();
                   for(int j=0; j<numDOF; j++)
                     {
                       const int dof= (*theDofs)(j);
                       if(noRows > dof)
                         { response(cnt) = theEigenvectors(dof,column); }
                       else 
                         response(cnt) = 0.0;
                       cnt++;                
                     }
                 }
            }
          else if(dataFlag >= 1000 && dataFlag < 2000)
            {
              int grad = dataFlag - 1000;
              for(int j=0; j<numDOF; j++)
                {
                  int dof= (*theDofs)(j);
                  dof+= 1; // Terje uses 1 through DOF for the dof indexing
                  response(cnt) = theNode->getDispSensitivity(dof, grad);
                  cnt++;
                }
            }
          else if(dataFlag >= 2000 && dataFlag < 3000)
            {
              int grad = dataFlag - 2000;
              for(int j=0; j<numDOF; j++)
                {
                  int dof= (*theDofs)(j);
                  dof+= 1; // Terje uses 1 through DOF for the dof indexing
                  response(cnt) = theNode->getVelSensitivity(dof, grad);
                  cnt++;
                }


            }
          else if(dataFlag  >= 3000)
            {
              int grad = dataFlag - 3000;
              for(int j=0; j<numDOF; j++)
                {
                  int dof= (*theDofs)(j);
                  dof+= 1; // Terje uses 1 through DOF for the dof indexing
                  response(cnt) = theNode->getAccSensitivity(dof, grad);
                  cnt++;
                }
            }
	  else
            {
              // unknown response
              for(int j=0; j<numDOF; j++)
                { response(cnt) = 0.0; }
            }
        }
      // insert the data into the database
      theHandler->write(response);
    }
    return 0;
  }

//! @brief Send the object through the communicator
//! being passed as parameter.
int XC::NodeRecorder::sendData(Communicator &comm)
  {
    int res= NodeRecorderBase::sendData(comm);
    setDbTagDataPos(14,gradIndex);
    return res;
  }

//! @brief Receive the object through the communicator
//! being passed as parameter.
int XC::NodeRecorder::receiveData(const Communicator &comm)
  {
    int res= NodeRecorderBase::receiveData(comm);
    gradIndex= getDbTagDataPos(14);
    return res;
  }

//! @brief Send the object through the communicator
//! being passed as parameter.
int XC::NodeRecorder::sendSelf(Communicator &comm)
  {
    int res= 0;
    if(comm.isDatastore() == 1)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; does not send data to a datastore\n";
    else
      {
        setDbTag(comm);
        const int dataTag= getDbTag();
        inicComm(14);
        res= sendData(comm);
        if(comm.sendIdData(getDbTagData(),dataTag)< 0)
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; failed to send data\n";
            return -1;
          }
      }
    return res;
  }

//! @brief Receive the object through the communicator
//! being passed as parameter.
int XC::NodeRecorder::recvSelf(const Communicator &comm)
  {
    int res= 0;
    if(comm.isDatastore() == 1)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; does not recv data to a datastore\n";
    else
      {
        inicComm(14);
        const int dataTag= getDbTag();
        res= comm.receiveIdData(getDbTagData(),dataTag);
        if(res < 0)
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; failed to recv data\n";
            return res;
          }
        res= receiveData(comm);
      }
    return res;
  }



int XC::NodeRecorder::initialize(void)
  {
    if(theDofs == 0 || theNodalTags == 0 || theDomain == 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; either nodes, dofs or domain has not been set\n";
        return -1;
      }

    //
    // create & set nodal array pointer
    //

    theNodes.clear();

    numValidNodes = 0;
    int numNode = theNodalTags->Size();
    for(int i=0; i<numNode; i++)
      {
        const int nodeTag= (*theNodalTags)(i);
        Node *theNode= theDomain->getNode(nodeTag);
        if(theNode) {numValidNodes++;}
      }

    theNodes.resize(numValidNodes);
    int count = 0;
    for(int i=0;i<numNode;i++)
      {
        const int nodeTag= (*theNodalTags)(i);
        Node *theNode= theDomain->getNode(nodeTag);
        if(theNode)
          {
            theNodes[count] = theNode;
            count++;
          }
      }

    //
    // resize the response vector
    //

    int timeOffset = 0;
    if(echoTimeFlag == true)
      timeOffset = 1;

    int numValidResponse = numValidNodes*theDofs->Size() + timeOffset;
    response.resize(numValidResponse);
    response.Zero();

    //
    // need to create the data description, i.e. what each column of data is
    //

    std::string dataToStore;
    if(dataFlag == 0)
      dataToStore= "disp";
    else if(dataFlag == 1)
      dataToStore= "vel";
    else if(dataFlag == 2)
      dataToStore= "accel";
    else if(dataFlag == 3)
      dataToStore= "deltaDisp";
    else if(dataFlag == 4)
      dataToStore= "incrDeltaDisp";
    else if(dataFlag == 5)
      dataToStore= "eigen_" + copia_desde(dataToStore,' ');
    else
      dataToStore= "invalid";

    int numDbColumns = numValidResponse;
    std::vector<std::string> dbColumns(numDbColumns);

    static std::string aColumn; // assumes a column name will not be longer than 256 characters

    if(echoTimeFlag == true)
      { dbColumns[0] = "time"; }

    int counter = timeOffset;
    for(int i=0;i<numValidNodes;i++)
      {
        const int nodeTag = theNodes[i]->getTag();
        for(int j=0;j<theDofs->Size();j++)
          {
            const int dof= (*theDofs)(j);
            //sprintf(aColumn, "Node%d_%s_%d", nodeTag, dataToStore, dof+1);
            aColumn= "Node" + boost::lexical_cast<std::string>(nodeTag) + "_" 
                            + dataToStore +"_" 
                            + boost::lexical_cast<std::string>(dof+1);
            dbColumns[counter] = aColumn;
            counter++;
          }
      }

    //
    // call open in the handler with the data description
    //

    if(theHandler != 0)
      theHandler->open(dbColumns);

    initializationDone = true;
    return 0;
  }
