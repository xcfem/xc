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
                                                                        
// $Revision: 1.6 $
// $Date: 2005/11/17 23:21:40 $
// $Source: /usr/local/cvs/OpenSees/SRC/graph/graph/Graph.cpp,v $
                                                                        
                                                                        
// Written: fmk 
// Created: 11/96
// Revision: A
//
// Description: This file contains the class implementation for Graph.
//
// What: "@(#) Graph.C, revA"

#include "Graph.h"
#include <solution/graph/graph/Vertex.h>
#include <utility/matrix/Vector.h>
#include <cstdlib>

void XC::Graph::inic(const size_t &sz)
  { myVertices= ArrayOfTaggedObjects(nullptr,sz,"vertice"); }

void XC::Graph::copia(const Graph &otro)
  {
    const size_t numVertex= otro.getNumVertex();
    if(numVertex>0)
      {
        inic(numVertex);

        Graph &otro_no_const= const_cast<Graph &>(otro);

        VertexIter &otherVertices= otro_no_const.getVertices();
        Vertex *vertexPtr= nullptr;

        // loop through other creating vertices if tag not the same in this
        while((vertexPtr = otherVertices()) != nullptr)
          {
            const int vertexTag= vertexPtr->getTag();
            const int vertexRef= vertexPtr->getRef();
            Vertex newVertex(vertexTag, vertexRef);
            this->addVertex(newVertex, false);
          }

        // loop through other adding all the edges that exist in other
        VertexIter &otherVertices2 = otro_no_const.getVertices();
        while((vertexPtr = otherVertices2()) != nullptr)
          {
            int vertexTag = vertexPtr->getTag();
            const std::set<int> &adjacency= vertexPtr->getAdjacency();
            for(std::set<int>::const_iterator i=adjacency.begin(); i!=adjacency.end(); i++)
              {
                if(this->addEdge(vertexTag, *i) < 0)
                  {
	            std::cerr << "Graph::" << __FUNCTION__
		              << "; could not add an edge!\n";
	            return;
                  }
              }
          }
      }
    else
      {
        inic(32);
	numEdge= 0;
	nextFreeTag= START_VERTEX_NUM;
      }
    theVertexIter= VertexIter(&myVertices);
  }

//! @brief Constructor.
//!
//! To create an empty Graph.
XC::Graph::Graph(int numVertices)
  :MovableObject(Graph_TAG), myVertices(nullptr,numVertices,"vertice"), theVertexIter(&myVertices), numEdge(0), nextFreeTag(START_VERTEX_NUM) {}

//! @brief Copy constructor.
XC::Graph::Graph(const Graph &other) 
  :MovableObject(other), myVertices(nullptr,32,"vertice"), theVertexIter(&myVertices), numEdge(0), nextFreeTag(START_VERTEX_NUM)
  { copia(other); }

//! @brief Assignment operator.
XC::Graph &XC::Graph::operator=(const Graph &other) 
  {
    MovableObject::operator=(other);
    copia(other);
    nextFreeTag= other.nextFreeTag;
    return *this;
  }

