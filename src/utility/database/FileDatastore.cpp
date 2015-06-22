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

// $Revision: 1.19 $
// $Date: 2006/01/12 23:25:31 $
// $Source: /usr/local/cvs/OpenSees/SRC/database/FileDatastore.cpp,v $


// Written: fmk
// Created: 10/98
//
// Description: This file contains the class implementation for FileDatastore.
// FileDatastore is a concrete subclas of XC::FE_Datastore. A FileDatastore
// object is used in the program to store/restore the geometry and state
// information in a modelador at a particular instance in the analysis. The
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
#include "modelador/Modelador.h"
#include <utility/matrix/ID.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include "boost/lexical_cast.hpp"
#include "domain/mesh/element/Element.h"
#include "domain/mesh/node/Node.h"

void XC::FileDatastore::libera(void)
  {
    if(data)
      {
        delete data;
        data= nullptr;
        sizeData= 0;
      }
  }
void XC::FileDatastore::alloc(const size_t &sz)
  {
    libera();
    data= new char[sz];
    if(data)
      sizeData= sz;
    else
      {
        std::cerr << "FileDatastore::alloc(size_t sz) - out of memory for size: " << sz << std::endl;
        sizeData= 0;
      }
  }

XC::FileDatastore::FileDatastore(const std::string &dataBaseName,Modelador &mdlr, FEM_ObjectBroker &theObjBroker)
  :FE_Datastore(mdlr, theObjBroker), dataBase(dataBaseName), data(nullptr), sizeData(0), currentMaxInt(0), currentMaxDouble(0)
  { resizeDouble(1024); }

XC::FileDatastore::~FileDatastore(void)
  {
    libera();
    //  while(theIDFilesIter != theIDFiles.end()) {
    //    theIDFilesIter++;
    // }

    for(theIDFilesIter = theIDFiles.begin(); theIDFilesIter != theIDFiles.end(); theIDFilesIter++)
      {
        FileDatastoreOutputFile *theFileStruct = theIDFilesIter->second;
        fstream *theFile = theFileStruct->theFile;
        if(theFile)
          {
            theFile->close();
            delete theFile;
          }
        delete theFileStruct;
      }

    theIDFiles.clear();

    for(theMatFilesIter = theMatFiles.begin(); theMatFilesIter != theMatFiles.end(); theMatFilesIter++)
      {
        FileDatastoreOutputFile *theFileStruct = theMatFilesIter->second;
        fstream *theFile = theFileStruct->theFile;
        if(theFile)
          {
            theFile->close();
            delete theFile;
          }
        delete theFileStruct;
      }
    theMatFiles.clear();
    for(theVectFilesIter = theVectFiles.begin(); theVectFilesIter != theVectFiles.end(); theVectFilesIter++)
      {
        FileDatastoreOutputFile *theFileStruct = theVectFilesIter->second;
        fstream *theFile = theFileStruct->theFile;
        if(theFile)
          {
            theFile->close();
            delete theFile;
          }
        delete theFileStruct;
      }
    theVectFiles.clear();
  }


int XC::FileDatastore::commitState(int commitTag)
  {
    int result = XC::FE_Datastore::commitState(commitTag);
    if(result == commitTag)
      resetFilePointers();
    return result;
  }


void XC::FileDatastore::resetFilePointers(void)
  {

  for(theIDFilesIter = theIDFiles.begin(); theIDFilesIter != theIDFiles.end(); theIDFilesIter++) {
    FileDatastoreOutputFile *theFileStruct = theIDFilesIter->second;
    fstream *theFile = theFileStruct->theFile;
    if(theFile != 0) {
      theFile->seekp(0, std::ios::beg);
      *(theIntData.dbTag) = theFileStruct->maxDbTag;
      theFile->write(data, sizeof(int));
      theFile->close();
      delete theFile;
      theFileStruct->theFile = 0;
    }
  }

  for(theMatFilesIter = theMatFiles.begin(); theMatFilesIter != theMatFiles.end(); theMatFilesIter++) {
    FileDatastoreOutputFile *theFileStruct = theMatFilesIter->second;
    fstream *theFile = theFileStruct->theFile;
    if(theFile != 0) {
      theFile->seekp(0, std::ios::beg);
      *(theIntData.dbTag) = theFileStruct->maxDbTag;
      theFile->write(data, sizeof(int));
      theFile->close();
      delete theFile;
      theFileStruct->theFile = 0;
    }
  }

  for(theVectFilesIter = theVectFiles.begin(); theVectFilesIter != theVectFiles.end(); theVectFilesIter++) {
    FileDatastoreOutputFile *theFileStruct = theVectFilesIter->second;
    fstream *theFile = theFileStruct->theFile;
    if(theFile != 0) {
      theFile->seekp(0, std::ios::beg);
      *(theIntData.dbTag) = theFileStruct->maxDbTag;
      theFile->write(data, sizeof(int));
      theFile->close();
      delete theFile;
      theFileStruct->theFile = 0;
    }
  }
  currentCommitTag = -1;
}


