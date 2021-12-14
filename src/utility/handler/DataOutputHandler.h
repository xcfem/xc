// -*-c++-*-
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
                                                                        
// $Revision: 1.2 $
// $Date: 2004/11/24 22:39:01 $
// $Source: /usr/local/cvs/OpenSees/SRC/handler/DataOutputHandler.h,v $

#ifndef _DataOutputHandler
#define _DataOutputHandler

#include "utility/actor/actor/MovableObject.h"
#include "utility/kernel/CommandEntity.h"
#include <map>

namespace XC {
class Vector;

//! @ingroup Utils
//!
//! @defgroup DOHandlersGrp Data output handlers.
//
//! @ingroup DOHandlersGrp
//
//! @brief Base class for data output handlers.
class DataOutputHandler: public MovableObject, public CommandEntity
  {
  public:
    typedef std::map<std::string,DataOutputHandler *> map_output_handlers;

    DataOutputHandler(int classTag);
    inline virtual ~DataOutputHandler(void) {}

    //virtual int open(const std::vector<std::string> &dataDescription, int numData) =0;
    virtual int open(const std::vector<std::string> &dataDescription) =0;
    virtual int write(Vector &data) =0;
  };
} // end of XC namespace

#endif
