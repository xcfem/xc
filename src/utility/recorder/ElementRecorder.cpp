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
#include "domain/mesh/element/Information.h"
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
#include "xc_basic/src/texto/cadena_carac.h"
#include "boost/lexical_cast.hpp"
#include "xc_utils/src/base/CmdStatus.h"

XC::ElementRecorder::ElementRecorder()
  :ElementRecorderBase(RECORDER_TAGS_ElementRecorder),data(nullptr)
  {}

XC::ElementRecorder::ElementRecorder(const ID &ele,const std::vector<std::string> &argv, bool echoTime, 
                                     Domain &theDom, DataOutputHandler &theOutputHandler, double dT)
  :ElementRecorderBase(RECORDER_TAGS_ElementRecorder,ele,argv,echoTime,theDom,theOutputHandler,dT),data(nullptr)
  {}

//! @brief Lee un objeto XC::ElementRecorder desde archivo
bool XC::ElementRecorder::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(ElementRecorder) Procesando comando: " << cmd << std::endl;
    if(cmd == "elementos")
      {
        std::vector<int> elems= crea_vector_int(status.GetString());
        const int sz= elems.size();
        eleID= ID(sz);
        for(int i= 0;i<sz;i++)
          eleID[i]= elems[i];
        return true;
      }
    else
      return ElementRecorderBase::procesa_comando(status);
  }

XC::ElementRecorder::~ElementRecorder(void)
  { if(data) delete data; }


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
    assert(data);
    int result = 0;
    if(deltaT == 0.0 || timeStamp >= nextTimeStampToRecord)
      {
        if(deltaT != 0.0) 
          nextTimeStampToRecord = timeStamp + deltaT;
        int loc = 0;
        if(echoTimeFlag == true) 
          (*data)(loc++) = timeStamp;
    
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
	              (*data)(loc++) = eleData(j);
	          }
              } 
          }
        //
        // send the response vector to the output handler for o/p
        //

        theHandler->write(*data);
      }
    // succesfull completion - return 0
    return result;
  }

int XC::ElementRecorder::restart(void)
  {
    if(data) data->Zero();
    return 0;
  }


int XC::ElementRecorder::initialize(void)
  {
    const int numEle= eleID.Size();
    if(numEle == 0 || theDomain == 0)
      {
        if(!theDomain)
	  std::cerr << "ElementRecorder::initialize(); no se asignó el dominio." << std::endl;
        if(numEle == 0)
	  std::cerr << "ElementRecorder::initialize(); no hay elementos." << std::endl;
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
    data = new Vector(numDbColumns);

    if(!data)
      {
        std::cerr << "XC::ElementRecorder::initialize() - out of memory\n";
        return -1;
      }
    initializationDone = true;
    return 0;
  }


int XC::ElementRecorder::sendSelf(CommParameters &cp)
  { return ElementRecorderBase::sendSelf(cp); }

int XC::ElementRecorder::recvSelf(const CommParameters &cp)
  { return ElementRecorderBase::recvSelf(cp); }

