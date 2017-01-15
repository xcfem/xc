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
                                                                        
// $Revision: 1.10 $
// $Date: 2005/11/23 22:37:43 $
// $Source: /usr/local/cvs/OpenSees/SRC/matrix/IDVarSize.cpp,v $
                                                                        
                                                                        
// Written: fmk 
// Revision: A
//
// Description: This file contains the class implementation for XC::IDVarSize.
//
// What: "@(#) IDVarSize.C, revA"

#include "IDVarSize.h"

//! @brief Constructor used to allocate a IDVarSize of size size and
//! reserve nReserve positions.
XC::IDVarSize::IDVarSize(const int &size,const int &nReserve)
  :ID()
  {
    reserve(nReserve);
  }

int &XC::IDVarSize::operator[](const int &i) 
  {
#ifdef _G3DEBUG
    // check if it is inside range [0,sz-1]
    if(i < 0)
      {
        std::cerr << "IDVarSize::[] - location " << i << " < 0\n";
        return ID_NOT_VALID_ENTRY;
      }
#endif
    const int sz= Size();
    // see if quick return
    if(i>=sz) //we have to enlarge the order of the IDVarSize
      {
        int newArraySize= std::max(i+1,sz*2);
        resize(newArraySize);
      }       
    return v_int::operator[](i);
  }

