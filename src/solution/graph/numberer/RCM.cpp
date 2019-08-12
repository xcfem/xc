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
                                                                        
// $Revision: 1.3 $
// $Date: 2003/10/06 20:54:08 $
// $Source: /usr/local/cvs/OpenSees/SRC/graph/numberer/RCM.cpp,v $
                                                                        
                                                                        
// Written: fmk 
// Revision: A
//
// Description: This file contains the class definition for XC::RCM.
// RCM is an object to perform the Reverse Cuthill-McKee numbering
// scheme on the vertices of a graph. This is done by invoking the
// number() method with the XC::Graph.
//
// What: "@(#) RCM.C, revA"

#include <solution/graph/numberer/RCM.h>
#include <solution/graph/graph/Graph.h>
#include <solution/graph/graph/Vertex.h>
#include <solution/graph/graph/VertexIter.h>
#include <utility/matrix/ID.h>

//! @brief  Constructor.
//! 
//! The flag \p GPS is used to mark whether the
//! Gibbs-Poole-Stodlmyer algorithm is used to determine a starting vertex
//! when no starting vertex is given.
XC::RCM::RCM(bool gps)
  :BaseNumberer(GraphNUMBERER_TAG_RCM), GPS(gps) {}

//! @brief Virtual constructor.
XC::GraphNumberer *XC::RCM::getCopy(void) const
  { return new RCM(*this); }

