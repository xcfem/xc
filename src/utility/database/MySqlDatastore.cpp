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
** (C) Copyright 2002, The Regents of the University of California    **
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
        std::cerr << "MySqlDatastore::MySqlDatastore() - could not connect to server\n";
        std::cerr << mysql_error(&mysql) << std::endl;
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
              std::cerr << "MySqlDatastore::MySqlDatastore() - could not open the database\n";
              std::cerr << mysql_error(&mysql) << std::endl;
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
            std::cerr << "MySqlDatastore::MySqlDatastore() - could not connect to server\n";
            std::cerr << mysql_error(&mysql) << std::endl;
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
                    std::cerr << "MySqlDatastore::MySqlDatastore() - could not open the database\n";
                    std::cerr << mysql_error(&mysql) << std::endl;
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
    std::cerr << "MySqlDatastore::sendMsg() - not yet implemented\n";
    return -1;
  }

int XC::MySqlDatastore::recvMsg(int dbTag, int commitTag, Message &, ChannelAddress *theAddress)
  {
    std::cerr << "MySqlDatastore::recvMsg() - not yet implemented\n";
    return -1;
  }



int XC::MySqlDatastore::sendMatrix(int dbTag, int commitTag, const XC::Matrix &theMatrix, ChannelAddress *theAddress)
  {
    if(!checkDbTag(dbTag))
      std::cerr << "Error en MySqlDatastore::sendMatrix." << std::endl;
    // check that we have a connection
    if(connection == false)
      return -1;

    // check that the data will fit, MySQL has a limit on blob sizes
    const int sizeData = theMatrix.getDataSize() * sizeof(double);
    if(sizeData > MAX_BLOB_SIZE)
      {
        std::cerr << "MySqlDatastore::sendMatrix - vector too big to send to MySQL databse, enlarge BLOBS!";
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
            std::cerr << "MySqlDatastore::sendMatrix() - failed to send the XC::Matrix to MySQL database";
            std::cerr << std::endl << mysql_error(&mysql) << std::endl;
            return -3;
          }
      }
    delete tmp;
    return 0;
  }

int XC::MySqlDatastore::recvMatrix(int dbTag, int commitTag, Matrix &theMatrix, ChannelAddress *theAddress)
  {
    if(!checkDbTag(dbTag))
      std::cerr << "Error en MySqlDatastore::recvMatrix." << std::endl;

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
        std::cerr << "MySqlDatastore::recvMatrix() - failed to receive vector from MySQL database";
        std::cerr << std::endl << mysql_error(&mysql) << std::endl;
        return -3;
      }

    // fetch the results from the database
    MYSQL_RES *results;
    MYSQL_ROW row;
    results = mysql_store_result(&mysql);
    if(!results)
      {
        // no vector stored in db with these keys
        std::cerr << "MySqlDatastore::recvMatrix - no data in database for XC::Matrix with dbTag, cTag: ";
        std::cerr << dbTag << ", " << commitTag << std::endl;
        return -4;
      }
    row = mysql_fetch_row(results);
    if(row == nullptr)
      {
        // no vector stored in db with these keys
        std::cerr << "MySqlDatastore::recvMatrix - no data in database for XC::Matrix with dbTag, cTag: ";
        std::cerr << dbTag << ", " << commitTag << std::endl;
        mysql_free_result(results);
        return -5;
      }

    // place the results into the vectors double array
    const double *data = reinterpret_cast<const double *>(row[0]);
    double *matrixData= theMatrix.getDataPtr();
    for(int i=0; i<theMatrix.getDataSize(); i++)
      matrixData[i] = data[i];

    // free the MYSQL_RES structure
    mysql_free_result(results);
    return 0;
  }



