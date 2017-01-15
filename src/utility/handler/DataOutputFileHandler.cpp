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
// $Source: /usr/local/cvs/OpenSees/SRC/handler/DataOutputFileHandler.cpp,v $
                                                                        
// Written: fmk 
// Date: 10/04
//
// Description: This file contains the class implementation for
// DataOutputFileHandler. 
//
// What: "@(#) DataOutputFileHandler.C, revA"

#include "utility/handler/DataOutputFileHandler.h"
#include <utility/matrix/Vector.h>
#include <utility/matrix/ID.h>
#include <utility/actor/channel/Channel.h>
#include <utility/actor/message/Message.h>
#include <fstream>
#include "utility/actor/actor/CommMetaData.h"


int setFile(std::ofstream &theFile,const std::string &name, XC::openMode mode)
  {
    if(name == "")
      {
        std::cerr << "XC::FileStream::setFile() - no name passed\n";
        return -1;
      }

    if(mode == XC::OVERWRITE) 
      theFile.open(name.c_str(), std::ios::out);
    else
      theFile.open(name.c_str(), std::ios::out| std::ios::app);

    if(theFile.bad())
      {
        std::cerr << "WARNING - XC::FileStream::setFile()";
        std::cerr << " - could not open file " << name << std::endl;
        return -1;
      }
    return 0;
  }

XC::DataOutputFileHandler::DataOutputFileHandler(const std::string &theFileName, echoMode theEMode, openMode theOMode)
  :DataOutputHandler(DATAHANDLER_TAGS_DataOutputFileHandler),
   fileName(""), theEchoMode(theEMode), theOpenMode(theOMode), numColumns(-1)
  {
    if(fileName != "" && setFile(outputFile,fileName, theOpenMode) < 0)
      {
        std::cerr << "XC::DataOutputFileHandler::DataOutputFileHandler() - setFile() failed\n";
        fileName= "";
      }
  }

int XC::DataOutputFileHandler::open(const std::vector<std::string> &dataDescription)
  {
    if(fileName.empty())
      {
        std::cerr << "XC::DataOutputFileHandler::open() - no filename.\n";
        return -1;
      }
    const int numData= dataDescription.size();
    if(numData < 0)
      {
        std::cerr << "XC::DataOutputFileHandler::open() - numColumns (" << numData << ") < 0\n";
        return -1;
      }
    else
      numColumns = numData;

    if(theEchoMode == DATA_FILE)
      {
        if(setFile(outputFile,fileName, theOpenMode) == 0)
          {
            for(int i=0; i<numData; i++)
              outputFile << dataDescription[i] << " ";
            outputFile << std::endl;
          }
      }
    else if(theEchoMode == XML_FILE)
      {
        // create a copy of the  file name
        int res = 0;
	std::string xmlFileName= fileName+".xml";
    
	std::ofstream xmlFile;
        if(setFile(xmlFile,xmlFileName, OVERWRITE) == 0)
          {
      
            // write the xml data
            xmlFile << "<?xml version=\"1.0\"?>\n";
            xmlFile << "<NumericalFileDataDescription>\n";
            xmlFile << "\t<DataFile>\n";
            xmlFile << "\t\t<DataFileName> " << xmlFileName << "</DataFileName>\n";
            xmlFile << "\t\t<NumberDataColumns> " << numData << "</NumberDataColumns>\n";
            xmlFile << "\t</DataFile>\n";
            for(int i=0; i<numData; i++)
              {
	        xmlFile << "\t<DataColumnDescription>\n";
	        xmlFile << "\t\t<ColumnLocation> " << i+1 << "</ColumnLocation>\n";      
	        xmlFile << "\t\t<Description> " << dataDescription[i].c_str() << "</Description>\n";
	        xmlFile << "\t</DataColumnDescription>\n";
              }
            xmlFile << "</NumericalFileDataDescription>\n";
            xmlFile.close();
          }
        else
          {
            std::cerr << "XC::DataOutputFileHandler::open - failed to open cml file: " << xmlFileName << std::endl;
            res = -1;
          }
      }
    return 0;
  }


int XC::DataOutputFileHandler::write(Vector &data) 
  {
    if(fileName.empty() || numColumns < 0)
      {
        std::cerr << "XC::DataOutputFileHandler::write() - no filename or data description has been set\n";
        return -1;
      }

    if(data.Size() == numColumns)
      outputFile << data;
    else
      {
        std::cerr << fileName;
        std::cerr << "XC::DataOutputStreamHandler::write() - Vector not of correct size\n";
        return -1;
      }
    return 0;
  }

//! @brief Envía los miembros del objeto a través del canal definido en el argumento.
int XC::DataOutputFileHandler::sendData(CommParameters &cp)
  {
    int res= cp.sendString(fileName,getDbTagData(),CommMetaData(0));

    if(theEchoMode == NONE)
      res+= cp.sendInt(0,getDbTagData(),CommMetaData(1));
    else if (theEchoMode == DATA_FILE)
      res+= cp.sendInt(1,getDbTagData(),CommMetaData(1));
    else
      res+= cp.sendInt(2,getDbTagData(),CommMetaData(1));

    if(theOpenMode == OVERWRITE)
      res+= cp.sendInt(0,getDbTagData(),CommMetaData(2));
    else
      res+= cp.sendInt(1,getDbTagData(),CommMetaData(2));

    res+= cp.sendInt(numColumns,getDbTagData(),CommMetaData(3));
    return res;
  }

//! @brief Recibe los miembros del objeto a través del canal definido en el argumento.
int XC::DataOutputFileHandler::recvData(const CommParameters &cp)
  {
    int res= cp.receiveString(fileName,getDbTagData(),CommMetaData(0));
    int em;
    res+= cp.receiveInt(em,getDbTagData(),CommMetaData(1));
    int om;
    res+= cp.receiveInt(om,getDbTagData(),CommMetaData(2));
    res+= cp.receiveInt(numColumns,getDbTagData(),CommMetaData(3));
    if(om==0)
      theOpenMode= OVERWRITE;
    else
      theOpenMode= APPEND;

    if(em==0)
      theEchoMode= NONE;
    else if(em == 1)
      theEchoMode= DATA_FILE;
    else
      theEchoMode= XML_FILE;
    return res;
  }

//! @brief Envía el objeto a través del canal definido en el argumento.
int XC::DataOutputFileHandler::sendSelf(CommParameters &cp)
  {
    inicComm(4);
    setDbTag(cp);
    const int dataTag= getDbTag();
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << "WARNING DataOutputFileHandler::sendSelf() - " 
                << dataTag << " failed to send.";
    return res;
  }

//! @brief Recibe el objeto a través del canal definido en el argumento.
int XC::DataOutputFileHandler::recvSelf(const CommParameters &cp)
  {
    inicComm(4);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << "WARNING DataOutputFileHandler::recvSelf() - "
                << dataTag << " failed to receive ID\n";
    else
      res+= recvData(cp);
    return res;
  }
