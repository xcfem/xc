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
                                                                        
// $Revision: 1.4 $
// $Date: 2004/11/13 08:08:14 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/algorithm/ObjWithRecorders.cpp,v $
                                                                        
                                                                        
#include "ObjWithRecorders.h"

XC::ObjWithRecorders::ObjWithRecorders(CommandEntity *owr,DataOutputHandler::map_output_handlers *oh)
  : CommandEntity(owr), RecorderContainer(oh) {}

//! @brief Return a Python dictionary with the object members values.
boost::python::dict XC::ObjWithRecorders::getPyDict(void) const
  {
    boost::python::dict retval= CommandEntity::getPyDict();
    retval["recorders"]= RecorderContainer::getPyDict();
    return retval;
  }

//! @brief Set the values of the object members from a Python dictionary.
void XC::ObjWithRecorders::setPyDict(const boost::python::dict &d)
  {
    CommandEntity::setPyDict(d);
    if(d.has_key("recorders"))
      {
        const boost::python::dict tmp= boost::python::extract<boost::python::dict>(d["recorders"]);
        RecorderContainer::setPyDict(tmp);
      }
  }
