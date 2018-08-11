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
//ObjWithRecorders.h
                                                                        
                                                                        
#ifndef ObjWithRecorders_h
#define ObjWithRecorders_h

#include "xc_utils/src/nucleo/EntCmd.h"
#include "utility/handler/DataOutputHandler.h"
#include <list>

namespace XC {
class Recorder;
 class Domain;

//! @ingroup Recorder
//
//! @brief Object that can manage Recorders.
class ObjWithRecorders: public EntCmd
  {
  public:
    typedef std::list<Recorder *> lista_recorders;
    typedef lista_recorders::iterator recorder_iterator;
    typedef lista_recorders::const_iterator const_recorder_iterator; 
  private:
    lista_recorders theRecorders; //!< recorders list.
    DataOutputHandler::map_output_handlers *output_handlers; //!< output handlers.

  protected:
    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);

    //! @brief Returns the number of recorders already defined.
    inline size_t getNumRecorders(void) const
      { return theRecorders.size(); }
    virtual Domain *get_domain_ptr(void)= 0;
  public:
    ObjWithRecorders(EntCmd *owr,DataOutputHandler::map_output_handlers *oh= nullptr);
    virtual ~ObjWithRecorders(void);

    Recorder *newRecorder(const std::string &,DataOutputHandler *oh= nullptr);
    virtual int addRecorder(Recorder &theRecorder);
    inline recorder_iterator recorder_begin(void)
      { return theRecorders.begin(); }
    inline const_recorder_iterator recorder_begin(void) const
      { return theRecorders.begin(); }
    inline recorder_iterator recorder_end(void)
      { return theRecorders.end(); }
    inline const_recorder_iterator recorder_end(void) const
      { return theRecorders.end(); }
    virtual int record(int track, double timeStamp= 0.0);
    void restart(void);
    virtual int removeRecorders(void);
    void setLinks(Domain *dom);
    void SetOutputHandlers(DataOutputHandler::map_output_handlers *oh);
  };
} // end of XC namespace

#endif


