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
//CorotShellMITC4.h

#ifndef CorotShellMITC4_h
#define CorotShellMITC4_h

#include "ShellMITC4Base.h"

namespace XC {

class ShellCorotCrdTransf3d;

//! \ingroup ElemPlanos
//
//! @brief MIT C4 shell element.
class CorotShellMITC4 : public ShellMITC4Base
  {
    static ShellCorotCrdTransf3d corot_trf;
  protected:
    DbTagData &getDbTagData(void) const;


  public:
    CorotShellMITC4(void);
    CorotShellMITC4(int tag,const SectionForceDeformation *ptr_mat); 
    CorotShellMITC4(int tag, int node1, int node2, int node3, int node4, SectionForceDeformation &theMaterial);
    Element *getCopy(void) const;

    int update(void);

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
      

  }; 

} // end of XC namespace

#endif
