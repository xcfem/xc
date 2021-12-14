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
// $Date: 2006/01/12 23:37:19 $
// $Source: /usr/local/cvs/OpenSees/SRC/graph/partitioner/Metis.cpp,v $
                                                                        
                                                                        
// Written: fmk 
// Created: Sun Sept 15 11:47:47: 1996
// Revision: A
//
// Description: This file contains the class definition for XC::Metis.
// Metis is a type of GraphPartitioner which uses 'METIS - Unstructured
// Graph Partitioning And Sparse XC::Matrix Ordering System', developed by
// G. Karypis and V. Kumar at the University of Minnesota. The metis
// files are found in metis-2.0 which were downloaded.
//     This class provides the C++ interface for metis which will allow
// it to fit seamlessly into our system.
//
// What: "@(#) Metis.C, revA"

#include <solution/graph/partitioner/Metis.h>
#include "solution/graph/graph/Graph.h"
#include <solution/graph/graph/Vertex.h>
#include <solution/graph/graph/VertexIter.h>

/* stuff needed to get the program working on the clump & NOW machines*/


//int IsWeighted;
extern "C" 
int METIS_PartGraphRecursive(int *, int *, int *, int *, int *, int *, int *, int *, int *, int *, int *);

extern "C" 
int METIS_PartGraphKway(int *, int *, int *, int *, int *, int *, int *, int *, int *, int *, int *);

extern "C"
int METIS_PartMeshDual(int *, int *, int *, int *, int *, int *, int *, int *, int *);
//int METIS_PartMeshDual(&ne, &nn, elmnts, &etype, &numflag, &nparts, &edgecut, epart, npart);

extern "C"
int METIS_PartMeshNodal(int *, int *, int *, int *, int *, int *, int *, int *, int *);
//int METIS_PartMeshNodal(&ne, &nn, elmnts, &etype, &numflag, &nparts, &edgecut, epart, npart);

//! To construct a Metis object which will use the default settings when
//! partitioning. 
XC::Metis::Metis(int numParts) 
:GraphNumberer(GraphNUMBERER_TAG_Metis),
 myPtype(0), myMtype(0), myCoarsenTo(0), myRtype(0), myIPtype(0),
 defaultOptions(true), numPartitions(numParts), theRefResult(0)
 {}

//! To construct a Metis object which will use the setting passed into the
//! constructor as options to metis's PMETIS() routine. {\em
//! checkOptions()} is invoked to ensure the settings are valid.
XC::Metis::Metis(int Ptype, 
	  int Mtype, 
	  int coarsenTo,
	  int Rtype, 
      	  int IPtype,
	  int numParts)
:GraphNumberer(GraphNUMBERER_TAG_Metis),
 myPtype(Ptype), myMtype(Mtype), myCoarsenTo(coarsenTo), myRtype(Rtype), 
 myIPtype(IPtype), defaultOptions(false), numPartitions(numParts), theRefResult(0)
  {
    // check the options are valid
    checkOptions();
  }

    
//! Sets the options for the partitioning to those passed as
//! arguments. Then invokes checkOptions() to see if the options are
//! valid. HOW ABOUT REFERRINGR TO MANUAL TO SEE WHAT OPTIONS MEAN.
bool XC::Metis::setOptions(int Ptype, 
		  int Mtype,
		  int coarsenTo,
		  int Rtype, 
		  int IPtype)
  {
    myPtype=Ptype; 
    myMtype = Mtype; 
    myCoarsenTo = coarsenTo; 
    myRtype = Rtype; 
    myIPtype = IPtype; 

    defaultOptions = false;
    
    return checkOptions();
  }    


