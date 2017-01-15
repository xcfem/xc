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
// $Date: 2004/11/24 22:45:28 $
// $Source: /usr/local/cvs/OpenSees/SRC/recorder/DamageRecorder.h,v $
                                                                        
#ifndef DamageRecorder_h
#define DamageRecorder_h


// Written: Arash Altoontash, Gregory Deierlein, 04/04
// Created: 04/04
// Revision: Arash Altoontash
//
// Description: This file contains the class definition for DamageRecorder.
// A DamageRecorder is used to obtain a response from an element section/material during 
// the analysis and apply the information to the damage model and record the damage index.
//
// What: "@(#) DamageRecorder.h, revA"

#include <utility/recorder/DomainRecorderBase.h>
#include <fstream>
#include <utility/matrix/ID.h>


namespace XC {
class Domain;
class Vector;
class Matrix;
class Element;
class Response;
class FE_Datastore;
class DamageModel;

class DamageRecorder: public DomainRecorderBase
  {
  private:	
    int eleID, numSec, dofID;
    ID responseID;                 // integer element returns in setResponse
    ID sectionTags;

    std::vector<Response *> theResponses;
    std::vector<DamageModel *> theDamageModels;

    bool echoTimeFlag; //!< flag indicating if pseudo time also printed
    std::string fileName; //!< file name  
    std::ofstream theFile; //!< output stream

    double deltaT;
    double nextTimeStampToRecord;

    void libera(void);
    void alloc(const size_t &,DamageModel *);
  public:
    DamageRecorder(int elemid, ID &secIDs, int dofid, DamageModel *dmgPtr, Domain &,
		   bool echotimeflag, double deltat , const std::string &);

    ~DamageRecorder(void);
    int record(int commitTag, double timeStamp);
    int playback(int commitTag);

    int restart(void);    
  };
} // end of XC namespace


#endif
