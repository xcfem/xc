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
// $Date: 2000/09/15 08:23:21 $
// $Source: /usr/local/cvs/OpenSees/SRC/graph/partitioner/GraphPartitioner.h,v $
                                                                        
                                                                        
// File: ~/graph/partitioner/GraphPartitioner.h
// 
// Written: fmk 
// Created: Sun Sept 15 11:47:47: 1996
// Revision: A
//
// Description: This file contains the class definition for GraphPartitioner.
// GraphPartitioner is an abstract base class. Its subtypes are responsible for
// partitioning the vertices of a graph. The partitioning is done in the method
// partition which sets the colors of the vertices of the graph to colors 1
// through numParrtitions.
//
// What: "@(#) GraphPartitioner.h, revA"

#ifndef GraphPartitioner_h
#define GraphPartitioner_h

namespace XC {
class ID;
class Graph;

//! @ingroup Graph
//
//! @brief Base class for graph partitioners. A graph
//! partitioner is an algorithm for partitioning
//! (coloring) the vertices of a graph.
//!
//! GraphPartitioner is an abstract base class. Its
//! subtypes are responsible for partitioning the vertices
//! of a graph. The partitioning is done in the method
//! partition which sets the colors of the vertices of the graph to colors 1
//! through numPartitions.
class GraphPartitioner
  {
  protected:
    //! @brief Constructor.
    GraphPartitioner(void) {};
  public:
    //! @brief Method invoked to partition the graph.
    //!
    //! This is the method invoked to partition the graph into \p numPart
    //! partitions. On completion of the routine each vertex will be assigned
    //! a color \f$1\f$ through \p numPart, the color assigned indicating the
    //! partition to which the vertex belongs. Returns a 0 if successful, a
    //! negative number if not; the value depending on the subclass.
    virtual int partition(Graph &theGraph, int numPart) =0;
  };
} // end of XC namespace

#endif

