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
                                                                        
// $Revision: 1.3 $
// $Date: 2006/01/12 23:39:21 $
// $Source: /usr/local/cvs/OpenSees/SRC/graph/numberer/MyRCM.cpp,v $
                                                                        
                                                                        
// File: ~/graph/numberer/MyRCM.C
// 
// Written: fmk 
// Created: Sun Sept 15 11:47:47: 1996
// Revision: A
//
// Description: This file contains the class definition for XC::MyRCM.
// MyRCM is an object to perform the Reverse Cuthill-McKee numbering
// scheme on the vertices of a graph. This is done by invoking the
// number() method with the XC::Graph.
//
// What: "@(#) MyRCM.C, revA"

#include <solution/graph/numberer/MyRCM.h>
#include <solution/graph/graph/Graph.h>
#include <solution/graph/graph/Vertex.h>
#include <solution/graph/graph/VertexIter.h>
#include <utility/matrix/ID.h>

// Constructor
XC::MyRCM::MyRCM(int startVertex, bool minDegreeFlag)
  :BaseNumberer(GraphNUMBERER_TAG_MyRCM),
   startVertexTag(startVertex), minDegree(minDegreeFlag) {}

XC::GraphNumberer *XC::MyRCM::getCopy(void) const
  { return new MyRCM(*this); }


void XC::MyRCM::setStartVertex(int startVertex)
  { startVertexTag = startVertex; }

void XC::MyRCM::setMinDegreeFlag(bool flag)
  { minDegree = flag; }


// const XC::ID &number(Graph &theGraph,int startVertexTag = -1,
//                  bool minDegree = false)
//    Method to perform the Reverse Cuthill-mcKenn numbering scheme. The
// user can supply a starting vertex, if none is provided the first vertex
// returned by the iter is used. If minDegree flag is set to true, at each 
// level set the adjacent vertices not yet added from a vertex in the previos
// level set are added in descending degree. The result of the numbering scheme
// is returned in an XC::ID which contains the references for the vertices.
//
// side effects: this routine changes the color of the vertices.

const XC::ID &XC::MyRCM::number(Graph &theGraph, int startVertex)
  {
    // see if we can do quick return
    if(!checkSize(theGraph)) 
      return theRefResult;	    

    // we first set the Tmp of all vertices to -1, indicating
    // they have not yet been added.
    
    Vertex *vertexPtr= nullptr;
    VertexIter &vertexIter = theGraph.getVertices();
    
    while ((vertexPtr = vertexIter()) != 0)
	vertexPtr->setTmp(-1);

    // we now set up; setting our markers and getting first vertex
    if (startVertex != -1)
	startVertexTag = startVertex;
    
    if (startVertexTag != -1) {
	vertexPtr = theGraph.getVertexPtr(startVertexTag);
	if (vertexPtr == 0) {
	    std::cerr << "WARNING:  XC::MyRCM::number - No vertex with tag ";
	    std::cerr << startVertexTag << "Exists - using first come from iter\n";
	    startVertexTag = -1;
	}
    }	

    // if no starting vertex use the first one we get from the XC::VertexIter
    
    VertexIter &vertexIter2 = theGraph.getVertices();    
    if (startVertexTag == -1) 
	vertexPtr = vertexIter2();

    const int numVertex= getNumVertex();
    int currentMark= numVertex-1;  // marks current vertex visiting.
    int nextMark= currentMark -1;  // indiactes where to put next Tag in XC::ID.
    theRefResult(currentMark) = vertexPtr->getTag();
    vertexPtr->setTmp(currentMark);

    // we continue till the XC::ID is full

    while (nextMark >= 0) {

	// get the current vertex and its adjacency
	
	vertexPtr = theGraph.getVertexPtr(theRefResult(currentMark));
	const std::set<int> &adjacency = vertexPtr->getAdjacency();

	// go through the vertices adjacency and add vertices which
	// have not yet been Tmp'ed to the (theRefResult)

	for(std::set<int>::const_iterator i=adjacency.begin(); i!=adjacency.end(); i++)
          {
	    
	    const int vertexTag= *i;
	    vertexPtr = theGraph.getVertexPtr(vertexTag);
	    if((vertexPtr->getTmp()) == -1)
              {
		vertexPtr->setTmp(nextMark);
		theRefResult(nextMark--) = vertexTag;
	      }
	  }

	// go to the next vertex
	//  we decrement because we are doing reverse Cuthill-McKee
	
	currentMark--;

	// check to see if graph is disconneted
	
	if ((currentMark == nextMark) && (currentMark >= 0)) {
	    std::cerr << "WARNING:  XC::MyRCM::number - Disconnected graph\n";
	    
	    // loop over iter till we get a vertex not yet Tmped
	    
	    while (((vertexPtr = vertexIter2()) != 0) && 
		   (vertexPtr->getTmp() != -1)) 
		;
	    
	    nextMark--;
	    vertexPtr->setTmp(currentMark);
	    theRefResult(currentMark) = vertexPtr->getTag();
	}

    }
    
    // now set the vertex references instead of the vertex tags
    // in the result, we change the Tmp to indicate number and return
    
    for(int i=0; i<numVertex; i++)
      {
	const int vertexTag = theRefResult(i);
	vertexPtr = theGraph.getVertexPtr(vertexTag);
	vertexPtr->setTmp(i+1); // 1 through numVertex
	theRefResult(i) = vertexPtr->getTag();
      }

    theGraph.Print(std::cerr, 3);
    std::cerr << theRefResult;
    return theRefResult;
  }



