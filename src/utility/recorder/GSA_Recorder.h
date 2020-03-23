// -*-c++-*-
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
                                                                        
// $Revision: 1.4 $
// $Date: 2004/11/24 22:45:28 $
// $Source: /usr/local/cvs/OpenSees/SRC/recorder/GSA_Recorder.h,v $
                                                                        
#ifndef GSA_Recorder_h
#define GSA_Recorder_h

// Written: fmk 
// Created: 02/03
//
// Description: This file contains the class definition for 
// GSA_Recorder. A GSA_Recorder is used to create an o/p file
// which can be read by the Ove Arup's GSA program for
// postprocessing.
//
// What: "@(#) GSA_Recorder.h, revA"


#include <utility/recorder/DomainRecorderBase.h>
#include <utility/handler/FileStream.h>

namespace XC {
class Domain;
class FE_Datastore;

//! @ingroup Recorder
//
//! @brief  GSA_Recorder is used to create an o/p file
//! which can be read by the Ove Arup's GSA program for
//! postprocessing.
class GSA_Recorder: public DomainRecorderBase
  {
  private:	
    int ndm, ndf;
    int counter;
    FileStream theFile;
    double deltaT;
    double nextTimeStampToRecord;
  public:
    GSA_Recorder(Domain &theDomain, 
		 const std::string &fileName,
		 const std::string &title1,
		 const std::string &title2,
		 const std::string &title3,
		 const std::string &jobno,
		 const std::string &initials,
		 const std::string &spec,
		 const std::string &currency,
		 const std::string &length,
		 const std::string &force,
		 const std::string &temp,
		 double deltaT = 0.0); 

    ~GSA_Recorder(void);
    int record(int commitTag, double timeStamp);
    int playback(int commitTag);
    int restart(void);    
  };
} // end of XC namespace

#endif
