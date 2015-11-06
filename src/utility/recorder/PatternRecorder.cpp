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
// $Date: 2004/11/24 22:45:28 $
// $Source: /usr/local/cvs/OpenSees/SRC/recorder/PatternRecorder.cpp,v $

// Written: MHS 
// Created: 2002
//
// Description: This file contains the class definition for XC::PatternRecorder.
// A XC::PatternRecorder is used to record the load values from a pattern.

#include <utility/recorder/PatternRecorder.h>
#include <domain/domain/Domain.h>
#include <domain/load/pattern/LoadPattern.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/ID.h>
#include <utility/matrix/Matrix.h>
#include <utility/database/FE_Datastore.h>

#include <fstream>
using std::ifstream;

#include <iomanip>
using std::ios;

XC::PatternRecorder::PatternRecorder(void)
  :DomainRecorderBase(RECORDER_TAGS_PatternRecorder,nullptr),
   thePattern(-1), flag(-1), deltaT(0.0), nextTimeStampToRecord(0.0)
  {}

XC::PatternRecorder::PatternRecorder(int pattern, Domain &theDom,
				     const std::string &theFileName,
				     double dT, int startFlag)
  :DomainRecorderBase(RECORDER_TAGS_PatternRecorder,&theDom),
   thePattern(pattern), flag(startFlag), fileName(theFileName), deltaT(dT), nextTimeStampToRecord(0.0)
  {
    
    // open the file
    theFile.open(fileName.c_str(), ios::out);
    if(theFile.bad())
      {
        std::cerr << "WARNING - XC::PatternRecorder::PatternRecorder()";
        std::cerr << " - could not open file " << fileName << std::endl;
      }
    //theFile << setiosflags(ios::scientific) << setprecision(15);
  }

XC::PatternRecorder::~PatternRecorder(void)
  {
    if(!theFile) theFile.close();
  }

int XC::PatternRecorder::record(int commitTag, double timeStamp)
  {
    double value = 0.0;
  
    if(deltaT == 0.0 || timeStamp >= nextTimeStampToRecord)
      {
        if(deltaT != 0.0)
          nextTimeStampToRecord = timeStamp + deltaT;
    
        LoadPattern *pattern = theDomain->getConstraints().getLoadPattern(thePattern);
        if(pattern) value = pattern->getLoadFactor();
      }
  
    // write them to the file
    if (flag == 1)
      theFile << timeStamp << " ";
    else if (flag == 2)
      theFile << timeStamp << " ";
  
    theFile << value << " ";
  
    theFile << std::endl;
    theFile.flush();
    return 0;
  }

int XC::PatternRecorder::playback(int commitTag)
  {
    if(theFile.bad()) return 0;
  
    // close o/p file to ensure all buffered data gets written to file
    theFile.close(); 
  
    // open a stream for reading from the file
    ifstream inputFile;
    inputFile.open(fileName.c_str(), ios::in);
    if(inputFile.bad())
      {
        std::cerr << "WARNING - XC::PatternRecorder::playback() - could not open file ";
        std::cerr << fileName << std::endl;
        return -1;
      }   
  
    double data;
    // read file up until line we want
    for(int i=0; i<(commitTag-1); i++)
    // now read in a line
    if(flag == 1 || flag == 2)
      {
        inputFile >> data;
        inputFile >> data;
      }
    // now read in our line and print out
    if(flag == 1 || flag == 2)
      {
        inputFile >> data;
        std::cerr << data << " ";
        inputFile >> data;
        std::cerr << data << " ";
        std::cerr << std::endl;
      }
    inputFile.close();
  
    // open file again for writing
    theFile.open(fileName.c_str(), ios::app);
    if(theFile.bad())
      {
        std::cerr << "WARNING - XC::PatternRecorder::playback() - could not open file ";
        std::cerr << fileName << std::endl;
        return -1;
      }    
  
    // does nothing
    return 0;
  }

int XC::PatternRecorder::restart(void)
  {
    theFile.close();
    theFile.open(fileName.c_str(), ios::out);
    if(theFile.bad())
      {
        std::cerr << "WARNING - XC::PatternRecorder::restart() - could not open file ";
        std::cerr << fileName << std::endl;
      }
    return 0;
  }