//! @brief Appends a vertex to the graph. If the adjacency list of the
//! vertex is not empty, we check before that all the vertices of the
//! list are already on the graph.
//!
//! Causes the graph to add a vertex to the graph. If \p checkAdjacency
//! is \p true, a check is made to ensure that all the Vertices in the
//! adjacency list of the Vertex are in the Graph. If a vertex in the
//! adjacency is not in the Graph the vertex is not added, a warning
//! message is printed and \p false is returned. If successful,
//! returns the result of invoking addComponent() on the
//! TaggedStorage object used to store the Vertices.
bool XC::Graph::addVertex(const Vertex &vrt, bool checkAdjacency)
  {
    Vertex *vertexPtr=new Vertex(vrt);
    if(!vertexPtr)
      {
	std::cerr << typeid(Graph).name() << "::" << __FUNCTION__
	          << "; WARNING - attempting to add a nullptr vertex*\n";
	return false;
      }

    if(checkAdjacency == true)
      {
	if(vertexPtr->getDegree() != 0)
          {
	    const std::set<int> &adjacency= vertexPtr->getAdjacency();
            for(std::set<int>::const_iterator i=adjacency.begin(); i!=adjacency.end(); i++)
              {
		Vertex *other= this->getVertexPtr(*i);
		if(other == 0)
                  {
		    std::cerr << typeid(Graph).name() << "::" << __FUNCTION__
			      << "; WARNING  - vertex with adjacent vertex not in graph\n";
		    return false;
		  }		
	      }
	  }
      }

    const int tag= vertexPtr->getTag();
    bool result = myVertices.addComponent(vertexPtr);
    if(result == false)
      std::cerr << *this
	        << typeid(Graph).name() << "::" << __FUNCTION__
		<< "; BAD VERTEX\n: " << *vertexPtr
	        << "WARNING  - vertex could not be stored in TaggedObjectStorage object\n";

    // check nextFreeTag
    if(tag >= nextFreeTag)
      nextFreeTag = vertexPtr->getTag() + 1;
    return result;
  }


//! @brief Adds an edge to the graph. Previously we check that its vertices
//! are already defined in the graph, otherwise it returns -1 (error condition)
//!
//! Causes the Graph to add an edge {\em (vertexTag,otherVertexTag)} to
//! the Graph. A check is first made to see if vertices with tags given by
//! \p vertexTag and \p otherVertexTag exist in the graph. If they
//! do not exist a \f$-1\f$ is returned, otherwise the method invokes {\em
//! addEdge()} on each of the corresponding vertices in the 
//! graph. Increments \p numEdge by \f$1\f$ and returns \f$0\f$ if sucessfull,
//! a \f$1\f$ if the edge already existed, and a \f$-2\f$ if one addEdge()
//! was successful, but the other was not.  
//!
//! @param vertexTag: end of the edge.
//! @param otherVertexTag: the other end of the edge.
int XC::Graph::addEdge(int vertexTag, int otherVertexTag)
  {
    int retval= -1;

    // get pointers to the vertices, if one does not exist return
    Vertex *vertex1 = this->getVertexPtr(vertexTag);
    Vertex *vertex2 = this->getVertexPtr(otherVertexTag);
    if((!vertex1) || (!vertex2))
      {
	std::cerr << typeid(Graph).name() << "::" << __FUNCTION__
		  << "; WARNING - one or both of the vertices "
		  << vertexTag << " " << otherVertexTag
		  << " not in the graph\n";
	retval= -1;
      }
    else
      {
        // add an edge to each vertex
        int result = vertex1->addEdge(otherVertexTag);
        if(result == 1)
          retval= 0;  // already there
        else if(result == 0)
          {  // added to vertexTag now add to other
            if((result = vertex2->addEdge(vertexTag)) == 0)
              {
                numEdge++;
                retval= result;
              }
            else
              {
	        std::cerr << typeid(Graph).name() << "::" << __FUNCTION__
			  << "; WARNING - " << vertexTag
			  << " added to " << otherVertexTag
			  << " adjacency - but already there in otherVertexTag!.\n"
			  << *this;
		exit(0);
	        retval= -2;
	      }
          }
        else
          {
	    std::cerr << typeid(Graph).name() << "::" << __FUNCTION__
		      << "; WARNING - " << vertexTag
		      << " added to " << otherVertexTag
		      << " adjacency - but not vica versa!.\n"
		      << *this;
	    exit(0);
	    retval= -2;
          }
      }
    return retval;
  }

//! @brief Returns a pointer to the vertex identified by the tag being passed as parameter.
XC::Vertex *XC::Graph::getVertexPtr(int vertexTag)
  {
    TaggedObject *res = myVertices.getComponentPtr(vertexTag);
    if(res)
      return dynamic_cast<Vertex *>(res);
    else
      return nullptr;
  }

