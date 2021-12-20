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

// $Revision: 1.19 $
// $Date: 2006/01/12 23:25:31 $
// $Source: /usr/local/cvs/OpenSees/SRC/database/FileDatastore.cpp,v $


// Written: fmk
// Created: 10/98
//
// Description: This file contains the class implementation for FileDatastore.
// FileDatastore is a concrete subclas of FE_Datastore. A FileDatastore
// object is used in the program to store/restore the geometry and state
// information in a model at a particular instance in the analysis. The
// information is stored in text files.
//
// What: "@(#) FileDatastore.C, revA"

#include "utility/database/FileDatastore.h"

#include <cstring>
#include <cstdlib>
#include <cstdio>

#include <iostream>
#include <iomanip>


#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include "preprocessor/Preprocessor.h"
#include <utility/matrix/ID.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include "boost/lexical_cast.hpp"
#include "domain/mesh/element/Element.h"
#include "domain/mesh/node/Node.h"

void XC::FileDatastoreOutputFile::resetFilePointers(IntData &theIntData, const std::vector<char> &charPtrData)
  {
    if(theFile)
      {
	theFile->seekp(0, std::ios::beg);
	*(theIntData.dbTag)= maxDbTag;
	theFile->write(charPtrData.data(), sizeof(int));
	theFile->close();
	// delete theFile;
	// theFile= nullptr;
      }
  }

int XC::FileDatastoreOutputFile::openFile(const std::string &fileName, int dataSize, IntData &theIntData, std::vector<char> &charPtrData)
  {
    std::shared_ptr<fstream> res(new fstream());
    if(res == 0)
      {
        std::cerr << "FileDatastoreOutputFile::" << __FUNCTION__
		  << "; out of memory; failed to open file: " << fileName
		  << std::endl;
        return 0;
      }

    res.get()->open(fileName.c_str(), std::ios::in | std::ios::out | std::ios::binary);

    // if file did not exist, need to pass trunc flag to open it
    if(res.get()->bad() == true || res.get()->is_open() == false)
      {
	// delete & new again for unix gcc compiler to work!
	res= std::shared_ptr<fstream>(new fstream());
	if(res == 0)
	  {
	    std::cerr << "FileDatastoreOutputFile::" << __FUNCTION__
		      << "; out of memory; failed to open file: "
		      << fileName << std::endl;
	    return -1;
	  }
	res.get()->open(fileName.c_str(), std::ios::in | std::ios::out | std::ios::trunc | std::ios::binary);
      }

    if(res.get()->bad() == true || res.get()->is_open() == false)
      {
	 std::cerr << "FileDatastoreOutputFile::" << __FUNCTION__
		   << "; FATAL - could not open file: "
		   << fileName << std::endl;
	 res= std::shared_ptr<fstream>(nullptr);
	 return -1;
      }

    // set the position for writing to eof
    res.get()->seekp(0,std::ios::end);
    STREAM_POSITION_TYPE fileEnd= res.get()->tellp();
    int maxDataTag= 0;

    if(fileEnd == 0 || fileEnd == -1)
      {
	*(theIntData.dbTag)= maxDataTag;
	res.get()->write(charPtrData.data(), sizeof(int));
	fileEnd= sizeof(int);
	maxDataTag= -1;
      }
    else
      {
        res.get()->seekg(0, std::ios::beg);
        res.get()->read(charPtrData.data(), sizeof(int));
        maxDataTag= *(theIntData.dbTag);
      }

    // move to start of data part
    res.get()->seekp(sizeof(int), std::ios::beg);
    res.get()->seekg(sizeof(int), std::ios::beg);

    // fill in the structure data
    theFile= res;
    fileEnd= fileEnd;
    maxDbTag= maxDataTag; 	

    return 0;
  }

void XC::FileDatastore::free_mem(void)
  {}

void XC::FileDatastore::alloc(const size_t &sz)
  {
    free_mem();
    charPtrData.resize(sz);
  }

