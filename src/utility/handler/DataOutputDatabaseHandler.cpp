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
// $Date: 2004/11/24 22:40:16 $
// $Source: /usr/local/cvs/OpenSees/SRC/handler/DataOutputDatabaseHandler.cpp,v $
                                                                        
// Written: fmk 
// Date: 10/04
//
// Description: This file contains the class implementation for
// DataOutputDatabaseHandler. 
//
// What: "@(#) DataOutputDatabaseHandler.C, revA"

#include "utility/handler/DataOutputDatabaseHandler.h"
#include <utility/database/FE_Datastore.h>
#include <utility/matrix/Vector.h>
#include <cstring>

XC::DataOutputDatabaseHandler::DataOutputDatabaseHandler(FE_Datastore *database, const std::string &tName)
  :DataOutputHandler(DATAHANDLER_TAGS_DataOutputDatabaseHandler), theDatabase(database), tableName(tName), commitTag(0)
  {}

// int XC::DataOutputDatabaseHandler::open(char **dataDescription, int numData)
//   {
//     //
//     // check the args are valid & that database has been set
//     //

//     if(!theDatabase)
//       {
//         std::cerr << "XC::DataOutputStreamHandler::open() - database has not been set\n";
//         return -1;
//       } 
//     if(!tableName)
//       {
//         std::cerr << "XC::DataOutputDatabaseHandler::open() - no tableName passed or failed to get memory\n";
//         return -1;
//       }

//     if(!dataDescription)
//       {
//         std::cerr << "XC::DataOutputDatabaseHandler::open() - no column description data passed\n";
//         return -1;
//       }

//     if(numData < 0)
//       {
//         std::cerr << "XC::DataOutputDatabaseHandler::open() - numColumns (" << numData << ") < 0\n";
//         return -1;
//       }
//     else
//       numColumns = numData;

//     //
//     // remove old if open has already been called
//     //

//     if(!columns)
//       {
//         for(int j=0; j<numColumns; j++)
//           delete [] columns[j];
//         delete [] columns;
//         columns= nullptr;
//       }

//     //
//     // create memory to store the dataDescription and make a copy of it
//     //

//     columns = new char *[numColumns];
//     if(columns == 0)
//       {
//         std::cerr << "XC::DataOutputDatabaseHandler::open() - out of memory creating array for columns of size (" << numData << ") < 0\n";
//         numColumns = 0;
//         return -1;
//       } 

//     // make copy
//     for(int i=0; i<numColumns; i++)
//       {
//         columns[i] = new char[strlen(dataDescription[i])+1];
//         if(!columns[i])
//           {
//             std::cerr << "XC::DataOutputDatabaseHandler::open() - out of memory creating copy of string " << dataDescription[i] << std::endl;
//             for(int j=0; j<i; j++)
// 	      delete [] columns[j];
//             delete [] columns;
//             numColumns = 0;
//             return -1;
//           }
//         strcpy(columns[i], dataDescription[i]);
//       }

//     // ask database to create a table
//     return theDatabase->createTable(tableName, numColumns, columns);
//   }

int XC::DataOutputDatabaseHandler::open(const std::vector<std::string> &dataDescription)
  {
    //
    // check the args are valid & that database has been set
    //

    if(!theDatabase)
      {
        std::cerr << "XC::DataOutputStreamHandler::open() - database has not been set\n";
        return -1;
      } 
    if(tableName=="")
      {
        std::cerr << "XC::DataOutputDatabaseHandler::open() - no tableName passed or failed to get memory\n";
        return -1;
      }

    const int numData= dataDescription.size();
    if(numData < 0)
      {
        std::cerr << "XC::DataOutputDatabaseHandler::open() - numColumns (" << numData << ") < 0\n";
        return -1;
      }

    columns= dataDescription;
    // ask database to create a table
    return theDatabase->createTable(tableName, columns);
  }

int XC::DataOutputDatabaseHandler::write(Vector &data) 
  {
    int result = 0;

    if(static_cast<size_t>(data.Size()) == columns.size())
      if(theDatabase)
        result = theDatabase->insertData(tableName, columns, commitTag, data);
      else
        {
          std::cerr << "XC::DataOutputStreamHandler::write() - database has not been set\n";
          return -1;
        } 
    else
      {
        std::cerr << "XC::DataOutputStreamHandler::write() - Vector not of correct size or open() has not been called\n" << columns.size() << " " << data.Size() << std::endl;
        return -1;
      }
    commitTag++;
    return result;
  }

int XC::DataOutputDatabaseHandler::setDatabase(FE_Datastore &database, const std::string &tName)
  {
    tableName= tName;
    theDatabase = &database;
    return 0;
  }


int XC::DataOutputDatabaseHandler::sendSelf(CommParameters &)
  {
    std::cerr << "XC::DataOutputDatabaseHandler::sendSelf() - not yet implemented\n";
    return -1;
  }
int XC::DataOutputDatabaseHandler::recvSelf(const CommParameters &)
  {
    std::cerr << "XC::DataOutputDatabaseHandler::sendSelf() - not yet implemented\n";
    return -1;
  }
