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
** file 'COPYRIGHT'  in XC::main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.5 $
// $Date: 2006/01/10 18:16:20 $
// $Source: /usr/local/cvs/OpenSees/SRC/database/NEESData.cpp,v $
                                                                        
// Written: fmk 
// Created: 03/04
//
// Description: This file contains the class definition for NeesData.
// NeesData is a concretes subclass of XC::FE_Datastore. NEESData datastore
// is used to store data from the recorders in a file format that can
// be stored in the NEES data repository.

// NEESData objects currently only implement the createTable() and 
// insertDataIntoTable() methods of the abstract base XC::FE_Datastore
// class. At this time the other methods should never be called.
//
// What: "@(#) NEESData.C, revA"

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <iomanip>

#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <utility/matrix/ID.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include <utility/database/NEESData.h>

XC::NEESData::NEESData(const std::string &dataBaseName, Modelador &mdlr, FEM_ObjectBroker &theObjBroker) 
  :FE_Datastore(mdlr, theObjBroker), dataBase(dataBaseName), tables(0), numTables(0)  {}

XC::NEESData::~NEESData(void) 
  {
    // clean up all memory allocated
    for(int i=0; i<numTables; i++)
      {
        NEES_table* nextTable = tables->next;
        tables->columns.erase(tables->columns.begin(),tables->columns.end());
        delete tables;
        tables = nextTable;
      }
  }

int XC::NEESData::getDbTag(void) const
  {
    std::cerr << "XC::NEESData::getDbTag(void) - should not be called\n";
    return -1;
  }

int XC::NEESData::commitState(int commitTag)
  {
    std::cerr << "XC::NEESData::commitState(int commitTag) - should not be called\n";
    return -1;
  }

int XC::NEESData::sendMsg(int dataTag, int commitTag, const XC::Message &, ChannelAddress *theAddress)
  {
    std::cerr << "XC::NEESData::sendMsg(int dataTag, int commitTag,  - should not be called\n";
    return -1;
  }		       

int XC::NEESData::recvMsg(int dataTag, int commitTag, Message &,  ChannelAddress *theAddress)
  {
    std::cerr << "XC::NEESData::recvMsg(int dataTag, int commitTag,  - should not be called\n";
    return -1;
  }		       


int XC::NEESData::sendMatrix(int dataTag, int commitTag, const XC::Matrix &theMatrix, ChannelAddress *theAddress)
  {
    std::cerr << "XC::NEESData::sendMatrix(int dataTag, int commitTag,  - should not be called\n";
    return -1;
  }		       




int XC::NEESData::recvMatrix(int dataTag, int commitTag, Matrix &theMatrix, ChannelAddress *theAddress)    
  { return -1; }		       



int XC::NEESData::sendVector(int dataTag, int commitTag, const XC::Vector &theVector, ChannelAddress *theAddress)
  {
    std::cerr << "XC::NEESData::sendVector(int dataTag, int commitTag,  - should not be called\n";
    return -1;
  }		       

int XC::NEESData::recvVector(int dataTag, int commitTag, Vector &theVector, ChannelAddress *theAddress)    
  {
    std::cerr << "XC::NEESData::recvVector(int dataTag, int commitTag,  - should not be called\n";
    return -1;
  }		       



int XC::NEESData::sendID(int dataTag, int commitTag, const XC::ID &theID, ChannelAddress *theAddress)
  {
    std::cerr << "XC::NEESData::sendID(int dataTag, int commitTag,  - should not be called\n";
    return -1;
  }

int XC::NEESData::recvID(int dataTag, int commitTag, ID &theID, ChannelAddress *theAddress)    
  {
    std::cerr << "XC::NEESData::recvID(int dataTag, int commitTag,  - should not be called\n";
    return -1;
  }		       


#include <fstream>