//! @brief Constructor.
//!
//! Opens the files for the domain and base component relations, files have
//! names {\em name.relation}, and stores the end of file locations. Creates
//! three arrays of file pointers for the ID, Vector and Matrix files and then
//! zeros these arrays. If the files could not be opened, or there is not enough
//! memory for the arrays an error message is printed and the program
//! is terminated.
XC::FileDatastore::FileDatastore(const std::string &dataBaseName,Preprocessor &preprocessor, FEM_ObjectBroker &theObjBroker)
  :FE_Datastore(preprocessor, theObjBroker), dataBase(dataBaseName), charPtrData(), currentMaxInt(0), currentMaxDouble(0)
  { resizeDouble(1024); }

//! @brief Destructor.
//! 
//! Each file that is opened is closed and the arrays of file pointers
//! obtained from the heap in the constructor are returned to the heap.
XC::FileDatastore::~FileDatastore(void)
  {
    free_mem();

    theIDFiles.clear();
    theMatFiles.clear();
    theVectFiles.clear();
  }


int XC::FileDatastore::commitState(int commitTag)
  {
    int result= FE_Datastore::commitState(commitTag);
    if(result == commitTag)
      resetFilePointers();
    return result;
  }


void XC::FileDatastore::resetFilePointers(void)
  {
    for(theIDFilesIter= theIDFiles.begin(); theIDFilesIter != theIDFiles.end(); theIDFilesIter++)
      {
	theIDFilesIter->second.resetFilePointers(theIntData, charPtrData);
      }

    for(theMatFilesIter= theMatFiles.begin(); theMatFilesIter != theMatFiles.end(); theMatFilesIter++)
      {
	theMatFilesIter->second.resetFilePointers(theIntData, charPtrData);
      }

    for(theVectFilesIter= theVectFiles.begin(); theVectFilesIter != theVectFiles.end(); theVectFilesIter++)
      {
	theVectFilesIter->second.resetFilePointers(theIntData, charPtrData);
      }
    currentCommitTag= -1;
  }


//! @brief Prints an error message and returns \f$-1\f$ as not yet implemented.
int XC::FileDatastore::sendMsg(int dBTag, int commitTag, const XC::Message &, ChannelAddress *theAddress)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not yet implemented\n";
    return -1;
  }

//! @brief Prints an error message and returns \f$-1\f$ as not yet implemented.
int XC::FileDatastore::recvMsg(int dBTag, int commitTag, Message &, ChannelAddress *theAddress)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not yet implemented\n";
    return -1;
  }


std::string XC::FileDatastore::getFileName(const std::string &tp, int idSize,int commitTag) const
  { return dataBase + tp + boost::lexical_cast<std::string>(idSize)+"."+boost::lexical_cast<std::string>(commitTag); }

XC::FileDatastoreOutputFile *XC::FileDatastore::getFileStruct(FilesMap &filesMap, const std::string &tp, int objSize, int stepSize, int commitTag)
  {    
    FileDatastoreOutputFile *retval= nullptr;
    FilesMap::iterator filesIter= filesMap.find(objSize);
    if(filesIter == filesMap.end())
      {

        // we first check if we need to resize send buffer
        if(objSize > currentMaxInt)
          {
            if(this->resizeInt(objSize) < 0)
              {
	        std::cerr << getClassName() << "::" << __FUNCTION__
			  << "; failed in resizeInt()\n";
	        return retval;
              }
          }

        const std::string fileName= getFileName(tp,objSize,commitTag);

        FileDatastoreOutputFile theFileStruct;
        if(theFileStruct.openFile(fileName, stepSize, theIntData, charPtrData) < 0)
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; could not open file\n";
            return retval;
          }
        else
	  {
            filesMap.insert(FilesMap_type(objSize, theFileStruct));
            FilesMap::iterator tmp= filesMap.find(objSize);
	    assert(tmp!=filesMap.end());
	    retval= &(tmp->second);
	  }
      }
    else
      {
        FileDatastoreOutputFile &theFileStruct= filesIter->second;
	retval= &theFileStruct;
        // make sure not close from a last commit
        if(theFileStruct.getFStream() == nullptr)
          {
            if(objSize > currentMaxInt)
              {
                if(this->resizeInt(objSize) < 0)
                  {
                    std::cerr << getClassName() << "::" << __FUNCTION__
			      << "; failed in resizeInt()\n";
                    return retval;
	          }
              }
            const std::string fileName= getFileName(tp,objSize,commitTag);
            if(theFileStruct.openFile(fileName, stepSize, theIntData, charPtrData) < 0)
              {
        	std::cerr << getClassName() << "::" << __FUNCTION__
			  << "; could not open file\n";
                return retval;
              }
          }
      }
    if(!retval)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; something went wrong." << std::endl;
    return retval;
  }

