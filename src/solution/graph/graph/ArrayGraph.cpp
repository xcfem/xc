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
// $Source: /usr/local/cvs/OpenSees/SRC/graph/graph/ArrayGraph.cpp,v $
                                                                        
                                                                        
// File: ~/graph/ArrayGraph.C
// 
// Written: fmk 
// Created: Sun Sept 15 11:47:47: 1996
// Revision: A
//
// Description: This file contains the class definition for XC::ArrayGraph.
// The vertices in an XC::ArrayGraph are held in an array. This is more efficient
// than holding them in a List data structure, but problems can arise with
// large Graphs in getting enough contiguous memory for the array.
//
// What: "@(#) ArrayGraph.C, revA"

#include "solution/graph/graph/ArrayGraph.h"
#include "solution/graph/graph/Vertex.h"

//! @brief Frees memory.
//! 
//! Goes through \p theVertices and anywhere it finds a non-zero pointer,
//! invokes the vertex destructor on that pointer. 
void XC::ArrayGraph::libera(void)
  {
    for(int i=0; i<numVertex; i++)
      if(theVertices[i])
        {
          delete theVertices[i];
          theVertices[i]= nullptr;
        }
  }

//! @brief Constructor.
//!
//! To construct an empty ArrayGraph. Creates a Vertex ** array, {\em
//! theVertices} of size \p arraySize and sets the number of vertices,
//! \p numVertex, and number of edges \p numEdge to \f$0\f$. If it fails
//! to get an array of appropriate size it sets its \p arraySize to
//! \f$0\f$; subclasses can check if successfull construction by invoking the
//! protected member function getArraySize().
XC::ArrayGraph::ArrayGraph(int arraySize)
  :numVertex(0), lastEmpty(0), theVertices(arraySize,static_cast<Vertex *>(nullptr)), myIter(*this) {}

//! @brief Destructor.
XC::ArrayGraph::~ArrayGraph(void)
  { libera(); }

//! @brief Add a vertex to the graph.
//!
//! Method to add a vertex to the graph. If the adjacency list
//! of the vertex is not empty the graph will first check to see all
//! vertices in the the the vertices adjacency list exist in the graph
//! before the vertex is added. It then checks if it neeeds a new_ array
//! and if so creates one, i.e. if the {\em arraySize} $=$ {\em
//! numVertex} it creates a new_ array, whose size is double the original
//! and copies the pointers to the vertices, before invoking {\em
//! delete()} on the old array. It now tries to add the vertex in the
//! array at location {\em vertexTag}. If this fails it adds at the first
//! empty location it comes to. Returns a 0 if successfull addition, a
//! $-1$ otherwise and a message to cerr explaining the problem.  
bool XC::ArrayGraph::addVertex(const Vertex &vrt)
  {
    Vertex *vertexPtr= new Vertex(vrt);
    // check the vertex * and its adjacency list
    if(!vertexPtr)
      {
	std::cerr << typeid(*this).name() << "::" << __FUNCTION__
		  << "; attempting to add a nullptr vertex*\n";
	return false;
      }
    
    if(vertexPtr->getDegree() != 0)
      {
	const std::set<int> &adjacency= vertexPtr->getAdjacency();
	for(std::set<int>::const_iterator i=adjacency.begin(); i!=adjacency.end(); i++)
          {
	    const Vertex *other= this->getVertexPtr(*i);
	    if(other == nullptr)
              {
		std::cerr << typeid(*this).name() << "::" << __FUNCTION__
			  << "; vertex with adjacent vertex not in graph.\n";
		return false;
	      }		
	  }
      }

    // check if we have room to place the vertex
    int vsize= theVertices.size();
    if(numVertex == vsize)
      {
        theVertices.resize(vsize*2,nullptr);
        vsize= theVertices.size();
      }

    // now see if we can add the Vertex into the array in its vertexTag location
    const int vertexTag = vertexPtr->getTag();
    if((vertexTag >= 0) && (vertexTag < vsize) && (theVertices[vertexTag] == 0))
      {
	theVertices[vertexTag]= vertexPtr;
	numVertex++;
	return 0;
      }
    else
      {
	
	// we have to search through the array till we find an empty spot
	for(int i=0; i<vsize; i++) 
	  if(theVertices[i] == nullptr)
            {
              lastEmpty= i+1; // stores the lastEmpty place
	      theVertices[i]= vertexPtr;
	      numVertex++;
	      return true;
	    }
      }
    // we should never get here
    return false;
  }

