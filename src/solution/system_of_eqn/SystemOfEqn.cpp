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
                                                                        
// $Revision: 1.1.1.1 $
// $Date: 2000/09/15 08:23:27 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/SystemOfEqn.cpp,v $
                                                                        
                                                                        
// File: ~/system_of_eqn/SystemOfEqn.C
//
// Written: fmk 
// Created: 11/96
// Revision: A
//
// Description: This file contains the class implementation of XC::SystemOfEqn.
//
// What: "@(#) SystemOfEqn.C, revA"

#include <solution/system_of_eqn/SystemOfEqn.h>
#include <solution/analysis/model/AnalysisModel.h>
#include "solution/AnalysisAggregation.h"
#include "solution/graph/graph/Graph.h"

//! @brief Constructor. The integer \p classTag is provided to
//! the constructor for the base class MovableObject.
XC::SystemOfEqn::SystemOfEqn(AnalysisAggregation *owr,int clasTag)
  : MovableObject(clasTag), EntCmd(owr) {}

//! @brief Returns a pointer to the solution method that owns this object.
XC::AnalysisAggregation *XC::SystemOfEqn::getAnalysisAggregation(void)
  { return dynamic_cast<AnalysisAggregation *>(Owner()); }

//! @brief Returns a const pointer to the solution method that owns this object.
const XC::AnalysisAggregation *XC::SystemOfEqn::getAnalysisAggregation(void) const
  { return dynamic_cast<const AnalysisAggregation *>(Owner()); }

//! @brief Returns a const pointer to the analysis model.
const XC::AnalysisModel *XC::SystemOfEqn::getAnalysisModelPtr(void) const
  {
    const AnalysisAggregation *sm= getAnalysisAggregation();
    assert(sm);
    return sm->getAnalysisModelPtr();
  }

//! @brief Returns a pointer to the analysis model.
XC::AnalysisModel *XC::SystemOfEqn::getAnalysisModelPtr(void)
  {
    const AnalysisAggregation *sm= getAnalysisAggregation();
    assert(sm);
    return sm->getAnalysisModelPtr();
  }

//! @brief Check number of DOFs in the graph.
int XC::SystemOfEqn::checkSize(Graph &theGraph) const
  {
    const int retval= theGraph.getNumVertex();
    if(retval==0)
      std::cerr << "WARNING! " << getClassName() << "::" << __FUNCTION__
	        << "; model has zero DOFs, add nodes or reduce constraints." << std::endl;
    return retval;
  }

