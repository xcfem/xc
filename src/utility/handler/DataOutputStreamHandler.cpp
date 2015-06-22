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
// $Source: /usr/local/cvs/OpenSees/SRC/handler/DataOutputStreamHandler.cpp,v $
                                                                        
// Written: fmk 
// Date: 10/04
//
// Description: This file contains the class implementation for
// DataOutputStreamHandler. 
//
// What: "@(#) DataOutputStreamHandler.C, revA"

#include "DataOutputStreamHandler.h"
#include <utility/matrix/Vector.h>

XC::DataOutputStreamHandler::DataOutputStreamHandler(bool echoD)
  :DataOutputHandler(DATAHANDLER_TAGS_DataOutputStreamHandler), 
   echoDescription(echoD), numColumns(0) {}

int XC::DataOutputStreamHandler::open(const std::vector<std::string> &dataDescription, int numData)
  {
    if(numData < 0)
     {
       std::cerr << "XC::DataOutputStreamHandler::open() - numColumns (" << numData << ") < 0\n";
       return -1;
     }
    else
      numColumns = numData;

    if(echoDescription == true)
      {
        for(int i=0; i<numData; i++)
          std::cerr << dataDescription[i] << " ";
        std::cerr << std::endl;
      }
   return 0;
  }

int XC::DataOutputStreamHandler::open(const std::vector<std::string> &dataDescription)
  {
    int numData= dataDescription.size();
    if(numData < 0)
      {
        std::cerr << "XC::DataOutputStreamHandler::open() - numColumns (" << numData << ") < 0\n";
        return -1;
      }
    else
      numColumns = numData;
    if(echoDescription == true)
      {
        for(int i=0; i<numData; i++)
          std::cerr << dataDescription[i] << " ";
        std::cerr << std::endl;
      }
    return 0;
  }

int XC::DataOutputStreamHandler::write(Vector &data) 
  {
    if (data.Size() == numColumns)
      outputStream.stream() << data;
  else {
    std::cerr << "XC::DataOutputStreamHandler::write() - Vector not of correct size\n";
    return -1;
  }
  
  return 0;
}

//! @brief Envía el objeto a través del canal definido en el argumento.
int XC::DataOutputStreamHandler::sendSelf(CommParameters &cp)
  { return 0; }

//! @brief Recibe el objeto a través del canal definido en el argumento.
int XC::DataOutputStreamHandler::recvSelf(const CommParameters &cp)
  { return 0; }