//! @brief Send (write) the ID.
//!
//! //! If a file for IDs of this size has not yet been created, one is created
//! now and the cell in the array of file pointers is set. If file can not be
//! created a warning message is printed and program is terminated. A
//! sequential search is made in the file to see if information is already
//! stored for a ID with this \p dbTag and \p commitTag. The data is then
//! written at this location, or eof if no location was found. The end of file
//! location for IDss of this size is updated. If successful \f$0\f$ is
//! returned. A warning message and a negative number is returned if the
//! operation fails: \f$-1\f$ if ID size is too large.
int XC::FileDatastore::sendID(int dBTag, int commitTag, const ID &theID, ChannelAddress *theAddress)
  {
    if(!checkDbTag(dBTag))
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "ERROR in checkDBTag." << std::endl;

    if(currentCommitTag != commitTag)
      this->resetFilePointers();

    currentCommitTag= commitTag;


    //
    // next we see if we already have this file;
    //  if not we need to create data structure & open it
    //  if we have data structure, need to check file is opened (we close in a commit)
    //

    const int idSize= theID.Size();
    const int stepSize= (1 + idSize)*sizeof(int);

    FileDatastoreOutputFile *theFileStruct= getFileStruct(theIDFiles, ".IDs.", idSize, stepSize, commitTag);
    
    //
    // find location in file to place the data
    //

    fstream *theStream= theFileStruct->getFStream();
    bool found= false;
    STREAM_POSITION_TYPE pos= theStream->tellg();
    STREAM_POSITION_TYPE fileEnd= theFileStruct->fileEnd;

    // we first check if the data can go at the end of the file
    // true if commitTag larger than any we have encountered so far

    if(theFileStruct->maxDbTag < dBTag)
      {
        pos= fileEnd;
        found= true;
        theFileStruct->maxDbTag= dBTag;
      }

    // try current location
    if(pos < fileEnd && found == false)
      {
        theStream->read(charPtrData.data(), stepSize);
        if(*(theIntData.dbTag) == dBTag )
          { found= true; }
      }

    // we have to search from the beginning of the file
    if(found == false)
      {
        *(theIntData.dbTag)= -1;
        pos= sizeof(int);
        theStream->seekg(pos, std::ios::beg);
        while(pos < fileEnd && found == false)
          {
            theStream->read(charPtrData.data(), stepSize);
            if(*(theIntData.dbTag) == dBTag)
	      found= true;
            else
              pos += stepSize;
          }
        if(found == false)
         { pos= fileEnd; }
      }

    //
    // we now place the data to be sent into our buffer
    //

    *(theIntData.dbTag)= dBTag;
    for(int i=0; i<idSize; i++)
      theIntData.values[i]= theID(i);

    //
    // we now write the data
    //

    theStream->seekp(pos, std::ios::beg); // reset so can go write at the end

    theStream->write(charPtrData.data(), stepSize);
    if(theStream->bad())
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; error writing to file\n";
        return -1;
      }

    // update the size of file if we have added to eof
    if(fileEnd <= pos)
      theFileStruct->fileEnd= pos + stepSize;

    return 0;
  }		

