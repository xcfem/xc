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
                                                                        
// $Revision: 1.6 $
// $Date: 2005/03/11 22:08:00 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/load/pattern/PathSeries.cpp,v $
                                                                        
                                                                        
// File: ~/domain/load/pattern/PathSeries.C
//
// Written: fmk 
// Created: 07/99
// Revision: A
//
// Purpose: This file contains the class definition for XC::PathSeries.
// PathSeries is a concrete class. A XC::PathSeries object provides
// a linear time series. the factor is given by the pseudoTime and 
// a constant factor provided in the constructor.
//
// What: "@(#) PathSeries.C, revA"


#include "PathSeries.h"
#include <utility/matrix/Vector.h>
#include <cmath>
#include <fstream>
#include <iomanip>

#include "utility/actor/actor/MovableVector.h"
#include "utility/matrix/ID.h"

//! @brief Constructor.
XC::PathSeries::PathSeries(void)	
  :PathSeriesBase(TSERIES_TAG_PathSeries), pathTimeIncr(1.0) {}

		   
//! @brief Constructor.
XC::PathSeries::PathSeries(const XC::Vector &theLoadPath, double theTimeIncr, double theFactor)
  :PathSeriesBase(TSERIES_TAG_PathSeries,theLoadPath,theFactor), pathTimeIncr(theTimeIncr) {}


//! @brief Constructor.
XC::PathSeries::PathSeries(const std::string &fileName, double theTimeIncr, double theFactor)
  :PathSeriesBase(TSERIES_TAG_PathSeries,theFactor),pathTimeIncr(theTimeIncr)
  { readFromFile(fileName); }

//! @brief Lee el path desde archivo.
void XC::PathSeries::readFromFile(const std::string &fileName)
  {
    // determine the number of data points .. open file and count num entries
    int numDataPoints= getNumDataPointsOnFile(fileName);
    // create a vector and read in the data
    if(numDataPoints)
      {
        // first open the file
	std::ifstream theFile1;
        theFile1.open(fileName.c_str(), std::ios::in);
        if(theFile1.bad() || !theFile1.is_open())
          {
            std::cerr << "WARNING - XC::PathSeries::PathSeries()";
            std::cerr << " - could not open file " << fileName << std::endl;
          }
        else
          {
            // now create the vector
            thePath.resize(numDataPoints);
            load_vector_from_file(thePath,theFile1);
            theFile1.close(); // finally close the file
          }
      }
  }

//! @brief Devuelve el valor correspondiente al instante being passed as parameter.
double XC::PathSeries::getFactor(double pseudoTime) const
  {
    double retval= 0.0;
    // check for a quick return
    if(pseudoTime > 0.0)
      {

        // determine indexes into the data array whose boundary holds the time
        double incr = pseudoTime/pathTimeIncr;
        int incr1 = static_cast<int>(floor(incr));
        int incr2 = incr1+1;

        if(incr2 <thePath.Size())
          {
            const double value1= thePath(incr1);
            const double value2= thePath(incr2);
            retval= cFactor*(value1 + (value2-value1)*(pseudoTime/pathTimeIncr - incr1));
          }
      }
    return retval;
  }


//! @brief Returns series duration.
double XC::PathSeries::getDuration(void) const
  { return thePath.Size() * pathTimeIncr; }

int XC::PathSeries::sendData(CommParameters &cp)
  {
    int res= PathSeriesBase::sendData(cp);
    res+= cp.sendDouble(pathTimeIncr,getDbTagData(),CommMetaData(5));
    return res;
  }

int XC::PathSeries::recvData(const CommParameters &cp)
  {
    int res= PathSeriesBase::recvData(cp);
    res+= cp.receiveDouble(pathTimeIncr,getDbTagData(),CommMetaData(5));
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::PathSeries::sendSelf(CommParameters &cp)
  {
    inicComm(6);
    int result= sendData(cp);

    const int dataTag= getDbTag();
    result+= cp.sendIdData(getDbTagData(),dataTag);
    if(result < 0)
      std::cerr << "PathSeries::sendSelf() - ch failed to send data\n";
    return result;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::PathSeries::recvSelf(const CommParameters &cp)
  {
    inicComm(6);

    const int dataTag = this->getDbTag();  
    int result = cp.receiveIdData(getDbTagData(),dataTag);
    if(result<0)
      std::cerr << "PathSeries::sendSelf() - ch failed to receive data\n";
    else
      result+= recvData(cp);
    return result;    
  }

//! @brief Imprime el objeto.
void XC::PathSeries::Print(std::ostream &s, int flag) const
  {
    PathSeriesBase::Print(s,flag);
  }