//! @brief returns true if options are o.k., false otherwise
//!
//! If options are not valid sets the default options. EXPAND ON VALID
//! OPTIONS OR REFER TO METIS MANUAL.
bool XC::Metis::checkOptions(void) 
  {

    // check default not set
    if(defaultOptions == true)
      return true;

    
    // otherwise check all options for valid value
    bool okFlag = true;    
    
    if((myPtype != 1) || (myPtype != 2))
      {
	okFlag = false;
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING: illegal Ptype "
		  << myPtype << std::endl;
      }
	
    if((myMtype != 1) ||  (myMtype != 2) || (myMtype != 3) || 
	((myMtype != 4) || (myMtype != 5) || myMtype != 11) || 
	(myMtype != 21) || (myMtype != 51))
      {
	okFlag = false;
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING: illegal Mtype "
		  << myMtype << std::endl;
      }

    if(myPtype == 1)
      {
	if ((myRtype != 1) || (myRtype != 2) || (myRtype != 3) ||
	    (myRtype != 11) || (myRtype != 12) || (myRtype != 13) ||
	    (myRtype != 20))
          {
	    okFlag = false;
	    std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; WARNING: illegal Rtype "
		      << myRtype << " for Ptype "
		      << myPtype << std::endl;	    
	  }
      }
    else if(myPtype == 2)
      {
	if((myRtype != 11) || (myRtype != 12) || (myRtype != 20))
          {
	    okFlag = false;
		std::cerr << getClassName() << "::" << __FUNCTION__
			  << "; WARNING: illegal Rtype " << myRtype
			  << " for Ptype " << myPtype << std::endl;	    
	      }
      }
    if((myIPtype != 1) || (myIPtype != 2) || (myIPtype != 3) || (myIPtype != 4))
      {
	okFlag = false;
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING: illegal IPtype "
		  << myIPtype << std::endl;
      }
	
    if(myCoarsenTo < 0)
      {
	okFlag = false;
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING: illegal coarsen To "
		  << myCoarsenTo << std::endl;
      }	    

    if(okFlag == false)
      defaultOptions = true;
    return okFlag;
  }


//! @brief Sets the default options.
bool XC::Metis::setDefaultOptions(void)
  {
    defaultOptions = true;
    return true;
  }


