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

// $Revision: 1.13 $
// $Date: 2005/07/06 22:00:20 $
// $Source: /usr/local/cvs/OpenSees/SRC/recorder/DriftRecorder.cpp,v $

// Written: MHS
// Created: Oct 2001
//
// Description: This file contains the class definition for XC::DriftRecorder.

#include <cmath>

#include <utility/recorder/DriftRecorder.h>
#include "domain/domain/Domain.h"
#include "domain/domain/PseudoTimeTracker.h"
#include <domain/mesh/node/Node.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/ID.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include <utility/handler/DataOutputHandler.h>
#include <cstring>
#include <cstdio>
#include <utility/actor/channel/Channel.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include "xc_basic/src/texto/cadena_carac.h"
#include "boost/lexical_cast.hpp"
#include "utility/actor/actor/ArrayCommMetaData.h"


void XC::DriftRecorder::libera_nodes(void)
  {
    oneOverL.resize(0);
    data.resize(0);
    theNodes.clear();
  }

int XC::DriftRecorder::alloc_nodes(const int &numNodes,const int &timeOffset)
  {
    libera_nodes();
    theNodes= std::vector<Node *>(2*numNodes,static_cast<Node *>(nullptr));
    oneOverL= Vector(numNodes);
    data= Vector(numNodes+timeOffset); // data(0) allocated for time
    return 0;
  }

void XC::DriftRecorder::libera_ndIJ(void)
  {
    if(ndI) delete ndI;
    ndI= nullptr;
    if(ndJ) delete ndJ;
    ndJ= nullptr;
  }

void XC::DriftRecorder::alloc_ndIJ(const int &sz)
  {
    libera_ndIJ();
    ndI=new ID(sz);
    ndJ=new ID(sz);
    assert(ndI);
    assert(ndJ);
  }

void XC::DriftRecorder::set_ndIJ(const ID &nI,const ID &nJ)
  {
    libera_ndIJ();
    ndI=new ID(nI);
    ndJ=new ID(nJ);
    assert(ndI);
    assert(ndJ);
  }

void XC::DriftRecorder::setup_ndIJ(const int &sz)
  {
    int old_size= 0;
    if(ndI) old_size= ndI->Size();
    if(old_size != sz)
      alloc_ndIJ(sz);
  }



XC::DriftRecorder::DriftRecorder(void)
  :HandlerRecorder(RECORDER_TAGS_DriftRecorder),
   ndI(nullptr), ndJ(nullptr), theNodes(0), dof(0), perpDirn(0), oneOverL(), data(),numNodes(0)
  {}


XC::DriftRecorder::DriftRecorder(int ni, int nj,
                             int df,
                             int dirn,
                             Domain &theDom,
                             DataOutputHandler &theDataOutputHandler,
                             bool timeFlag)
  :HandlerRecorder(RECORDER_TAGS_DriftRecorder,theDom,theDataOutputHandler,timeFlag),
   ndI(nullptr), ndJ(nullptr), theNodes(0), dof(df), perpDirn(dirn),
   oneOverL(), data(), numNodes(0)
  {
    setup_ndIJ(1);
    (*ndI)(0)= ni;
    (*ndJ)(0)= nj;
  }


XC::DriftRecorder::DriftRecorder(const ID &nI,const ID &nJ, int df,int dirn, Domain &theDom, DataOutputHandler &theDataOutputHandler, bool timeFlag)
  :HandlerRecorder(RECORDER_TAGS_DriftRecorder,theDom,theDataOutputHandler,timeFlag),
   ndI(nullptr), ndJ(nullptr), theNodes(0), dof(df), perpDirn(dirn),
   oneOverL(), data(), numNodes(0)
  {
    assert(nI.Size()==nJ.Size());
    set_ndIJ(nI,nJ);
  }

XC::DriftRecorder::~DriftRecorder(void)
  {
    libera_ndIJ();
    libera_nodes();
  }

int XC::DriftRecorder::record(int commitTag, double timeStamp)
  {

    if(!theDomain || !ndI || !ndJ)
      { return 0; }

    if(!theHandler)
      {
        std::cerr << "XC::DriftRecorder::record() - no DataOutputHandler has been set\n";
        return -1;
      }

    if(initializationDone != true)
      if(this->initialize() != 0)
        {
          std::cerr << "XC::DriftRecorder::record() - failed in initialize()\n";
          return -1;
        }

    if(numNodes == 0 || data.Nulo())
      return 0;

    int timeOffset = 0;
    if(echoTimeFlag == true)
      {
        data(0) = theDomain->getTimeTracker().getCurrentTime();
        timeOffset = 1;
      }

    for(int i=0; i<numNodes; i++)
      {
        Node *nodeI = theNodes[2*i];
        Node *nodeJ = theNodes[2*i+1];

        if(oneOverL(i) != 0.0)
          {
            const Vector &dispI= nodeI->getTrialDisp();
            const Vector &dispJ= nodeJ->getTrialDisp();

            const double dx= dispJ(dof)-dispI(dof);
            data(i+timeOffset)=  dx* oneOverL(i);
          }
        else
          data(i+timeOffset) = 0.0;
      }

    theHandler->write(data);
    return 0;
  }

int XC::DriftRecorder::restart(void)
  { return 0; }