int XC::FileDatastore::sendMsg(int dBTag, int commitTag, const XC::Message &, ChannelAddress *theAddress)
  {
    std::cerr << "FileDatastore::sendMsg() - not yet implemented\n";
    return -1;
  }

int XC::FileDatastore::recvMsg(int dBTag, int commitTag, Message &, ChannelAddress *theAddress)
  {
    std::cerr << "FileDatastore::recvMsg() - not yet implemented\n";
    return -1;
  }


std::string XC::FileDatastore::getFileName(const std::string &tp, int idSize,int commitTag) const
  { return dataBase + tp + boost::lexical_cast<std::string>(idSize)+"."+boost::lexical_cast<std::string>(commitTag); }

int XC::FileDatastore::sendID(int dBTag, int commitTag, const ID &theID, ChannelAddress *theAddress)
  {
    if(!checkDbTag(dBTag))
      std::cerr << "Error en FileDatastore::sendID." << std::endl;

    if(currentCommitTag != commitTag)
      this->resetFilePointers();

    currentCommitTag = commitTag;

    FileDatastoreOutputFile *theFileStruct =0;

    //
    // next we see if we already have this file;
    //  if not we need to create data structure & open it
    //  if we have data structure, need to check file is opened (we close in a commit)
    //

    int idSize = theID.Size();
    int stepSize = (1 + idSize)*sizeof(int);

    theIDFilesIter = theIDFiles.find(idSize);
    if(theIDFilesIter == theIDFiles.end())
      {

        // we first check if we need to resize send buffer
        if(idSize > currentMaxInt)
          {
            if(this->resizeInt(idSize) < 0)
              {
	        std::cerr << "FileDatastore::sendID() - failed in resizeInt()\n";
	        return -1;
              }
          }

        theFileStruct = new FileDatastoreOutputFile;

        if(!theFileStruct)
          {
            std::cerr << "FileDatastore::sendID() - out of memory\n";
            return -1;
          }

        const std::string fileName= getFileName(".IDs.",idSize,commitTag);

        if(this->openFile(fileName, theFileStruct, stepSize) < 0)
          {
            std::cerr << "FileDatastore::sendID() - could not open file\n";
            return -1;
          }
        else
          theIDFiles.insert(MAP_FILES_TYPE(idSize, theFileStruct));
      }
    else
      {
        theFileStruct = theIDFilesIter->second;
        // make sure not close from a last commit
        if(theFileStruct->theFile == 0)
          {
            if(idSize > currentMaxInt)
              {
                if(this->resizeInt(idSize) < 0)
                  {
                    std::cerr << "FileDatastore::sendID() - failed in resizeInt()\n";
                    return -1;
	          }
              }
            const std::string fileName= getFileName(".IDs.",idSize,commitTag);
            if(this->openFile(fileName, theFileStruct, stepSize) < 0)
              {
        	std::cerr << "FileDatastore::sendID() - could not open file\n";
                return -1;
              }
          }
      }

    //
    // find location in file to place the data
    //

    fstream *theStream = theFileStruct->theFile;
    bool found = false;
    STREAM_POSITION_TYPE pos = theStream->tellg();
    STREAM_POSITION_TYPE fileEnd = theFileStruct->fileEnd;

    // we first check if the data can go at the end of the file
    // true if commitTag larger than any we have encountered so far

    if(theFileStruct->maxDbTag < dBTag)
      {
        pos = fileEnd;
        found = true;
        theFileStruct->maxDbTag = dBTag;
      }

    // try current location
    if(pos < fileEnd && found == false)
      {
        theStream->read(data, stepSize);
        if(*(theIntData.dbTag) == dBTag )
          { found = true; }
      }

    // we have to search from the beginning of the file
    if(found == false)
      {
        *(theIntData.dbTag) = -1;
        pos = sizeof(int);
        theStream->seekg(pos, std::ios::beg);
        while(pos < fileEnd && found == false)
          {
            theStream->read(data, stepSize);
            if(*(theIntData.dbTag) == dBTag)
	      found = true;
            else
              pos += stepSize;
          }
        if(found == false)
         { pos = fileEnd; }
      }

    //
    // we now place the data to be sent into our buffer
    //

    *(theIntData.dbTag) = dBTag;
    for(int i=0; i<idSize; i++)
      theIntData.data[i] = theID(i);

    //
    // we now write the data
    //

    theStream->seekp(pos, std::ios::beg); // reset so can go write at the end

    theStream->write(data, stepSize);
    if(theStream->bad())
      {
        std::cerr << "FileDatastore::sendID() - error writing to file\n";
        return -1;
      }

    // update the size of file if we have added to eof
    if(fileEnd <= pos)
      theFileStruct->fileEnd = pos + stepSize;

    //std::cerr << "WROTE: " << dBTag << " " << pos << std::endl;
    return 0;
  }		

