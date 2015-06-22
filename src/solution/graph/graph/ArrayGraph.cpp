//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
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
** file 'COPYRIGHT'  in XC::main directory for information on usage and   **
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

void XC::ArrayGraph::libera(void)
  {
    for(int i=0; i<numVertex; i++)
      if(theVertices[i])
        {
          delete theVertices[i];
          theVertices[i]= nullptr;
        }
  }

XC::ArrayGraph::ArrayGraph(int arraySize)
  :numVertex(0), lastEmpty(0), theVertices(arraySize,static_cast<Vertex *>(nullptr)), myIter(*this) {}

XC::ArrayGraph::~ArrayGraph(void)
  { libera(); }


//! @brief Method to add a vertex to the graph. If the adjacency list
//! of the vertex is not empty the graph will first check to see all
//! vertices in the the the vertices adjacency list exist in the graph
//! before the vertex is added. It then checks if it neeeds a new_ array
//! and if so creates one, i.e. if the {\em arraySize} $=$ {\em
//! numVertex} it creates a new_ array, whose size is double the original
//! and copies the pointers to the vertices, before invoking {\em
//! delete()} on the old array. It now tries to add the vertex in the
//! array at location {\em vertexTag}. If this fails it adds at the first
//! empty location it comes to. Returns a 0 if successfull addition, a
//! $-1$ otherwise and a message to std::cerr explaining the problem.  
bool XC::ArrayGraph::addVertex(const Vertex &vrt)
  {
    Vertex *vertexPtr=new Vertex(vrt);
    // check the vertex * and its adjacency list
    if(!vertexPtr)
      {
	std::cerr << "WARNING ArrayGraph::addVertex";
	std::cerr << " - attempting to add a nullptr vertex*\n";
	return false;
      }
    
    if(vertexPtr->getDegree() != 0)
      {
	const std::set<int> &adjacency= vertexPtr->getAdjacency();
	for(std::set<int>::const_iterator i=adjacency.begin(); i!=adjacency.end(); i++)
          {
	    const Vertex *other = this->getVertexPtr(*i);
	    if(other == 0)
              {
		std::cerr << "WARNING XC::ArrayGraph::addVertex";
		std::cerr << " - vertex with adjacent vertex not in graph\n";
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
    int vertexTag = vertexPtr->getTag();
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
              lastEmpty = i+1; // stores the lastEmpty place
	      theVertices[i]= vertexPtr;
	      numVertex++;
	      return true;
	    }
      }
    // we should never get here
    return false;
  }

//! @brief A method which returns a pointer to the vertex whose tag is given by 
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

//! @brief A method which returns a pointer to the vertex whose tag is given by 
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

// int addEdge(int vertexTag, int otherVertexTag);
// Causes the XC::Graph to add an edge {\em (vertexTag,otherVertexTag)} to
// the XC::Graph. A check is first made to see if vertices with tags given by
// {\em vertexTag} and {\em otherVertexTag} exist in the graph. If they
// do not exist a $-1$ is returned, otherwise the method invokes {\em
// addEdge()} on each of the corresponding vertices in the 
// graph. Returns $0$ if sucessfull, a negative number if not.

int XC::ArrayGraph::addEdge(int vertexTag, int otherVertexTag)
  {
    // get pointers to the vertices, if one does not exist return

    Vertex *vertex1 = this->getVertexPtr(vertexTag);
    Vertex *vertex2 = this->getVertexPtr(otherVertexTag);
    if((vertex1 == 0) || (vertex2 == 0))
	return -1;

    // add an edge to each vertex
    int result;
    if((result = vertex1->addEdge(otherVertexTag)) == 0)
      if((result = vertex2->addEdge(vertexTag)) == 0)
        numEdge++;
    return result;
  }

// VertexIter \&getVertices(void);} 
// A method which first invokes {\em reset()} on the graphs XC::ArrayVertexIter
// and then returns a reference to this iter.
XC::VertexIter &XC::ArrayGraph::getVertices(void) 
  {
    // reset the iter and then return it
    myIter.reset();
    return myIter;
  }


int XC::ArrayGraph::getNumVertex(void) const
  { return numVertex; }


int XC::ArrayGraph::getNumEdge(void) const
  { return numEdge; }


int XC::ArrayGraph::getArraySize(void) const
  { return theVertices.size(); }


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

std::ostream &XC::operator<<(std::ostream &s, const XC::ArrayGraph &M)
  {
    M.Print(s);
    return s;
  }

