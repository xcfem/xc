// -*-c++-*-
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
                                                                        
// $Revision: 1.2 $
// $Date: 2003/02/14 23:01:22 $
// $Source: /usr/local/cvs/OpenSees/SRC/graph/graph/ArrayGraph.h,v $
                                                                        
                                                                        
// File: ~/graph/graph/ArrayGraph.h
// 
// Written: fmk 
// Created: Sun Sept 15 11:47:47: 1996
// Revision: A
//
// Description: This file contains the class definition for ArrayGraph.
// The vertices in an ArrayGraph are held in an array. This is more efficient
// than holding them in a List data structure, but problems can arise with
// large Graphs in getting enough contiguous memory for the array.
//
// What: "@(#) ArrayGraph.h, revA"

#ifndef ArrayGraph_h
#define ArrayGraph_h

#include "Graph.h"
#include <solution/graph/graph/ArrayVertexIter.h>
#include <vector>

namespace XC {
//! @ingroup Graph
//
//! @brief Graph that uses an array to store its vertices.
//!
//! ArrayGraph is a subtype of Graph. The vertices for this type
//! of graph are held in a simple array data structure whose initial size
//! is specified at construction. This size can increase if
//! needed. The array storage scheme is more efficient than a List storage
//! scheme in terms of accessing the vertices; in very large problems
//! where memory is limited this type of scheme may have problems getting
//! enough contiguous meory in which case a List might be a better
//! choice. {\bf There is a question as to whether or not the public
//! methods should be declared as virtual. Good OOP programming would have
//! all methods declared as virtual, however as subclasses cannot gain
//! access to the private member data there does not seem to be much point
//! in declaring them, except for the destructor, virtual in this
//! instance.}
class ArrayGraph: public Graph
  {
  private:
    int numVertex; //!< number of vertices.
    int lastEmpty; //!< last empty position in the container.
    std::vector<Vertex *> theVertices; //!< container for the vertex pointers.
    ArrayVertexIter myIter; //!< iterator on the vertex pointer container.
    void free_mem(void);
  protected:
    int getArraySize(void) const;

    friend class ArrayVertexIter;
    ArrayGraph(const ArrayGraph &);
    ArrayGraph &operator=(const ArrayGraph &);
  public:
    ArrayGraph(int arraySize);
    virtual ~ArrayGraph(void);

    virtual bool addVertex(const Vertex &vertexPtr);
    virtual int addEdge(int vertexTag, int otherVertexTag);
    
    virtual Vertex *getVertexPtr(int vertexTag);
    virtual const Vertex *getVertexPtr(int vertexTag) const;
    virtual VertexIter &getVertices(void);
    int getNumVertex(void) const;
    int getNumEdge(void) const;

    virtual void Print(std::ostream &s) const;
    friend std::ostream &operator<<(std::ostream &, const ArrayGraph &);    
  };

std::ostream &operator<<(std::ostream &, const ArrayGraph &);    
} // end of XC namespace

#endif

