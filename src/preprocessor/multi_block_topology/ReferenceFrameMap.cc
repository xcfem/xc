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
//ReferenceFrameMap.cc

#include "ReferenceFrameMap.h"
#include "preprocessor/Preprocessor.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"

#include "preprocessor/multi_block_topology/ReferenceFrame.h"
#include "preprocessor/set_mgmt/Set.h"


#include "CartesianReferenceFrame3d.h"
#include "boost/lexical_cast.hpp"

//! @brief Constructor.
XC::ReferenceFrameMap::ReferenceFrameMap(MultiBlockTopology *mbt)
  : ModelComponentContainer<ReferenceFrame>(mbt) {}

//! @brief Creates a new reference system of the type passed as paramenter.
XC::ReferenceFrame *XC::ReferenceFrameMap::New(const std::string &type)
  {
    ReferenceFrame *retval= busca(getTag());
    if(!retval) //New reference system.
      {
        if(type == "cartesianas")
          {
            Preprocessor *preprocessor= getPreprocessor();
            retval= new CartesianReferenceFrame3d("r"+boost::lexical_cast<std::string>(getTag()),preprocessor);
            (*this)[getTag()]= retval;
            tag++;
          }
        else
	  std::cerr << getClassName() << "::" << __FUNCTION__
	            << "; reference system type: '" << type
                    << "' unknown." << std::endl;
      }
    return retval;
  }