//! @brief Receive (read) the ID.
//!
//! If a file for IDs of this size has not yet been created, an error message
//! is printed and \f$-1\f$ is returned.  A sequential search
//! is made in the file to see if information is already stored for a ID with
//! this \p dbTag and \p commitTag. If no information is stored a
//! \f$-1\f$ is returned. If information is stored, the information is
//! retrieved and the data in the ID is set. Returns \f$0\f$ if successful. 
int XC::FileDatastore::recvID(int dBTag, int commitTag, ID &theID, ChannelAddress *theAddress)
  {
    if(!checkDbTag(dBTag))
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "Error in checkDbTag." << std::endl;
    if(currentCommitTag != commitTag)
      this->resetFilePointers();

    currentCommitTag= commitTag;

    //
    // next we see if we already have this file;
    //  if not we need to create data structure & open it
    //  if we have data structure, need to check file is opened (we close in a commit)
    //

    const int idSize= theID.Size();
    const int stepSize= (1 + idSize)*sizeof(int);

    FileDatastoreOutputFile *theFileStruct= getFileStruct(theIDFiles, ".IDs.", idSize, stepSize, commitTag);

    fstream *theStream= theFileStruct->getFStream();
    STREAM_POSITION_TYPE fileEnd= theFileStruct->fileEnd;
    STREAM_POSITION_TYPE pos= theStream->tellg();

    //
    // find location in file to place the data
    //

    bool found= false;

    // we try the current file position first

    if(pos < fileEnd)
      {
        theStream->read(charPtrData.data(), stepSize);
        if(*(theIntData.dbTag) == dBTag)
          {
            found= true;
            pos+= stepSize;
          }
      }

    // we must search from the beginning of the file
    if(found == false)
      {
        pos= sizeof(int);
        theStream->seekg(pos, std::ios::beg);
        while((pos < fileEnd) && (found == false))
          {
            theStream->read(charPtrData.data(), stepSize);
            if(*(theIntData.dbTag) == dBTag)
	      found= true;
            else
	      pos += stepSize;
          }
      }

    if(found == false)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; failed\n";
        return -1;
      }
    //std::cerr << "READ: " << dBTag << " " << pos << std::endl;

    // we now place the received data into the XC::ID
    for(int i=0; i<idSize; i++)
      theID(i)= theIntData.values[i];
    return 0;
  }		

//! @brief Send (write) the matrix.
//!
//! First determines the size of the matrix, \f$noRows * noCols\f$. If a files
//! for matrices of this size has not yet been created, one is created now
//! and the cell in the array of file pointers is set. If file can not be
//! created a warning message is printed and program is terminated. A sequential
//! search is made in the file to see if information is already stored for a
//! Matrix with this \p dbTag and \p commitTag. The data is then written at this
//! location, or eof if no location was found. The end of file location
//! for Matrices of this size is updated. If successful \f$0\f$ is
//! returned. A warning message and a negative number is returned if the
//! operation fails: \f$-1\f$ if Matrix size is too large.
int XC::FileDatastore::sendMatrix(int dBTag, int commitTag,const Matrix &theMatrix,ChannelAddress *theAddress)
  {
    if(!checkDbTag(dBTag))
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "Error in checkDbTag." << std::endl;

    if(currentCommitTag != commitTag)
      this->resetFilePointers();

    currentCommitTag= commitTag;

    //
    // next we see if we already have this file;
    //  if not we need to create data structure & open it
    //  if we have data structure, need to check file is opened (we close in a commit)
    //

    // we first ensure that the XC::Matrix is not too big
    const int noMatCols= theMatrix.noCols();
    const int noMatRows= theMatrix.noRows();
    const int matSize= noMatRows * noMatCols;
    const int stepSize= sizeof(int) + matSize*sizeof(double);
    
    FileDatastoreOutputFile *theFileStruct= getFileStruct(theMatFiles, ".MATs.", matSize, stepSize, commitTag);

    //
    // find location in file to place the data
    //

    fstream *theStream= theFileStruct->getFStream();
    bool found= false;
    STREAM_POSITION_TYPE pos= theStream->tellg();
    STREAM_POSITION_TYPE fileEnd= theFileStruct->fileEnd;

    // we first check if the data can go at the end of the file
    // true if commitTag larger than any we have encountered so far

    if(theFileStruct->maxDbTag < dBTag)
      {
        pos= fileEnd;
        found= true;
        theFileStruct->maxDbTag= dBTag;
      }

    // try current location
    if(pos < fileEnd && found == false)
      {
        theStream->read(charPtrData.data(), stepSize);
        if(*(theIntData.dbTag) == dBTag )
          { found= true; }
      }

    // we have to search from the beginning of the file
    if(found == false)
      {
        *(theIntData.dbTag)= -1;
        pos= sizeof(int);
        theStream->seekg(pos, std::ios::beg);
        while(pos < fileEnd && found == false)
          {
            theStream->read(charPtrData.data(), stepSize);
            if(*(theIntData.dbTag) == dBTag)
	      found= true;
            else
	      pos += stepSize;
          }
        if(!found)
          { pos= fileEnd; }
      }

    //
    // we now place the data to be sent into our buffer
    //

    *(theDoubleData.dbTag)= dBTag;
    int loc=0;
    for(int j=0; j<noMatCols; j++)
    for(int k=0; k < noMatRows; k++)
      {
        theDoubleData.values[loc]= theMatrix(k,j);
        loc++;
      }

    //
    // we now write the data
    //

    theStream->seekp(pos, std::ios::beg); // reset so can go write at the end
    theStream->write(charPtrData.data(), stepSize);

    // update the size of file if we have added to eof
    if(theFileStruct->fileEnd <= pos)
      theFileStruct->fileEnd += stepSize;
    return 0;
  }		

