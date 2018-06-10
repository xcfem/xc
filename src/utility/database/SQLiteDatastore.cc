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
// SQLiteDatastore.cpp

#include <utility/database/SQLiteDatastore.h>
#include "xc_utils/src/sqlite/SqLiteQuery.h"
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/ID.h>
#include "boost/lexical_cast.hpp"

XC::SQLiteDatastore::SQLiteDatastore(const std::string &projectName, Preprocessor &preprocessor, FEM_ObjectBroker &theObjectBroker, int run)
  :DBDatastore(preprocessor, theObjectBroker), connection(false), db(projectName)
  {
    if(this->createOpenSeesDatabase(projectName) == 0)
      connection= true;
    else
      std::cerr << "SQLiteDatastore::SQLiteDatastore() - could not open the database\n";
  }

int XC::SQLiteDatastore::sendMsg(int dataTag, int commitTag,const XC::Message &,ChannelAddress *theAddress)
  {
    std::cerr << "SQLiteDatastore::sendMsg() - not yet implemented\n";
    return -1;
  }

int XC::SQLiteDatastore::recvMsg(int dataTag, int commitTag, Message &, ChannelAddress *theAddress)
  {
    std::cerr << "SQLiteDatastore::recvMsg() - not yet implemented\n";
    return -1;
  }

//! @brief Inserts data on a BLOB field.
bool XC::SQLiteDatastore::insertData(const std::string &tbName,const int &dbTag,const int &commitTag,const void *blobData,const int &sz,const int &typeSize)
  {
    const int numBytes= sz*typeSize;
    query= "INSERT INTO " + tbName + " VALUES (" + boost::lexical_cast<std::string>(dbTag) + ",";
    query+= boost::lexical_cast<std::string>(commitTag) + ",";
    query+= boost::lexical_cast<std::string>(sz) + ",?)";
    return db.getDefaultQuery()->insert_blob1(query,blobData,numBytes);
  }

//! @brief Updates data o a BLOB field.
bool XC::SQLiteDatastore::updateData(const std::string &tbName,const int &dbTag,const int &commitTag,const void *blobData,const int &sz,const int &typeSize)
  {
    const int numBytes= sz*typeSize;
    query= "UPDATE " + tbName + " SET data= ? WHERE dbTag= " + boost::lexical_cast<std::string>(dbTag);
    query+= " AND commitTag= " + boost::lexical_cast<std::string>(commitTag) + " AND size= ";
    query+= boost::lexical_cast<std::string>(sz);
    return db.getDefaultQuery()->insert_blob1(query,blobData,numBytes);
  }

const void *XC::SQLiteDatastore::retrieveData(const std::string &tbName,const int &dbTag,const int &commitTag,const int &sz)
  {
    const void *retval= nullptr;
    // check that we have a connection
    if(connection)
      {
        // to receive the data the database we do the following:
        // 1. use SELECT to receive the data from the database
        // 2. fetch the results from the server and copy to the Matrices data area
        // 3. clean up the SQLITE_RES datastructure

        // form the SELECT query
        query= "SELECT data FROM " + tbName + " WHERE dbTag= " + boost::lexical_cast<std::string>(dbTag) + " AND commitTag= " + boost::lexical_cast<std::string>(commitTag) + " AND size= " + boost::lexical_cast<std::string>(sz);

        sqlite3_stmt *result= db.getDefaultQuery()->get_result(query);

        // fetch the results from the database
        if(!result)
          {
            // no data stored in db with these keys
            std::cerr << "SQLiteDatastore::retrieveData - no data in table= " << tbName << " for object with dbTag= "
                      << dbTag << " commitTag= " << commitTag << " and size= " << sz << " query= " << query << std::endl;
          }
        else
          {
            db.getDefaultQuery()->fetch_row();
            retval= db.getDefaultQuery()->getBlob("data");
            if(!retval)
              {
                // no vector stored in db with these keys
                std::cerr << "SQLiteDatastore::retrieveData - no data in table= " << tbName << " for object with dbTag= "
                      << dbTag << " and commitTag= " << commitTag << " and size= " << sz << " query= " << query << std::endl;
                db.getDefaultQuery()->free_result();
              }
          }
      }
    return retval;
  }

int XC::SQLiteDatastore::sendMatrix(int dbTag, int commitTag, const Matrix &theMatrix, ChannelAddress *theAddress)
  {
    if(!checkDbTag(dbTag))
      std::cerr << "Error en SQLiteDatastore::sendMatrix." << std::endl;
    int retval= -1;
    if(connection)
      {
        // to send the data the database we do the following:
        // 1. try to INSERT the data into the database
        // 2. if INSERT fails we have to reformulate the query to UPDATE the existing data,
        //    as the row given by dbTag, commitTag and vectorSize may already already exist.
        bool ok= insertData("Matrices",dbTag,commitTag,theMatrix.getDataPtr(),theMatrix.getDataSize(),sizeof(double));
        if(!ok)
          ok= updateData("Matrices",dbTag,commitTag,theMatrix.getDataPtr(),theMatrix.getDataSize(),sizeof(double));
        if(ok)
          retval= 0;
      }
    return retval;
  }

