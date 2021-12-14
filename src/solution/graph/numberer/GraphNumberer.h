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
// $Date: 2000/09/15 08:23:21 $
// $Source: /usr/local/cvs/OpenSees/SRC/graph/numberer/GraphNumberer.h,v $
                                                                        
                                                                        
// File: ~/graph/numberer/GraphNumberer.h
// 
// Written: fmk 
// Created: 11/96
// Revision: A
//
// Description: This file contains the class definition for GraphNumberer.
// GraphNumberer is an abstract base class. Its subtypes are responsible for
// numbering the vertices of a graph.
//
// What: "@(#) GraphNumberer.h, revA"

#ifndef GraphNumberer_h
#define GraphNumberer_h
#include <utility/actor/actor/MovableObject.h>
#include "utility/kernel/CommandEntity.h"

namespace XC {
class ID;
class Graph;
class Channel;
class ObjectBroker;

//! @ingroup Graph
//
//! @brief Numberer for problem's degrees of freedom.
//!
//! GraphNumberer is an abstract class. The GraphNumberer
//! class defines the interface that all programmers must provide when
//! introducing new GraphNumberer subclasses. A GraphNumberer is an
//! algorithmic class for numbering the Vertices of a Graph; that is
//! assigning a unique integer value (\f$0\f$ through \p numVertex \f$-1\f$) to
//! each Vertex (uses \p tmp variable of Vertex) of the Graph.
class GraphNumberer: public MovableObject, public CommandEntity
  {
  protected:
    friend class DOF_Numberer;
    GraphNumberer(int classTag);
    //! @brief Virtual constructor.
    virtual GraphNumberer *getCopy(void) const= 0;
  public:
    virtual ~GraphNumberer(void) {}

    //! @brief Graph numbering.
    //!
    //! This is the method invoked to perform the graph numbering, that is to
    //! assign a unique integer \f$1\f$ through \p numVertex, to each Vertex in
    //! the Graph. Returns an ordered ID containing the vertex references in
    //! the order of the numbering, i.e. \f$ID(0)\f$ contains the integer
    //! reference for the vertex assigned the number 1, \f$ID(1)\f$ contains
    //! the integer reference for the vertex assigned the number 2 and so on.
    //! A side effect of the numbering is that the \p Tmp variable of each
    //! vertex is set to the number assigned to that vertex. If \p lastVertex
    //! is not \f$-1\f$ the Vertex whose tag is given by \p lastVertex
    //! should be numbered last (it does not have to be though THIS MAY CHANGE).
    virtual const ID &number(Graph &theGraph, int lastVertex = -1) =0;
    
    //! @brief Graph numbering.
    //!
    //! This is the method invoked to perform the graph numbering, that is to
    //! assign a unique integer \f$1\f$ through \p numVertex, to each Vertex in
    //! the Graph. Returns an ordered ID containing the vertex references in
    //! the order of the numbering, i.e. \f$ID(0)\f$ contains the integer
    //! reference for the vertex assigned the number 1, \f$ID(1)\f$ contains
    //! the integer reference for the vertex assigned the number 2 and so on.
    //! A side effect of the numbering is that the \p Tmp variable of each
    //! vertex is set to the number assigned to that vertex. If \p lastVertex
    //! is not \f$-1\f$ the Vertex whose tag is given by \p lastVertex
    //! should be numbered last (it does not have to be though THIS MAY CHANGE).
    virtual const ID &number(Graph &theGraph, const ID &lastVertices) =0;
  };
} // end of XC namespace

#endif