//! @brief Returns a pointer to the vertex identified by the tag being passed as parameter.
//!
//! A method which returns a pointer to the vertex whose tag is given by {\em
//! vertexTag}. If no such vertex exists in the graph \f$0\f$ is
//! returned. Invokes {\em getComponentPtr(vertexTag)} on the vertex
//! storage object and casts this to a Vertex * if not null.
const XC::Vertex *XC::Graph::getVertexPtr(int vertexTag) const
  {
    const TaggedObject *res= myVertices.getComponentPtr(vertexTag);
    if(res)
      return dynamic_cast<const Vertex *>(res);
    else
      return nullptr;
  }

//! @brief Returns an iterator to the vertices of the graph.
//!
//! A method which returns a reference to the graphs VertexIter. This iter
//! can be used for iterating through the vertices of the graph.
XC::VertexIter &XC::Graph::getVertices(void)
  {
    // reset the iter and then return it
    theVertexIter.reset();
    return theVertexIter;
  }

//! @brief Return the number of vertices in the graph.
int XC::Graph::getNumVertex(void) const
  { return myVertices.getNumComponents(); }

//! @brief Return the number of edges in the graph.
int XC::Graph::getNumEdge(void) const
  { return numEdge; }

//! @brief Returns the siguiente identificador (tag) libre.
int XC::Graph::getFreeTag(void)
  { return nextFreeTag; }

//! @brief Removes from the graph the vertex identified by
//! the tag being passed as parameter.
//!
//! To remove the Vertex from the Graph whose tag is equal to {\em
//! vertexTag}. If \p removeEdgeFlag is \p true will also remove the
//! Vertex from the remaining Vertices adjacency lists. returns a pointer
//! to the removed Vertex if successful, \f$0\f$ if the Vertex was not in the
//! Graph. Invokes {\em removeComponent(vertexTag)} on the vertex
//! storage object and casts this to a Vertex * if not null. DOES NOT YET
//! DEAL WITH \p removeEdgeFlag.
bool XC::Graph::removeVertex(int tag, bool flag)
  {
    Vertex *result= nullptr;
    bool retval= false;
    TaggedObject *mc= myVertices.getComponentPtr(tag);
    if(mc)
      {
        result= dynamic_cast<Vertex *>(mc);
        if(flag == true)
          { // remove all edges associated with the vertex
	    std::cerr << typeid(Graph).name() << "::" << __FUNCTION__
		      << "; no code to remove edges yet\n";
          }
        myVertices.removeComponent(tag);
        retval= true;
      }
    return retval;
  }

//! @brief Mezcla los dos grafos.
int XC::Graph::merge(Graph &other)
  {
    int result =0;
    VertexIter &otherVertices = other.getVertices();
    Vertex *vertexPtrOther;

    // loop through other creating vertices if tag not the same in this
    while((vertexPtrOther = otherVertices()) != 0)
      {
        const int vertexTag = vertexPtrOther->getTag();
        Vertex *vertexPtr= this->getVertexPtr(vertexTag);
        if(vertexPtr == 0)
          {
            int vertexRef = vertexPtrOther->getRef();
            Vertex newVertex(vertexTag, vertexRef);
            if(!vertexPtr)
              {
	        std::cerr << typeid(Graph).name() << "::" << __FUNCTION__
			  << "; out of memory\n";
	        return -1;
              }
            this->addVertex(newVertex, false);
          }
      }

    // loop through other adding all the edges that exist in other
    VertexIter &otherVertices2 = other.getVertices();
    while ((vertexPtrOther = otherVertices2()) != 0)
      {
        int vertexTag = vertexPtrOther->getTag();
        const std::set<int> &adjacency= vertexPtrOther->getAdjacency();
        for(std::set<int>::const_iterator i=adjacency.begin(); i!=adjacency.end(); i++)
          {
            if(this->addEdge(vertexTag, *i) < 0)
              {
	        std::cerr << typeid(Graph).name() << "::" << __FUNCTION__
			  << "; could not add an edge!\n";
	        return -2;	
              }
          }
      }
    return result;
  }

