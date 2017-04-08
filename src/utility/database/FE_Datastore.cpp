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
                                                                        
// $Revision: 1.8 $
// $Date: 2005/11/07 23:53:45 $
// $Source: /usr/local/cvs/OpenSees/SRC/database/FE_Datastore.cpp,v $
                                                                        
// Written: fmk 
// Created: 10/98
// Revision: A
//
// Description: This file contains the class implementation for XC::FE_Datastore.
// FE_Datastore is an abstract base class. An XC::FE_Datastore object is used
// in the program to store/restore the geometry and state information in
// a model at a particular instance in the analysis.
//
// What: "@(#) FE_Datastore.C, revA"

#include "utility/database/FE_Datastore.h"
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <utility/actor/actor/MovableObject.h>
#include "preprocessor/Preprocessor.h"
#include <utility/matrix/ID.h>


#include "domain/mesh/element/Element.h"
#include "domain/mesh/node/Node.h"

int XC::FE_Datastore::lastDbTag(0);

//! @brief Constructor.
XC::FE_Datastore::FE_Datastore(Preprocessor &m, FEM_ObjectBroker &theBroker) 
  :theObjectBroker(&theBroker), preprocessor(&m), savedStates() {}

bool XC::FE_Datastore::isDatastore(void) const
  { return true; }

/********************************************************************
 *                   CHANNEL METHODS  THAT DO NOTHING               *
 ********************************************************************/

char *XC::FE_Datastore::addToProgram(void)
  { return nullptr; }

int XC::FE_Datastore::setUpConnection(void)
  { return 0; }

int XC::FE_Datastore::setNextAddress(const ChannelAddress &otherChannelAddress)
  { return 0; }


XC::ChannelAddress *XC::FE_Datastore::getLastSendersAddress(void)
  { return nullptr; }


int XC::FE_Datastore::sendObj(int commitTag,MovableObject &theObject, ChannelAddress *theAddress)
  { return sendMovable(commitTag,theObject); }

int XC::FE_Datastore::recvObj(int commitTag,MovableObject &theObject, FEM_ObjectBroker &theNewBroker, ChannelAddress *theAddress)
  { return receiveMovable(commitTag,theObject,theNewBroker); }

//! @brief Salva el estado en la database.
int XC::FE_Datastore::commitState(int commitTag)
  {
    int res = 0;
    if(commitTag<1)
      std::cerr << "FE_Datastore::commitState se esperaba un valor de commitTag mayor que 0, se obtuvo: "
                << commitTag << std::endl;
    clearDbTags();
    if(preprocessor)
      {
        CommParameters cp(commitTag,*this);
        res = preprocessor->sendSelf(cp);
        if(res < 0)
          std::cerr << "FE_Datastore::commitState - modeler failed to sendSelf\n";
        else
          {
            ID maxlastDbTag(1);
            maxlastDbTag(0)= lastDbTag;
            if(this->sendID(-1,commitTag,maxlastDbTag) < 0)
              std::cerr << "FE_Datastore::commitState - failed to send max lastDbTag data from database - problems may arise\n";
            else
              savedStates.insert(commitTag);
	  }
      }
    return res;
  }

//! @brief Returns true if the estado se salvó
//" con anterioridad en la database.
bool XC::FE_Datastore::isSaved(int commitTag) const
  { return (savedStates.count(commitTag)>0); }

int XC::FE_Datastore::restoreState(int commitTag)
  {
    int res= 0;
    clearDbTags();
    if(isSaved(commitTag))
      {
        if(preprocessor)
          {
            CommParameters cp(commitTag,*this,*theObjectBroker);
            res= preprocessor->recvSelf(cp);
            if(res < 0)
              { std::cerr << "FE_Datastore::restoreState - preprocessor failed to recvSelf\n"; }
            ID maxlastDbTag(1);
            res= recvID(-1,commitTag,maxlastDbTag);
            if(res<0)
              std::cerr << "FE_Datastore::restoreState - failed to get max lastDbTag data from database - problems may arise\n";
            else
              lastDbTag = maxlastDbTag(0);
          }
      }
    return res;
  }

int XC::FE_Datastore::createTable(const std::string &table, const std::vector<std::string> &columns)
  {
    std::cerr << "FE_Datastore::createTable - not yet implemented\n";
    return -1;
  }


int XC::FE_Datastore::insertData(const std::string &tableName,const std::vector<std::string> &columns, int commitTag, const XC::Vector &data)
  {
    std::cerr << "FE_Datastore::insertData - not yet implemented\n";
    return -1;
  }


int XC::FE_Datastore::getData(const std::string &table,const std::vector<std::string> &columns, int commitTag, Vector &data)
  {
    std::cerr << "FE_Datastore::getData - not yet implemented\n";
    return -1;
  }

int XC::FE_Datastore::getDbTag(void) const
  {
    lastDbTag++;
    return lastDbTag;
  }

int XC::FE_Datastore::save(const int &commitTag)
  {
    const int retval= commitState(commitTag);
    if(retval < 0)
      std::cerr << "WARNING - database failed to commitState \n";
    return retval;
  }

int XC::FE_Datastore::restore(const int &commitTag)
  {
    const int retval= restoreState(commitTag);
    if(retval < 0)
      std::cerr << "WARNING - database failed to commitState \n";
    return retval;
  }

