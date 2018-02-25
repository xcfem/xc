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
//PropRecorder.h
                                                                        
#ifndef PropRecorder_h
#define PropRecorder_h

#include <utility/recorder/DomainRecorderBase.h>
#include "xc_utils/src/nucleo/python_utils.h"

namespace XC {

//! @ingroup Recorder
//
class PropRecorder: public DomainRecorderBase
  {
  protected:
    std::string CallbackSetup; //!< Python script to execute before any record calls.
    std::string CallbackRecord; //!< Python script to execute on each record call.
    std::string CallbackRestart; //!< Python script to execute on each restart call.
    int lastCommitTag; //!< CommitTag of the last record call.
    double lastTimeStamp; //!< TimeStamp of the last record call.

    void callSetupCallback(const int &,const double &);
    template <class Container>
    void callRecordCallback(Container &c,const int &,const double &);
    template <class Container>
    void callRestartCallback(Container &c);
  public:
    PropRecorder(int classTag, Domain *ptr_dom= nullptr);

    std::string getCurrentCombinationName(void) const;

    inline int getLastCommitTag(void) const
      { return lastCommitTag; }
    inline double getLastTimeStamp(void) const
      { return lastTimeStamp; }
    double getCurrentTime(void) const;
    double getCommittedTime(void) const;
    int getCommitTag(void) const;

    void setCallbackSetup(const std::string &);
    std::string getCallbackSetup(void);
    void setCallbackRecord(const std::string &);
    std::string getCallbackRecord(void);
    void setCallbackRestart(const std::string &);
    std::string getCallbackRestart(void);

  };

//! @brief Calls record callback on each container element.
template <class Container>
void XC::PropRecorder::callRecordCallback(Container &c,const int &commitTag,const double &timeStamp)
  {
    this->callSetupCallback(commitTag,timeStamp);
    for(typename Container::iterator i= c.begin();i!=c.end();i++)
      {
	typename Container::value_type tmp= *i;
        if(tmp)
          {
            boost::python::object pyObj(boost::ref(*tmp));
            EntCmd_exec(pyObj,CallbackRecord);
          }
        else
	  std::cerr << getClassName() << "::" << __FUNCTION__
	            << "; pointer is null." << std::endl;
      }
  }

//! @brief Calls restart callback on each container element.
template <class Container>
void PropRecorder::callRestartCallback(Container &c)
  {
    for(typename Container::iterator i= c.begin();i!=c.end();i++)
      {
	typename Container::value_type tmp= *i;
        if(tmp)
          {
            boost::python::object pyObj(boost::ref(*tmp));
            EntCmd_exec(pyObj,this->CallbackRestart);
          }
        else
	  std::cerr << getClassName() << "::" << __FUNCTION__
	            << "; pointer is null." << std::endl;
      }
  }
 
} // end of XC namespace

#endif