int
XC::MyRCM::sendSelf(CommParameters &cp)
{
    return 0;
}

int XC::MyRCM::recvSelf(const CommParameters &cp)
  { return 0; }


const XC::ID &XC::MyRCM::number(Graph &theGraph, const XC::ID &startVertices)
  {
    // see if we can do quick return
    if(!checkSize(theGraph)) 
      return theRefResult;

    ID copyStart(startVertices);

    const int numVertex= getNumVertex();   
    // we determine which node to start with
    int minStartVertexTag =0;
    int minAvgProfile = 0;
    const int startVerticesSize= startVertices.Size();
    
    for(int j=0; j<startVerticesSize; j++)
      {
	// we first set the Tmp of all vertices to -1, indicating
	// they have not yet been added.
    
	Vertex *vertexPtr;
	VertexIter &vertexIter = theGraph.getVertices();
    
	while ((vertexPtr = vertexIter()) != 0)
	    vertexPtr->setTmp(-1);

	// we now set up; setting our markers and set first vertices
	VertexIter &vertexIter2 = theGraph.getVertices();
	int currentMark = numVertex-1;  // marks current vertex visiting.
	int nextMark = currentMark-1;

	for (int k=0; k<startVerticesSize; k++)
	    if (k != j)
		copyStart(k) = 0;
	    else	
		copyStart(k) = 1;
	
	vertexPtr = theGraph.getVertexPtr(startVertices(j));	
	theRefResult(currentMark) = vertexPtr->getTag();
	vertexPtr->setTmp(currentMark);

	int numFromStart = 1;
	int avgProfile = 1;	
	while (numFromStart < startVerticesSize) {
	    // get the current vertex and its adjacency

	    vertexPtr = theGraph.getVertexPtr(theRefResult(currentMark));
	    const std::set<int> &adjacency = vertexPtr->getAdjacency();

	    // go through the vertices adjacency and add vertices which
	    // have not yet been Tmp'ed to the (theRefResult)

	    for(std::set<int>::const_iterator i=adjacency.begin(); i!=adjacency.end(); i++)
              {
		const int vertexTag= *i;
		const int loc=startVertices.getLocation(vertexTag);
		if (loc >= 0) {
		    vertexPtr = theGraph.getVertexPtr(vertexTag);
		    if ((vertexPtr->getTmp()) == -1) {
			vertexPtr->setTmp(nextMark);
			copyStart(loc) = 1;
			numFromStart++;
			avgProfile += currentMark - nextMark;			
			theRefResult(nextMark--) = vertexTag;
		    }
		}
	    }

	    // go to the next vertex
	    //  we decrement because we are doing reverse Cuthill-McKee
	
	    currentMark--;

	    // check to see if graph is disconneted
	
	    if (currentMark == nextMark && numFromStart < startVerticesSize) {
		// loop over iter till we get a vertex not yet included
		
		for (int l=0; l<startVerticesSize; l++)
		    if (copyStart(l) == 0) {
			int vertexTag = startVertices(l);			
			vertexPtr = theGraph.getVertexPtr(vertexTag);			
			nextMark--;
			copyStart(l) = 1;
			vertexPtr->setTmp(currentMark);
			numFromStart++;
			theRefResult(currentMark) = vertexPtr->getTag();
			l =startVerticesSize;
		    }
	    }
	}
	
	currentMark = numVertex-1; // set current to the first again
	nextMark =  numVertex - startVerticesSize -1;

	// we continue till the XC::ID is full

	while (nextMark >= 0) {
	    // get the current vertex and its adjacency
	
	    vertexPtr = theGraph.getVertexPtr(theRefResult(currentMark));
	    const std::set<int> &adjacency = vertexPtr->getAdjacency();

	    // go through the vertices adjacency and add vertices which
	    // have not yet been Tmp'ed to the (theRefResult)

	    for(std::set<int>::const_iterator i=adjacency.begin(); i!=adjacency.end(); i++)
              {
		const int vertexTag= *i;
		vertexPtr = theGraph.getVertexPtr(vertexTag);
		if((vertexPtr->getTmp()) == -1)
                  {
		    vertexPtr->setTmp(nextMark);
		    avgProfile += currentMark - nextMark;

		    theRefResult(nextMark--) = vertexTag;
		  }
	      }
	    // go to the next vertex
	    //  we decrement because we are doing reverse Cuthill-McKee
	
	    currentMark--;

	    // check to see if graph is disconneted
	
	    if ((currentMark == nextMark) && (currentMark >= 0)) {
	    
		// loop over iter till we get a vertex not yet Tmped
		
		while (((vertexPtr = vertexIter2()) != 0) && 
		       (vertexPtr->getTmp() != -1)) 
		    ;
		
		nextMark--;
		vertexPtr->setTmp(currentMark);
		theRefResult(currentMark) = vertexPtr->getTag();
	    }
	}
	
	if (j == 0 || minAvgProfile > avgProfile) {
	    minStartVertexTag = startVertices(j);
	    minAvgProfile = avgProfile;
	}

    }

    
    // now we numebr based on minStartVErtexTag

    // we first set the Tmp of all vertices to -1, indicating
    // they have not yet been added.
    
    Vertex *vertexPtr;
    VertexIter &vertexIter = theGraph.getVertices();
    
    while ((vertexPtr = vertexIter()) != 0)
	vertexPtr->setTmp(-1);

    // we now set up; setting our markers and set first vertices
    VertexIter &vertexIter2 = theGraph.getVertices();    
    int currentMark = numVertex-1;  // marks current vertex visiting.
    int nextMark = currentMark-1;
    
    vertexPtr = theGraph.getVertexPtr(minStartVertexTag);	
    theRefResult(currentMark) = vertexPtr->getTag();
    vertexPtr->setTmp(currentMark);
    currentMark--;	
    
    int loc = startVertices.getLocation(minStartVertexTag);
    for (int k=0; k<startVerticesSize; k++)
	if (k != loc)
	    copyStart(k) = 0;
     

    int numFromStart = 1;
    while (numFromStart < startVerticesSize) {
	// get the current vertex and its adjacency

	vertexPtr = theGraph.getVertexPtr(theRefResult(currentMark));
	const std::set<int> &adjacency = vertexPtr->getAdjacency();

	// go through the vertices adjacency and add vertices which
	// have not yet been Tmp'ed to the (theRefResult)

	for(std::set<int>::const_iterator i=adjacency.begin(); i!=adjacency.end(); i++)
          {
	    const int vertexTag= *i;
	    const int loc =startVertices.getLocation(vertexTag);
	    if(loc >= 0)
              {
		vertexPtr = theGraph.getVertexPtr(vertexTag);
		if((vertexPtr->getTmp()) == -1)
                  {
		    vertexPtr->setTmp(nextMark);
		    copyStart(loc) = 1;
		    numFromStart++;
		    theRefResult(nextMark--) = vertexTag;
		  }
	      }
	  }

	// go to the next vertex
	//  we decrement because we are doing reverse Cuthill-McKee
	
	currentMark--;

	// check to see if graph is disconneted
	
	if (currentMark == nextMark && numFromStart < startVerticesSize) {
	    // loop over iter till we get a vertex not yet included
		
	    for (int l=0; l<startVerticesSize; l++)
		if (copyStart(l) == 0) {
		    int vertexTag = startVertices(l);			
		    vertexPtr = theGraph.getVertexPtr(vertexTag);			
		    nextMark--;
		    copyStart(l) = 1;
		    vertexPtr->setTmp(currentMark);
		    numFromStart++;
		    theRefResult(currentMark) = vertexPtr->getTag();
		    l =startVerticesSize;
		}
	}	
    }
	
    currentMark = numVertex-1; // set current to the first again
    nextMark =  numVertex - startVerticesSize -1;


    currentMark = numVertex-1; // set current to the first again
    
    // we continue till the XC::ID is full
    while (nextMark >= 0) {

	// get the current vertex and its adjacency
	
	vertexPtr = theGraph.getVertexPtr(theRefResult(currentMark));
	const std::set<int> &adjacency = vertexPtr->getAdjacency();
	
	// go through the vertices adjacency and add vertices which
	// have not yet been Tmp'ed to the (theRefResult)
	
	for(std::set<int>::const_iterator i=adjacency.begin(); i!=adjacency.end(); i++)
          {
	    
	    const int vertexTag= *i;
	    vertexPtr = theGraph.getVertexPtr(vertexTag);
	    if((vertexPtr->getTmp()) == -1)
              {
		vertexPtr->setTmp(nextMark);
		theRefResult(nextMark--) = vertexTag;
	      }
	  }

	// go to the next vertex
	//  we decrement because we are doing reverse Cuthill-McKee
	
	currentMark--;
	
	// check to see if graph is disconneted
	
	if ((currentMark == nextMark) && (currentMark >= 0)) {
	    std::cerr << "WARNING:  XC::MyRCM::number - Disconnected graph ";
	    
	    // loop over iter till we get a vertex not yet Tmped
	    
	    while (((vertexPtr = vertexIter2()) != 0) && 
		   (vertexPtr->getTmp() != -1)) 
		;
		
	    nextMark--;
	    vertexPtr->setTmp(currentMark);
	    theRefResult(currentMark) = vertexPtr->getTag();
	}
	
    }

    // now set the vertex references instead of the vertex tags
    // in the result, we change the Tmp to indicate number and return
    
    for(int m=0; m<numVertex; m++)
      {
	int vertexTag = theRefResult(m);
	vertexPtr = theGraph.getVertexPtr(vertexTag);
	vertexPtr->setTmp(m+1); // 1 through numVertex
	theRefResult(m) = vertexPtr->getTag();
      }

    return theRefResult;
  }