int XC::SQLiteDatastore::recvMatrix(int dbTag, int commitTag, Matrix &theMatrix, ChannelAddress *theAddress)
  {
    if(!checkDbTag(dbTag))
      std::cerr << "Error en SQLiteDatastore::recvMatrix." << std::endl;
    int retval= -1;
    if(connection)
      {
        const void *blob= retrieveData("Matrices",dbTag,commitTag,theMatrix.getDataSize());
	if(blob)
          {
            // place the results into the vectors double array
            const double *blobData = reinterpret_cast<const double *>(blob);
            double *matrixData= theMatrix.getDataPtr();
            for(int i=0; i<theMatrix.getDataSize(); i++)
              matrixData[i] = blobData[i];
            // free the SQLITE_RES structure
            db.getDefaultQuery()->free_result();
            retval= 0;
          }
      }
    return retval;
  }



int XC::SQLiteDatastore::sendVector(int dbTag, int commitTag, const Vector &theVector, ChannelAddress *theAddress)
  {
    if(!checkDbTag(dbTag))
      std::cerr << "Error en SQLiteDatastore::sendVector." << std::endl;
    int retval= -1;
    if(connection)
      {
        // to send the data the database we do the following:
        // 1. try to INSERT the data into the database
        // 2. if INSERT fails we have to reformulate the query to UPDATE the existing data,
        //    as the row given by dbTag, commitTag and vectorSize may already already exist.
        bool ok= (insertData("Vectors",dbTag,commitTag,theVector.getDataPtr(),theVector.Size(),sizeof(double))!= SQLITE_OK);
        if(!ok)
          ok= updateData("Vectors",dbTag,commitTag,theVector.getDataPtr(),theVector.Size(),sizeof(double));
        if(ok)
          retval= 0;
      }
    return retval;
  }

int XC::SQLiteDatastore::recvVector(int dbTag, int commitTag, Vector &theVector,ChannelAddress *theAddress)
  {
    if(!checkDbTag(dbTag))
      std::cerr << "Error en SQLiteDatastore::recvVector." << std::endl;
    int retval= -1;
    if(connection)
      {
        const void *blob= retrieveData("Vectors",dbTag,commitTag,theVector.Size());
	if(blob)
          {
            // place the results into the vectors double array
            const double *blobData = reinterpret_cast<const double *>(blob);
            for(int i=0; i<theVector.Size(); i++)
              theVector[i] = blobData[i];
            // free the SQLITE_RES structure
            db.getDefaultQuery()->free_result();
            retval= 0;
          }
      }
    return retval;
  }


int XC::SQLiteDatastore::sendID(int dbTag, int commitTag, const ID &theID, ChannelAddress *theAddress)
  {
    if(!checkDbTag(dbTag))
      std::cerr << "Error en SQLiteDatastore::sendID." << std::endl;
    int retval= -1;
    if(connection)
      {
        // to send the data the database we do the following:
        // 1. try to INSERT the data into the database
        // 2. if INSERT fails we have to reformulate the query to UPDATE the existing data,
        //    as the row given by dbTag, commitTag and vectorSize may already already exist.
        bool ok= insertData("IDs",dbTag,commitTag,theID.getDataPtr(),theID.Size(),sizeof(int));
        if(!ok)
          ok= updateData("IDs",dbTag,commitTag,theID.getDataPtr(),theID.Size(),sizeof(int));
        if(ok)
          retval= 0;
      }
    return retval;
  }

int XC::SQLiteDatastore::recvID(int dbTag, int commitTag,ID &theID,ChannelAddress *theAddress)
  {
    if(!checkDbTag(dbTag))
      std::cerr << "Error en SQLiteDatastore::recvID." << std::endl;
    int retval= -1;
    if(connection)
      {
        const void *blob= retrieveData("IDs",dbTag,commitTag,theID.Size());
	if(blob)
          {
            // place the results into the vectors double array
            const int *blobData = reinterpret_cast<const int *>(blob);
            for(int i=0; i<theID.Size(); i++)
              theID[i] = blobData[i];
            // free the SQLITE_RES structure
            db.getDefaultQuery()->free_result();
            retval= 0;
          }
      }
    return retval;
  }

int XC::SQLiteDatastore::createTable(const std::string &tableName, const std::vector<std::string> &columns)
  {
    const int numColumns= columns.size();
    // check that we have a connection
    if(connection)
      {
        // create the sql query
        query= "CREATE TABLE " + tableName + " (dbTag INT NOT nullptr, commitTag INT NOT nullptr, ";
        for(int j=0; j<numColumns; j++)
          query+= columns[j] + " DOUBLE NOT nullptr, ";
        query+= "PRIMARY KEY (dbTag, commitTag) )";
        db.getDefaultQuery()->execute(query);
        return 0;
      }
    else
      return -1;
  }

