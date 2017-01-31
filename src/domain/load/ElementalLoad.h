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
                                                                        
// $Revision: 1.3 $
// $Date: 2002/06/11 20:48:07 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/ElementalLoad.h,v $
                                                                        
                                                                        
#ifndef ElementalLoad_h
#define ElementalLoad_h

// Written: fmk 
//
// Purpose: This file contains the class definition for ElementalLoad.
// ElementalLoad is an abstract class.

#include "domain/load/Load.h"

namespace XC {
 

//! \ingroup Loads
//!
//!
//! @defgroup ElemLoads Loads overt elements.
//
//! @ingroup ElemLoads
//
//! @brief Base class for loads over elements.
class ElementalLoad: public Load
  {
  protected:
    ID elemTags; //!< Tags of loaded elements.

    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);
  public:
    ElementalLoad(int tag= 0, int classTag= 0);
    ElementalLoad(int tag, int classTag,const ID &);

    int numElements(void) const;
    int setElementTags(const ID &theEleTags);
    virtual const ID &getElementTags(void) const;
  };
} // end of XC namespace

#endif