//! @brief Receive (read) the matrix.
//!
//! First determines the size of the matrix, \f$noRows * noCols\f$. If a files
//! for matrices of this size has not yet been created, an error message
//! is printed and \f$-1\f$ is returned.  A sequential search
//! is made in the file to see if information is already stored for a Matrix
//! with this \p dbTag and \p commitTag. If no information is stored a
//! \f$-1\f$ is returned. If information is stored, the information is
//! retrieved and the data in the Matrix is set. returns \f$0\f$ if
//! successful.
int XC::FileDatastore::recvMatrix(int dBTag, int commitTag,Matrix &theMatrix,ChannelAddress *theAddress)
  {
    if(!checkDbTag(dBTag))
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "Error in checkDbTag." << std::endl;

    if(currentCommitTag != commitTag)
      this->resetFilePointers();

    currentCommitTag= commitTag;

  //
  // next we see if we already have this file;
  //  if not we need to create data structure & open it
  //  if we have data structure, need to check file is opened (we close in a commit)
  //

  // we first check XC::Matrix not too big
  const int noMatCols= theMatrix.noCols();
  const int noMatRows= theMatrix.noRows();
  const int matSize= noMatRows * noMatCols;
  const int stepSize= sizeof(int) + matSize*sizeof(double);
  
  FileDatastoreOutputFile *theFileStruct= getFileStruct(theMatFiles, ".MATs.", matSize, stepSize, commitTag);

  fstream *theStream= theFileStruct->getFStream();
  STREAM_POSITION_TYPE fileEnd= theFileStruct->fileEnd;
  STREAM_POSITION_TYPE pos= theStream->tellg();

  //
  // find location in file to place the data
  //

  bool found= false;

  // we try the current file position first

  if(pos < fileEnd) {
    theStream->read(charPtrData.data(), stepSize);
    if((*(theIntData.dbTag) == dBTag)) {
      found= true;
      pos += stepSize;
    }
  }

  // we must search from the beginning of the file
  if(found == false) {
    pos= sizeof(int);
    theStream->seekg(pos, std::ios::beg);
    while((pos < fileEnd) && (found == false)) {
      theStream->read(charPtrData.data(), stepSize);
      if(*(theIntData.dbTag) == dBTag)
	found= true;
      else
	pos += stepSize;
    }
  }

  if(found == false)
    {
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; failed\n";
      return -1;
    }

  int loc=0;
  for(int j=0; j<noMatCols; j++)
    for(int k=0; k < noMatRows; k++) {
      theMatrix(k,j)= theDoubleData.values[loc];
      loc++;
    }

  return 0;
}		

