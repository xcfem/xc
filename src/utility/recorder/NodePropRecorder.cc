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
//NodePropRecorder.cc

#include <utility/recorder/NodePropRecorder.h>
#include <domain/domain/Domain.h>
#include <domain/mesh/node/Node.h>
#include <domain/mesh/element/Element.h>
#include "xc_utils/src/nucleo/python_utils.h"

//! @brief Constructor.
XC::NodePropRecorder::NodePropRecorder(Domain *ptr_dom)
  :PropRecorder(RECORDER_TAGS_NodePropRecorder,ptr_dom) {}

//! @brief Asigns nodes to recorder.
void XC::NodePropRecorder::setNodes(const ID &iNodos)
  {
    const int sz= iNodos.Size();
    if(sz)
      {
        for(int i= 0;i<sz;i++)
          nodes.push_back(theDomain->getNode(iNodos(i)));
      }
    else
      std::cerr << "Error; " << nombre_clase() << "::" << __FUNCTION__
                << " node list is empty." << std::endl;
  }

//! @brief Records object properties when commit is triggered.
int XC::NodePropRecorder::record(int commitTag, double timeStamp)
  {
    lastCommitTag= commitTag;
    lastTimeStamp= timeStamp;
    for(dq_nodes::iterator i= nodes.begin();i!=nodes.end();i++)
      {
        Node *tmp= *i;
        if(tmp)
          {
            boost::python::object pyObj(boost::ref(*tmp));
            EntCmd_exec(pyObj,CallbackRecord);
          }
        else
	  std::cerr << nombre_clase() << "::" << __FUNCTION__
	            << "; trying to use a NULL pointer." << std::endl;
      }
    return 0;
  }

//! @brief Reinicia el recorder.
int XC::NodePropRecorder::restart(void)
  {
    for(dq_nodes::iterator i= nodes.begin();i!=nodes.end();i++)
      {
        Node *tmp= *i;
        if(tmp)
          {
            boost::python::object pyObj(boost::ref(*tmp));
            EntCmd_exec(pyObj,CallbackRecord);
          }
        else
	  std::cerr << "XC::NodePropRecorder::restart(); se intentó llamar a a null pointer." << std::endl;
      }
    return 0;
  }


