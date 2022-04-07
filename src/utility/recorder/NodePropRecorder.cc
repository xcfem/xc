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
//NodePropRecorder.cc

#include <utility/recorder/NodePropRecorder.h>
#include <domain/domain/Domain.h>
#include <domain/mesh/node/Node.h>
#include <domain/mesh/element/Element.h>
#include "utility/kernel/python_utils.h"
#include <boost/algorithm/string/predicate.hpp>

//! @brief Constructor.
XC::NodePropRecorder::NodePropRecorder(Domain *ptr_dom)
  :PropRecorder(RECORDER_TAGS_NodePropRecorder,ptr_dom) {}

//! @brief Assigns nodes to recorder.
void XC::NodePropRecorder::setNodes(const ID &iNodes)
  {
    const int sz= iNodes.Size();
    if(sz)
      {
        for(int i= 0;i<sz;i++)
          nodes.push_back(theDomain->getNode(iNodes(i)));
      }
    else
      std::cerr << "Error; " << getClassName() << "::" << __FUNCTION__
                << " node list is empty." << std::endl;
  }


//! @brief Records object properties when commit is triggered.
int XC::NodePropRecorder::record(int commitTag, double timeStamp)
  {
    callRecordCallback(nodes,commitTag,timeStamp);
    return 0;
  }

//! @brief Restarts recorder.
int XC::NodePropRecorder::restart(void)
  {
    callRestartCallback(nodes);
    return 0;
  }


