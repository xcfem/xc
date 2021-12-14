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

//! @brief Default constructor.
XC::PathSeries::PathSeries(void)	
  :PathSeriesBase(TSERIES_TAG_PathSeries), pathTimeIncr(1.0) {}

		   
//! @brief Constructor.
//! 
//! Used to construct a PathSeries when the data points are specified in a
//! Vector.
//!
//! @param theLoadPath: vector containing the data points which are
//! specified at \p theTimeIncr time intervals.
//! @param theTimeIncr: time step.
//! @param theFactor:  constant factor used in the relation.
XC::PathSeries::PathSeries(const Vector &theLoadPath, double theTimeIncr, double theFactor)
  : PathSeriesBase(TSERIES_TAG_PathSeries,theLoadPath,theFactor),
    pathTimeIncr(theTimeIncr) {}


//! @brief Constructor.
//! 
//! Used to construct a PathSeries when the data points are specified in a
//! file.
//!
//! @param fileName: name of the file containing the data points.
//! specified at \p theTimeIncr time intervals.
//! @param theTimeIncr: time step.
//! @param theFactor:  constant factor used in the relation.
XC::PathSeries::PathSeries(const std::string &fileName, double theTimeIncr, double theFactor)
  :PathSeriesBase(TSERIES_TAG_PathSeries,theFactor),pathTimeIncr(theTimeIncr)
  { readFromFile(fileName); }

//! @brief Read path from file.
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
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; could not open file: " << fileName << std::endl;
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

//! @brief Returns the value of the factor at the pseudo-time.
//!
//! Determines the load factor based on the \p pseudoTime and the data
//! points. Returns \f$0.0\f$ if \p pseudoTime is greater than time of last
//! data point, otherwise returns a linear interpolation of the data
//! points times the factor \p cFactor.
double XC::PathSeries::getFactor(double pseudoTime) const
  {
    double retval= 0.0;
    if(pseudoTime > 0.0)
      {
        // determine indexes into the data array whose boundary holds the time
        const double incr = pseudoTime/pathTimeIncr;
        const int incr1= static_cast<int>(floor(incr));
        const int incr2= incr1+1;

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

//! @brief Sends object data.
int XC::PathSeries::sendData(Communicator &comm)
  {
    int res= PathSeriesBase::sendData(comm);
    res+= comm.sendDouble(pathTimeIncr,getDbTagData(),CommMetaData(5));
    return res;
  }

//! @brief Receives object data.
int XC::PathSeries::recvData(const Communicator &comm)
  {
    int res= PathSeriesBase::recvData(comm);
    res+= comm.receiveDouble(pathTimeIncr,getDbTagData(),CommMetaData(5));
    return res;
  }

//! @brief Sends object through the communicator argument.
int XC::PathSeries::sendSelf(Communicator &comm)
  {
    inicComm(6);
    int result= sendData(comm);

    const int dataTag= getDbTag();
    result+= comm.sendIdData(getDbTagData(),dataTag);
    if(result < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; ch failed to send data\n";
    return result;
  }

//! @brief Receives object through the communicator argument.
int XC::PathSeries::recvSelf(const Communicator &comm)
  {
    inicComm(6);

    const int dataTag = this->getDbTag();  
    int result = comm.receiveIdData(getDbTagData(),dataTag);
    if(result<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; ch failed to receive data\n";
    else
      result+= recvData(comm);
    return result;    
  }

//! @brief Print stuff.
void XC::PathSeries::Print(std::ostream &s, int flag) const
  {
    PathSeriesBase::Print(s,flag);
  }