int XC::FileDatastore::recvID(int dBTag, int commitTag, ID &theID, ChannelAddress *theAddress)
  {
    if(!checkDbTag(dBTag))
      std::cerr << "Error en FileDatastore::recvID." << std::endl;
    if(currentCommitTag != commitTag)
      this->resetFilePointers();

    currentCommitTag = commitTag;

    FileDatastoreOutputFile *theFileStruct;

    //
    // next we see if we already have this file;
    //  if not we need to create data structure & open it
    //  if we have data structure, need to check file is opened (we close in a commit)
    //

    int idSize = theID.Size();
    int stepSize = (1 + idSize)*sizeof(int);

    theIDFilesIter = theIDFiles.find(idSize);
    if(theIDFilesIter == theIDFiles.end())
      {

        // we first check if we need to resize recv buffer
        if(idSize > currentMaxInt)
          {
            if(this->resizeInt(idSize) < 0)
              {
	        std::cerr << "FileDatastore::recvID() - failed in resizeInt()\n";
                return -1;
              }
          }

        theFileStruct = new FileDatastoreOutputFile;
        if(theFileStruct == 0)
          {
            std::cerr << "FileDatastore::recvID() - out of memory\n";
            return -1;
          }

        const std::string fileName= getFileName(".IDs.",idSize,commitTag);

        if(this->openFile(fileName, theFileStruct, stepSize) < 0)
          {
            std::cerr << "FileDatastore::recvID() - could not open file\n";
            return -1;
          }
        else
          theIDFiles.insert(MAP_FILES_TYPE(idSize, theFileStruct));
      }
    else
      {
        theFileStruct = theIDFilesIter->second;

        // make sure not close from a last commit
        if(theFileStruct->theFile == 0)
          {
            if(idSize > currentMaxInt)
              {
                if(this->resizeInt(idSize) < 0)
                  {
                    std::cerr << "FileDatastore::recvID() - failed in resizeInt()\n";
                    return -1;
	          }
              }
            const std::string fileName= getFileName(".IDs.",idSize,commitTag);

            if(this->openFile(fileName, theFileStruct, stepSize) < 0)
              {
	        std::cerr << "FileDatastore::recvID() - could not open file\n";
                return -1;
              }
          }
      }
    fstream *theStream = theFileStruct->theFile;
    STREAM_POSITION_TYPE fileEnd = theFileStruct->fileEnd;
    STREAM_POSITION_TYPE pos = theStream->tellg();

    //
    // find location in file to place the data
    //

    bool found = false;

    // we try the current file position first

    if(pos < fileEnd)
      {
        theStream->read(data, stepSize);
        if(*(theIntData.dbTag) == dBTag)
          {
            found = true;
            pos += stepSize;
          }
      }

    // we must search from the beginning of the file
    if(found == false)
      {
        pos = sizeof(int);
        theStream->seekg(pos, std::ios::beg);
        while((pos < fileEnd) && (found == false))
          {
            theStream->read(data, stepSize);
            if(*(theIntData.dbTag) == dBTag)
	      found = true;
            else
	      pos += stepSize;
          }
      }

    if(found == false)
      {
        std::cerr << "FileDatastore::recvID() - failed\n";
        return -1;
      }
    //std::cerr << "READ: " << dBTag << " " << pos << std::endl;

    // we now place the received data into the XC::ID
    for(int i=0; i<idSize; i++)
      theID(i) = theIntData.data[i];
    return 0;
  }		


