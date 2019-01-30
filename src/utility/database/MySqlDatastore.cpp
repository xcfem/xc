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

// $Revision: 1.7 $
// $Date: 2005/11/07 21:34:25 $
// $Source: /usr/local/cvs/OpenSees/SRC/database/MySqlDatastore.cpp,v $

#include <utility/database/MySqlDatastore.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/ID.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include "boost/lexical_cast.hpp"

// #include <mysqld_error.h> .. use the following #define instead
#define ER_TABLE_EXISTS_ERROR 1050


#define MAX_BLOB_SIZE 16777215

XC::MySqlDatastore::MySqlDatastore(const std::string &projectName, Preprocessor &preprocessor, FEM_ObjectBroker &theObjectBroker, int run)
  :DBDatastore(preprocessor, theObjectBroker), dbRun(run), connection(true), sizeColumnString(0)
  {
    // initialise the mysql structure
    mysql_init(&mysql);

    // connect to the server
    if(mysql_real_connect(&mysql, nullptr, nullptr, nullptr, nullptr, 0, "/tmp/mysql.sock", 0) == nullptr)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; could not connect to server.\n"
		  << mysql_error(&mysql) << std::endl;
        connection = false;
      }
    else
      {
        // link to the database,
       if(mysql_select_db(&mysql, projectName.c_str()) != 0)
         {
          // if no database exists, try creating one
          if(this->createOpenSeesDatabase(projectName) != 0)
            {
              connection = false;
              mysql_close(&mysql);
              std::cerr << getClassName() << "::" << __FUNCTION__
			<< "; could not open the database\n"
			<< mysql_error(&mysql) << std::endl;
            }
         }
      }
  }



XC::MySqlDatastore::MySqlDatastore(const std::string &databaseName, const std::string &host, const std::string &user, const std::string &passwd,
                               unsigned int port, const std::string &socket, unsigned int clientFlag,Preprocessor &preprocessor, FEM_ObjectBroker &theObjectBroker,
                               int run)
  :DBDatastore(preprocessor, theObjectBroker), dbRun(run), connection(true), sizeColumnString(0)
  {
    // initialise the mysql structure
    mysql_init(&mysql);

    // connect to the server & database
    if(mysql_real_connect(&mysql, host.c_str(), user.c_str(), passwd.c_str(), databaseName.c_str(), port, socket.c_str(), clientFlag) == nullptr)
      {

         // connect to the server & see if can link to database, or create a new one if one does not exist
        if(mysql_real_connect(&mysql, host.c_str(), user.c_str(), passwd.c_str(), nullptr, port, socket.c_str(), clientFlag) == nullptr)
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; could not connect to server\n"
		      << mysql_error(&mysql) << std::endl;
            connection = false;

          }
        else
          {
            // link to the database,
            if(mysql_select_db(&mysql, databaseName.c_str()) != 0)
              {
                // if no database exists, try creating one
                if(this->createOpenSeesDatabase(databaseName) != 0)
                  {
                    connection = false;
                    mysql_close(&mysql);
                    std::cerr << getClassName() << "::" << __FUNCTION__
			      << "; could not open the database\n"
			      << mysql_error(&mysql) << std::endl;
                  }
              }
          }
      }
  }


XC::MySqlDatastore::~MySqlDatastore()
  {
    if(connection)
      mysql_close(&mysql);
  }


int XC::MySqlDatastore::sendMsg(int dbTag, int commitTag,const XC::Message &,ChannelAddress *theAddress)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not yet implemented\n";
    return -1;
  }

int XC::MySqlDatastore::recvMsg(int dbTag, int commitTag, Message &, ChannelAddress *theAddress)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not yet implemented\n";
    return -1;
  }



