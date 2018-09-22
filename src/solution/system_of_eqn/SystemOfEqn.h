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
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/SystemOfEqn.h,v $
                                                                        
                                                                        
// File: ~/system_of_eqn/SystemOfEqn.h
//
// Written: fmk 
// Created: 11/96
// Revision: A
//
// Description: This file contains the class definition for SystemOfEqn.
// SystemOfEqn is an abstract base class and thus no objects of it's type
// can be instantiated. It has pure virtual functions which must be
// implemented in it's derived classes. 
//
// What: "@(#) SystemOfEqn.h, revA"

#ifndef SystemOfEqn_h
#define SystemOfEqn_h

#include <utility/actor/actor/MovableObject.h>
#include "xc_utils/src/kernel/CommandEntity.h"

namespace XC {
class Graph;
class AnalysisModel;
class FEM_ObjectBroker;
class AnalysisAggregation;

//!  @ingroup Solu
//! 
//! \defgroup SOE System of equations.
//
//! @ingroup SOE
//
//! @brief System of equations base class.
//!
//! Responsible for storing the system of equations it represents.
//! A Solver object, which is associated with the SystemOfEqn object, is
//! responsible for performing the numerical operations to solve for the
//! system of equations.
class SystemOfEqn: public MovableObject, public CommandEntity
  {
    AnalysisAggregation *getAnalysisAggregation(void);
    const AnalysisAggregation *getAnalysisAggregation(void) const;
  protected:
    virtual AnalysisModel *getAnalysisModelPtr(void);
    virtual const AnalysisModel *getAnalysisModelPtr(void) const;

    friend class AnalysisAggregation;
    SystemOfEqn(AnalysisAggregation *,int classTag);
    //! @brief Virtual constructor.
    virtual SystemOfEqn *getCopy(void) const= 0;
  public:
    inline virtual ~SystemOfEqn(void) {}
    int checkSize(Graph &theGraph) const;
    //! @brief Invoked to cause the system of equation object to solve
    //! itself. To return 0 if successful, negative number if not.
    virtual int solve(void)= 0;
  };
} // end of XC namespace


#endif


