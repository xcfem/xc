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
** file 'COPYRIGHT'  in XC::main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        


// $Revision: 1.11 $
// $Date: 2006/01/18 19:43:42 $
// $Source: /usr/local/cvs/OpenSees/SRC/recorder/EnvelopeNodeRecorder.cpp,v $
                                                                        
// Written: fmk 
//
// Description: This file contains the class definition for XC::EnvelopeNodeRecorder.
// A XC::EnvelopeNodeRecorder is used to record the envelop of specified dof responses 
// at a collection of nodes over an analysis. (between commitTag of 0 and
// last commitTag).
//
// What: "@(#) EnvelopeNodeRecorder.C, revA"

#include <cmath>

#include <utility/recorder/EnvelopeNodeRecorder.h>
#include <domain/domain/Domain.h>
#include <domain/mesh/node/Node.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/ID.h>
#include <utility/matrix/Matrix.h>
#include <utility/database/FE_Datastore.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <utility/handler/DataOutputHandler.h>

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include "boost/lexical_cast.hpp"

XC::EnvelopeNodeRecorder::EnvelopeNodeRecorder(void)
  :NodeRecorderBase(RECORDER_TAGS_EnvelopeNodeRecorder), envelope() {}

XC::EnvelopeNodeRecorder::EnvelopeNodeRecorder(const XC::ID &dofs, 
					   const XC::ID &nodes, 
					   const char *dataToStore,
					   Domain &theDom,
					   DataOutputHandler &theOutputHandler,
					   double dT, bool echoTime)
  :NodeRecorderBase(RECORDER_TAGS_EnvelopeNodeRecorder,dofs,nodes,theDom,theOutputHandler,dT,echoTime), envelope()
    {
      // verify dof are valid 
      int numDOF = dofs.Size();
      theDofs = new ID(0, numDOF);

      int count = 0;
      for(int i=0; i<numDOF; i++)
        {
          int dof = dofs(i);
          if(dof>= 0)
            {
              (*theDofs)[count] = dof;
              count++;
            }
          else
            {
              std::cerr << "XC::EnvelopeNodeRecorder::EnvelopeNodeRecorder - invalid dof  " << dof;
              std::cerr << " will be ignored\n";
            }
        }

      // 
      // create memory to hold nodal XC::ID's (neeed parallel)
      //

      int numNode = nodes.Size();
      if(numNode != 0)
        {
          theNodalTags = new XC::ID(nodes);
          if(theNodalTags == 0)
            { std::cerr << "XC::EnvelopeNodeRecorder::EnvelopeNodeRecorder - out of memory\n"; }
        }

      //
      // set the data flag used as a switch to get the response in a record
      //

      if(dataToStore == 0 || (strcmp(dataToStore, "disp") == 0))
        { dataFlag = 0; } 
      else if ((strcmp(dataToStore, "vel") == 0))
        { dataFlag = 1; } 
      else if ((strcmp(dataToStore, "accel") == 0))
        { dataFlag = 2; } 
      else if ((strcmp(dataToStore, "incrDisp") == 0))
        { dataFlag = 3; } 
      else if ((strcmp(dataToStore, "incrDeltaDisp") == 0))
        { dataFlag = 4; } 
      else if ((strcmp(dataToStore, "unbalance") == 0))
        { dataFlag = 5; } 
      else if ((strncmp(dataToStore, "eigen",5) == 0))
        {
          int mode = atoi(&(dataToStore[5]));
          if(mode > 0)
            dataFlag = 10 + mode;
          else
            dataFlag = 6;
        }
      else
        {
          dataFlag = 6;
          std::cerr << "XC::EnvelopeNodeRecorder::EnvelopeNodeRecorder - dataToStore " << dataToStore;
          std::cerr << "not recognized (disp, vel, accel, incrDisp, incrDeltaDisp)\n";
        }
  }


XC::EnvelopeNodeRecorder::~EnvelopeNodeRecorder(void)
  {
    //
    // write the data
    //
    Vector *currentData= envelope.getCurrentData();
    Matrix *data= envelope.getData();
    if(theHandler != 0 && currentData != 0)
      {
        for(int i=0; i<3; i++)
          {
            int size= currentData->Size();
            for(int j=0; j<size; j++)
	      (*currentData)(j) = (*data)(i,j);
            theHandler->write(*currentData);
          }
      }
  }

