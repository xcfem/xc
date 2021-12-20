// -*-c++-*-
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
                                                                        
// $Revision: 1.8 $
// $Date: 2005/11/08 00:04:32 $
// $Source: /usr/local/cvs/OpenSees/SRC/utility/database/FE_Datastore.h,v $
                                                                        
                                                                        
#ifndef FE_Datastore_h
#define FE_Datastore_h

// File: ~/utility/database/FE_Datastore.h
//
// Written: fmk 
// Created: 10/98
// Revision: A
//
// Description: This file contains the class definition for FE_Datastore.
// FE_Datastore is an abstract base class. An FE_datastore object is used
// in the program to store/restore the geometry and state information in
// a model at a particular instance in the analysis.
//
// What: "@(#) FE_Datastore.h, revA"

#include "utility/actor/channel/Channel.h"
#include <vector>

namespace XC {
class Preprocessor;
class FEM_ObjectBroker;

//! @ingroup Database
//
//! @brief Base class for objects that store/restore model information.
//
//! Objects of this class are used in the program to store/restore
//! the geometry and state information in the domain at particular
//! instances. How, where and how the data is stored depends on the
//! implementation provided by the concrete subclasses.  
class FE_Datastore: public Channel
  {
  private:
    FEM_ObjectBroker *theObjectBroker; //!< Deals with the object serialization.
    std::set<int> savedStates;
  protected:
    mutable int lastDbTag;
    FEM_ObjectBroker *getObjectBroker(void);
    const Preprocessor *getPreprocessor(void) const;
    Preprocessor *getPreprocessor(void);
  public:
    FE_Datastore(Preprocessor &, FEM_ObjectBroker &theBroker);
    inline virtual ~FE_Datastore(void) {} 

    // methods defined in the Channel class interface which mean nothing for a Datastore
    char *addToProgram(void);
    int setUpConnection(void);
    int setNextAddress(const ChannelAddress &otherChannelAddress);
    ChannelAddress *getLastSendersAddress(void);

    // methods defined in the Channel class interface which can be implemented here    
    int sendObj(int commitTag,MovableObject &theObject, ChannelAddress *theAddress =0);

    int recvObj(int commitTag,MovableObject &theObject, FEM_ObjectBroker &theBroker,ChannelAddress *theAddress =0);
		
    // pure virtual functions in addition to those defined
    // in the ModelBuilder and Channel classes for database applications
    int getDbTag(void) const;
    virtual bool isDatastore(void) const;

    virtual int commitState(int commitTag);
    virtual int restoreState(int commitTag);
    bool isSaved(int commitTag) const;

    virtual int createTable(const std::string &tableName, const std::vector<std::string> &);
    virtual int insertData(const std::string &tableName,const std::vector<std::string> &, int commitTag, const Vector &data);
    virtual int getData(const std::string &tableName,const std::vector<std::string> &, int commitTag, Vector &data);

    virtual int save(const int &commitTag);
    virtual int restore(const int &commitTag);

  };
} // end of XC namespace


#endif

