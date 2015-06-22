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
// $Revision: 1.3 $
// $Date: 2005/11/07 21:34:25 $
// $Source: /usr/local/cvs/OpenSees/SRC/database/OracleDatastore.cpp,v $


// Written: Jun Peng  (junpeng@stanford.edu)
//
// Description: This file contains the class implementation for XC::OracleDatastore.
// OracleDatastore is a concrete subclas of DBDatastore. A OracleDatastore
// object is used in the program to store/restore the geometry and state
// information in a modelador at a particular instance in the analysis. The
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



XC::OracleDatastore::OracleDatastore(const std::string &dataBaseName,Modelador &mdlr, FEM_ObjectBroker &theObjBroker)
  :DBDatastore(mdlr, theObjBroker)
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

int XC::OracleDatastore::setUpShadow(void)
  { return 0; }

int XC::OracleDatastore::setUpActor(void)
  { return 0; }

int XC::OracleDatastore::setNextAddress(const XC::ChannelAddress &otherChannelAddress)
  { return 0; }


XC::ChannelAddress *XC::OracleDatastore::getLastSendersAddress(void)
  { return 0; }


/********************************************************************
 *                USEFULE METHODS  TO STORE/RETRIEVE DATA           *
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