int XC::EnvelopeNodeRecorder::record(int commitTag, double timeStamp)
  {
    if(theDomain == 0 || theNodalTags == 0 || theDofs == 0)
      { return 0; }

    if(theHandler == 0)
      {
        std::cerr << "XC::EnvelopeNodeRecorder::record() - no XC::DataOutputHandler has been set\n";
        return -1;
      }


  if (initializationDone != true) 
    if (this->initialize() != 0) {
      std::cerr << "XC::EnvelopeNodeRecorder::record() - failed in initialize()\n";
      return -1;
    }

  int numDOF = theDofs->Size();
  
    Vector *currentData= envelope.getCurrentData();
    Matrix *data= envelope.getData();
  if (deltaT == 0.0 || timeStamp >= nextTimeStampToRecord) {
    
    if (deltaT != 0.0) 
      nextTimeStampToRecord = timeStamp + deltaT;
    
    for (int i=0; i<numValidNodes; i++) {
      int cnt = i*numDOF;
      Node *theNode = theNodes[i];
      if (dataFlag == 0) {
	const XC::Vector &response = theNode->getTrialDisp();
	for (int j=0; j<numDOF; j++) {
	  int dof = (*theDofs)(j);
	  if (response.Size() > dof) {
	    (*currentData)(cnt) = response(dof);
	  }else 
	    (*currentData)(cnt) = 0.0;
	  
	  cnt++;
	}
      } else if (dataFlag == 1) {
	const XC::Vector &response = theNode->getTrialVel();
	for (int j=0; j<numDOF; j++) {
	  int dof = (*theDofs)(j);
	  if (response.Size() > dof) {
	    (*currentData)(cnt) = response(dof);
	  } else 
	    (*currentData)(cnt) = 0.0;
	  
	  cnt++;
	}
      } else if (dataFlag == 2) {
	const XC::Vector &response = theNode->getTrialAccel();
	for (int j=0; j<numDOF; j++) {
	  int dof = (*theDofs)(j);
	  if (response.Size() > dof) {
	    (*currentData)(cnt) = response(dof);
	  } else 
	    (*currentData)(cnt) = 0.0;
	  
	  cnt++;
	}
      } else if (dataFlag == 3) {
	const XC::Vector &response = theNode->getIncrDisp();
	for (int j=0; j<numDOF; j++) {
	  int dof = (*theDofs)(j);
	  if (response.Size() > dof) {
	    (*currentData)(cnt) = response(dof);
	  } else 
	    (*currentData)(cnt) = 0.0;
	  
	  cnt++;
	}
      } else if (dataFlag == 4) {
	const XC::Vector &response = theNode->getIncrDeltaDisp();
	for (int j=0; j<numDOF; j++) {
	  int dof = (*theDofs)(j);
	  if (response.Size() > dof) {
	    (*currentData)(cnt) = response(dof);
	  } else 
	    (*currentData)(cnt) = 0.0;
	  
	  cnt++;
	}
      } else if (dataFlag == 5) {
	const XC::Vector &theResponse = theNode->getUnbalancedLoad();
	for (int j=0; j<numDOF; j++) {
	  int dof = (*theDofs)(j);
	  if (theResponse.Size() > dof) {
	    (*currentData)(cnt) = theResponse(dof);
	  } else 
	    (*currentData)(cnt) = 0.0;
	  
	  cnt++;
	}
      } else if (dataFlag > 10) {
	int mode = dataFlag - 10;
	int column = mode - 1;
	const XC::Matrix &theEigenvectors = theNode->getEigenvectors();
	if (theEigenvectors.noCols() > column) {
	  int noRows = theEigenvectors.noRows();
	  for (int j=0; j<numDOF; j++) {
	    int dof = (*theDofs)(j);
	    if (noRows > dof) {
	      (*currentData)(cnt) = theEigenvectors(dof,column);
	    } else 
	      (*currentData)(cnt) = 0.0;
	    cnt++;		
	  }
	} else {
	  for (int j=0; j<numDOF; j++) {
	    (*currentData)(cnt) = 0.0;
	    cnt++;		
	  }
	}
      }
    }
  }
    
  // check if currentData modifies the saved data
  int sizeData = currentData->Size();
  if(echoTimeFlag == false)
    {
      bool writeIt = false;
    if(envelope.First()) {
      for (int i=0; i<sizeData; i++) {
	(*data)(0,i) = (*currentData)(i);
	(*data)(1,i) = (*currentData)(i);
	(*data)(2,i) = fabs((*currentData)(i));
	envelope.First() = false;
	writeIt = true;
      } 
    } else {
      for (int i=0; i<sizeData; i++) {
	double value = (*currentData)(i);
	if ((*data)(0,i) > value) {
	  (*data)(0,i) = value;
	  double absValue = fabs(value);
	  if ((*data)(2,i) < absValue) 
	    (*data)(2,i) = absValue;
	  writeIt = true;
	} else if ((*data)(1,i) < value) {
	  (*data)(1,i) = value;
	  double absValue = fabs(value);
	  if ((*data)(2,i) < absValue) 
	    (*data)(2,i) = absValue;
	  writeIt = true;
	}
      }
    }
  } else {
    sizeData /= 2;
    bool writeIt = false;
    if(envelope.First()) {
      for (int i=0; i<sizeData; i++) {
	(*data)(0,i*2) = timeStamp;
	(*data)(1,i*2) = timeStamp;
	(*data)(2,i*2) = timeStamp;
	(*data)(0,i*2+1) = (*currentData)(i);
	(*data)(1,i*2+1) = (*currentData)(i);
	(*data)(2,i*2+1) = fabs((*currentData)(i));
	envelope.First()= false;
	writeIt = true;
      } 
    } else {
      for (int i=0; i<sizeData; i++) {
	double value = (*currentData)(i);
	if ((*data)(0,2*i+1) > value) {
	  (*data)(0,i*2) = timeStamp;
	  (*data)(0,i*2+1) = value;
	  double absValue = fabs(value);
	  if ((*data)(2,i*2+1) < absValue) {
	    (*data)(2,i*2+1) = absValue;
	    (*data)(2,i*2) = timeStamp;
	  }
	  writeIt = true;
	} else if ((*data)(1,2*i+1) < value) {
	  (*data)(1,2*i) = timeStamp;
	  (*data)(1,2*i+1) = value;
	  double absValue = fabs(value);
	  if ((*data)(2,2*i+1) < absValue) { 
	    (*data)(2,2*i) = timeStamp;
	    (*data)(2,2*i+1) = absValue;
	  }
	  writeIt = true;
	}
      }
    }
  }

  return 0;
}


