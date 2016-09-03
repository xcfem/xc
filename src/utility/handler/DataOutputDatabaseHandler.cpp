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
