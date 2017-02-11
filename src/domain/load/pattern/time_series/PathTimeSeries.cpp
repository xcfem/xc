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

// $Revision: 1.8 $
// $Date: 2005/10/20 21:58:54 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/load/pattern/PathTimeSeries.cpp,v $


// Written: fmk
// Created: 07/99
// Revision: A
//
// Purpose: This file contains the class definition for XC::PathTimeSeries.
// PathTimeSeries is a concrete class. A XC::PathTimeSeries object provides
// a linear time series. the factor is given by the pseudoTime and
// a constant factor provided in the constructor.
//
// What: "@(#) PathTimeSeries.C, revA"


#include <domain/load/pattern/time_series/PathTimeSeries.h>
#include <utility/matrix/Vector.h>
#include <cmath>

#include <fstream>

#include <iomanip>
#include "utility/actor/actor/ArrayCommMetaData.h"
#include "utility/matrix/ID.h"


//! @brief Default constructor.
XC::PathTimeSeries::PathTimeSeries(void)
  :PathSeriesBase(TSERIES_TAG_PathTimeSeries), currentTimeLoc(0) {}

//! @brief Constructor.
XC::PathTimeSeries::PathTimeSeries(const Vector &theLoadPath, const Vector &theTimePath, double theFactor)
  :PathSeriesBase(TSERIES_TAG_PathTimeSeries,theLoadPath,theFactor),time(theTimePath), currentTimeLoc(0) {}


//! @brief Constructor.
XC::PathTimeSeries::PathTimeSeries(const std::string &filePathName,const std::string &fileTimeName, double theFactor)
  :PathSeriesBase(TSERIES_TAG_PathTimeSeries, theFactor), currentTimeLoc(0)
  { readFromFiles(filePathName,fileTimeName); }


//! @brief Constructor.
XC::PathTimeSeries::PathTimeSeries(const std::string &fileName, double theFactor)
  :PathSeriesBase(TSERIES_TAG_PathTimeSeries,theFactor), currentTimeLoc(0)
  { readFromFile(fileName); }

//! @brief Lee el path desde archivo.
void XC::PathTimeSeries::readFromFile(const std::string &fileName)
  {
    // determine the number of data points
    int numDataPoints= 0;
    double dataPoint;
    std::ifstream theFile;

    // first open and go through file counting entries
    theFile.open(fileName.c_str(), std::ios::in);
    if(theFile.bad() || !theFile.is_open())
      {
        std::cerr << "WARNING - PathTimeSeries::PathTimeSeries()";
        std::cerr << " - could not open file " << fileName << std::endl;
      }
    else
      {
        while(theFile >> dataPoint)
          {
            numDataPoints++;
            theFile >> dataPoint; // Read in second value of pair
          }
      }
    theFile.close();

    // create a vector and read in the data
    if(numDataPoints != 0)
      {
        // now create the two vector
        thePath.resize(numDataPoints);
        time.resize(numDataPoints);

        // first open the file and read in the data
        std::ifstream theFile1;
        theFile1.open(fileName.c_str(), std::ios::in);
        if(theFile1.bad() || !theFile1.is_open())
          {
            std::cerr << "WARNING - PathTimeSeries::PathTimeSeries()";
            std::cerr << " - could not open file " << fileName << std::endl;
          }
        else
          { // read in the time and then read the value
            int count = 0;
            while(theFile1 >> dataPoint)
              {
                time(count) = dataPoint;
                theFile1 >> dataPoint;
                thePath(count) = dataPoint;
                count++;
              }

            // finally close the file
            theFile1.close();
          }

      }
  }

//! @brief Lee el path desde DOS archivos.
void XC::PathTimeSeries::readFromFiles(const std::string &filePathName,const std::string &fileTimeName)
  {
    // determine the number of data points
    int numDataPoints1= getNumDataPointsOnFile(filePathName);
    int numDataPoints2= getNumDataPointsOnFile(fileTimeName);


    // check number of data entries in both are the same
    if(numDataPoints1 != numDataPoints2)
      {
        std::cerr << "WARNING XC::PathTimeSeries::PathTimeSeries() - files containing data ";
        std::cerr << "points for path and time do not contain same number of points\n";
      }
    else
      {
        // create a vector and read in the data
        if(numDataPoints1>0)
          {
            // now create the two vector
            thePath.resize(numDataPoints1);
            time.resize(numDataPoints1);

            // first open the path file and read in the data
            std::ifstream theFile2;
            theFile2.open(filePathName.c_str(), std::ios::in);
            if(theFile2.bad() || !theFile2.is_open())
              {
                std::cerr << "WARNING - PathTimeSeries::PathTimeSeries()";
                std::cerr << " - could not open file " << filePathName << std::endl;
              }
            else
              { // read in the path data and then do the time
                
                load_vector_from_file(thePath,theFile2);
                theFile2.close();

                // now open the time file and read in the data
                std::ifstream theFile3;
                theFile3.open(fileTimeName.c_str(), std::ios::in);
                if(theFile3.bad() || !theFile3.is_open())
                  {
                    std::cerr << "WARNING - PathTimeSeries::PathTimeSeries()";
                    std::cerr << " - could not open file " << fileTimeName << std::endl;
                  }
                else
                  {
                    load_vector_from_file(time,theFile3);
                    theFile3.close();
                  }
              }
         }
     }
  }