//! @brief Send (write) the vector.
//!
//! If a file for Vectors of this size has not yet been created, one is created
//! now and the cell in the array of file pointers is set. If file can not be
//! created a warning message is printed and program is terminated. A sequential
//! search is made in the file to see if information is already stored for a
//! Vector with this \p dbTag and \p commitTag. The data is then written at this
//! location, or eof if no location was found. The end of file location
//! for Vectors of this size is updated. If successful \f$0\f$ is
//! returned. A warning message and a negative number is returned if the
//! operation fails: \f$-1\f$ if Vector size is too large.
int XC::FileDatastore::sendVector(int dBTag, int commitTag,const Vector &theVector,ChannelAddress *theAddress)
  {
    if(!checkDbTag(dBTag))
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "Error in checkDbTag." << std::endl;

  if(currentCommitTag != commitTag)
    this->resetFilePointers();

  currentCommitTag= commitTag;

  //
  // next we see if we already have this file;
  //  if not we need to create data structure & open it
  //  if we have data structure, need to check file is opened (we close in a commit)
  //

  // we first ensure that the Matrix is not too big
  const int vectSize= theVector.Size();
  const int stepSize= sizeof(int) + vectSize*sizeof(double);
  
  FileDatastoreOutputFile *theFileStruct= getFileStruct(theVectFiles, ".VECs.", vectSize, stepSize, commitTag);

  //
  // find location in file to place the data
  //

  fstream *theStream= theFileStruct->getFStream();
  bool found= false;
  STREAM_POSITION_TYPE pos= theStream->tellg();
  STREAM_POSITION_TYPE fileEnd= theFileStruct->fileEnd;

  // we first check if the data can go at the end of the file
  // true if commitTag larger than any we have encountered so far
  found= false;


  if(theFileStruct->maxDbTag < dBTag)  {
    pos= fileEnd;
    found= true;
    theFileStruct->maxDbTag= dBTag;
  }


  // try current location
  if(pos < fileEnd && found == false) {
// #ifdef _WIN32
    // must be a bug in the vc compiler! .. we are here already tellg() above!!
    theStream->seekg(pos, std::ios::beg);
// #endif
    theStream->read(charPtrData.data(), stepSize);
    if(*(theIntData.dbTag) == dBTag ) {
      found= true;
    }
  }

  // we have to search from the beginning of the file
  if(found == false) {
    *(theIntData.dbTag)= -1;
    pos= sizeof(int);
    theStream->seekg(pos, std::ios::beg);
    while(pos < fileEnd && found == false) {
      theStream->read(charPtrData.data(), stepSize);

      if(*(theIntData.dbTag) == dBTag)
	found= true;
      else
	pos += stepSize;
    }

    if(found == false) {
      pos= fileEnd;
    }
  }

  //
  // we now place the data to be sent into our buffer
  //

  *(theDoubleData.dbTag)= dBTag;
  for(int i=0; i<vectSize; i++)
    theDoubleData.values[i]= theVector(i);

  //
  // we now write the data
  //

  theStream->seekp(pos, std::ios::beg); // reset so can go write at the end
  theStream->write(charPtrData.data(), stepSize);

  // update the size of file if we have added to eof
  if(theFileStruct->fileEnd <= pos)
    theFileStruct->fileEnd += stepSize;

  return 0;
}		

//! @brief Receive (read) the vector.
//!
//! If a file for Vectors of this size has not yet been created, an error
//! message is printed and \f$-1\f$ is returned.  A sequential search
//! is made in the file to see if information is already stored for a Vector
//! with this \p dbTag and \p commitTag. If no information is stored a
//! \f$-1\f$ is returned. If information is stored, the information is
//! retrieved and the data in the Vector is set. Returns \f$0\f$ if
//! successful.
int XC::FileDatastore::recvVector(int dBTag, int commitTag, Vector &theVector, ChannelAddress *theAddress)
{
    if(!checkDbTag(dBTag))
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "Error in checkDbTag." << std::endl;

  if(currentCommitTag != commitTag)
    this->resetFilePointers();

  currentCommitTag= commitTag;


  //
  // next we see if we already have this file;
  //  if not we need to create data structure & open it
  //  if we have data structure, need to check file is opened (we close in a commit)
  //

  // we first check XC::Vector not too big
  const int vectSize= theVector.Size();
  const int stepSize= sizeof(int) + vectSize*sizeof(double);
  
  FileDatastoreOutputFile *theFileStruct= getFileStruct(theVectFiles, ".VECs.", vectSize, stepSize, commitTag);

  fstream *theStream= theFileStruct->getFStream();
  STREAM_POSITION_TYPE fileEnd= theFileStruct->fileEnd;
  STREAM_POSITION_TYPE pos= theStream->tellg();

  //
  // find location in file to place the data
  //

  bool found= false;

  // we try the current file position first

  if(pos < fileEnd) {
    theStream->read(charPtrData.data(), stepSize);
    if((*(theIntData.dbTag) == dBTag)) {
      found= true;
      pos += stepSize;
    }
  }

  // we must search from the beginning of the file
  if(found == false) {
    pos= sizeof(int);
    theStream->seekg(pos, std::ios::beg);
    while((pos < fileEnd) && (found == false)) {
      theStream->read(charPtrData.data(), stepSize);
      if(*(theIntData.dbTag) == dBTag)
	found= true;
      else
	pos += stepSize;
    }
  }

  if(found == false) {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; failed\n";
    return -1;
  }

  for(int i=0; i<vectSize; i++)
    theVector(i)= theDoubleData.values[i];

  return 0;
}		