int XC::DriftRecorder::initialize(void)
  {

    initializationDone = true; // still might fail but don't want back in again

    //
    // clean up old memory
    //

    if(!theNodes.empty())
      theNodes.clear();
    libera_nodes();

    //
    // check valid node ID's
    //

    if(!ndI || !ndJ)
      {
        std::cerr << "XC::DriftRecorder::initialize() - no nodal id's set\n";
        return -1;
      }

    const int ndIsize= ndI->Size();
    const int ndJsize= ndJ->Size();

    if(ndIsize == 0)
      {
        std::cerr << "XC::DriftRecorder::initialize() - no nodal id's set\n";
        return -1;
      }

    if(ndIsize != ndJsize)
      {
        std::cerr << "XC::DriftRecorder::initialize() - error node arrays differ in size"
                  << " (" << ndIsize << "!=" << ndJsize << "),\n";
        return -2;
      }

    //
    // lets loop through & determine number of valid nodes
    //


    numNodes = 0;

    for(int i=0; i<ndIsize; i++)
      {
        int ni = (*ndI)(i);
        int nj = (*ndJ)(i);

        Node *nodeI = theDomain->getNode(ni);
        if(!nodeI)
	  std::cerr << "DriftRecorder::initialize(); no se encontró el nodo: "
                    << ni << std::endl;
        Node *nodeJ = theDomain->getNode(nj);
        if(!nodeJ)
	  std::cerr << "DriftRecorder::initialize(); no se encontró el nodo: "
                    << nj << std::endl;

        if(nodeI && nodeJ )
          {
            const Vector &crdI = nodeI->getCrds();
            const Vector &crdJ = nodeJ->getCrds();

            if(crdI.Size() > perpDirn  && crdJ.Size() > perpDirn)
              if(crdI(perpDirn) != crdJ(perpDirn))
                numNodes++;
          }
      }

    if(numNodes == 0)
      {
        std::cerr << "XC::DriftRecorder::initialize() - no valid nodes or perpendicular direction\n";
        return 0;
      }

    //
    // allocate memory
    //

    int timeOffset = 0;
    if(echoTimeFlag == true)
      timeOffset = 1;

    alloc_nodes(numNodes,timeOffset);

    //
    // set node pointers and determine one over L
    //

    int counter = 0;
    int counterI = 0;
    int counterJ = 1;
    for(int j=0; j<ndIsize; j++)
      {
        int ni = (*ndI)(j);
        int nj = (*ndJ)(j);

        Node *nodeI= theDomain->getNode(ni);
        Node *nodeJ= theDomain->getNode(nj);

        if(nodeI != 0 && nodeJ != 0)
          {
            const XC::Vector &crdI = nodeI->getCrds();
            const XC::Vector &crdJ = nodeJ->getCrds();

            if(crdI.Size() > perpDirn  && crdJ.Size() > perpDirn)
              if(crdI(perpDirn) != crdJ(perpDirn))
                {
                  oneOverL(counter) = 1.0/fabs(crdJ(perpDirn) - crdI(perpDirn));
                  theNodes[counterI] = nodeI;
                  theNodes[counterJ] = nodeJ;
                  counterI+=2;
                  counterJ+=2;
                  counter++;
                }
          }
      }

    //
    // create the data description for the OutputHandler & compute length between nodes
    //

    int numDbColumns = timeOffset + numNodes;
    std::vector<std::string> dbColumns(numDbColumns);

    static std::string aColumn; // assumes a column name will not be longer than 256 characters

    if(echoTimeFlag == true)
      {
        dbColumns[0] = "time";
      }

    for(int k=0; k<numNodes; k++)
      {
        Node *nodI= theNodes[2*k];
        Node *nodJ= theNodes[2*k+1];
        int ni = nodI->getTag();
        int nj = nodJ->getTag();

        //sprintf(aColumn, "Drift%d_%d_%d", ni, nj, perpDirn);
        aColumn= "Drift_"  + boost::lexical_cast<std::string>(ni) + "_"
                           + boost::lexical_cast<std::string>(nj) + "_"
                           + boost::lexical_cast<std::string>(perpDirn);
        dbColumns[k+timeOffset] = aColumn;
      }

    //
    // call open in the handler with the data description
    //

    if(theHandler)
      theHandler->open(dbColumns);

    //
    // mark as having been done & return
    //

    return 0;
  }


//! @brief Sends the object through the communicator being passed as parameter.
int XC::DriftRecorder::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(12);
    int res= sendData(cp);

    res+= cp.sendIDPtr(ndI,getDbTagData(),ArrayCommMetaData(5,6,7));
    res+= cp.sendIDPtr(ndJ,getDbTagData(),ArrayCommMetaData(8,9,10));
    res+= cp.sendInts(dof,perpDirn,getDbTagData(),CommMetaData(11));
    if(cp.sendIdData(getDbTagData(),dataTag)<0)
      {
        std::cerr << "DriftRecorder::sendSelf() - failed to send data\n";
        return -1;
      }
    return res;
  }

//! @brief Receives the object through the communicator being passed as parameter.
int XC::DriftRecorder::recvSelf(const CommParameters &cp)
  {
    inicComm(12);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      {
        std::cerr << "XC::DriftRecorder::sendSelf() - failed to receive data\n";
        return -1;
      }
    else
      {
        res= receiveData(cp);       
        ndI= cp.receiveIDPtr(ndI,getDbTagData(),ArrayCommMetaData(5,6,7));
        ndJ= cp.receiveIDPtr(ndJ,getDbTagData(),ArrayCommMetaData(8,9,10));
        res+= cp.receiveInts(dof,perpDirn,getDbTagData(),CommMetaData(11));
      }
    return 0;
  }