double XC::PathTimeSeries::getTimeIncr(double pseudoTime) const
  {
    // NEED TO FILL IN, FOR NOW return 1.0
    return 1.0;
  }

//! @brief Devuelve el valor correspondiente al instante being passed as parameter.
double XC::PathTimeSeries::getFactor(double pseudoTime) const
  {
    // check for a XC::quick return
    if(thePath.Size()<1)
      return 0.0;

    // determine indexes into the data array whose boundary holds the time
    double time1= time(currentTimeLoc);

    // check for another XC::quick return
    if(pseudoTime == time1)
      return cFactor * thePath(currentTimeLoc);

    const int size = time.Size();
    const int sizem1 = size - 1;
    const int sizem2 = size - 2;

    // check we are not at the end
    if(pseudoTime > time1 && currentTimeLoc == sizem1)
      return 0.0;

    if(pseudoTime < time1 && currentTimeLoc == 0)
      return 0.0;

    // otherwise go find the current interval
    double time2 = time(currentTimeLoc+1);
    if(pseudoTime > time2)
      {
        while((pseudoTime > time2) && (currentTimeLoc < sizem2))
          {
            currentTimeLoc++;
            time1 = time2;
            time2 = time(currentTimeLoc+1);
          }
        // if pseudo time greater than ending time reurn 0
        if(pseudoTime > time2)
          return 0.0;
      }
    else if(pseudoTime < time1)
      {
        while((pseudoTime < time1) && (currentTimeLoc > 0))
          {
            currentTimeLoc--;
            time2 = time1;
            time1 = time(currentTimeLoc);
          }
        // if starting time less than initial starting time return 0
        if(pseudoTime < time1) return 0.0;
      }
    double value1 = thePath(currentTimeLoc);
    double value2 = thePath(currentTimeLoc+1);
    return cFactor*(value1 + (value2-value1)*(pseudoTime-time1)/(time2 - time1));
  }

//! @brief Returns series duration.
double XC::PathTimeSeries::getDuration(void) const
  {
    if(thePath.Size()<1)
      {
        std::cerr << "WARNING -- PathTimeSeries::getDuration() on empty Vector" << std::endl;
        return 0.0;
      }
    int lastIndex = time.Size(); // index to last entry in time vector
    return(time(lastIndex-1));
  }

//! @brief Send members through the channel being passed as parameter.
int XC::PathTimeSeries::sendData(CommParameters &cp)
  {
    int res= PathSeriesBase::sendData(cp);
    res+= cp.sendVector(time,getDbTagData(),CommMetaData(5));
    res+= cp.sendInt(currentTimeLoc,getDbTagData(),CommMetaData(6));
    return res;
  }

//! @brief Receives members through the channel being passed as parameter.
int XC::PathTimeSeries::recvData(const CommParameters &cp)
  {
    int res= PathSeriesBase::recvData(cp);
    res+= cp.receiveVector(time,getDbTagData(),CommMetaData(5));
    res+= cp.receiveInt(currentTimeLoc,getDbTagData(),CommMetaData(6));
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::PathTimeSeries::sendSelf(CommParameters &cp)
  {
    inicComm(9);
    int result= sendData(cp);

    const int dataTag= getDbTag();
    result+= cp.sendIdData(getDbTagData(),dataTag);
    if(result < 0)
      std::cerr << "PathTimeSeries::sendSelf() - ch failed to send data\n";
    return result;
  }


//! @brief Receives object through the channel being passed as parameter.
int XC::PathTimeSeries::recvSelf(const CommParameters &cp)
  {
    inicComm(9);

    const int dataTag = this->getDbTag();  
    int result = cp.receiveIdData(getDbTagData(),dataTag);
    if(result<0)
      std::cerr << "PathTimeSeries::sendSelf() - ch failed to receive data\n";
    else
      result+= recvData(cp);
    return result;    
  }



//! @brief Imprime el objeto.
void XC::PathTimeSeries::Print(std::ostream &s, int flag) const
  {
    PathSeriesBase::Print(s,flag);
    if(flag == 1)
      s << " specified time: " << time;
  }