int XC::MySqlDatastore::sendVector(int dbTag, int commitTag, const XC::Vector &theVector, ChannelAddress *theAddress)
  {
    if(!checkDbTag(dbTag))
      std::cerr << "Error en MySqlDatastore::sendVector." << std::endl;
    // check that we have a connection
    if(!connection)
      return -1;

    // check that the data will fit, MySQL has a limit on blob sizes
    const int sizeData = theVector.Size() * sizeof(double);
    if(sizeData > MAX_BLOB_SIZE)
      {
        std::cerr << "MySqlDatastore::sendVector - vector too big to send to MySQL databse, enlarge BLOBS!";
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
            std::cerr << "MySqlDatastore::sendVector() - failed to send the XC::Vector to MySQL database";
            std::cerr << std::endl << mysql_error(&mysql) << std::endl;
            return -3;
          }
      }
    delete tmp;
    return 0;
  }

int XC::MySqlDatastore::recvVector(int dbTag, int commitTag, Vector &theVector,ChannelAddress *theAddress)
  {
    if(!checkDbTag(dbTag))
      std::cerr << "Error en MySqlDatastore::recvVector." << std::endl;
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
        std::cerr << "MySqlDatastore::recvVector() - failed to receive vector from MySQL database";
        std::cerr << std::endl << mysql_error(&mysql) << std::endl;
        return -3;
      }

    // fetch the results from the database
    MYSQL_RES *results;
    MYSQL_ROW row;
    results= mysql_store_result(&mysql);
    if(results == nullptr)
      {
        // no vector stored in XC::db with these keys
        std::cerr << "MySqlDatastore::recvVector - no data in database for XC::Vector with dbTag, cTag: ";
        std::cerr << dbTag << ", " << commitTag << std::endl;
        return -4;
      }
    row = mysql_fetch_row(results);
    if(row == nullptr)
      {
        // no vector stored in XC::db with these keys
        std::cerr << "MySqlDatastore::recvVector - no data in database for XC::Vector with dbTag, cTag: ";
        std::cerr << dbTag << ", " << commitTag << std::endl;
        mysql_free_result(results);
        return -5;
      }

    // place the results into the vectors double array
    double *data = (double *)row[0];
    for(int i=0; i<theVector.Size(); i++)
      theVector[i] = data[i];


    // free the MYSQL_RES structure
    mysql_free_result(results);
    return 0;
  }


int XC::MySqlDatastore::sendID(int dbTag, int commitTag, const XC::ID &theID, ChannelAddress *theAddress)
  {
    if(!checkDbTag(dbTag))
      std::cerr << "Error en MySqlDatastore::sendID." << std::endl;
    // check that we have a connection
    if(!connection)
      return -1;

    // check that the data will fit, MySQL has a limit on blob sizes
    const int sizeData = theID.Size() * sizeof(int);
    if(sizeData > MAX_BLOB_SIZE)
      {
        std::cerr << "MySqlDatastore::sendID - vector too big to send to MySQL databse, enlarge BLOBS!";
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
            std::cerr << "MySqlDatastore::sendID() - failed to send the XC::ID to MySQL database";
            std::cerr << std::endl << mysql_error(&mysql) << std::endl;
            return -3;
          }
      }
    delete tmp;
    return 0;
  }