//! @brief Method that performs the graph partitioning.
//!
//! This is the method invoked to partition the graph into \p numPart
//! partitions. On completion of the routine each vertex will be assigned
//! a color \f$1\f$ through \p numPart, the color assigned indicating the
//! partition to which the vertex belongs. 
//! 
//! To partition a number of integer arrays are created, {\em options[5]},
//! {\em partition[numVertex+1]}, {\em xadj[numVertex+1]} and {\em
//! adjncy[2*numEdge]} (CURRENTLY ASSUMING GRAPH IS SYMMETRIC - THIS MAY
//! CHANGE \& xadj and partition 1 LARGER THAN REQUIRED). If not enough
//! memory is available for the arrays, a warning message is printed and
//! \f$-2\f$ is returned. The data for \p xadj and \p adjncy are
//! determined from the Vertices of the Graph by iterating over each
//! Vertex from \f$0\f$ through \p numVertex \f$-1\f$. If default options are
//! specified {\em options[0]} is set to \f$0\f$, otherwise \f$1\f$ with {\em
//! options[1:4] = coarsenTo, mType, ipType, rType}. if \p pType equals
//! \f$1\f$ \p PMETIS is called, otherwise \p KMETIS is called. Both are
//! called with the following arguments: {\em numVertex, xadj,adjncy, 0,
//! 0, \&weightFlag, options, numPart, \&numbering, \&edgecut, partition} 
//! The colors of the partitions are then set equal to the color indicated
//! in \p partition.  The integer arrays are destroyed and \f$0\f$
//! returned.
int XC::Metis::partition(Graph &theGraph, int numPart)
  {
    // first we check that the options are valid
    if (checkOptions() == false)
	return -1;
    
    // now we get room for the data structures metis needs

    int numVertex = theGraph.getNumVertex();
    int numEdge = theGraph.getNumEdge();
    //    std::cerr << getClassName() << "::" << __FUNCTION__ << "--- numVertex: " << numVertex << " numEdge: "<<  numEdge << "\n";
    std::vector<int> options(5,0);
    std::vector<int> partition(numVertex+1,0);    
    std::vector<int> xadj(numVertex+2,0);
    std::vector<int> adjncy(2*numEdge,0);
    int *vwgts= nullptr;
    int *ewgts= nullptr;
    int numbering = 0;
    int weightflag = 0; // no weights on our graphs yet

    if (START_VERTEX_NUM == 0)
	numbering = 0;	
    else if (START_VERTEX_NUM == 1)
	numbering = 1;
    else
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "WARNING; No partitioning done"
		  << " vertex numbering must start at 0 or 1\n";
	return (-2);
      }
    int edgecut;
    
    // we build these data structures
    
    int indexEdge = 0;
    xadj[0]= 0;

    Vertex *vertexPtr;
    for (int vertex =0; vertex<numVertex; vertex++) {
	vertexPtr = theGraph.getVertexPtr(vertex+START_VERTEX_NUM);
	
	// check we don't have an invalid vertex numbering scheme
	// if so WARNING message, clean up and return -2

	if(vertexPtr== nullptr)
	  {
	    std::cerr << getClassName() << "::" << __FUNCTION__
		      << "WARNING; No partitioning done"
		      << " Metis requires consecutive Vertex Numbering\n";    
	    return -2;
	  }
	
	const std::set<int> &adjacency = vertexPtr->getAdjacency();
	for(std::set<int>::const_iterator i=adjacency.begin(); i!=adjacency.end(); i++)
          adjncy[indexEdge++]= *i-START_VERTEX_NUM;
	
	xadj[vertex+1] = indexEdge;
    }
    
    
    if(defaultOptions == true) 
	options[0]= 0;
    else
      {
	options[0]=  1;
	options[1]= myCoarsenTo;
	options[2]= myMtype;
	options[3]= myIPtype;
	options[4]= myRtype;
      }
    
    
    // we now the metis routines
    //

    
    if (myPtype == 1)
      {
        //std::cerr << getClassName() << "::" << __FUNCTION__ << " PartGraphRecursive \n";
        METIS_PartGraphRecursive(&numVertex, &xadj[0], &adjncy[0], vwgts, ewgts, &weightflag,&numbering, &numPart, &options[0], &edgecut, &partition[0]);
      }
    else
      {		
        //std::cerr << getClassName() << "::" << __FUNCTION__ << "  PartGraphKway w/ \n";
	/*
	for (int i=0 ; i<numVertex; i++){
	  std::cerr << " list of node :" << i ;
	  std::cerr << " contains:  " ;
	  for (int j=xadj[i]; j<=xadj[i+1]-1; j++){
	    std::cerr << adjncy[j] << " " ;
	  }
	  std::cerr << "\n";
	}
	std::cerr << " numpart " << numPart;
	std::cerr << " numbering " << numbering << "\n";
	*/
	METIS_PartGraphKway(&numVertex, &xadj[0], &adjncy[0], vwgts, ewgts, &weightflag, &numbering, &numPart, &options[0], &edgecut, &partition[0]);
      }
    //
    //std::cerr << getClassName() << "::" << __FUNCTION__ << "  returned ok \n";
    
    // we set the vertex colors to correspond to the partitioned scheme
    for(int vert =0; vert<numVertex; vert++)
      {
	vertexPtr = theGraph.getVertexPtr(vert+START_VERTEX_NUM);
	vertexPtr->setColor(partition[vert]+1); // start colors at 1
      }

    //return
    return 0;
  }

int XC::Metis::partitionGraph(int *nvtxs, int *xadj, int *adjncy, int *vwgt, 
		      int *adjwgt, int *wgtflag, int *numflag, int *nparts, 
		      int *options, int *edgecut, int *part, bool whichToUse)
{
  // which to use -> if true use edge cut partitinioning, else uses communication
  // based partition, standard applications , uniformly mesh regions need use the edge-cut
  // first one works fine second is weird
  if (whichToUse) {
    METIS_PartGraphRecursive(nvtxs, xadj, adjncy, vwgt, adjwgt, wgtflag,numflag, nparts,options, edgecut, part);
  }
  else {
    METIS_PartGraphKway(nvtxs, xadj, adjncy, vwgt, adjwgt, wgtflag,numflag, nparts,options, edgecut, part);
  }
  return 0;
}

int XC::Metis::partitionHexMesh(int* elmnts, int* epart, int* npart, int ne, int nn, int nparts, bool whichToUse)
  {
    // which to use -> if true use edge cut partitinioning, else uses communication
    // based partition, standard applications , uniformly mesh regions need use the edge-cut
    // first one works fine second is weird
    int numflag =0;
    int edgecut =0;
    int etype = 3;
  
    if(whichToUse)
      METIS_PartMeshNodal(&ne, &nn, elmnts, &etype, &numflag, &nparts, &edgecut, epart, npart);
    else
      METIS_PartMeshDual(&ne, &nn, elmnts, &etype, &numflag, &nparts, &edgecut, epart, npart);
    return 0;
  }