int XC::FileDatastore::sendMatrix(int dBTag, int commitTag,const XC::Matrix &theMatrix,ChannelAddress *theAddress)
  {
    if(!checkDbTag(dBTag))
      std::cerr << "Error en FileDatastore::sendMatrix." << std::endl;

    if(currentCommitTag != commitTag)
      this->resetFilePointers();

    currentCommitTag = commitTag;

    FileDatastoreOutputFile *theFileStruct;

    //
    // next we see if we already have this file;
    //  if not we need to create data structure & open it
    //  if we have data structure, need to check file is opened (we close in a commit)
    //

    // we first ensure that the XC::Matrix is not too big
    const int noMatCols= theMatrix.noCols();
    const int noMatRows = theMatrix.noRows();
    const int matSize = noMatRows * noMatCols;
    const int stepSize = sizeof(int) + matSize*sizeof(double);

    theMatFilesIter = theMatFiles.find(matSize);
    if(theMatFilesIter == theMatFiles.end())
      {
        // we first check if we need to resize send buffer
        if(matSize > currentMaxDouble)
          {
            if(this->resizeDouble(matSize) < 0)
              {
	        std::cerr << "FileDatastore::sendMatrix() - failed in resizeInt()\n";
	        return -1;
              }
          }

        theFileStruct = new FileDatastoreOutputFile;
        if(theFileStruct == 0)
          {
            std::cerr << "FileDatastore::sendMatrix() - out of memory\n";
            return -1;
          }

        const std::string fileName= getFileName(".MATs.",matSize,commitTag);

        if(this->openFile(fileName, theFileStruct, stepSize) < 0)
          {
            std::cerr << "FileDatastore::sendMatrix() - could not open file\n";
            return -1;
          }
        else
          theMatFiles.insert(MAP_FILES_TYPE(matSize, theFileStruct));
      }
    else
      {
        theFileStruct = theMatFilesIter->second;

        // make sure not close from a last commit
        if(theFileStruct->theFile == 0)
          {
            if(matSize > currentMaxDouble)
              {
                if(this->resizeDouble(matSize) < 0)
                  {
	            std::cerr << "FileDatastore::sendMatrix() - failed in resizeInt()\n";
	            return -1;
	          }
              }

            const std::string fileName= getFileName(".MATs.",matSize,commitTag);

            if(this->openFile(fileName, theFileStruct, stepSize) < 0)
              {
	        std::cerr << "FileDatastore::sendMatrix() - could not open file\n";
                return -1;
              }
          }
      }

    //
    // find location in file to place the data
    //

    fstream *theStream = theFileStruct->theFile;
    bool found = false;
    STREAM_POSITION_TYPE pos = theStream->tellg();
    STREAM_POSITION_TYPE fileEnd = theFileStruct->fileEnd;

    // we first check if the data can go at the end of the file
    // true if commitTag larger than any we have encountered so far

    if(theFileStruct->maxDbTag < dBTag)
      {
        pos = fileEnd;
        found = true;
        theFileStruct->maxDbTag = dBTag;
      }

    // try current location
    if(pos < fileEnd && found == false)
      {
        theStream->read(data, stepSize);
        if(*(theIntData.dbTag) == dBTag )
          { found = true; }
      }

    // we have to search from the beginning of the file
    if(found == false)
      {
        *(theIntData.dbTag) = -1;
        pos = sizeof(int);
        theStream->seekg(pos, std::ios::beg);
        while(pos < fileEnd && found == false)
          {
            theStream->read(data, stepSize);
            if(*(theIntData.dbTag) == dBTag)
	      found = true;
            else
	      pos += stepSize;
          }
        if(!found)
          { pos = fileEnd; }
      }

    //
    // we now place the data to be sent into our buffer
    //

    *(theDoubleData.dbTag) = dBTag;
    int loc=0;
    for(int j=0; j<noMatCols; j++)
    for(int k=0; k < noMatRows; k++)
      {
        theDoubleData.data[loc] = theMatrix(k,j);
        loc++;
      }

    //
    // we now write the data
    //

    theStream->seekp(pos, std::ios::beg); // reset so can go write at the end
    theStream->write(data, stepSize);

    // update the size of file if we have added to eof
    if(theFileStruct->fileEnd <= pos)
      theFileStruct->fileEnd += stepSize;
    return 0;
  }		




