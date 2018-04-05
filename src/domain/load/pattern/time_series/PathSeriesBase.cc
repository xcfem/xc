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
//PathSeriesBase.cpp


#include "PathSeriesBase.h"
#include <utility/matrix/Vector.h>

#include <fstream>
#include "utility/actor/actor/ArrayCommMetaData.h"
#include "utility/matrix/ID.h"

//! @brief Constructor.
XC::PathSeriesBase::PathSeriesBase(int classTag, const double &theFactor)	
  :CFactorSeries(classTag,theFactor), lastSendCommitTag(-1) {}

		   
//! @brief Constructor.
XC::PathSeriesBase::PathSeriesBase(int classTag,const Vector &theLoadPath,const double &theFactor)
  :CFactorSeries(classTag,theFactor),
   thePath(theLoadPath), lastSendCommitTag(-1)
  {}

//! @brief Return the number of points that define the path.
size_t XC::PathSeriesBase::getNumDataPoints(void) const
  { return thePath.Size(); }

//! @brief Return the number of data en on the file with the name being passed as parameter.
size_t XC::PathSeriesBase::getNumDataPointsOnFile(const std::string &fName) const
  {
    double dataPoint;
    std::ifstream theFile;
    size_t retval= 0;
    // first open and go through file containg path
    theFile.open(fName.c_str(), std::ios::in);
    if(theFile.bad() || !theFile.is_open())
      {
        std::cerr << "WARNING - XC::PathSeriesBase::getNumDataPointsOnFile(fName)";
        std::cerr << " - could not open file " << fName << std::endl;
      }
    else
      {
        while(theFile >> dataPoint)
          retval++;
      }
    theFile.close();
    return retval;
  }

//! @brief Lee desde archivo
size_t XC::PathSeriesBase::load_vector_from_file(Vector &v,std::istream &is)
  {
    size_t count = 0;
    double dataPoint;
    while(is >> dataPoint)
      {
	v(count) = dataPoint;
	count++;
      }
    return count;
  }

//! @brief Return the peak value of the factor.
double XC::PathSeriesBase::getPeakFactor(void) const
  {
    double peak = fabs(thePath(0));
    const int num = thePath.Size();
    double temp= 0.0;

    for(int i=1;i<num;i++)
      {
        temp= fabs(thePath(i));
        if(temp > peak)
          peak= temp;
      }
    return (peak*cFactor);
  }

//! @brief Printing stuff.
void XC::PathSeriesBase::Print(std::ostream &s, int flag) const
  {
    if(flag == 1)
      s << " specified path: " << thePath;
  }


//! @brief Send members through the channel being passed as parameter.
int XC::PathSeriesBase::sendData(CommParameters &cp)
  {
    int res= sendData(cp);
    res+= cp.sendVector(thePath,getDbTagData(),CommMetaData(1));
    res+= cp.sendInt(lastSendCommitTag,getDbTagData(),CommMetaData(2));
    return res;
  }

//! @brief Receives members through the channel being passed as parameter.
int XC::PathSeriesBase::recvData(const CommParameters &cp)
  {
    int res= recvData(cp);
    res+= cp.receiveVector(thePath,getDbTagData(),CommMetaData(1));
    res+= cp.receiveInt(lastSendCommitTag,getDbTagData(),CommMetaData(2));
    return res;
  }