const XC::ID &XC::Metis::number(Graph &theGraph, int lastVertex)
  {
    // first we check that the options are valid
    // first check our size, if not same make new
    int numVertex = theGraph.getNumVertex();
    // delete the old
	
    theRefResult.resize(numVertex);

    if (checkOptions() == false)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "ERROR: check options failed\n";
        return theRefResult;
      }
    
    // now we get room for the data structures metis needs
    int numEdge = theGraph.getNumEdge();

    std::vector<int> options(5,0);
    std::vector<int> partition(numVertex+1,0);    
    std::vector<int> xadj(numVertex+2,0);
    std::vector<int> adjncy(2*numEdge,0);
    int *vwgts= nullptr;
    int *ewgts= nullptr;
    int numbering = 0;
    int weightflag = 0; // no weights on our graphs yet

    if (START_VERTEX_NUM == 0)
	numbering = 0;	
    else if (START_VERTEX_NUM == 1)
	numbering = 1;
    else
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "WARNING; no partitioning done"
		  << " vertex numbering must start at 0 or 1\n";
	return theRefResult;
      }
    int edgecut;
    
    // we build these data structures
    
    int indexEdge = 0;
    xadj[0] = 0;

    Vertex *vertexPtr;
    for (int vertex =0; vertex<numVertex; vertex++) {
	vertexPtr = theGraph.getVertexPtr(vertex+START_VERTEX_NUM);
	
	// check we don't have an invalid vertex numbering scheme
	// if so WARNING message, clean up and return -2

	if(vertexPtr == nullptr)
	  {
	    std::cerr << getClassName() << "::" << __FUNCTION__
		      << "WARNING; No partitioning done"
		      << " Metis requires consecutive Vertex Numbering\n";
	    
	    return theRefResult;
	  }
	
	const std::set<int> &adjacency= vertexPtr->getAdjacency();
	for(std::set<int>::const_iterator i=adjacency.begin(); i!=adjacency.end(); i++)
          adjncy[indexEdge++] = *i-START_VERTEX_NUM;
	xadj[vertex+1] = indexEdge;
    }


    if (defaultOptions == true) 
	options[0]= 0;
    else
      {
	options[0]= 1;
	options[1]= myCoarsenTo;
	options[2]= myMtype;
	options[3]= myIPtype;
	options[4]= myRtype;
      }
    
    // we now the metis routines
    //
    if (myPtype == 1) 

      METIS_PartGraphRecursive(&numVertex, &xadj[0], &adjncy[0], vwgts, ewgts, &weightflag,&numbering, &numPartitions, &options[0], &edgecut, &partition[0]);
    else		
      METIS_PartGraphKway(&numVertex, &xadj[0], &adjncy[0], vwgts, ewgts, &weightflag, 
	     &numbering, &numPartitions, &options[0], &edgecut, &partition[0]);
    //
    /*
    if (myPtype == 1)

      PMETIS(&numVertex, &xadj[0], &adjncy[0], vwgts, ewgts, &weightflag, 
	     &numPartitions, &options[0], &numbering, &edgecut, &partition[0]);
    else		
      KMETIS(&numVertex, &xadj[0], &adjncy[0], vwgts, ewgts, &weightflag, 
	     &numPartitions, &options[0], &numbering, &edgecut, &partition[0]);
    */
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "number -2\n";
    // we assign numbers now based on the partitions returned.
    // each vertex in partition i is assigned a number less than
    // those in partition i+1: NOTE WE DON'T CARE WHAT THE NUMBERING IS
    // WITHIN A PARTITION
    int count = 0;
    for (int i=0; i<numPartitions; i++) {
      for (int vert=0; vert<numVertex; vert++) {
	if (partition[vert] == i) {
	  vertexPtr = theGraph.getVertexPtr(vert+START_VERTEX_NUM);
	  vertexPtr->setTmp(count+1);
	  theRefResult(count) = vertexPtr->getRef();
	  count++;
	}
      }
    }
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "number -3\n";
    //return
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "number -4\n";    
    return theRefResult;
  }


const XC::ID &XC::Metis::number(Graph &theGraph, const ID &lastVertices)
  { return this->number(theGraph); }

int XC::Metis::sendSelf(Communicator &comm)
  { return 0; }

int XC::Metis::recvSelf(const Communicator &comm)
  { return 0; }