int XC::MySqlDatastore::recvID(int dbTag, int commitTag,ID &theID,ChannelAddress *theAddress)
  {
    if(!checkDbTag(dbTag))
      std::cerr << "Error en MySqlDatastore::recvID." << std::endl;
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
        std::cerr << "MySqlDatastore::recvID() - failed to receive vector from MySQL database";
        std::cerr << std::endl << mysql_error(&mysql) << std::endl;
        return -3;
      }

    // fetch the results from the database
    MYSQL_RES *results;
    MYSQL_ROW row;
    results = mysql_store_result(&mysql);
    if(results == nullptr)
      {
        // no vector stored in XC::db with these keys
        std::cerr << "MySqlDatastore::recvID - no data in database for XC::ID with dbTag, cTag: ";
        std::cerr << dbTag << ", " << commitTag << std::endl;
        return -4;
      }
    row = mysql_fetch_row(results);
    if(row == nullptr)
      {
        // no vector stored in XC::db with these keys
        std::cerr << "MySqlDatastore::recvID - no data in database for XC::ID with dbTag, cTag: ";
        std::cerr << dbTag << ", " << commitTag << std::endl;
        mysql_free_result(results);
        return -5;
      }

    // place the results into the vectors double array
    int *data = (int *)row[0];
    for(int i=0; i<theID.Size(); i++)
      theID[i] = data[i];


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
            std::cerr << "MySqlDatastore::createTable() - failed to create the table in the database";
            std::cerr << std::endl << mysql_error(&mysql) << std::endl;
            std::cerr << "SQL query: " << query << std::endl;
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
            std::cerr << "MySqlDatastore::insertData() - failed to send the data to MySQL database";
            std::cerr << query;
            std::cerr << std::endl << mysql_error(&mysql) << std::endl;
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
        std::cerr << "MySqlDatastore::getData() - failed to receive vector from MySQL database";
        std::cerr << std::endl << mysql_error(&mysql) << std::endl;
        return -3;
      }

    // fetch the results from the database
    MYSQL_RES *results;
    MYSQL_ROW row;
    results = mysql_store_result(&mysql);
    if(results == nullptr)
      {
        // no vector stored in XC::db with these keys
        std::cerr << "MySqlDatastore::getData - no data in database for XC::Vector with dbTag, cTag: ";
        std::cerr << dbTAG << ", " << commitTag << std::endl;
        return -4;
      }
    row = mysql_fetch_row(results);
    if(row == nullptr)
      {
        // no vector stored in XC::db with these keys
        std::cerr << "MySqlDatastore::getData - no data in database for XC::Vector with dbTag, cTag: ";
        std::cerr << dbTAG << ", " << commitTag << std::endl;
        mysql_free_result(results);
        return -5;
      }

    // place the results into the vectors double array
    char *dataRes;
    for(int i=0; i<data.Size(); i++)
      {
        dataRes= row[i+2];
        data[i]= atof(dataRes);
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
        std::cerr << "MySqlDatastore::createOpenSeesDatabase() - could not create the database\n";
        return -1;
      }

    // link to the database,
    if(mysql_select_db(&mysql, projectName.c_str()) != 0)
      {
        std::cerr << "MySqlDatastore::createOpenSeesDatabase() - could not set the database\n";
        return -2;
      }

    // now create the tables in the database
    query= "CREATE TABLE Messages ( dbTag INT NOT nullptr, commitTag INT NOT nullptr, size INT NOT nullptr,\
                                          data MEDIUMBLOB, PRIMARY KEY (dbTag, commitTag, size) )";

    if(this->execute(query.c_str()) != 0)
      {
        std::cerr << "MySqlDatastore::createOpenSeesDatabase() - could not create the Messagess table\n";
        return -3;
      }

    query= "CREATE TABLE Matrices ( dbTag INT NOT nullptr, commitTag INT NOT nullptr, size INT NOT nullptr,\
                                         data MEDIUMBLOB, PRIMARY KEY (dbTag, commitTag, size) )";

    if(this->execute(query.c_str()) != 0)
      {
        std::cerr << "MySqlDatastore::createOpenSeesDatabase() - could not create the Matricess table\n";
        return -3;
      }

    query= "CREATE TABLE Vectors ( dbTag INT NOT nullptr, commitTag INT NOT nullptr, size INT NOT nullptr,\
                                         data MEDIUMBLOB, PRIMARY KEY (dbTag, commitTag, size) )";

    if(this->execute(query.c_str()) != 0)
      {
        std::cerr << "MySqlDatastore::createOpenSeesDatabase() - could not create the Vectors table\n";
        return -3;
      }

    query= "CREATE TABLE IDs ( dbTag INT NOT nullptr, commitTag INT NOT nullptr, size INT NOT nullptr,\
                                     data MEDIUMBLOB, PRIMARY KEY (dbTag, commitTag, size) )";

    if(this->execute(query.c_str()) != 0)
      {
        std::cerr << "MySqlDatastore::createOpenSeesDatabase() - could not create the XC::ID's table\n";
        return -3;
      }
    // done
    return 0;
  }

int XC::MySqlDatastore::execute(const std::string &query)
  {

    if(mysql_query(&mysql,query.c_str()) != 0)
      {
        std::cerr << "MySqlDatastore::execute() - could not execute command: " << query;
        std::cerr << std::endl << mysql_error(&mysql) << std::endl;
        return -1;
      }
    else
      return 0;
  }

