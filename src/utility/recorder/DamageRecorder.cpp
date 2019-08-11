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
// $Date: 2004/11/24 22:45:28 $
// $Source: /usr/local/cvs/OpenSees/SRC/recorder/DamageRecorder.cpp,v $
                                                                        
// Written: Arash Altoontash, Gregory Deierlein, 
// Created: 04/04
// Revision: A
//
// Description: This file contains the class implementation of XC::ElementRecorder.
//
// What: "@(#) ElementRecorder.C, revA"

#include <utility/recorder/DamageRecorder.h>
#include "domain/mesh/element/utils/Information.h"
#include <domain/domain/Domain.h>
#include <domain/mesh/element/Element.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/ID.h>
#include <string.h>
#include <utility/recorder/response/Response.h>
#include <utility/database/FE_Datastore.h>
#include <material/damage/DamageModel.h>
#include <iomanip>
#include <boost/lexical_cast.hpp>

void XC::DamageRecorder::free_mem(void)
  {
    if(!theResponses.empty())
      {
        const size_t sz= theResponses.size();
        for(size_t i=0;i<sz;i++)
          {
            delete theResponses[i];
            theResponses[i]= nullptr;
          }
        theResponses.clear();
      }
    if(!theDamageModels.empty())
      {
        const size_t sz= theDamageModels.size();
        for(size_t i=0;i<sz;i++)
          {
            delete theDamageModels[i];
            theDamageModels[i]= nullptr;
          }
        theDamageModels.clear();
      }
  }

void XC::DamageRecorder::alloc(const size_t &numSec, DamageModel *dmgPtr)
  {
    free_mem();
    if(dmgPtr)
      {
        theDamageModels= std::vector<DamageModel *>(numSec,nullptr);
        for(size_t j= 0;j<numSec;j++)
          {
            theDamageModels[j] = dmgPtr->getCopy();
            if(!theDamageModels[j])
              {
                std::cerr << "DamageRecorder::DamageRecorder - out of memory copying damage models ";
                exit(-1);
              }
            theDamageModels[j]->revertToStart();
          }
      }
    else
      {
        std::cerr << "DamageRecorder::DamageRecorder - no damage pointer associated with the damage recorder" <<std::endl;
      }
    // allocate pointers to element responses and damage models
    theResponses= std::vector<Response *>(3*numSec,nullptr);
  }

XC::DamageRecorder::DamageRecorder( int elemid, ID &secIDs, int dofid, DamageModel *dmgPtr,
				    Domain &dom, bool echotimeflag, double deltat , const std::string &theFileName )
  :DomainRecorderBase(RECORDER_TAGS_DamageRecorder,&dom),
   eleID(elemid) , numSec(secIDs.Size()), dofID(dofid),
   responseID(secIDs.Size()), sectionTags(secIDs.Size()),
   echoTimeFlag(echotimeflag), fileName(theFileName), deltaT(deltat), nextTimeStampToRecord(0.0)                                
  {
    alloc(numSec,dmgPtr);
    // Get the element        
    Information eleInfo(1.0);
    Element *theEle= theDomain->getElement(eleID);
    if(!theEle)
      {
        std::cerr << "WARNING XC::DamageRecorder::DamageRecorder() - no element with tag: "
               << eleID << " exists in XC::Domain\n";
        exit(-1);
      }
  
  
    // establish response for the element sections or materials
    const int argc = 3;
    std::vector<std::string > argv(argc);
  
    argv[0]= "-section";
    argv[2]= "deformation";
    for(int j=0;j<numSec;j++)
      {
        sectionTags(j) = secIDs(j);
        argv[1]= boost::lexical_cast<std::string>(sectionTags(j));
    
        theResponses[j]= theEle->setResponse(argv, eleInfo);
        if(!theResponses[j])
          {
            std::cerr << "DamageRecorder::DamageRecorder - out of memory creating deformation response ";
            exit(-1);
          }
      }
  
  
    argv[2]=  "force";
    for(int j=0;j<numSec;j++)
      {
        sectionTags(j) = secIDs(j);
        argv[1]= boost::lexical_cast<std::string>(sectionTags(j));
        theResponses[j+numSec] = theEle->setResponse(argv, eleInfo);
        if(theResponses[j+numSec] == 0)
          {
            std::cerr << "DamageRecorder::DamageRecorder - out of memory creating force response ";
            exit(-1);
          }
      }
  
    argv[2]= "stiffness";
    for(int j=0;j<numSec;j++)
      {
        sectionTags(j) = secIDs(j);
        argv[1]= boost::lexical_cast<std::string>(sectionTags(j));
        theResponses[j+2*numSec]= theEle->setResponse(argv, eleInfo);
        if(!theResponses[j+2*numSec])
          {
            std::cerr << "DamageRecorder::DamageRecorder - out of memory creating tanegnt response ";
            exit(-1);
          }
      }
  
  
    // if file is specified, open the file
    if(!fileName.empty())
      {
        // open the file
        theFile.open(fileName.c_str(), std::ios::out);
        if(theFile.bad())
          {
            std::cerr << "WARNING - XC::DamageRecorder::DamageRecorder()";
            std::cerr << " - could not open file " << fileName << std::endl;
          }    
      }
  }




