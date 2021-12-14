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
// $Revision: 1.3 $
// $Date: 2005/11/07 21:34:25 $
// $Source: /usr/local/cvs/OpenSees/SRC/database/OracleDatastore.cpp,v $


// Written: Jun Peng  (junpeng@stanford.edu)
//
// Description: This file contains the class implementation for XC::OracleDatastore.
// OracleDatastore is a concrete subclas of DBDatastore. A OracleDatastore
// object is used in the program to store/restore the geometry and state
// information in a model at a particular instance in the analysis. The
// information is stored in text files.
//
// What: "@(#) OracleDatastore.C, revA"


#include "utility/database/OracleDatastore.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <cstdio>


#include <utility/actor/actor/MovableObject.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <utility/matrix/ID.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>

#define IsCreateTable  0

extern "C" void connectToDB(char *theUser, char* thePass);
extern "C" void commitDatabase();
extern "C" void createIDTable(char *tableName);
extern "C" void createVectorTable(char *tableName);
extern "C" void createMatrixTable(char *tableName);
extern "C" void cleanTables();

extern "C" void changeProjTag(int projectID);

// a method to query the project Tag on TagTab.
extern "C" int queryProjTag();
// a method to query the project Tag based on its name.
extern "C" int queryTagOnName(const char *projName);

extern "C" void insertIDTable(int idSize, int *tempProjTag, int *tempDataTag,int *tempCommitTag, int *tempPos, int *tempID);

extern "C" void queryIDTable(int idSize, int projTag, int dataTag,int commitTag, int *tempID);

extern "C" void insertVectorTable(int vectSize, int *tempProjTag, int *tempDataTag,int *tempCommitTag, int *tempPos, double *tempVect);
extern "C" void queryVectorTable(int vectSize, int projTag, int dataTag,int commitTag, double *tempVect);

extern "C" void insertMatrixTable(int matrixSize, int *tempProjTag, int *tempDataTag,int *tempCommitTag, int *row, int *col, double *tempMatrix);
extern "C" void queryMatrixTable(int rowSize, int colSize, int projTag, int dataTag,int commitTag, double *tempMatrix);



XC::OracleDatastore::OracleDatastore(const std::string &dataBaseName,Preprocessor &preprocessor, FEM_ObjectBroker &theObjBroker)
  :DBDatastore(preprocessor, theObjBroker)
  {
    std::string junpeng("junpeng"); 
    std::string g3iscool("g3iscool"); 
    connectToDB(const_cast<char *>(junpeng.c_str()), const_cast<char *>(g3iscool.c_str()));
    projTag = this->searchProjTag(dataBaseName);

    if(IsCreateTable)
      {
        std::string id("ID"); 
        std::string vec("VECTOR");
        std::string mat("MATRIX");
        createIDTable(const_cast<char *>(id.c_str()));
        createVectorTable(const_cast<char *>(vec.c_str()));
        createMatrixTable(const_cast<char *>(mat.c_str()));
      }
    //  cleanTables();
  }


XC::OracleDatastore::~OracleDatastore(void)
  { commitDatabase(); }


int XC::OracleDatastore::getProjTag(void)
  { return projTag; }

/* normally this method will not be directly used for saving data,
 * only used for data query.
 */
void XC::OracleDatastore::setProjTag(int projectID)
  { projTag = projectID; }

/* Search "PROJTAB" to find the projTag based on its name,
 * which is a key.
 */
int XC::OracleDatastore::searchProjTag(const std::string &projName)
  {
    int projTag = queryTagOnName(const_cast<char *>(projName.c_str()));
    return projTag;
  }

/********************************************************************
 *                   CHANNEL METHODS  THAT DO NOTHING               *
 ********************************************************************/

char *XC::OracleDatastore::addToProgram(void)
  { return 0; }

//! @brief Return \f$0\f$.
int XC::OracleDatastore::setUpShadow(void)
  { return 0; }

//! @brief Return \f$0\f$.
int XC::OracleDatastore::setUpActor(void)
  { return 0; }

int XC::OracleDatastore::setNextAddress(const XC::ChannelAddress &otherChannelAddress)
  { return 0; }


XC::ChannelAddress *XC::OracleDatastore::getLastSendersAddress(void)
  { return 0; }


/********************************************************************
 *                USEFUL METHODS  TO STORE/RETRIEVE DATA           *
 ********************************************************************/

int XC::OracleDatastore::commitState(int commitTag)
  {
    int result= DBDatastore::commitState(commitTag);
    return result;
  }

int XC::OracleDatastore::sendObj(int commitTag,MovableObject &theObject,ChannelAddress *theAddress)
  { return sendMovable(commitTag,theObject); }

int XC::OracleDatastore::recvObj(int commitTag,MovableObject &theObject,FEM_ObjectBroker &theNewBroker,ChannelAddress *theAddress)
  { return receiveMovable(commitTag,theObject,theNewBroker); }


int XC::OracleDatastore::sendMsg(int dataTag, int commitTag,const XC::Message &,ChannelAddress *theAddress)
  {
    std::cerr << "OracleDatastore::sendMsg() - not yet implemented\n";
    return -1;
  }

int XC::OracleDatastore::recvMsg(int dataTag, int commitTag,Message &,ChannelAddress *theAddress)
  {
    std::cerr << "OracleDatastore::recvMsg() - not yet implemented\n";
    return -1;
  }


