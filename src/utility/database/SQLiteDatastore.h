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
//SQLiteDatastore.h

#ifndef SQLiteDatastore_h
#define SQLiteDatastore_h

#include "DBDatastore.h"
#include "xc_utils/src/sqlite/SqLiteDatabase.h"

namespace XC {
//! @ingroup Utils
//
//! @defgroup Database Database utilities.
//
//! @ingroup Database
//
class SQLiteDatastore: public DBDatastore
  {
  private:
    bool connection;
    SqLiteDatabase db; //!< database SqLite.
    std::string query;
    bool insertData(const std::string &,const int &,const int &,const void *,const int &,const int &);
    bool updateData(const std::string &,const int &,const int &,const void *,const int &,const int &);
    const void *retrieveData(const std::string &tbName,const int &dbTag,const int &commitTag,const int &sz);
  protected:
    int createOpenSeesDatabase(const std::string &projectName);
    int execute(const std::string &query);
  public:
    SQLiteDatastore(const std::string &,Preprocessor &, FEM_ObjectBroker &,int dbRun = 0);    

    // methods for sending and recieving matrices, vectors and id's
    int sendMsg(int , int , const Message &, ChannelAddress *a= NULL);    
    int recvMsg(int , int , Message &, ChannelAddress *a= NULL);        

    int sendMatrix(int , int , const Matrix &,ChannelAddress *a= NULL);
    int recvMatrix(int , int , Matrix &, ChannelAddress *a= NULL);

    int sendVector(int , int , const Vector &,ChannelAddress *a= NULL);
    int recvVector(int , int , Vector &,ChannelAddress *a= NULL);
    
    int sendID(int , int ,const ID &,ChannelAddress *a= NULL);
    int recvID(int , int ,ID &,ChannelAddress *a= NULL);    

    int createTable(const std::string &,const std::vector<std::string> &);
    int insertData(const std::string &,const std::vector<std::string> &, int , const Vector &);
    int getData(const std::string &,const std::vector<std::string> &, int , Vector &);
  };
} // end of XC namespace

#endif
