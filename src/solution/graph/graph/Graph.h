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
                                                                        
// $Revision: 1.3 $
// $Date: 2005/11/03 23:11:55 $
// $Source: /usr/local/cvs/OpenSees/SRC/graph/graph/Graph.h,v $
                                                                        
                                                                        
#ifndef Graph_h
#define Graph_h

// Written: fmk 
// Revision: A
//
// Description: This file contains the class definition for Graph.
// The Graph class provides the abstraction of a graph, a collection of
// vertices and edges. The Graph class is a container class which stores
// and provides access to Vertex objects. The Vertices contain information 
// about the edges in this design.
//
// What: "@(#) Graph.h, revA"

#include <iostream>
#include "utility/actor/actor/MovableObject.h"
#include "utility/tagged/storage/ArrayOfTaggedObjects.h"
#include "solution/graph/graph/VertexIter.h"

namespace XC {
class Vertex;
class VertexIter;
class TaggedObjectStorage;
class Channel;
class FEM_ObjectBroker;

//! @ingroup Graph
//
//! @brief The Graph class provides the abstraction of a graph

//! Provides the interface for all graph classes, a collection of
//! vertices and edges. The Graph class is a container class which stores
//! and provides access to Vertex objects. The Vertices contain information 
//! about the edges in this design.
//! A Graph is a container class
//! responsible for holding the vertex set and edge set. The class is
//! responsible for:
//!   -# providing methods to add vertices and edges.
//!   -# accessing the vertices and edges.
//! All the methods for the class are declared as
//! virtual to allow subclasses to change its behavior.
class Graph: public MovableObject
  {
  protected:
    ArrayOfTaggedObjects myVertices;
    VertexIter theVertexIter;
    int numEdge;
    int nextFreeTag;

    void inic(const size_t &);
    void copy(const Graph &other);
    int sendData(Communicator &);
    int recvData(const Communicator &);

  public:
    Graph(int numVertices= 32);    
    Graph(const Graph &other);
    Graph &operator=(const Graph &other);

    virtual bool addVertex(const Vertex &vertexPtr, bool checkAdjacency = true);
    virtual int addEdge(int vertexTag, int otherVertexTag);
    
    virtual Vertex *getVertexPtr(int vertexTag);
    virtual const Vertex *getVertexPtr(int vertexTag) const;
    virtual VertexIter &getVertices(void);
    virtual int getNumVertex(void) const;
    virtual int getNumEdge(void) const;
    virtual int getFreeTag(void);
    virtual bool removeVertex(int tag, bool removeEdgeFlag = true);
    const Vertex *BuscaRef(int ref) const;
    void getBand(int &,int &) const;
    int getVertexDiffMaxima(void) const;
    int getVertexDiffExtrema(void) const;


    virtual int merge(Graph &other);
    
    virtual void Print(std::ostream &os, int flag =0) const;
    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);

    friend std::ostream &operator<<(std::ostream &, const Graph &);    
  };

std::ostream &operator<<(std::ostream &, const Graph &);    
} // end of XC namespace

#endif