int XC::FileDatastore::recvMatrix(int dBTag, int commitTag,Matrix &theMatrix,ChannelAddress *theAddress)
  {
    if(!checkDbTag(dBTag))
      std::cerr << "Error en FileDatastore::recvMatrix." << std::endl;

    if(currentCommitTag != commitTag)
      this->resetFilePointers();

    currentCommitTag = commitTag;

    FileDatastoreOutputFile *theFileStruct;

  //
  // next we see if we already have this file;
  //  if not we need to create data structure & open it
  //  if we have data structure, need to check file is opened (we close in a commit)
  //

  // we first check XC::Matrix not too big
  int noMatCols= theMatrix.noCols();
  int noMatRows = theMatrix.noRows();
  int matSize = noMatRows * noMatCols;
  int stepSize = sizeof(int) + matSize*sizeof(double);

  theMatFilesIter = theMatFiles.find(matSize);
  if(theMatFilesIter == theMatFiles.end()) {

    // we first check if we need to resize recv buffer
    if(matSize > currentMaxDouble) {
      if(this->resizeDouble(matSize) < 0) {
	std::cerr << "FileDatastore::recvMatrix() - failed in resizeDouble()\n";
	return -1;
      }
    }

    theFileStruct = new FileDatastoreOutputFile;

    if(theFileStruct == 0) {
      std::cerr << "FileDatastore::recvMatrix() - out of memory\n";
      return -1;
    }

        const std::string fileName= getFileName(".MATs.",matSize,commitTag);

    if(this->openFile(fileName, theFileStruct, stepSize) < 0) {
      std::cerr << "FileDatastore::recvMatrix() - could not open file\n";
      return -1;
    } else
      theMatFiles.insert(MAP_FILES_TYPE(matSize, theFileStruct));


  } else {

    theFileStruct = theMatFilesIter->second;

    // make sure not close from a last commit
    if(theFileStruct->theFile == 0) {

      if(matSize > currentMaxDouble) {
	if(this->resizeDouble(matSize) < 0) {
	  std::cerr << "FileDatastore::recvMatrix() - failed in resizeInt()\n";
	  return -1;
	}
      }
      const std::string fileName= getFileName(".MATs.",matSize,commitTag);
      if(this->openFile(fileName, theFileStruct, stepSize) < 0) {
	std::cerr << "FileDatastore::recvMatrix() - could not open file\n";
	return -1;
      }

    }
  }


  fstream *theStream = theFileStruct->theFile;
  STREAM_POSITION_TYPE fileEnd = theFileStruct->fileEnd;
  STREAM_POSITION_TYPE pos = theStream->tellg();

  //
  // find location in file to place the data
  //

  bool found = false;

  // we try the current file position first

  if(pos < fileEnd) {
    theStream->read(data, stepSize);
    if((*(theIntData.dbTag) == dBTag)) {
      found = true;
      pos += stepSize;
    }
  }

  // we must search from the beginning of the file
  if(found == false) {
    pos = sizeof(int);
    theStream->seekg(pos, std::ios::beg);
    while((pos < fileEnd) && (found == false)) {
      theStream->read(data, stepSize);
      if(*(theIntData.dbTag) == dBTag)
	found = true;
      else
	pos += stepSize;
    }
  }

  if(found == false) {
    std::cerr << "FileDatastore::recvMatrix() - failed\n";
    return -1;
  }

  int loc=0;
  for(int j=0; j<noMatCols; j++)
    for(int k=0; k < noMatRows; k++) {
      theMatrix(k,j) = theDoubleData.data[loc];
      loc++;
    }

  return 0;
}		



