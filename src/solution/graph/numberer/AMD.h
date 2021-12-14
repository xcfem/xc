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
                                                                        
// $Revision: 1.1 $
// $Date: 2009-12-10 00:40:01 $
// $Source: /usr/local/cvs/OpenSees/SRC/graph/numberer/AMD.h,v $
                                                                        
                                                                        
// Written: fmk 
// Created: 12/09
// Revision: A
//
// Description: This file contains the class definition for AMD.
// AMD is an object to perform the Approx. Min Degree Ordering
//
// What: "@(#) AMD.h, revA"

#ifndef AMD_h
#define AMD_h

#include "BaseNumberer.h"

namespace XC {
//! @ingroup Graph
//
//! @brief AMD numberer uses approximate minimum degree
//! ordering to number the equations.
//!
//! Reference: <a href="https://people.engr.tamu.edu/davis/publications_files/An_Approximate_Minimum_Degree_Ordering_Algorithm.pdf">Approximate minimum degree ordering algorhitm.</a> 
class AMD: public BaseNumberer
  {
  protected:
    friend class FEM_ObjectBroker;
    friend class DOF_Numberer;
    AMD(void);
    GraphNumberer *getCopy(void) const;
  public:
    
    const ID &number(Graph &theGraph, int lastVertex = -1);
    const ID &number(Graph &theGraph, const ID &lastVertices);

    virtual int sendSelf(Communicator &);
    virtual int recvSelf(const Communicator &);    
  };
} // end of XC namespace

#endif

