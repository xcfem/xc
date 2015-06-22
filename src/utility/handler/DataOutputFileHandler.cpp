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
#include "xc_utils/src/base/CmdStatus.h"
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

//! @brief Lee un objeto XC::DataOutputFileHandler desde archivo
bool XC::DataOutputFileHandler::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(DataOutputFileHandler) Procesando comando: " << cmd << std::endl;
    if(cmd == "file_name")
      {
        fileName= interpretaString(status.GetString());
        return true;
      }
    else if(cmd == "echo_mode")
      {
        const std::string mode= interpretaString(status.GetString());
        if(mode == "xml")
          theEchoMode= XML_FILE;
        else if(mode == "data")
          theEchoMode= DATA_FILE;
        return true;
      }
    else if(cmd == "open_mode")
      {
        const std::string mode= interpretaString(status.GetString());
        if(mode == "overwrite")
          theOpenMode= OVERWRITE;
        else if (mode == "append")
          theOpenMode= APPEND;
        else
          theOpenMode= OVERWRITE;          
        return true;
      }
    else
      return DataOutputHandler::procesa_comando(status);
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