int XC::FileDatastore::sendVector(int dBTag, int commitTag,const XC::Vector &theVector,ChannelAddress *theAddress)
  {
    if(!checkDbTag(dBTag))
      std::cerr << "Error en FileDatastore::sendVector." << std::endl;

  if(currentCommitTag != commitTag)
    this->resetFilePointers();

  currentCommitTag = commitTag;

  FileDatastoreOutputFile *theFileStruct;

  //
  // next we see if we already have this file;
  //  if not we need to create data structure & open it
  //  if we have data structure, need to check file is opened (we close in a commit)
  //

  // we first ensure that the XC::Matrix is not too big
  int vectSize = theVector.Size();
  int stepSize = sizeof(int) + vectSize*sizeof(double);

  theVectFilesIter = theVectFiles.find(vectSize);
  if(theVectFilesIter == theVectFiles.end()) {

    // we first check if we need to resize send buffer
    if(vectSize > currentMaxDouble) {
      if(this->resizeDouble(vectSize) < 0) {
	std::cerr << "FileDatastore::sendVector() - failed in resizeInt()\n";
	return -1;
      }
    }

    theFileStruct = new FileDatastoreOutputFile;

    if(theFileStruct == 0) {
      std::cerr << "FileDatastore::sendVector() - out of memory\n";
      return -1;
    }

    const std::string fileName= getFileName(".VECs.",vectSize,commitTag);

    if(this->openFile(fileName, theFileStruct, stepSize) < 0) {
      std::cerr << "FileDatastore::sendVector() - could not open file\n";
      return -1;
    } else
      theVectFiles.insert(MAP_FILES_TYPE(vectSize, theFileStruct));


  } else {

    theFileStruct = theVectFilesIter->second;

    // make sure not close from a last commit
    if(theFileStruct->theFile == 0) {

      if(vectSize > currentMaxDouble) {
	if(this->resizeDouble(vectSize) < 0) {
	  std::cerr << "FileDatastore::sendVector() - failed in resizeInt()\n";
	  return -1;
	}
      }
      const std::string fileName= getFileName(".VECs.",vectSize,commitTag);

      if(this->openFile(fileName, theFileStruct, stepSize) < 0) {
	std::cerr << "FileDatastore::sendVector() - could not open file\n";
	return -1;
      }

    }
  }

  //
  // find location in file to place the data
  //

  fstream *theStream = theFileStruct->theFile;
  bool found = false;
  STREAM_POSITION_TYPE pos = theStream->tellg();
  STREAM_POSITION_TYPE fileEnd = theFileStruct->fileEnd;

  // we first check if the data can go at the end of the file
  // true if commitTag larger than any we have encountered so far
  found = false;


  if(theFileStruct->maxDbTag < dBTag)  {
    pos = fileEnd;
    found = true;
    theFileStruct->maxDbTag = dBTag;
  }


  // try current location
  if(pos < fileEnd && found == false) {
// #ifdef _WIN32
    // must be a bug in the vc compiler! .. we are here already tellg() above!!
    theStream->seekg(pos, std::ios::beg);
// #endif
    theStream->read(data, stepSize);
    if(*(theIntData.dbTag) == dBTag ) {
      found = true;
    }
  }

  // we have to search from the beginning of the file
  if(found == false) {
    *(theIntData.dbTag) = -1;
    pos = sizeof(int);
    theStream->seekg(pos, std::ios::beg);
    while(pos < fileEnd && found == false) {
      theStream->read(data, stepSize);

      if(*(theIntData.dbTag) == dBTag)
	found = true;
      else
	pos += stepSize;
    }

    if(found == false) {
      pos = fileEnd;
    }
  }

  //
  // we now place the data to be sent into our buffer
  //

  *(theDoubleData.dbTag) = dBTag;
  for(int i=0; i<vectSize; i++)
    theDoubleData.data[i] = theVector(i);

  //
  // we now write the data
  //

  theStream->seekp(pos, std::ios::beg); // reset so can go write at the end
  theStream->write(data, stepSize);

  // update the size of file if we have added to eof
  if(theFileStruct->fileEnd <= pos)
    theFileStruct->fileEnd += stepSize;

  return 0;
}		