int XC::OracleDatastore::sendMatrix(int dataTag, int commitTag, const Matrix &theMatrix, ChannelAddress *theAddress)
  {
    if(!checkDbTag(dataTag))
      std::cerr << "Error en OracleDatastore::sendMatrix." << std::endl;
    int numRows = theMatrix.noRows();
    int numCols = theMatrix.noCols();
    int matrixSize = numRows * numCols;
    int pt;

    int *tempProjTag = new int[matrixSize];
    int *tempDataTag = new int[matrixSize];
    int *tempCommitTag = new int[matrixSize];
    int *row = new int[matrixSize];
    int *col = new int[matrixSize];
    double *tempMatrix = new double[matrixSize];

    for(int i = 0; i < numRows; i++)
      {
        for(int j = 0; j < numCols; j++)
          {
            pt = i *numCols + j;
            tempProjTag[pt] = projTag;
            tempDataTag[pt] = dataTag;
            tempCommitTag[pt] = commitTag;
            row[pt] = i;
            col[pt] = j;
            tempMatrix[pt] = theMatrix(i, j);
          }
      }
    insertMatrixTable(matrixSize, tempProjTag, tempDataTag, tempCommitTag, row, col, tempMatrix);
    delete [] tempDataTag;
    delete [] tempCommitTag;
    delete [] row;
    delete [] col;
    delete [] tempMatrix;
    return 0;
  }


int XC::OracleDatastore::recvMatrix(int dataTag, int commitTag,Matrix &theMatrix,ChannelAddress *theAddress)
  {
    if(!checkDbTag(dataTag))
      std::cerr << "Error en OracleDatastore::recvMatrix." << std::endl;
    int numRows = theMatrix.noRows();
    int numCols = theMatrix.noCols();
    int matrixSize = numRows * numCols;

    double *tempMatrix = new double[matrixSize];

    queryMatrixTable(numRows, numCols, projTag, dataTag, commitTag, tempMatrix);

    for(int i = 0; i < numRows; i++)
      {
        for(int j = 0; j < numCols; j++)
          { theMatrix(i, j) = tempMatrix[i*numCols + j]; }
      }
    delete [] tempMatrix;
    return 0;
  }


int XC::OracleDatastore::sendVector(int dataTag, int commitTag,const XC::Vector &theVector,ChannelAddress *theAddress)
  {
    if(!checkDbTag(dataTag))
      std::cerr << "Error en OracleDatastore::sendVector." << std::endl;
    int vectSize = theVector.Size();
    int *tempProjTag = new int[vectSize];
    int *tempDataTag = new int[vectSize];
    int *tempCommitTag = new int[vectSize];
    int *tempPos = new int[vectSize];
    double *tempVect = new double[vectSize];

    for(int i=0; i<vectSize; i++)
      {
        tempProjTag[i] = projTag;
        tempDataTag[i] = dataTag;
        tempCommitTag[i] = commitTag;
        tempPos[i] = i;
        tempVect[i] = theVector(i);
      }
    insertVectorTable(vectSize, tempProjTag, tempDataTag, tempCommitTag, tempPos, tempVect);

    delete [] tempProjTag;
    delete [] tempDataTag;
    delete [] tempCommitTag;
    delete [] tempPos;
    delete [] tempVect;
    return 0;
  }

int XC::OracleDatastore::recvVector(int dataTag, int commitTag,Vector &theVector,ChannelAddress *theAddress)
  {
    if(!checkDbTag(dataTag))
      std::cerr << "Error en OracleDatastore::recvVector." << std::endl;
    int vectSize = theVector.Size();
    double *tempVect = new double[vectSize];

    queryVectorTable(vectSize, projTag, dataTag, commitTag, tempVect);

    for(int i=0; i<vectSize; i++)
      { theVector(i) = tempVect[i]; }
    delete [] tempVect;
    return 0;
  }



int XC::OracleDatastore::sendID(int dataTag, int commitTag,const XC::ID &theID,ChannelAddress *theAddress)
  {
    if(!checkDbTag(dataTag))
      std::cerr << "Error en OracleDatastore::sendID." << std::endl;
    int idSize = theID.Size();
    int *tempProjTag = new int[idSize];
    int *tempDataTag = new int[idSize];
    int *tempCommitTag = new int[idSize];
    int *tempPos = new int[idSize];
    int *tempID = new int[idSize];

    for(int i=0; i<idSize; i++)
      {
        tempProjTag[i] = projTag;
        tempDataTag[i] = dataTag;
        tempCommitTag[i] = commitTag;
        tempPos[i] = i;
        tempID[i] = theID(i);
      }

    insertIDTable(idSize, tempProjTag, tempDataTag, tempCommitTag, tempPos, tempID);

    delete [] tempDataTag;
    delete [] tempCommitTag;
    delete [] tempPos;
    delete [] tempID;
    return 0;
  }


int XC::OracleDatastore::recvID(int dataTag, int commitTag,ID &theID,ChannelAddress *theAddress)
  {
    if(!checkDbTag(dataTag))
      std::cerr << "Error en OracleDatastore::recvID." << std::endl;
    int idSize = theID.Size();
    int *tempID = new int[idSize];

    queryIDTable(idSize, projTag, dataTag, commitTag, tempID);

    for(int i=0; i<idSize; i++)
      { theID(i) = tempID[i]; }
    delete [] tempID;
    return 0;
  }