int XC::MySqlDatastore::sendMatrix(int dbTag, int commitTag, const XC::Matrix &theMatrix, ChannelAddress *theAddress)
  {
    if(!checkDbTag(dbTag))
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; error when cheking dbTag." << std::endl;
    // check that we have a connection
    if(connection == false)
      return -1;

    // check that the data will fit, MySQL has a limit on blob sizes
    const int sizeData = theMatrix.getDataSize() * sizeof(double);
    if(sizeData > MAX_BLOB_SIZE)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; vector too big to send to MySQL databse, enlarge BLOBS!";
        return  -2;
      }

    //
    // to send the data the database we do the following:
    // 1. try to INSERT the data into the database
    // 2. if INSERT fails we have to reformulate the query to UPDATE the existing data,
    //    as the row given by dbTag, commitTag and vectorSize may already already exist.
    //

    // form the INSERT query
    query= "INSERT INTO Matrices VALUES (" + boost::lexical_cast<std::string>(dbTag) + ",";
    query+= boost::lexical_cast<std::string>(commitTag) + ",";
    query+= boost::lexical_cast<std::string>(theMatrix.getDataSize()) + ",'";
    char *tmp= new char[2 * sizeData];
    mysql_escape_string(tmp, reinterpret_cast<const char *>(theMatrix.getDataPtr()), sizeData); // add vector values in binary form
    query+= std::string(tmp) + "')";

    // execute the INSERT query
    if(mysql_query(&mysql, query.c_str()) != 0)
      {

        //
        // if INSERT fails we reformulate query and try an UPDATE
        //

        // formulate the query
        query= "UPDATE Matrices SET data='";
        mysql_escape_string(tmp, reinterpret_cast<const char *>(theMatrix.getDataPtr()), sizeData);
        query+= std::string(tmp) + "' WHERE dbTag= " + boost::lexical_cast<std::string>(dbTag) + " AND commitTag= " + boost::lexical_cast<std::string>(commitTag) + " AND size= " + boost::lexical_cast<std::string>(theMatrix.getDataSize());

        // invoke the query on the database
        if(mysql_query(&mysql, query.c_str()) != 0)
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; failed to send the XC::Matrix to MySQL database"
		      << std::endl << mysql_error(&mysql) << std::endl;
            return -3;
          }
      }
    delete tmp;
    return 0;
  }

int XC::MySqlDatastore::recvMatrix(int dbTag, int commitTag, Matrix &theMatrix, ChannelAddress *theAddress)
  {
    if(!checkDbTag(dbTag))
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; error when checking dbTags." << std::endl;

    // check that we have a connection
    if(connection == false)
      return -1;


    //
    // to receive the data the database we do the following:
    // 1. use SELECT to receive the data from the database
    // 2. fetch the results from the server and copy to the Matrices data area
    // 3. clean up the MYSQL_RES datastructure
    //
    // NOTE: probably using a malloc() and free() under the covers for this, could be
    // very expensive .. IS THERE ANOTHER WAY TO DO THIS, i.e. SAVE A MYSQL_RES FOR
    // NUMEROUS USES??

    // form the SELECT query
    query= "Select data FROM Matrices WHERE dbTag= " + boost::lexical_cast<std::string>(dbTag) + " AND commitTag= " + boost::lexical_cast<std::string>(commitTag) + " AND size= " + boost::lexical_cast<std::string>(theMatrix.getDataSize());

    // execute the SELECT query
    if(mysql_query(&mysql, query.c_str()) != 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; failed to receive vector from MySQL database"
		  << std::endl << mysql_error(&mysql) << std::endl;
        return -3;
      }

    // fetch the results from the database
    MYSQL_RES *results;
    MYSQL_ROW row;
    results = mysql_store_result(&mysql);
    if(!results)
      {
        // no vector stored in db with these keys
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; no data in database for Matrix with dbTag, cTag: "
		  << dbTag << ", " << commitTag << std::endl;
        return -4;
      }
    row = mysql_fetch_row(results);
    if(row == nullptr)
      {
        // no vector stored in db with these keys
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; no data in database for Matrix with dbTag, cTag: "
		  << dbTag << ", " << commitTag << std::endl;
        mysql_free_result(results);
        return -5;
      }

    // place the results into the vectors double array
    const double *dblPtrData = reinterpret_cast<const double *>(row[0]);
    double *matrixData= theMatrix.getDataPtr();
    for(int i=0; i<theMatrix.getDataSize(); i++)
      matrixData[i] = dblPtrData[i];

    // free the MYSQL_RES structure
    mysql_free_result(results);
    return 0;
  }



