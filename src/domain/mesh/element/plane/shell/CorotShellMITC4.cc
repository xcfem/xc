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
//CorotShellMITC4.cpp

#include "CorotShellMITC4.h"
#include "ShellCorotCrdTransf3d.h"

XC::ShellCorotCrdTransf3d XC::CorotShellMITC4::corot_trf;

//! @brief Constructor
XC::CorotShellMITC4::CorotShellMITC4(void)
  : ShellMITC4Base(ELE_TAG_CorotShellMITC4,&corot_trf) {}

//! @brief Constructor
XC::CorotShellMITC4::CorotShellMITC4(int tag,const SectionForceDeformation *ptr_mat)
  : ShellMITC4Base(tag,ELE_TAG_CorotShellMITC4,ptr_mat,&corot_trf) {}

//! @brief Constructor
XC::CorotShellMITC4::CorotShellMITC4(int tag,int node1,int node2,int node3,int node4,SectionForceDeformation &theMaterial )
  : ShellMITC4Base(tag,ELE_TAG_CorotShellMITC4,&theMaterial,&corot_trf) {}

//! @brief Virtual constructor.
XC::Element* XC::CorotShellMITC4::getCopy(void) const
  { return new CorotShellMITC4(*this); }

//! @brief Update state variables.
int XC::CorotShellMITC4::update(void)
  {
    ShellMITC4Base::update();
    const ShellCorotCrdTransf3d *corot_transf= dynamic_cast<const ShellCorotCrdTransf3d *>(theCoordTransf);
    //local nodal coordinates in plane of shell
    for(int i= 0;i<4;i++)
      {
        const Vector &coorI= theNodes[i]->getCrds();
        xl[0][i]= coorI^corot_transf->G1trial();
        xl[1][i]= coorI^corot_transf->G2trial();
      }
    return 0;
  }

//! @brief Returns a vector to store the dbTags
//! de los miembros of the clase.
XC::DbTagData &XC::CorotShellMITC4::getDbTagData(void) const
  {
    static DbTagData retval(16);
    return retval;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::CorotShellMITC4::sendSelf(CommParameters &cp)
  {
    inicComm(16);
    int res= sendData(cp);

    const int dataTag= getDbTag();
    res= cp.sendIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "CorotShellMITC4::sendSelf() - failed to send ID data\n";
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::CorotShellMITC4::recvSelf(const CommParameters &cp)
  {
    inicComm(16);

    const int dataTag= getDbTag();
    int res = cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "CorotShellMITC4::sendSelf() - failed to receive ID data\n";
    else
      res+= recvData(cp);
    return res;
  }