//! @brief Returns the extremos del ancho de banda.
void XC::Graph::getBand(int &numSubD,int &numSuperD) const
  {
    numSubD = 0;
    numSuperD = 0;
    Vertex *vertexPtr;
    Graph *this_no_const= const_cast<Graph *>(this);
    VertexIter &theVertices= this_no_const->getVertices();
    
    while((vertexPtr = theVertices()) != 0)
      {
        const int vertexNum= vertexPtr->getTag();
        const std::set<int> &theAdjacency= vertexPtr->getAdjacency();
        for(std::set<int>::const_iterator i= theAdjacency.begin(); i!= theAdjacency.end(); i++)
          {
            const int otherNum= *i;
            const int diff= vertexNum - otherNum;
	    if(diff > 0)
              {
                if(diff > numSuperD)
                  numSuperD = diff;
	      }
            else 
              if(diff < numSubD)
	        numSubD = diff;
          }
      }
    numSubD*= -1;
  }

//! @brief Returns the maximum (positive) of the difference between vertices indexes.
int XC::Graph::getVertexDiffMaxima(void) const
  {
    int retval = 0;
    const Vertex *vertexPtr= nullptr;
    Graph *this_no_const= const_cast<Graph *>(this);
    VertexIter &theVertices = this_no_const->getVertices();

    while((vertexPtr = theVertices()) != 0)
      {
        int vertexNum = vertexPtr->getTag();
        const std::set<int> &theAdjacency= vertexPtr->getAdjacency();
        for(std::set<int>::const_iterator i= theAdjacency.begin(); i!= theAdjacency.end(); i++)
          {
            const int otherNum= *i;
            const int diff= vertexNum-otherNum;
            if(retval < diff)
              retval= diff;
          }
      }

    return retval;
  }

//! @brief Returns the extreme (positive or negative) of the difference between vertices indexes.
int XC::Graph::getVertexDiffExtrema(void) const
  {
    int retval = 0;
  
    const Vertex *vertexPtr= nullptr;
    Graph *this_no_const= const_cast<Graph *>(this);
    VertexIter &theVertices = this_no_const->getVertices();
  
    while((vertexPtr = theVertices()) != 0)
      {
        int vertexNum = vertexPtr->getTag();
        const std::set<int> &theAdjacency= vertexPtr->getAdjacency();
        for(std::set<int>::const_iterator i= theAdjacency.begin(); i!= theAdjacency.end(); i++)
          {
            const int otherNum= *i;
            const int diff = vertexNum - otherNum;
            if(diff > 0)
              {
	        if(diff > retval)
	          retval = diff;
              }
            else 
	      if(diff < -retval)
	        retval = -diff;
          }
      }
    return retval;
  }
//! @brief Prints the graph.
void XC::Graph::Print(std::ostream &os, int flag)
  { myVertices.Print(os, flag); }

//! @brief Prints stuff.
std::ostream &XC::operator<<(std::ostream &s, Graph &M)
  {
    M.Print(s);
    return s;
  }

//! @brief Send object members through the channel being passed as parameter.
int XC::Graph::sendData(CommParameters &cp)
  {
    //setDbTagDataPos(0,getTag());
    int res= cp.sendInts(numEdge,nextFreeTag,getDbTagData(),CommMetaData(2));
    res+= cp.sendMovable(myVertices,getDbTagData(),CommMetaData(3));
    return res;
  }

//! @brief Receives object members through the channel being passed as parameter.
int XC::Graph::recvData(const CommParameters &cp)
  {
    //setTag(getDbTagDataPos(0));
    int res= cp.receiveInts(numEdge,nextFreeTag,getDbTagData(),CommMetaData(2));
    res+= myVertices.recibe<Vertex>(getDbTagDataPos(3),cp,&FEM_ObjectBroker::getNewVertex);
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::Graph::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(3);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << typeid(Graph).name() << "::" << __FUNCTION__
		<< "; failed to send data\n";
    return res;
  }


//! @brief Receives object through the channel being passed as parameter.
int XC::Graph::recvSelf(const CommParameters &cp) 
  {
    inicComm(3);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << typeid(Graph).name() << "::" << __FUNCTION__
		<< "; failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
          std::cerr << typeid(Graph).name() << "::" << __FUNCTION__
		    << "; failed to receive data.\n";
      }
    return res;
  }