int XC::NEESData::createTable(const std::string &tableName, const std::vector<std::string> &columns)
  {
    const int numColumns= columns.size();
    // create a copy of the data file name
    int res = 0;
    std::string copyTableName;
    std::string fileName;

    bool hasOutExtension = false;
    copyTableName= tableName;
    const size_t pos= tableName.find(".out");
    if(pos!=std::string::npos)
      {
        copyTableName= tableName.substr(0,pos);
        hasOutExtension= true;
      }

    // first ensure createTable has not already been called with this table name
    NEES_table *t = tables;
    for(int i=0; i<numTables; i++)
      {
        if(t->name==tableName)
          {
            std::cerr << "WARNING: XC::NEESData::createTable - table already exists: " << copyTableName << std::endl;
            return -1;
          }
        t = t->next;
      }

    fileName= copyTableName + ".out";

    if(numColumns <= 0)
      {
        std::cerr << "WARNING: XC::NEESData::createTable - number of data columns < 0 for table name: " << copyTableName << std::endl;
        return -1;
      }

    // 
    // test opening the data file
    // 

    std::ofstream dataFile;
    dataFile.open(fileName.c_str(), std::ios::out | std::ios::trunc); 

    if(!dataFile.is_open())
      {
        std::cerr << "XC::NEESData::createTable - failed to open file: " << fileName << std::endl;
        return -1;
      }

    // 
    // now open the xml file & write the data to it
    // 

    fileName= copyTableName + ".xml";

    std::ofstream xmlFile;
    xmlFile.open(fileName.c_str(), std::ios::out | std::ios::trunc); 
  
    if(xmlFile.is_open())
      {
        // write the xml data
        xmlFile << "<?xml version=\"1.0\"?>\n";
        xmlFile << "<NumericalFileDataDescription>\n";
        xmlFile << "\t<DataFile>\n";
        xmlFile << "\t\t<DataFileName> " << copyTableName << ".out </DataFileName>\n";
        xmlFile << "\t\t<NumberDataColumns> " << numColumns << "</NumberDataColumns>\n";
        xmlFile << "\t</DataFile>\n";
        for(int i=0; i<numColumns; i++)
          {
            xmlFile << "\t<DataColumnDescription>\n";
            xmlFile << "\t\t<ColumnLocation> " << i+1 << "</ColumnLocation>\n";      
            xmlFile << "\t\t<Description> " << columns[i] << "</Description>\n";
            xmlFile << "\t</DataColumnDescription>\n";
          }
        xmlFile << "</NumericalFileDataDescription>\n";
        xmlFile.close();
      }
    else
      {
        std::cerr << "XC::NEESData::createData - failed to open file: " << copyTableName << std::endl;
        res = -1;
      }

    // if we can get here we can open the data and have written to the xml file
    // now add a new struct to our list of tables
    NEES_table *nextTable= new NEES_table;
    if(nextTable == 0)
      {
        std::cerr << "XC::NEESData::createData - out of memory creating Table structure for table: " << copyTableName << std::endl;
        res = -1;
      }    
    fileName= tableName;

    nextTable->name = fileName;
    nextTable->columns.resize(numColumns);
    nextTable->hasOutExtension = hasOutExtension;

    for(int k=0; k<numColumns; k++)
      nextTable->columns[k]= columns[k];    

    nextTable->next= tables;
    tables = nextTable;
    numTables++;
    return 0;
  }

int XC::NEESData::insertData(const std::string &tableName, const std::vector<std::string> &columns, int commitTag, const Vector &data)
  {
    // search the tables for valid table
    NEES_table *t= tables;
    for(int i=0; i<numTables; i++)
      {
        if(t->name==tableName) 
          i = numTables;
        else 
          t = t->next;
      }

    if( t == 0)
      {
        std::cerr << "XC::NEESData::insertData - table: " << tableName << " has not been created\n";
        return -1;
      }      

    if(t->getNumColumns() != static_cast<size_t>(data.Size()))
      {
        std::cerr << "XC::NEESData::insertData - incorrect number of columns for table: " << tableName << "\n";
        return -2;
      }

    std::string fileName= t->name;
    if(t->hasOutExtension == false)
      fileName+= ".out";

    std::ofstream table;
    table.open(fileName.c_str(), std::ios::app); 

    table << setiosflags(std::ios::scientific);
    table << std::setprecision(16);

    if(table.is_open())
      {
        // write the data
        for(int i=0; i<data.Size(); i++)
          { table << data(i) << "\t"; }
    
        table << "\n";
        table.close();
      }
    else
      {
        std::cerr << "NEESData::insertData - failed to open file: " << fileName << std::endl;
        return -1;
      }
    fileName = tableName;
    return 0;
  }


int XC::NEESData::getData(const std::string &tableName, const std::vector<std::string> &columns, int commitTag, Vector &data)
  { return 0; }