int XC::MySqlDatastore::sendVector(int dbTag, int commitTag, const XC::Vector &theVector, ChannelAddress *theAddress)
  {
    if(!checkDbTag(dbTag))
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "error when checking dbTag." << std::endl;
    // check that we have a connection
    if(!connection)
      return -1;

    // check that the data will fit, MySQL has a limit on blob sizes
    const int sizeData = theVector.Size() * sizeof(double);
    if(sizeData > MAX_BLOB_SIZE)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; vector too big to send to MySQL databse, enlarge BLOBS!";
        return  -2;
      }


    //
    // to send the data the database we do the following:
    // 1. try to INSERT the data into the database
    // 2. if INSERT fails we have to reformulate the query to UPDATE the existing data,
    //    as the row given by dbTag, commitTag and vectorSize may already already exist.
    //

    // form the INSERT query
    query= "INSERT INTO Vectors VALUES (" + boost::lexical_cast<std::string>(dbTag) + ",";
    query+= boost::lexical_cast<std::string>(commitTag) + ",";
    query+= boost::lexical_cast<std::string>(theVector.Size()) + ",'";
    char *tmp= new char[2 * sizeData];
    mysql_escape_string(tmp, reinterpret_cast<const char *>(theVector.getDataPtr()), sizeData); // add vector values in binary form
    query+= std::string(tmp) + "')";

    // execute the INSERT query
    if(mysql_query(&mysql, query.c_str()) != 0)
      {

        //
        // if INSERT fails we reformulate query and try an UPDATE
        //

        // formulate the query
        query= "UPDATE Vectors SET data='";
        mysql_escape_string(tmp, reinterpret_cast<const char *>(theVector.getDataPtr()), sizeData);
        query+= std::string(tmp) + "' WHERE dbTag= " + boost::lexical_cast<std::string>(dbTag) + " AND commitTag= " + boost::lexical_cast<std::string>(commitTag) + " AND size= " + boost::lexical_cast<std::string>(theVector.Size());

        // invoke the query on the database
        if(mysql_query(&mysql,query.c_str()) != 0)
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; failed to send the Vector to MySQL database"
		      << std::endl << mysql_error(&mysql) << std::endl;
            return -3;
          }
      }
    delete tmp;
    return 0;
  }

int XC::MySqlDatastore::recvVector(int dbTag, int commitTag, Vector &theVector,ChannelAddress *theAddress)
  {
    if(!checkDbTag(dbTag))
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; error when checking dbTag." << std::endl;
    // check that we have a connection
    if(!connection)
      return -1;

    //
    // to receive the data the database we do the following:
    // 1. use SELECT to receive the data from the database
    // 2. fetch the results from the server and copy to the Vectors data area
    // 3. clean up the MYSQL_RES datastructure
    //
    // NOTE: probably using a malloc() and free() under the covers for this, could be
    // very expensive .. IS THERE ANOTHER WAY TO DO THIS, i.e. SAVE A MYSQL_RES FOR
    // NUMEROUS USES??

    // form the SELECT query
    query= "Select data FROM Matrices WHERE dbTag= " + boost::lexical_cast<std::string>(dbTag) + " AND commitTag= " + boost::lexical_cast<std::string>(commitTag) + " AND size= " + boost::lexical_cast<std::string>(theVector.Size());

    // execute the SELECT query
    if(mysql_query(&mysql, query.c_str()) != 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; failed to receive vector from MySQL database"
		  << std::endl << mysql_error(&mysql) << std::endl;
        return -3;
      }

    // fetch the results from the database
    MYSQL_RES *results;
    MYSQL_ROW row;
    results= mysql_store_result(&mysql);
    if(results == nullptr)
      {
        // no vector stored in XC::db with these keys
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; no data in database for Vector with dbTag, cTag: "
		  << dbTag << ", " << commitTag << std::endl;
        return -4;
      }
    row = mysql_fetch_row(results);
    if(row == nullptr)
      {
        // no vector stored in XC::db with these keys
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; no data in database for Vector with dbTag, cTag: "
		  << dbTag << ", " << commitTag << std::endl;
        mysql_free_result(results);
        return -5;
      }

    // place the results into the vectors double array
    double *dblPtrData = (double *)row[0];
    for(int i=0; i<theVector.Size(); i++)
      theVector[i] = dblPtrData[i];


    // free the MYSQL_RES structure
    mysql_free_result(results);
    return 0;
  }


