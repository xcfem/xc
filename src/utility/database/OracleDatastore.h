// -*-c++-*-
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
                                                                        
#ifndef OracleDatastore_h
#define OracleDatastore_h

// $Revision: 1.2 $
// $Date: 2005/11/07 21:34:25 $
// $Source: /usr/local/cvs/OpenSees/SRC/database/OracleDatastore.h,v $

// Written:  Jun Peng 
//
// Description: This file contains the class definition for OracleDatastore.
//
// What: "@(#) OracleDatastore.h, revA"

#include "DBDatastore.h"
#include <fstream>

namespace XC {
class FEM_ObjectBroker;


//! @ingroup Database
//
//! @brief Store model data in a <a href="https://en.wikipedia.org/wiki/Oracle_Database">Oracle</a> database.
//
//! OracleDatastore is a concrete subclas of DBDatastore. A OracleDatastore 
//! object is used in the program to store/restore the geometry and state 
//! information in a model at a particular instance in the analysis. The
//! information is stored in ORACLE8i Database.
class OracleDatastore: public DBDatastore
  {
  private:
    
    // private attributes
    int projTag;
    std::string dataBase;

    int lastDomainChangeStamp;
  public:
    OracleDatastore(const std::string &dataBase,Preprocessor &preprocessor, FEM_ObjectBroker &theBroker);    
    
    ~OracleDatastore(void);

    // methods to set and get a project tag
    int getProjTag();
    void setProjTag(int projectID);

    // method to search for project Tag
    int searchProjTag(const std::string &projName);

    // methods defined in the Channel class interface which do nothing
    char *addToProgram(void);
    int setUpShadow(void);
    int setUpActor(void);
    int setNextAddress(const ChannelAddress &otherChannelAddress);
    ChannelAddress *getLastSendersAddress(void);
    
    int commitState(int commitTag);        

    // methods for sending and receiving the data
    int sendObj(int commitTag,MovableObject &,ChannelAddress *theAddress= nullptr);
    int recvObj(int commitTag,MovableObject &theObject, FEM_ObjectBroker &,ChannelAddress *theAddress= nullptr);
                
    int sendMsg(int dbTag, int commitTag,const Message &,ChannelAddress *theAddress= nullptr);    
    int recvMsg(int dbTag, int commitTag,Message &,ChannelAddress *theAddress= nullptr);        

    int sendMatrix(int dbTag, int commitTag,const Matrix &,ChannelAddress *theAddress= nullptr);
    int recvMatrix(int dbTag, int commitTag,Matrix &,ChannelAddress *theAddress= nullptr);
    
    int sendVector(int dbTag, int commitTag,const Vector &, ChannelAddress *theAddress= nullptr);
    int recvVector(int dbTag, int commitTag,Vector &,ChannelAddress *theAddress= nullptr);
    
    int sendID(int dbTag, int commitTag, const ID &, ChannelAddress *theAddress= nullptr);
    int recvID(int dbTag, int commitTag, ID &theID, ChannelAddress *theAddress= nullptr);    
  };
} // end of XC namespace


#endif