int XC::SQLiteDatastore::insertData(const std::string &tableName,const std::vector<std::string> &columns, int commitTag, const Vector &data)
  {
    // check that we have a connection
    if(connection)
      {
        // check that query string is big enough to hold the data, if not enlarge
        //const int sizeData = 128 + strlen(tableName);

        // form the insert query
        query= "INSERT INTO " + tableName + "VALUES (" + boost::lexical_cast<std::string>(dbTAG) + "," + boost::lexical_cast<std::string>(commitTag);
        for(int i=0; i<data.Size(); i++)
          query+= boost::lexical_cast<std::string>(data(i));
        query+= ")";

        // execute the query
        if(db.getDefaultQuery()->execute(query) != SQLITE_OK)
          {
          
            // if INSERT fails we reformulate query and try an UPDATE
            // formulate the query
            query= "UPDATE " + tableName + " SET " + columns[0] + "= " + boost::lexical_cast<std::string>(data(0));
            for(int i=1; i<data.Size(); i++)
              query+= columns[i] + "= " + boost::lexical_cast<std::string>(data(i));
            query+= " WHERE dbTag= " + boost::lexical_cast<std::string>(dbTAG) + " AND commitTag= " + boost::lexical_cast<std::string>(commitTag);

            // invoke the query on the database
            if(db.getDefaultQuery()->execute(query) != SQLITE_OK)
              {
                std::cerr << "SQLiteDatastore::insertData() - failed to send the data to SQLite database";
                std::cerr << query;
                std::cerr << std::endl << db.getDefaultQuery()->getError() << std::endl;
                return -3;
              }
          }
        return 0;
      }
    else
      return -1;
  }

int XC::SQLiteDatastore::getData(const std::string &tableName,const std::vector<std::string> &columns, int commitTag, Vector &data)
  {
    // check that we have a connection
    if(!connection)
      {

        // check that query string is big enough to hold the data, if not enlarge
        //const int sizeData = 128 + strlen(tableName);


        // to receive the data the database we do the following:
        // 1. use SELECT to receive the data from the database
        // 2. fetch the results from the server and copy to the Vectors data area
        // 3. clean up the SQLITE_RES datastructure
        //
        // form the SELECT query
        query= "Select * FROM " + tableName + " WHERE dbTag= " + boost::lexical_cast<std::string>(dbTAG) + " AND commitTag= " + boost::lexical_cast<std::string>(commitTag);

        sqlite3_stmt *result= db.getDefaultQuery()->get_result(query);

        // fetch the results from the database
        if(!result)
          {
            // no data stored in db with these keys
            std::cerr << "SQLiteDatastore::getData - no data in database for object with dbTag, cTag: ";
            std::cerr << dbTAG << ", " << commitTag << std::endl;
          }
        else
          {
            for(int i=0; i<data.Size(); i++)
              data[i] = db.getDefaultQuery()->getDouble(i);
          }
        db.getDefaultQuery()->free_result();
        return 0;
      }
    else
      return -1;
  }


int XC::SQLiteDatastore::createOpenSeesDatabase(const std::string &projectName)
  {

    const std::string campos= "(dbTag INTEGER NOT nullptr,commitTag INTEGER NOT nullptr, size INTEGER NOT nullptr, data BLOB, PRIMARY KEY (dbTag, commitTag, size) )";
    // now create the tables in the database

    query= "CREATE TABLE Messages " + campos;
    if(execute(query) != 0)
      std::cerr << "SQLiteDatastore::createOpenSeesDatabase() - could not create the Messagess table\n";
    query= "CREATE TABLE Matrices " + campos;
    if(execute(query) != 0)
      std::cerr << "SQLiteDatastore::createOpenSeesDatabase() - could not create the Matricess table\n";

    query= "CREATE TABLE Vectors " + campos;
    if(execute(query) != 0)
      std::cerr << "SQLiteDatastore::createOpenSeesDatabase() - could not create the Vectors table\n";

    query= "CREATE TABLE IDs " + campos;
    if(execute(query) != 0)
       std::cerr << "SQLiteDatastore::createOpenSeesDatabase() - could not create the ID's table\n";
    return 0;
  }

int XC::SQLiteDatastore::execute(const std::string &query)
  {
    bool tmp= db.getDefaultQuery()->execute(query);
    if(!tmp)
      {
        std::cerr << "SQLiteDatastore::execute() - could not execute command: " << query;
        std::cerr << std::endl << db.getDefaultQuery()->getError() << std::endl;
        return -1;
      }
    else
      return 0;
  }

