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
#include "ASDShellQ4CorotationalTransformation.h"


XC::ASDShellQ4Transformation *XC::ASDShellQ4CorotationalTransformation::getCopy(void) const
  { return new ASDShellQ4CorotationalTransformation(*this); }

void XC::ASDShellQ4CorotationalTransformation::setDomain(Domain* domain, const ID& node_ids)
  {
    // call base class setDomain to
    // get nodes and save initial displacements and rotations
    ASDShellQ4Transformation::setDomain(domain, node_ids);
    // init state variables
    revertToStart();
  }

void XC::ASDShellQ4CorotationalTransformation::revertToStart(void)
  {
    // create the reference (undeformed configuration) coordinate system
    ASDShellQ4LocalCoordinateSystem LCS= createReferenceCoordinateSystem();

    // save reference orientation and center
    m_Q0= QuaternionType::FromRotationMatrix(LCS.Orientation());
    m_C0= LCS.Center();

    // save initial rotations, no need to take current rotation
    // since we will remove the initial ones (themselves)...
    for(int i= 0; i < 4; i++)
      {
	m_RV[i]= Vector3Type(0.0, 0.0, 0.0);
	m_QN[i]= QuaternionType::FromRotationVector(m_RV[i]);

	m_RV_converged[i]= m_RV[i];
	m_QN_converged[i]= m_QN[i];
      }
  }