int XC::EnvelopeNodeRecorder::restart(void)
  { return envelope.restart(); }

//! @brief Envía el objeto a través del comunicador que
//! se pasa como parámetro.
int XC::EnvelopeNodeRecorder::sendData(CommParameters &cp)
  {
    int res= NodeRecorderBase::sendData(cp);
    res+= cp.sendMovable(envelope,getDbTagData(),CommMetaData(14));
    return res;
  }

//! @brief Recibe el objeto a través del comunicador que
//! se pasa como parámetro.
int XC::EnvelopeNodeRecorder::receiveData(const CommParameters &cp)
  {
    int res= NodeRecorderBase::receiveData(cp);
    res+= cp.receiveMovable(envelope,getDbTagData(),CommMetaData(14));
    return res;
  }

//! @brief Envía el objeto a través del comunicador que
//! se pasa como parámetro.
int XC::EnvelopeNodeRecorder::sendSelf(CommParameters &cp)
  {
    int res= 0;
    if(cp.isDatastore() == 1)
      std::cerr << "EnvelopeNodeRecorder::sendSelf() - does not send data to a datastore\n";
    else
      {
        setDbTag(cp);
        const int dataTag= getDbTag();
        inicComm(15);
        res= sendData(cp);
        if(cp.sendIdData(getDbTagData(),dataTag)< 0)
          {
            std::cerr << "EnvelopeNodeRecorder::sendSelf() "
                      << "- failed to send idData\n";
            return -1;
          }
      }
    return res;
  }

//! @brief Recibe el objeto a través del comunicador que
//! se pasa como parámetro.
int XC::EnvelopeNodeRecorder::recvSelf(const CommParameters &cp)
  {
    int res= 0;
    if(cp.isDatastore() == 1)
      std::cerr << "EnvelopeNodeRecorder::recvSelf() - does not recv data to a datastore\n";
    else
      {
        inicComm(15);
        const int dataTag= getDbTag();
        res= cp.receiveIdData(getDbTagData(),dataTag);
        if(res < 0)
          {
            std::cerr << "EnvelopeNodeRecorder::recvSelf() - failed to recv idData\n";
            return res;
          }
        res= receiveData(cp);
      }
    return res;
  }

//! @brief Asigna valores iniciales.
int XC::EnvelopeNodeRecorder::initialize(void)
{
  if (theDofs == 0 || theNodalTags == 0 || theDomain == 0) {
    std::cerr << "XC::EnvelopeNodeRecorder::initialize() - either nodes, dofs or domain has not been set\n";
    return -1;
  }
  
  //
  // create & set nodal array pointer
  //

  theNodes.clear();
  
  numValidNodes = 0;
  int i;
  int numNode = theNodalTags->Size();
  for (i=0; i<numNode; i++) {
    int nodeTag = (*theNodalTags)(i);
    Node *theNode = theDomain->getNode(nodeTag);
    if (theNode != 0) {
      numValidNodes++;
    }
  }

  theNodes.resize(numValidNodes);

  int count = 0;
  for(int i=0; i<numNode; i++) {
    int nodeTag = (*theNodalTags)(i);
    Node *theNode = theDomain->getNode(nodeTag);
    if (theNode != 0) {
      theNodes[count] = theNode;
      count++;
    }
  }

  //
  // resize the response vector
  //
  int numValidResponse = numValidNodes*theDofs->Size();

  if (echoTimeFlag == true)
    numValidResponse *= 2;

  envelope.alloc(numValidResponse);

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
    dataToStore= "eigen_"+boost::lexical_cast<std::string>(dataToStore.c_str()-10);
  else
    dataToStore= "invalid";
  
  int numDbColumns = numValidResponse;
  std::vector<std::string> dbColumns(numDbColumns);

  //static char aColumn[128]; // assumes a column name will not be longer than 256 characters
  
  int counter = 0;
  for(int i=0; i<numValidNodes; i++)
    {
      int nodeTag = theNodes[i]->getTag();
      for(int j=0; j<theDofs->Size(); j++)
        {
          if(echoTimeFlag == true)
            {
              dbColumns[counter]= "time";
	      counter++;
            }
          int dof = (*theDofs)(j);
          dbColumns[counter]= "Node"+boost::lexical_cast<std::string>(nodeTag)+"_"+dataToStore+"_"+boost::lexical_cast<std::string>(dof);
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