#include <fstream>


int XC::FileDatastore::createTable(const std::string &tableName,const std::vector<std::string> &columns)
  {
    const int numColumns= columns.size();
    // open the file
    int res= 0;
    const std::string fileName= dataBase + "." + tableName;

    std::ofstream table;
    table.open(fileName.c_str(), std::ios::out | std::ios::trunc);

    if(table.bad() == true || table.is_open() == false)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; failed to open file: " << fileName << std::endl;
        res= -1;
      }

    // write the data
    for(int i=0; i<numColumns; i++)
      { table << columns[i] << "\t"; }
    table << "\n";
    table.close();
    return res;
  }

int XC::FileDatastore::insertData(const std::string &tableName,const std::vector<std::string> &columns,int commitTag, const XC::Vector &vectorData)
  {
    // open the file
    const std::string fileName= dataBase + "." + tableName;

    std::ofstream table;
    table.open(fileName.c_str(), std::ios::app);
    if(table.bad() == true || table.is_open() == false)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; failed to open file: " << fileName << std::endl;
	return -1;
      }

    table << setiosflags(std::ios::scientific);
    table << std::setprecision(16);

    // write the data
    for(int i=0; i<vectorData.Size(); i++)
      { table << vectorData(i) << "\t"; }

    table << "\n";
    table.close();

    return 0;
  }


int XC::FileDatastore::getData(const std::string &tableName,const std::vector<std::string> &columns, int commitTag, Vector &)
  { return 0; }



int XC::FileDatastore::resizeInt(int newSz)
  {
    if(newSz <= 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; invalidSize: " << newSz << std::endl;
        return -1; // invalid size
      }
    const int sizeOfChar= sizeof(char);
    const int sizeOfInt= sizeof(int);
    const int sizeOfDouble= sizeof(double);
    const size_t newSize= (newSz+1)*sizeOfInt/sizeOfChar;

    const size_t sizeData= charPtrData.size();
    if(newSize < sizeData)
      return 0; // already big enough


    alloc(newSize);

    currentMaxInt= (sizeData/sizeOfChar-sizeOfInt)/sizeOfInt;
    currentMaxDouble= (sizeData/sizeOfChar-sizeOfInt)/sizeOfDouble;
    char *dataPtr= &(charPtrData.data())[sizeof(int)];
    theIntData.dbTag= reinterpret_cast<int *>(charPtrData.data());
    theIntData.values= reinterpret_cast<int *>(dataPtr);
    theDoubleData.dbTag= reinterpret_cast<int *>(charPtrData.data());
    theDoubleData.values= reinterpret_cast<double *>(dataPtr);
    return 0;
  }

int XC::FileDatastore::resizeDouble(int newSz)
  {
    if(newSz <= 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; invalidSize: " << newSz << std::endl;
        return -1; // invalid size
      }
    const int sizeOfChar= sizeof(char);
    const int sizeOfInt= sizeof(int);
    const int sizeOfDouble= sizeof(double);
    const size_t newSize= (newSz*sizeOfDouble+sizeOfInt)/sizeOfChar;

    const size_t sizeData= charPtrData.size();
    if(newSize < sizeData)
      return 0; // already big enough

    alloc(newSize);

    currentMaxInt= (sizeOfChar*sizeData-sizeOfInt)/sizeOfInt;
    currentMaxDouble= (sizeOfChar*sizeData-sizeOfInt)/sizeOfDouble;
    char *dataPtr= &(charPtrData.data())[sizeof(int)];
    theIntData.dbTag= reinterpret_cast<int *>(charPtrData.data());
    theIntData.values= reinterpret_cast<int *>(dataPtr);
    theDoubleData.dbTag= reinterpret_cast<int *>(charPtrData.data());
    theDoubleData.values= reinterpret_cast<double *>(dataPtr);
    return 0;
  }
