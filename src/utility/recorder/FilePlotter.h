// -*-c++-*-
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
                                                                        
// $Revision: 1.8 $
// $Date: 2005/03/18 22:10:19 $
// $Source: /usr/local/cvs/OpenSees/SRC/recorder/FilePlotter.h,v $
                                                                        
// Written: fmk 
// Created: 11/99
// Revision: A
//
// Description: This file contains the class definition for FilePlotter.
// A FilePlotter will create a line graph using xy points found in a file.

//
// What: "@(#) ModelBuilder.h, revA"

#ifndef FilePlotter_h
#define FilePlotter_h

#include <utility/recorder/Recorder.h>

namespace XC {
class ID;
class Vector;

//! @ingroup Recorder
//
//! @brief A FilePlotter will create a line graph using xy points
//! found in a file. Probably to deprecate.
class FilePlotter: public Recorder
  {
  private:
/*     ColorMap *theMap; */
/*     Renderer *theRenderer; */
    ID *cols;
    std::string fileName1;
    std::string fileName2;
    
    double deltaT;
    double nextTimeStampToRecord;    

    Vector *data1a;
    Vector *data1b;
    Vector *data2a;
    Vector *data2b;
  public:
    FilePlotter(const std::string &fileName,const std::string &windowTitle, 
		int xLoc, int yLoc, int width, int height, double dT);

    FilePlotter(const std::string &fileName1,const std::string &fileName2,
		const std::string &windowTitle, 
		int xLoc, int yLoc, int width, int height, double dT);
    
    ~FilePlotter(void);

    int plotFile(void);
    int plotFiles(void);

    int record(int commitTag, double timeStamp);
    int playback(int commitTag);
    int restart(void);    

    int setFile(const std::string &newFile);
    int setCol(const ID &theCols);
  };
} // end of XC namespace

#endif







