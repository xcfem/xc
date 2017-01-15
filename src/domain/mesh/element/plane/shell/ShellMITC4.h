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
/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 1999, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.9 $
// $Date: 2003/02/25 23:33:01 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/plane/shell/ShellMITC4.h,v $

// Ed "C++" Love
//
// B-bar four node shell element with membrane and drill
//
#ifndef ShellMITC4_h
#define ShellMITC4_h

#include "ShellMITC4Base.h"

namespace XC {

class ShellLinearCrdTransf3d;

//! \ingroup ElemPlanos
//
//! @brief Elemento shell MIT C4.
class ShellMITC4 : public ShellMITC4Base
  {
    static ShellLinearCrdTransf3d linear_trf;
  protected:
    DbTagData &getDbTagData(void) const;

  public:
    ShellMITC4(void);
    ShellMITC4(int tag,const SectionForceDeformation *ptr_mat); 
    ShellMITC4(int tag, int node1, int node2, int node3, int node4, const SectionForceDeformation &);
    Element *getCopy(void) const;


    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
      
  }; 

} // end of XC namespace

#endif