// const ID &number(Graph &theGraph,int startVertexTag= -1,
//                  bool minDegree= false)
//! @brief Method to perform the Reverse Cuthill-mcKenn numbering scheme. The
//! user can supply a starting vertex, if none is provided the first vertex
//! returned by the iter is used. If minDegree flag is set to true, at each 
//! level set the adjacent vertices not yet added from a vertex in the previous
//! level set are added in descending degree. The result of the numbering scheme
//! is returned in an ID which contains the references for the vertices.
//!
//! side effects: this routine changes the color of the vertices.
//! If the present ID used for the result is not of size equal to the
//! number of Vertices in \p theGraph, it deletes the old and
//! constructs a new ID. Starts by iterating through the Vertices of the
//! graph setting the \p tmp variable of each to \f$-1\f$. The Vertices are
//! then numbered using a depth first sort of the Graph, with each
//! unmarked Vertex in the Graph at a distance \f$d\f$ from starting Vertex
//! being placed in the d'th level set. As this is RCM, the Vertices in
//! level set \f$n\f$ are assigned a higher number than those in level set
//! \f$n+1\f$ with the \p tmp variable of the starting Vertex being
//! assigned \p numVertices \f$-1\f$. The \p tags of the Vertices are
//! placed into the ID at location given by their \p tmp variable. These
//! are replaced with the \p ref variable of each Vertex, which is
//! returned on successful completion. 
//!
//! The Vertex chosen as the starting Vertex is the one whose tag is given
//! by \p lastVertex. If this is \f$-1\f$ or the Vertex corresponding to
//! \p lastVertex does not exist then another Vertex is chosen. If the
//! \p GPS flag in constructor is \p false the first Vertex from the
//! Graphs VertexIter is used; if \p true a RCM numbering using the
//! first Vertex from the VertexIter is performed and the Vertices in the
//! last level set are then used to create an ID \p lastVertices with
//! which {\em number(theGraph, lastVertices)} can be invoked to determine
//! the numbering.
const XC::ID &XC::RCM::number(Graph &theGraph, int startVertex)
  {
    // see if we can do quick return
    if(!checkSize(theGraph)) 
      return theRefResult;

    // we first set the Tmp of all vertices to -1, indicating
    // they have not yet been added.
    Vertex *vertexPtr= nullptr;
    VertexIter &vertexIter= theGraph.getVertices();
    
    while((vertexPtr= vertexIter()) != 0)
      vertexPtr->setTmp(-1);

    // we now set up; setting our markers and getting first vertex
    int startVertexTag= startVertex;   
    if(startVertexTag != -1)
      {
        vertexPtr= theGraph.getVertexPtr(startVertexTag);
        if(vertexPtr == 0)
          {
            std::cerr << "WARNING:  RCM::number - No vertex with tag ";
            std::cerr << startVertexTag << "Exists - using first come from iter\n";
            startVertexTag= -1;
          }
      }

    // if no starting vertex use the first one we get from the VertexIter
    VertexIter &vertexIter2= theGraph.getVertices();    
    Vertex *start= nullptr;
    const int numVertex= getNumVertex();
    if(startVertexTag == -1)
      {
        vertexPtr= vertexIter2();
        start= vertexPtr;

        // if GPS true use gibbs-poole-stodlmyer determine the last 
        // level set assuming a starting vertex and then use one of the 
        // nodes in this set to base the numbering on        
        if(GPS == true)
          {  
            int currentMark= numVertex-1;  // marks current vertex visiting.
            int nextMark= currentMark -1;  // marks where to put next Tag
            int startLastLevelSet= nextMark;
            theRefResult(currentMark)= vertexPtr->getTag();
            vertexPtr->setTmp(currentMark);    

            // we continue till the ID is full

            while(nextMark >= 0)
              {
                // get the current vertex and its adjacency  
                vertexPtr= theGraph.getVertexPtr(theRefResult(currentMark));
                const std::set<int> &adjacency= vertexPtr->getAdjacency();

                // go through the vertices adjacency and add vertices which
                // have not yet been Tmp'ed to the (theRefResult)

                for(std::set<int>::const_iterator i= adjacency.begin(); i!= adjacency.end(); i++)
                  {            
                    const int vertexTag= *i;
                    vertexPtr= theGraph.getVertexPtr(vertexTag);
                    if((vertexPtr->getTmp()) == -1)
                      {
                        vertexPtr->setTmp(nextMark);
                        theRefResult(nextMark--)= vertexTag;
                      }
                  }

                // go to the next vertex
                //  we decrement because we are doing reverse Cuthill-McKee
                currentMark--;
                
                if(startLastLevelSet == currentMark)
                  startLastLevelSet= nextMark;

                // check to see if graph is disconnected
        
                if((currentMark == nextMark) && (currentMark >= 0))
                  {

                    // loop over iter till we get a vertex not yet Tmped
                    while(((vertexPtr= vertexIter2()) != 0) && 
                           (vertexPtr->getTmp() != -1)) 
                        ;
            
                    nextMark--;
                    startLastLevelSet= nextMark;
                    vertexPtr->setTmp(currentMark);
                    theRefResult(currentMark)= vertexPtr->getTag();
                  }
              }

            // create an id of the last level set
            if(startLastLevelSet > 0)
              {
                ID lastLevelSet(startLastLevelSet);
                for(int i=0; i<startLastLevelSet; i++)
                  lastLevelSet(i)= theRefResult(i); 
                return this->number(theGraph,lastLevelSet);
              }
          }
        else // we start with just the first vertex we get
          vertexPtr= start;
      }

    VertexIter &vertexIter3= theGraph.getVertices();
    Vertex *otherPtr= nullptr;

    // set to -1 all the vertices     
    while((otherPtr= vertexIter3()) != 0)
      otherPtr->setTmp(-1);

    VertexIter &vertexIter4= theGraph.getVertices();

    int currentMark= numVertex-1;  // marks current vertex visiting.
    int nextMark= currentMark -1;  // indiactes where to put next Tag in XC::ID.
    theRefResult(currentMark)= vertexPtr->getTag();
    vertexPtr->setTmp(currentMark);

    // we continue till the ID is full
    while(nextMark >= 0)
      {
        // get the current vertex and its adjacency
        vertexPtr= theGraph.getVertexPtr(theRefResult(currentMark));
        const std::set<int> &adjacency= vertexPtr->getAdjacency();

        // go through the vertices adjacency and add vertices which
        // have not yet been Tmp'ed to the (theRefResult)

        for(std::set<int>::const_iterator i= adjacency.begin(); i!= adjacency.end(); i++)
          {            
            const int vertexTag= *i;
            vertexPtr= theGraph.getVertexPtr(vertexTag);
            if((vertexPtr->getTmp()) == -1)
              {
                vertexPtr->setTmp(nextMark);
                theRefResult(nextMark--)= vertexTag;
              }
          }

        // go to the next vertex
        //  we decrement because we are doing reverse Cuthill-McKee
        currentMark--;

        // check to see if graph is disconnected
        
        if((currentMark == nextMark) && (currentMark >= 0))
          {
            // std::cerr << "WARNING:  XC::RCM::number - Disconnected graph -2 \n ";
            // loop over iter till we get a vertex not yet Tmped         
            while(((vertexPtr= vertexIter4()) != 0) && 
                   (vertexPtr->getTmp() != -1)) 
              ;
            
            nextMark--;
            vertexPtr->setTmp(currentMark);
            theRefResult(currentMark)= vertexPtr->getTag();
          }
      }
    
    // now set the vertex references instead of the vertex tags
    // in the result, we change the Tmp to indicate number and return  
    for(int i=0; i<numVertex; i++)
      {
        const int vertexTag= theRefResult(i);
        vertexPtr= theGraph.getVertexPtr(vertexTag);
        vertexPtr->setTmp(i+1); // 1 through numVertex
        theRefResult(i)= vertexPtr->getTag();
      }
    return theRefResult;
  }



