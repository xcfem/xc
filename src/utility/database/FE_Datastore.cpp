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
#include "preprocessor/Preprocessor.h"
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <utility/actor/actor/MovableObject.h>
#include <utility/matrix/ID.h>


#include "domain/mesh/element/Element.h"
#include "domain/mesh/node/Node.h"

int XC::FE_Datastore::lastDbTag(0);

//! @brief Constructor.
//!
//! @param prep: preprocessor used to build the finite element model.
//! @param theBroker: deals with object serialization.
XC::FE_Datastore::FE_Datastore(Preprocessor &prep, FEM_ObjectBroker &theBroker) 
  : Channel(&prep), theObjectBroker(&theBroker),  savedStates() {}

//! @brief Returns a pointer to \p theBroker object passed in the constructor. 
XC::FEM_ObjectBroker *XC::FE_Datastore::getObjectBroker(void)
  { return theObjectBroker; }

//! @brief Returns a pointer to the object preprocessor.
const XC::Preprocessor *XC::FE_Datastore::getPreprocessor(void) const
  { return dynamic_cast<const Preprocessor *>(Owner()); }

//! @brief Returns a pointer to the object preprocessor.
XC::Preprocessor *XC::FE_Datastore::getPreprocessor(void)
  { return dynamic_cast<Preprocessor *>(Owner()); }

//! @brief Return true (this channel type IS a Datastore).
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

//! @brief Stores the current state on the database.
//!
//! Invoked to store the current state of the domain in the database. The
//! integer \p commitTag is used to identify the state for subsequent
//! calls to restore the information from the database. To return
//! \f$0\f$ if successful, a negative number if not.
//! 
//! The object loops over all the components of the
//! Domain object invoking {\em sendSelf(commitTag, this)} on each of
//! these objects. Returns \f$0\f$ if successful, a negative number if
//! not. For each domain component that could not send itself a warning
//! message is printed.
int XC::FE_Datastore::commitState(int commitTag)
  {
    int res = 0;
    if(commitTag<1)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< ";a commitTag greater than 0 was expected, we get: "
                << commitTag << std::endl;
    clearDbTags();
    if(getPreprocessor())
      {
        CommParameters cp(commitTag,*this);
        res = getPreprocessor()->sendSelf(cp);
        if(res < 0)
          std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; preprocessor failed to sendSelf\n";
        else
          {
            ID maxlastDbTag(1);
            maxlastDbTag(0)= lastDbTag;
            if(this->sendID(-1,commitTag,maxlastDbTag) < 0)
              std::cerr << getClassName() << "::" << __FUNCTION__
			<< "; failed to send max lastDbTag data"
		        << " from database - problems may arise.\n";
            else
              savedStates.insert(commitTag);
	  }
      }
    return res;
  }

//! @brief Returns true if the state identified by commitTag was
//! previously saved on the database.
bool XC::FE_Datastore::isSaved(int commitTag) const
  { return (savedStates.count(commitTag)>0); }

//! @brief Invoked to restore the state of the domain from a database.
//! 
//! Invoked to restore the state of the domain from a database. The state
//! of the domain at the end of this call is to be the same as the state
//! of the domain when {\em commitState(commitTag)} was invoked. To return
//! \f$0\f$ if successful, a negative number if not. 
//! 
//! The object loops over all the components of the Domain object invoking
//! {\em recvSelf(commitTag, this)} on each of these objects. Returns \f$0\f$
//! if successful, a negative number if not. For each domain component that
//! could not send itself a warning message is printed. 
int XC::FE_Datastore::restoreState(int commitTag)
  {
    int res= 0;
    clearDbTags();
    if(isSaved(commitTag))
      {
        if(getPreprocessor())
          {
            CommParameters cp(commitTag,*this,*theObjectBroker);
            res= getPreprocessor()->recvSelf(cp);
            if(res < 0)
              std::cerr << getClassName() << "::" << __FUNCTION__
	                << "; preprocessor failed to recvSelf\n";
            ID maxlastDbTag(1);
            res= recvID(-1,commitTag,maxlastDbTag);
            if(res<0)
              std::cerr << getClassName() << "::" << __FUNCTION__
                        << ";- failed to get max lastDbTag data"
		        << " from database - problems may arise\n";
            else
              lastDbTag = maxlastDbTag(0);
          }
      }
    return res;
  }

int XC::FE_Datastore::createTable(const std::string &table, const std::vector<std::string> &columns)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not yet implemented\n";
    return -1;
  }


int XC::FE_Datastore::insertData(const std::string &tableName,const std::vector<std::string> &columns, int commitTag, const XC::Vector &data)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not yet implemented\n";
    return -1;
  }


int XC::FE_Datastore::getData(const std::string &table,const std::vector<std::string> &columns, int commitTag, Vector &data)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not yet implemented\n";
    return -1;
  }

//! @brief To return a unique integer identifier at each call. This identifier
//! will be used by the objects to store/retrieve their information
//! to/from the database.
int XC::FE_Datastore::getDbTag(void) const
  {
    lastDbTag++;
    return lastDbTag;
  }

int XC::FE_Datastore::save(const int &commitTag)
  {
    const int retval= commitState(commitTag);
    if(retval < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; WARNING - database failed to commitState.\n";
    return retval;
  }

int XC::FE_Datastore::restore(const int &commitTag)
  {
    const int retval= restoreState(commitTag);
    if(retval < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; WARNING - database failed to commitState.\n";
    return retval;
  }