int XC::FileDatastore::recvVector(int dBTag, int commitTag, Vector &theVector, ChannelAddress *theAddress)
{
    if(!checkDbTag(dBTag))
      std::cerr << "Error en FileDatastore::recvMatrix." << std::endl;

  if(currentCommitTag != commitTag)
    this->resetFilePointers();

  currentCommitTag = commitTag;

  FileDatastoreOutputFile *theFileStruct;

  //
  // next we see if we already have this file;
  //  if not we need to create data structure & open it
  //  if we have data structure, need to check file is opened (we close in a commit)
  //

  // we first check XC::Vector not too big
  int vectSize = theVector.Size();
  int stepSize = sizeof(int) + vectSize*sizeof(double);

  theVectFilesIter = theVectFiles.find(vectSize);
  if(theVectFilesIter == theVectFiles.end()) {

    // we first check if we need to resize recv buffer
    if(vectSize > currentMaxDouble) {
      if(this->resizeDouble(vectSize) < 0) {
	std::cerr << "FileDatastore::recvVectrix() - failed in resizeDouble()\n";
	return -1;
      }
    }

    theFileStruct = new FileDatastoreOutputFile;

    if(theFileStruct == 0) {
      std::cerr << "FileDatastore::recvVectrix() - out of memory\n";
      return -1;
    }

    const std::string fileName= getFileName(".VECs.",vectSize,commitTag);

    if(this->openFile(fileName, theFileStruct, stepSize) < 0) {
      std::cerr << "FileDatastore::recvVectrix() - could not open file\n";
      return -1;
    } else
      theVectFiles.insert(MAP_FILES_TYPE(vectSize, theFileStruct));


  } else {

    theFileStruct = theVectFilesIter->second;

    // make sure not close from a last commit
    if(theFileStruct->theFile == 0) {

      if(vectSize > currentMaxDouble) {
	if(this->resizeDouble(vectSize) < 0) {
	  std::cerr << "FileDatastore::recvVectrix() - failed in resizeInt()\n";
	  return -1;
	}
      }
      const std::string fileName= getFileName(".VECs.",vectSize,commitTag);

      if(this->openFile(fileName, theFileStruct, stepSize) < 0) {
	std::cerr << "FileDatastore::recvVectrix() - could not open file\n";
	return -1;
      }

    }
  }

  fstream *theStream = theFileStruct->theFile;
  STREAM_POSITION_TYPE fileEnd = theFileStruct->fileEnd;
  STREAM_POSITION_TYPE pos = theStream->tellg();

  //
  // find location in file to place the data
  //

  bool found = false;

  // we try the current file position first

  if(pos < fileEnd) {
    theStream->read(data, stepSize);
    if((*(theIntData.dbTag) == dBTag)) {
      found = true;
      pos += stepSize;
    }
  }

  // we must search from the beginning of the file
  if(found == false) {
    pos = sizeof(int);
    theStream->seekg(pos, std::ios::beg);
    while((pos < fileEnd) && (found == false)) {
      theStream->read(data, stepSize);
      if(*(theIntData.dbTag) == dBTag)
	found = true;
      else
	pos += stepSize;
    }
  }

  if(found == false) {
    std::cerr << "FileDatastore::recvVector() - failed\n";
    return -1;
  }

  for(int i=0; i<vectSize; i++)
    theVector(i) = theDoubleData.data[i];

  return 0;
}		


#include <fstream>


int XC::FileDatastore::createTable(const std::string &tableName,const std::vector<std::string> &columns)
  {
    const int numColumns= columns.size();
    // open the file
    int res = 0;
    const std::string fileName= dataBase + "." + tableName;

    std::ofstream table;
    table.open(fileName.c_str(), std::ios::out | std::ios::trunc);

    if(table.bad() == true || table.is_open() == false)
      {
        std::cerr << "FileDatastore::insertData - failed to open file: " << fileName << std::endl;
        res = -1;
      }

    // write the data
    for(int i=0; i<numColumns; i++)
      { table << columns[i] << "\t"; }
    table << "\n";
    table.close();
    return res;
  }

int XC::FileDatastore::insertData(const std::string &tableName,const std::vector<std::string> &columns,int commitTag, const XC::Vector &data)
  {
    // open the file
    const std::string fileName= dataBase + "." + tableName;

    std::ofstream table;
  table.open(fileName.c_str(), std::ios::app);
  if(table.bad() == true || table.is_open() == false) {
    std::cerr << "FileDatastore::insertData - failed to open file: " << fileName << std::endl;
    return -1;
  }

  table << setiosflags(std::ios::scientific);
  table << std::setprecision(16);

  // write the data
  for(int i=0; i<data.Size(); i++) {
    table << data(i) << "\t";
  }

  table << "\n";
  table.close();

  return 0;
}


int XC::FileDatastore::getData(const std::string &tableName,const std::vector<std::string> &columns, int commitTag, Vector &data)
  { return 0; }



/*******************************************************************
 *              MISC METHODS & FUNCTONS FOR OPENING THE FILE       *
 *******************************************************************/