int XC::RCM::sendSelf(CommParameters &cp)
  { return 0; }

int XC::RCM::recvSelf(const CommParameters &cp)
  { return 0; }


//! @brief Determine the best starting vertex.
//! 
//! This method is invoked to determine the best starting Vertex for a RCM
//! using a Vertex whose tag is in \p lastVertices. To do a RCM
//! numbering is performed using each of the Vertices in {\em
//! startVertices} as the Vertex in level set \f$0\f$. The Vertex which
//! results in the numbering with the smallest profile is chosen as 
//! the starting Vertex. The RCM algorithm outlined above is then called
//! with this starting Vertex.
const XC::ID &XC::RCM::number(Graph &theGraph, const ID &startVertices)
  {

    // see if we can do quick return
    if(!checkSize(theGraph)) 
      return theRefResult;

    // determine one that gives the min avg profile            
    int minStartVertexTag =0;
    int minAvgProfile= 0;
    const int startVerticesSize= startVertices.Size();
    Vertex *vertexPtr;
    
    int startVertexTag =0;
    const int numVertex= getNumVertex();
    
    for(int i=0; i<startVerticesSize; i++)
      {
        // we first set the Tmp of all vertices to -1, indicating
        // they have not yet been added.
        VertexIter &vertexIter= theGraph.getVertices();
    
        while((vertexPtr= vertexIter()) != 0)
            vertexPtr->setTmp(-1);

        startVertexTag= startVertices(i);

        if(startVertexTag != -1) {
            vertexPtr= theGraph.getVertexPtr(startVertexTag);
            if(vertexPtr == 0) {
                std::cerr << "WARNING:  XC::RCM::number - No vertex with tag ";
                std::cerr << startVertexTag << "Exists - using first come from iter\n";
                startVertexTag= -1;
            }
        }        

        int currentMark= numVertex-1;  // marks current vertex visiting.
        int nextMark= currentMark -1;  // indiactes where to put next Tag in XC::ID.
        theRefResult(currentMark)= vertexPtr->getTag();
        vertexPtr->setTmp(currentMark);
        int avgProfile= 0;
        VertexIter &vertexIter2= theGraph.getVertices();    

        // we continue till the XC::ID is full

        while(nextMark >= 0)
          {
            // get the current vertex and its adjacency
            vertexPtr= theGraph.getVertexPtr(theRefResult(currentMark));
            const std::set<int> &adjacency= vertexPtr->getAdjacency();
 
            // go through the vertices adjacency and add vertices which
            // have not yet been Tmp'ed to the (theRefResult)

            for(std::set<int>::const_iterator i= adjacency.begin(); i!= adjacency.end(); i++)
              {            
                const int vertexTag= *i;
                vertexPtr= theGraph.getVertexPtr(vertexTag);
                if((vertexPtr->getTmp()) == -1) {
                    vertexPtr->setTmp(nextMark);
                    avgProfile += (currentMark - nextMark);
                    theRefResult(nextMark--)= vertexTag;
                }
            }

            // go to the next vertex
            //  we decrement because we are doing reverse Cuthill-McKee
        
            currentMark--;

            // check to see if graph is disconnected
        
            if((currentMark == nextMark) && (currentMark >= 0)) {
            
                // loop over iter till we get a vertex not yet Tmped
            
                while(((vertexPtr= vertexIter2()) != 0) && 
                       (vertexPtr->getTmp() != -1)) 
                    ;
                nextMark--;
        
                vertexPtr->setTmp(currentMark);
                theRefResult(currentMark)= vertexPtr->getTag();
            }

        }                

        if(i == 0 || minAvgProfile > avgProfile) {
            minStartVertexTag= startVertexTag;
            minAvgProfile= avgProfile;
        }
        
    }

        
    // we number based on minStartVertexTag
    minAvgProfile= 0;

    if(minStartVertexTag != startVertexTag) {

        // we could just call the other numbering routine - 
        // we will    just write it out again for now - CHANGE LATER
        startVertexTag= minStartVertexTag;

        // set all unmarked
        VertexIter &vertexIter= theGraph.getVertices();
        while((vertexPtr= vertexIter()) != 0)
            vertexPtr->setTmp(-1);
        
        vertexPtr= theGraph.getVertexPtr(startVertexTag);

        int currentMark= numVertex-1;  // marks current vertex visiting.
        int nextMark= currentMark -1;  // indiactes where to put next Tag in XC::ID.
        theRefResult(currentMark)= vertexPtr->getTag();
        vertexPtr->setTmp(currentMark);
        VertexIter &vertexIter2= theGraph.getVertices();    

        // we continue till the XC::ID is full

        while(nextMark >= 0)
          {
            // get the current vertex and its adjacency        
            vertexPtr= theGraph.getVertexPtr(theRefResult(currentMark));
            const std::set<int> &adjacency= vertexPtr->getAdjacency();

            // go through the vertices adjacency and add vertices which
            // have not yet been Tmp'ed to the (theRefResult)

            for(std::set<int>::const_iterator i= adjacency.begin(); i!= adjacency.end(); i++)
              {            
                const int vertexTag= *i;
                vertexPtr= theGraph.getVertexPtr(vertexTag);
                if((vertexPtr->getTmp()) == -1)
                  {
                    vertexPtr->setTmp(nextMark);
                    minAvgProfile += (currentMark-nextMark);
                    theRefResult(nextMark--)= vertexTag;
                  }
              }

            // go to the next vertex
            //  we decrement because we are doing reverse Cuthill-McKee
        
            currentMark--;

            // loop over iter till we get a vertex not yet Tmped
            if((currentMark == nextMark) && (currentMark >= 0))
              {
                // std::cerr << "WARNING:  XC::RCM::number - Disconnected graph\n";
                
                while(((vertexPtr= vertexIter2()) != 0) && 
                       (vertexPtr->getTmp() != -1)) 
                  ;
              
                nextMark--;
                vertexPtr->setTmp(currentMark);
                theRefResult(currentMark)= vertexPtr->getTag();
              }
          }            
      }        

    // now set the vertex references instead of the vertex tags
    // in the result, we change the Tmp to indicate number and return
    for(int j=0; j<numVertex; j++)
      {
        int vertexTag= theRefResult(j);
        vertexPtr= theGraph.getVertexPtr(vertexTag);
        vertexPtr->setTmp(j+1); // 1 through numVertex
        theRefResult(j)= vertexPtr->getTag();
      }        
    return theRefResult;
  }