XC::DamageRecorder::~DamageRecorder(void)
  {
    // close the file
    if(!theFile.bad())
      theFile.close();    
    free_mem();
  }


int XC::DamageRecorder::record(int commitTag, double timeStamp)
  {
        int result = 0;
        if (deltaT == 0.0 || timeStamp >= nextTimeStampToRecord) {
                
                if (deltaT != 0.0) 
                        nextTimeStampToRecord = timeStamp + deltaT;
                
                // print out the pseudo time if requested
                if (echoTimeFlag == true) {
                        if (!theFile) 
                                std::cerr << timeStamp << " ";                        
                        else 
                                theFile << timeStamp << " ";        
                }
                
                Vector DamageInformation(3);
                // get the responses and write to file if file or std::cerr specified
                // for each element do a getResponse() & print the result
                for (int i=0; i< numSec; i++) {
                        DamageInformation.Zero();
                        for ( int j=0 ; j<2 ; j++) {
                                if ( theResponses[i+numSec*j] == 0) {
                                        DamageInformation(j) = 0.0;
                                } else {
                                        if ( theResponses[i+numSec*j]->getResponse() < 0) {
                                                DamageInformation(j) = 0.0;
                                        } else {
                                                // ask the element for the response
                                                Information &eleinfo = theResponses[i+numSec*j]->getInformation();
                                                const Vector &infovector = eleinfo.getData();
                                                DamageInformation(j) = infovector(dofID);
                                        }
                                }
                        }
                        DamageInformation(2) = 0.0;
                        theDamageModels[i]->setTrial(DamageInformation);
                        theDamageModels[i]->commitState();
                        double Damageindex = theDamageModels[i]->getDamage();

                        // print results to file or stderr depending on whether
                        // a file was opened
                        
                        if (theFile.bad())
                                std::cerr << Damageindex << "  ";
                        else {
                                theFile << Damageindex;
                                theFile << "  ";  // added for OSP
                        }
                }
                
                if (theFile.bad()) 
                        std::cerr << std::endl;
                else {
                        theFile << " \n";
                        theFile.flush();
                }
    }
        
    // successful completion - return 0
    return result;
  }


int XC::DamageRecorder::playback(int commitTag)
  { return 0; }

int XC::DamageRecorder::restart(void)
  {
    theFile.close();
    theFile.open(fileName.c_str(), std::ios::out);
    if(!theFile)
      {
        std::cerr << "WARNING - XC::ElementRecorder::restart() - could not open file ";
        std::cerr << fileName << std::endl;
      }    
    return 0;
  }