int XC::FileDatastore::openFile(const std::string &fileName, FileDatastoreOutputFile *theFileStruct, int dataSize)
  {
    fstream *res = new fstream();
    if(res == 0)
      {
        std::cerr << "FileDatastore::openFile - out of memory; failed to open file: " << fileName << std::endl;
        return 0;
      }

  res->open(fileName.c_str(), std::ios::in | std::ios::out | std::ios::binary);

  // if file did not exist, need to pass trunc flag to open it
  if(res->bad() == true || res->is_open() == false) {
    // delete & new again for unix gcc compiler to work!
    delete res;
    res = new fstream();
    if(res == 0) {
      std::cerr << "FileDatastore::openFile - out of memory; failed to open file: " << fileName << std::endl;
      theFileStruct->theFile = res;
      return -1;
    }
    res->open(fileName.c_str(), std::ios::in | std::ios::out | std::ios::trunc | std::ios::binary);
  }

  if(res->bad() == true || res->is_open() == false)
   {
    std::cerr << "FATAL - XC::FileDatastore::openFile() - could not open file " << fileName << std::endl;
    delete res;
    theFileStruct->theFile = 0;
    return -1;
  }

  // set the position for writing to eof
  res->seekp(0,std::ios::end);
  STREAM_POSITION_TYPE fileEnd = res->tellp();
  int maxDataTag = 0;

  if(fileEnd == 0 || fileEnd == -1) {
    *(theIntData.dbTag) = maxDataTag;
    res->write(data, sizeof(int));
    fileEnd = sizeof(int);
    maxDataTag = -1;
  } else {
    res->seekg(0, std::ios::beg);
    res->read(data, sizeof(int));
    maxDataTag = *(theIntData.dbTag);
  }

  // move to start of data part
  res->seekp(sizeof(int), std::ios::beg);
  res->seekg(sizeof(int), std::ios::beg);

  // fill in the structure data
  theFileStruct->theFile = res;
  theFileStruct->fileEnd = fileEnd;

  theFileStruct->maxDbTag = maxDataTag; 	

  return 0;
}

int XC::FileDatastore::resizeInt(int newSize)
  {
    int sizeOfChar = sizeof(char);
    int sizeOfInt = sizeof(int);
    int sizeOfDouble = sizeof(double);
    newSize = (newSize+1)*sizeOfInt/sizeOfChar;

    if(newSize < sizeData)
      return 0; // already big enough

    if(newSize <= 0)
      {
        std::cerr << "FileDatastore::resizeInt(int newSize) - invalidSize " << newSize << std::endl;
        return -1; // invalid size
      }

    alloc(newSize);

    currentMaxInt = (sizeData/sizeOfChar-sizeOfInt)/sizeOfInt;
    currentMaxDouble = (sizeData/sizeOfChar-sizeOfInt)/sizeOfDouble;
    char *dataPtr = &data[sizeof(int)];
    theIntData.dbTag = reinterpret_cast<int *>(data);
    theIntData.data = reinterpret_cast<int *>(dataPtr);
    theDoubleData.dbTag = reinterpret_cast<int *>(data);
    theDoubleData.data = reinterpret_cast<double *>(dataPtr);
    return 0;
  }

int XC::FileDatastore::resizeDouble(int newSize)
  {
    const int sizeOfChar= sizeof(char);
    const int sizeOfInt= sizeof(int);
    const int sizeOfDouble= sizeof(double);
    newSize= (newSize*sizeOfDouble+sizeOfInt)/sizeOfChar;

    if(newSize < sizeData)
      return 0; // already big enough

    if(newSize <= 0)
      {
        std::cerr << "FileDatastore::resizeInt(int newSize) - invalidSize " << newSize << std::endl;
        return -1; // invalid size
      }
    alloc(newSize);

    currentMaxInt = (sizeOfChar*sizeData-sizeOfInt)/sizeOfInt;
    currentMaxDouble = (sizeOfChar*sizeData-sizeOfInt)/sizeOfDouble;
    char *dataPtr = &data[sizeof(int)];
    theIntData.dbTag = reinterpret_cast<int *>(data);
    theIntData.data = reinterpret_cast<int *>(dataPtr);
    theDoubleData.dbTag = reinterpret_cast<int *>(data);
    theDoubleData.data = reinterpret_cast<double *>(dataPtr);
    return 0;
  }
