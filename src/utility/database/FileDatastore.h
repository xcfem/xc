// -*-c++-*-
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
                                                                        
// $Revision: 1.14 $
// $Date: 2006/01/03 20:49:21 $
// $Source: /usr/local/cvs/OpenSees/SRC/utility/database/FileDatastore.h,v $
                                                                        
                                                                        
#ifndef FileDatastore_h
#define FileDatastore_h

// Written: fmk 
// Created: 10/98
//
// Description: This file contains the class definition for FileDatastore.
// FileDatastore is a concrete subclas of FE_Datastore. A FileDatastore 
// object is used in the program to store/restore the geometry and state 
// information in a modeler at a particular instance in the analysis. The
// information is stored in text files.
//
// What: "@(#) FileDatastore.h, revA"

#include <utility/database/FE_Datastore.h>

#include <fstream>
#include <map>
using std::fstream;
using std::map;


#define STREAM_POSITION_TYPE int

namespace XC {
class FEM_ObjectBroker;

//! @brief Output file stream wrapper.
typedef struct fileDatastoreOutputFile
  {
    fstream *theFile;
    STREAM_POSITION_TYPE fileEnd;
    int      maxDbTag;
  } FileDatastoreOutputFile;

typedef map<int, FileDatastoreOutputFile *>      MAP_FILES;
typedef MAP_FILES::value_type                    MAP_FILES_TYPE;
typedef MAP_FILES::iterator                      MAP_FILES_ITERATOR;

//! @brief Integer array wrapper.
typedef struct intData
  {
    int *dbTag;
    int *values;     
  } IntData;


//! @brief Double array wrapper.
typedef struct doubleData
  {
    int *dbTag;
    double *values;     
  } DoubleData;


//! @ingroup Database
//
//! @brief used in the program to store/restore the geometry and state
//! information in the domain at particular instances. This information is
//! stored in binary form in files.
//
//! An FileDatastore object is used in the program to store/restore the
//! geometry and state information in the domain at particular instances. This
//! information is stored in binary form in files. As no standard format is
//! used for the storage of integers and double values, files used to store
//! the data on one type of machine, may not be read by a FileDatastore object
//! on another type of machine where the storage of integers and doubles is
//! different.
//!
//! For each of the base relations, i.e. Domain, Nodes, Elements,
//! SFreedom\_Constraints, MFreedom\_Constraints, NodalLoads and ElementalLoads,
//! a separate file is used to store the information. Files are also used
//! for each size of ID, Vector and Matrix stored. At present, Messages
//! are not stored, only ID and Vector objects of size \f$<= 200\f$ can be
//! stored, the max \f$noRows * noCols\f$ of Matrices that can be stored
//! is \f$<= 2000\f$, and only a single relation is created for Matrices which
//! have similar sizes but differing dimensions. The data is stored in the
//! files following the schema outlined previously.
class FileDatastore: public FE_Datastore
  {
  private:
    // Private methods
    int resizeInt(int newSize);
    int resizeDouble(int newSize);
    void resetFilePointers(void);
    int openFile(const std::string &fileName, FileDatastoreOutputFile *, int dataSize);

    // private attributes
    std::string dataBase;
    MAP_FILES theIDFiles;
    MAP_FILES theVectFiles;
    MAP_FILES theMatFiles;
    MAP_FILES_ITERATOR theIDFilesIter;
    MAP_FILES_ITERATOR theVectFilesIter;
    MAP_FILES_ITERATOR theMatFilesIter;

    int lastDomainChangeStamp;
    int currentCommitTag;
    char *charPtrData;
    int sizeData;
    
    IntData    theIntData;
    DoubleData theDoubleData;

    int currentMaxInt;
    int currentMaxDouble;

    void free_mem(void);
    void alloc(const size_t &sz);
    std::string getFileName(const std::string &, int idSize,int commitTag) const;
  public:
    FileDatastore(const std::string &dataBase,Preprocessor &preprocessor, FEM_ObjectBroker &theBroker);    
    
    ~FileDatastore(void);

    // methods for sending and receiving the data
    int sendMsg(int dbTag, int commitTag, const Message &, ChannelAddress *theAddress =0);    
    int recvMsg(int dbTag, int commitTag, Message &, ChannelAddress *theAddress =0);        

    int sendMatrix(int dbTag, int commitTag, const Matrix &, ChannelAddress *theAddress= nullptr);
    int recvMatrix(int dbTag, int commitTag, Matrix &, ChannelAddress *theAddress= nullptr);
    
    int sendVector(int dbTag, int commitTag, const Vector &, ChannelAddress *theAddress= nullptr);
    int recvVector(int dbTag, int commitTag, Vector &, ChannelAddress *theAddress= nullptr);
    
    int sendID(int dbTag, int commitTag,const ID &, ChannelAddress *theAddress= nullptr);
    int recvID(int dbTag, int commitTag,ID &, ChannelAddress *theAddress= nullptr);

    int createTable(const std::string &tableName,const std::vector<std::string> &columns);
    int insertData(const std::string &tableName,const std::vector<std::string> &columns, int commitTag, const Vector &);
    int getData(const std::string &tableName,const std::vector<std::string> &columns, int commitTag, Vector &);

    // the commitState method
    int commitState(int commitTag);        
  };
} // end of XC namespace

#endif

