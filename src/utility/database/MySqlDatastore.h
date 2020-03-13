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
** (C) Copyright 2002, The Regents of the University of California    **
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
                                                                        
// $Revision: 1.5 $
// $Date: 2005/11/07 21:34:25 $
// $Source: /usr/local/cvs/OpenSees/SRC/utility/database/MySqlDatastore.h,v $

#ifndef MySqlDatastore_h
#define MySqlDatastore_h

// Written:  fmk
// Created:  02/02
//
// Description: This file contains the class definition for MySqlDatastore.
// MySqlDatastore is a concrete subclas of DBDatastore. A MySqlDatastore 
// object is used in the program to store/restore the geometry and state 
// information in a model at a particular instance in the analysis. The
// information is stored in a MySQL Database which can be local or remote.
//
// What: "@(#) MySqlDatastore.h, revA"

#include "DBDatastore.h"
#include <mysql.h>

namespace XC {
//! @ingroup Database
//
//! @brief Store model data in a <a href="https://en.wikipedia.org/wiki/MySQL">MySQL</a> database.
//
//! MySqlDatastore is a concrete subclass of DBDatastore. A MySqlDatastore 
//! object is used in the program to store/restore the geometry and state 
//! information in a model at a particular instance in the analysis. The
//! information is stored in a MySQL Database which can be local or remote.
class MySqlDatastore: public DBDatastore
  {
  private:
    int dbRun;
  
    bool connection;
    MYSQL mysql;

    std::string query;
    int sizeColumnString;
  protected:
    int createOpenSeesDatabase(const std::string &projectName);
    int execute(const std::string &query);
  public:
    MySqlDatastore(const std::string &,Preprocessor &, FEM_ObjectBroker &,int dbRun = 0);    
    MySqlDatastore(const std::string &,const std::string &, const std::string &, const std::string &, unsigned int port, const std::string &, unsigned int clientFlag, Preprocessor &, FEM_ObjectBroker &, int dbRun = 0);
    ~MySqlDatastore();

    // methods for sending and receiving matrices, vectors and id's
    int sendMsg(int dbTag, int commitTag, const Message &, ChannelAddress *theAddress= nullptr);    
    int recvMsg(int dbTag, int commitTag, Message &, ChannelAddress *theAddress= nullptr);        

    int sendMatrix(int dbTag, int commitTag, const Matrix &,ChannelAddress *theAddress= nullptr);
    int recvMatrix(int dbTag, int commitTag, Matrix &, ChannelAddress *theAddress= nullptr);

    int sendVector(int dbTag, int commitTag, const Vector &,ChannelAddress *theAddress= nullptr);
    int recvVector(int dbTag, int commitTag, Vector &,ChannelAddress *theAddress= nullptr);
    
    int sendID(int dbTag, int commitTag,const ID &,ChannelAddress *theAddress= nullptr);
    int recvID(int dbTag, int commitTag,ID &,ChannelAddress *theAddress= nullptr);    

    int createTable(const std::string &tableName,const std::vector<std::string> &columns);
    int insertData(const std::string &tableName,const std::vector<std::string> &columns, int commitTag, const Vector &data);
    int getData(const std::string &tableName,const std::vector<std::string> &columns, int commitTag, Vector &data);

    int setDbRun(int run);
    int getDbRun(void);
  };
} // end of XC namespace

#endif
