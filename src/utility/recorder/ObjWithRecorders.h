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
//ObjWithRecorders.h
                                                                        
                                                                        
#ifndef ObjWithRecorders_h
#define ObjWithRecorders_h

#include "RecorderContainer.h"
#include "utility/kernel/CommandEntity.h"
#include "utility/handler/DataOutputHandler.h"

namespace XC {
//! @ingroup Recorder
//
//! @brief Object that can manage Recorders.
class ObjWithRecorders: public CommandEntity, public RecorderContainer
  {
  public:
    ObjWithRecorders(CommandEntity *owr,DataOutputHandler::map_output_handlers *oh= nullptr);
    
    boost::python::dict getPyDict(void) const;
    void setPyDict(const boost::python::dict &);
  };
} // end of XC namespace

#endif