int XC::MySqlDatastore::sendID(int dbTag, int commitTag, const XC::ID &theID, ChannelAddress *theAddress)
  {
    if(!checkDbTag(dbTag))
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "error when checking dbTag." << std::endl;
    // check that we have a connection
    if(!connection)
      return -1;

    // check that the data will fit, MySQL has a limit on blob sizes
    const int sizeData = theID.Size() * sizeof(int);
    if(sizeData > MAX_BLOB_SIZE)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; vector too big to send to MySQL databse, enlarge BLOBS!";
        return  -2;
      }


    //
    // to send the data the database we do the following:
    // 1. try to INSERT the data into the database
    // 2. if INSERT fails we have to reformulate the query to UPDATE the existing data,
    //    as the row given by dbTag, commitTag and vectorSize may already already exist.
    //

    // form the INSERT query
    query= "INSERT INTO IDs VALUES (" + boost::lexical_cast<std::string>(dbTag) + ",";
    query+= boost::lexical_cast<std::string>(commitTag) + ",";
    query+= boost::lexical_cast<std::string>(theID.Size()) + ",'";
    char *tmp= new char[2 * sizeData];
    mysql_escape_string(tmp, reinterpret_cast<const char *>(theID.getDataPtr()), sizeData); // add vector values in binary form
    query+= std::string(tmp) + "')";

    // execute the INSERT query
    if(mysql_query(&mysql, query.c_str()) != 0)
      {

        //
        // if INSERT fails we reformulate query and try an UPDATE
        //

        // formulate the query
        query= "UPDATE Vectors SET data='";
        mysql_escape_string(tmp, reinterpret_cast<const char *>(theID.getDataPtr()), sizeData);
        query+= std::string(tmp) + "' WHERE dbTag= " + boost::lexical_cast<std::string>(dbTag) + " AND commitTag= " + boost::lexical_cast<std::string>(commitTag) + " AND size= " + boost::lexical_cast<std::string>(theID.Size());

        // invoke the query on the database
        if(mysql_query(&mysql, query.c_str()) != 0)
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; failed to send the ID to MySQL database"
		      << std::endl << mysql_error(&mysql) << std::endl;
            return -3;
          }
      }
    delete tmp;
    return 0;
  }

int XC::MySqlDatastore::recvID(int dbTag, int commitTag,ID &theID,ChannelAddress *theAddress)
  {
    if(!checkDbTag(dbTag))
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; error when checking dbTag." << std::endl;
    // check that we have a connection
    if(!connection)
      return -1;

    // check that query string is big enough to hold the data, if not enlarge
    //const int sizeData= theID.Size() * sizeof(int);

    //
    // to receive the data the database we do the following:
    // 1. use SELECT to receive the data from the database
    // 2. fetch the results from the server and copy to the IDs data area
    // 3. clean up the MYSQL_RES datastructure
    //
    // NOTE: probably using a malloc() and free() under the covers for this, could be
    // very expensive .. IS THERE ANOTHER WAY TO DO THIS, i.e. SAVE A MYSQL_RES FOR
    // NUMEROUS USES??

    // form the SELECT query
    query= "Select data FROM IDs WHERE dbTag= " + boost::lexical_cast<std::string>(dbTag) + " AND commitTag= " + boost::lexical_cast<std::string>(commitTag) + " AND size= " + boost::lexical_cast<std::string>(theID.Size());

    // execute the SELECT query
    if(mysql_query(&mysql, query.c_str()) != 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; failed to receive vector from MySQL database"
		  << std::endl << mysql_error(&mysql) << std::endl;
        return -3;
      }

    // fetch the results from the database
    MYSQL_RES *results;
    MYSQL_ROW row;
    results = mysql_store_result(&mysql);
    if(results == nullptr)
      {
        // no vector stored in XC::db with these keys
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; no data in database for XC::ID with dbTag, cTag: "
		  << dbTag << ", " << commitTag << std::endl;
        return -4;
      }
    row = mysql_fetch_row(results);
    if(row == nullptr)
      {
        // no vector stored in XC::db with these keys
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; no data in database for ID with dbTag, cTag: "
		  << dbTag << ", " << commitTag << std::endl;
        mysql_free_result(results);
        return -5;
      }

    // place the results into the vectors double array
    int *intPtrData = (int *)row[0];
    for(int i=0; i<theID.Size(); i++)
      theID[i] = intPtrData[i];


    // free the MYSQL_RES structure
    mysql_free_result(results);
    return 0;
  }




