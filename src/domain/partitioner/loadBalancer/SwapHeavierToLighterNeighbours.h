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
                                                                        
// $Revision: 1.1.1.1 $
// $Date: 2000/09/15 08:23:19 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/loadBalancer/SwapHeavierToLighterNeighbours.h,v $
                                                                        
                                                                        
// File: ~/domain/loadBalancer/SwapHeavierToLighterNeighbours.h
// 
// Written: fmk 
// Created: Fri Aug 29 17:43:25 1997
// Revision: A
//
// Description: This file contains the class definition for SwapHeavierToLighterNeighbours.
// A SwapHeavierToLighterNeighbours is an object used to balance the partition.
//
// What: "@(#) SwapHeavierToLighterNeighbours.h, revA"

#ifndef SwapHeavierToLighterNeighbours_h
#define SwapHeavierToLighterNeighbours_h

#include "LoadBalancer.h"

namespace XC{

//! @ingroup LoadBalancers
//
//! @brief A SwapHeavierToLighterNeighbours is an object used to balance a
//! PartitionedDomain.
//!
//! A SwapHeavierToLighterNeighbours is an object used to balance a
//! PartitionedDomain. It does this by shedding the boundary vertices on
//! the heaviest loaded partition (subdomain).
//! It will go thought the weigted partition graph and shed load from a heavier
//! to its lighter neighbours if the ratio between the two is greater than a 
//! certain percent. It repaets this process a number of times.
class SwapHeavierToLighterNeighbours: public LoadBalancer
  {
  public:
    SwapHeavierToLighterNeighbours();
    SwapHeavierToLighterNeighbours(double factorGreater, int numReleases);
    virtual int balance(Graph &theWeightedGraph);
  };
} // end of XC namespace

#endif


