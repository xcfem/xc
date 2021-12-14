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
//PropRecorder.cc

#include <utility/recorder/PropRecorder.h>
#include <domain/domain/Domain.h>


#include "utility/utils/text/text_string.h"

//! @brief Constructor.
XC::PropRecorder::PropRecorder(int classTag,Domain *ptr_dom)
  : DomainRecorderBase(classTag,ptr_dom), CallbackRecord(), CallbackRestart(),
  lastCommitTag(-1),lastTimeStamp(-1.0) {}

double XC::PropRecorder::getCurrentTime(void) const
  { return theDomain->getTimeTracker().getCurrentTime(); }
double XC::PropRecorder::getCommittedTime(void) const
  { return theDomain->getTimeTracker().getCommittedTime(); }
int XC::PropRecorder::getCommitTag(void) const
  { return theDomain->getCommitTag(); }

//! @brief Returns the name of the current combination.
std::string XC::PropRecorder::getCurrentCombinationName(void) const
  {
    std::string retval= "";
    if(theDomain)
      retval= theDomain->getCurrentCombinationName();
    return retval;
  }

//! @brief Runs setup callback.
void XC::PropRecorder::callSetupCallback(const int &commitTag,const double &timeStamp)
  {
    this->lastCommitTag= commitTag;
    this->lastTimeStamp= timeStamp;
    boost::python::object pyObj(boost::ref(*this));
    if(!CallbackSetup.empty())
      CommandEntity_exec(pyObj,CallbackSetup);
  }


void XC::PropRecorder::setCallbackRecord(const std::string &str)
  { CallbackRecord= str; }
std::string XC::PropRecorder::getCallbackRecord(void)
  { return CallbackRecord; }
void XC::PropRecorder::setCallbackSetup(const std::string &str)
  { CallbackSetup= str; }
std::string XC::PropRecorder::getCallbackSetup(void)
  { return CallbackSetup; }
void XC::PropRecorder::setCallbackRestart(const std::string &str)
  { CallbackRestart= str; }
std::string XC::PropRecorder::getCallbackRestart(void)
  { return CallbackRestart; }