int XC::MySqlDatastore::createTable(const std::string &tableName, const std::vector<std::string> &columns)
  {
    const int numColumns= columns.size();
    // check that we have a connection
    if(connection == false)
      return -1;

    // create the sql query
    query= "CREATE TABLE " + tableName + " (dbRun INT NOT nullptr, commitTag INT NOT nullptr, ";
    for(int j=0; j<numColumns; j++)
      query+= columns[j] + " DOUBLE NOT nullptr, ";
    query+= "PRIMARY KEY (dbRun, commitTag) )";


    // execute the query
    if(mysql_query(&mysql, query.c_str()) != 0)
      {
        if(mysql_errno(&mysql) != ER_TABLE_EXISTS_ERROR)
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; failed to create the table in the database"
		      << std::endl << mysql_error(&mysql) << std::endl
		      << "SQL query: " << query << std::endl;
            return -3;
          }
      }
    return 0;
  }

int XC::MySqlDatastore::insertData(const std::string &tableName,const std::vector<std::string> &columns, int commitTag, const Vector &data)
  {
    // check that we have a connection
    if(!connection)
      return -1;

    // check that query string is big enough to hold the data, if not enlarge
    //const int sizeData = 128 + strlen(tableName);

    // form the insert query
    query= "INSERT INTO " + tableName + "VALUES (" + boost::lexical_cast<std::string>(dbRun) + "," + boost::lexical_cast<std::string>(commitTag);
    for(int i=0; i<data.Size(); i++)
      query+= boost::lexical_cast<std::string>(data(i));
    query+= ")";

    // execute the query
    if(mysql_query(&mysql, query.c_str()) != 0)
      {
        //
        // if INSERT fails we reformulate query and try an UPDATE
        //

        // formulate the query
        query= "UPDATE " + tableName + " SET " + columns[0] + "= " + boost::lexical_cast<std::string>(data(0));
        for(int i=1; i<data.Size(); i++)
          query+= columns[i] + "= " + boost::lexical_cast<std::string>(data(i));
        query+= " WHERE dbRun= " + boost::lexical_cast<std::string>(dbTAG) + " AND commitTag= " + boost::lexical_cast<std::string>(commitTag);

        // invoke the query on the database
        if(mysql_query(&mysql,query.c_str()) != 0)
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; failed to send the data to MySQL database "
		      << query << std::endl << mysql_error(&mysql)
		      << std::endl;
            return -3;
          }
      }
    return 0;
  }

