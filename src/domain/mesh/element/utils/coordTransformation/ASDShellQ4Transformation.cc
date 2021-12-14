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
#include "ASDShellQ4Transformation.h"

XC::ASDShellQ4Transformation *XC::ASDShellQ4Transformation::getCopy(void) const
  { return new ASDShellQ4Transformation(*this); }
  
void XC::ASDShellQ4Transformation::setDomain(Domain *domain, const ID &node_ids)
  {
    // get nodes and save initial displacements and rotations
    for(size_t i = 0; i < 4; i++)
      {
	m_nodes[i] = domain->getNode(node_ids(i));
	if(m_nodes[i] == nullptr)
	  {
	    std::cerr << "ASDShellQ4Transformation::setDomain - no node "
		      << node_ids(i) << " exists in the model\n";
	    exit(-1);
	  }
	const Vector &iU = m_nodes[i]->getTrialDisp();
	if(iU.Size() != 6)
	  {
	    std::cerr << "ASDShellQ4Transformation::setDomain - node "
		      << node_ids(i)
		      << " has " << iU.Size()
		      << " DOFs, while 6 are expected\n";
	    exit(-1);
	  }
	const size_t index= i * 6;
	for(size_t j = 0; j < 6; j++)
	  { m_U0(index+j)= iU(j); }
      }
  }