//! @brief Return a pointer to the vertex identified by the tag.
//!
//! A method which returns a pointer to the vertex whose tag is given by 
//! vertexTag. The method first looks at location {\em vertexTag} for the
//! vertex, otherwise it must search through the array until it finds the
//! vertex it is looking for. If no such vertex exists in the graph $0$ is
//! returned.
XC::Vertex *XC::ArrayGraph::getVertexPtr(int vertexTag) 
  {
    // check first to see if it's in a nice position
    const int vsize= theVertices.size();
    if((vertexTag >= 0) && (vertexTag < vsize) &&
	(theVertices[vertexTag] != nullptr) &&
	(theVertices[vertexTag]->getTag() == vertexTag))
      {	return theVertices[vertexTag]; }
    // it's not nicely positioned, we have to search
    // through theVertices until we find it
    else 
      for(int i=0; i<vsize; i++)
	if((theVertices[i] != 0) && (theVertices[i]->getTag() == vertexTag))
          { return theVertices[i]; }
    // else the vertex is not there
    return 0;
  }

//! @brief Return a const pointer to the vertex identified by the tag.
//!
//! A method which returns a const pointer to the vertex whose tag is given by 
//! vertexTag. The method first looks at location {\em vertexTag} for the
//! vertex, otherwise it must search through the array until it finds the
//! vertex it is looking for. If no such vertex exists in the graph $0$ is
//! returned.
const XC::Vertex *XC::ArrayGraph::getVertexPtr(int vertexTag) const
  {
    // check first to see if it's in a nice position
    const int vsize= theVertices.size();
    if((vertexTag >= 0) && (vertexTag < vsize) && (theVertices[vertexTag] != 0) &&
	(theVertices[vertexTag]->getTag() == vertexTag))
      {	return theVertices[vertexTag]; }
    // it's not nicely positioned, we have to search
    // through theVertices until we find it
    else 
      for(int i=0; i<vsize; i++)
	if((theVertices[i] != 0) && (theVertices[i]->getTag() == vertexTag))
          { return theVertices[i]; }
    // else the vertex is not there
    return nullptr;
  }

//! @brief Add an edge to the graph.
//!
//! Causes the graph to add an edge {\em (vertexTag,otherVertexTag)} to
//! the graph. A check is first made to see if vertices with tags given by
//! {\em vertexTag} and {\em otherVertexTag} exist in the graph. If they
//! do not exist a $-1$ is returned, otherwise the method invokes {\em
//! addEdge()} on each of the corresponding vertices in the 
//! graph. Returns $0$ if sucessfull, a negative number if not.
int XC::ArrayGraph::addEdge(int vertexTag, int otherVertexTag)
  {
    // get pointers to the vertices, if one does not exist return
    Vertex *vertex1= this->getVertexPtr(vertexTag);
    Vertex *vertex2= this->getVertexPtr(otherVertexTag);
    if((vertex1 == nullptr) || (vertex2 == nullptr))
	return -1;

    // add an edge to each vertex
    int result;
    if((result = vertex1->addEdge(otherVertexTag)) == 0)
      if((result = vertex2->addEdge(vertexTag)) == 0)
        numEdge++;
    return result;
  }

//! @brief A method which first invokes {\em reset()} on the graphs
//! ArrayVertexIter and then returns a reference to this iter.
XC::VertexIter &XC::ArrayGraph::getVertices(void) 
  {
    myIter.reset(); // reset the iter and then return it
    return myIter;
  }


//! Return the number of vertices in the graph, returns numVertex.
int XC::ArrayGraph::getNumVertex(void) const
  { return numVertex; }

//! A method to return the number of edges in the graph.
int XC::ArrayGraph::getNumEdge(void) const
  { return numEdge; }

//! @brief Return the size of the graphs array.
int XC::ArrayGraph::getArraySize(void) const
  { return theVertices.size(); }


//! @brief Print the graph.
//!
//! A method to print the graph. It first prints out numVertex and numEdge
//! and then on each newline prints the vertexTag and the edges for that
//! vertex. It does this by going through theVertices array and invoking
//! Print()  on each non-zero pointer.
void XC::ArrayGraph::Print(std::ostream &s) const
  {
    s << numVertex << " " << numEdge << std::endl;
    
    Vertex *vertexPtr;
    
    // loop over the vertices and print each
    const size_t sz= theVertices.size();  
    for(size_t i=0; i<sz; i++)
      {
	vertexPtr = theVertices[i];
	if(vertexPtr)
	  vertexPtr->Print(s);
      }
  }

//! @brief Insertion on an output stream.
std::ostream &XC::operator<<(std::ostream &s, const ArrayGraph &M)
  {
    M.Print(s);
    return s;
  }