int XC::MySqlDatastore::getData(const std::string &tableName,const std::vector<std::string> &columns, int commitTag, Vector &data)
  {
    // check that we have a connection
    if(!connection)
      return -1;

    // check that query string is big enough to hold the data, if not enlarge
    //const int sizeData = 128 + strlen(tableName);


    // to receive the data the database we do the following:
    // 1. use SELECT to receive the data from the database
    // 2. fetch the results from the server and copy to the Vectors data area
    // 3. clean up the MYSQL_RES datastructure
    //
    // NOTE: probably using a malloc() and free() under the covers for this, could be
    // very expensive .. IS THERE ANOTHER WAY TO DO THIS, i.e. SAVE A MYSQL_RES FOR
    // NUMEROUS USES??

    // form the SELECT query
    query= "Select * FROM " + tableName + " WHERE dbRun= " + boost::lexical_cast<std::string>(dbRun) + " AND commitTag= " + boost::lexical_cast<std::string>(commitTag);

    // execute the SELECT query
    if(mysql_query(&mysql, query.c_str()) != 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; failed to receive vector from MySQL database"
		  << std::endl << mysql_error(&mysql) << std::endl;
        return -3;
      }

    // fetch the results from the database
    MYSQL_RES *results;
    MYSQL_ROW row;
    results = mysql_store_result(&mysql);
    if(results == nullptr)
      {
        // no vector stored in XC::db with these keys
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; no data in database for XC::Vector with dbTag, cTag: "
		  << dbTAG << ", " << commitTag << std::endl;
        return -4;
      }
    row = mysql_fetch_row(results);
    if(row == nullptr)
      {
        // no vector stored in XC::db with these keys
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; no data in database for XC::Vector with dbTag, cTag: "
		  << dbTAG << ", " << commitTag << std::endl;
        mysql_free_result(results);
        return -5;
      }

    // place the results into the vectors double array
    char *resultsData;
    for(int i=0; i<data.Size(); i++)
      {
        resultsData= row[i+2];
        data[i]= atof(resultsData);
      }

    // free the MYSQL_RES structure
    mysql_free_result(results);
    return 0;
  }


int XC::MySqlDatastore::setDbRun(int run)
  {
    dbRun = run;
    return dbRun;
  }

int XC::MySqlDatastore::getDbRun(void)
  { return dbRun; }


int XC::MySqlDatastore::createOpenSeesDatabase(const std::string &projectName)
  {

    // create the database
    query= "CREATE DATABASE "+ projectName;
    if(this->execute(query.c_str()) != 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; could not create the database\n";
        return -1;
      }

    // link to the database,
    if(mysql_select_db(&mysql, projectName.c_str()) != 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; could not set the database\n";
        return -2;
      }

    // now create the tables in the database
    query= "CREATE TABLE Messages ( dbTag INT NOT nullptr, commitTag INT NOT nullptr, size INT NOT nullptr,\
                                          data MEDIUMBLOB, PRIMARY KEY (dbTag, commitTag, size) )";

    if(this->execute(query.c_str()) != 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; could not create the Messagess table\n";
        return -3;
      }

    query= "CREATE TABLE Matrices ( dbTag INT NOT nullptr, commitTag INT NOT nullptr, size INT NOT nullptr,\
                                         data MEDIUMBLOB, PRIMARY KEY (dbTag, commitTag, size) )";

    if(this->execute(query.c_str()) != 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; could not create the Matricess table\n";
        return -3;
      }

    query= "CREATE TABLE Vectors ( dbTag INT NOT nullptr, commitTag INT NOT nullptr, size INT NOT nullptr,\
                                         data MEDIUMBLOB, PRIMARY KEY (dbTag, commitTag, size) )";

    if(this->execute(query.c_str()) != 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; could not create the Vectors table\n";
        return -3;
      }

    query= "CREATE TABLE IDs ( dbTag INT NOT nullptr, commitTag INT NOT nullptr, size INT NOT nullptr,\
                                     data MEDIUMBLOB, PRIMARY KEY (dbTag, commitTag, size) )";

    if(this->execute(query.c_str()) != 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; could not create the ID's table\n";
        return -3;
      }
    // done
    return 0;
  }

int XC::MySqlDatastore::execute(const std::string &query)
  {

    if(mysql_query(&mysql,query.c_str()) != 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; could not execute command: " << query
		  << std::endl << mysql_error(&mysql) << std::endl;
        return -1;
      }
    else
      return 0;
  }

