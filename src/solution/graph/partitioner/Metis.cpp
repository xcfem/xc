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


XC::Metis::Metis(int numParts) 
:GraphNumberer(GraphNUMBERER_TAG_Metis),
 myPtype(0), myMtype(0), myCoarsenTo(0), myRtype(0), myIPtype(0),
 defaultOptions(true), numPartitions(numParts), theRefResult(0)
 {}

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

    
bool
XC::Metis::setOptions(int Ptype, 
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


// bool checkOptions(void) const
//	returns true if options are o.k., false otherwise

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
	std::cerr << "WARNING: XC::Metis::partition ";
	std::cerr << " - Illegal Ptype " << myPtype << std::endl;
      }
	
    if((myMtype != 1) ||  (myMtype != 2) || (myMtype != 3) || 
	((myMtype != 4) || (myMtype != 5) || myMtype != 11) || 
	(myMtype != 21) || (myMtype != 51))
      {
	okFlag = false;
	std::cerr << "WARNING: Metis::partition ";
	std::cerr << " - Illegal Mtype " << myMtype << std::endl;
      }

    if(myPtype == 1)
      {
	if ((myRtype != 1) || (myRtype != 2) || (myRtype != 3) ||
	    (myRtype != 11) || (myRtype != 12) || (myRtype != 13) ||
	    (myRtype != 20))
          {
	    okFlag = false;
	    std::cerr << "WARNING: Metis::partition ";
	    std::cerr << " - Illegal Rtype " << myRtype << std::endl;
	    std::cerr << " for Ptype " << myPtype << std::endl;	    
	  }
      }
    else if(myPtype == 2)
      {
	if((myRtype != 11) || (myRtype != 12) || (myRtype != 20))
          {
	    okFlag = false;
		std::cerr << "WARNING: Metis::partition ";
		std::cerr << " - Illegal Rtype " << myRtype << std::endl;
		std::cerr << " for Ptype " << myPtype << std::endl;	    
	      }
      }
    if((myIPtype != 1) || (myIPtype != 2) || (myIPtype != 3) || (myIPtype != 4))
      {
	okFlag = false;
	std::cerr << "WARNING: XC::Metis::partition ";
	std::cerr << " - Illegal IPtype " << myIPtype << std::endl;
      }
	
    if(myCoarsenTo < 0)
      {
	okFlag = false;
	std::cerr << "WARNING: XC::Metis::partition ";
	std::cerr << " - Illegal coarsen To " << myCoarsenTo << std::endl;
      }	    

    if(okFlag == false)
      defaultOptions = true;
    return okFlag;
  }


bool XC::Metis::setDefaultOptions(void)
  {
    defaultOptions = true;
    return true;
  }


// int partition(Graph &theGraph, int numPart)
//	Method to partition the graph. It first creates the arrays needed
//	by the metis lib and then invokes a function from the metis lib to
//	partition the graph. The solors of the vertices of the graph are
//	set to colors 0 through numPart-1 to indicate which partition the
//	vrtices are in. Returns -1 if options are not set, -2 if metis failed.

int XC::Metis::partition(Graph &theGraph, int numPart)
  {
    // first we check that the options are valid
    if (checkOptions() == false)
	return -1;
    
    // now we get room for the data structures metis needs

    int numVertex = theGraph.getNumVertex();
    int numEdge = theGraph.getNumEdge();
    //    std::cerr << " Metis::partition --- numVertex: " << numVertex << " numEdge: "<<  numEdge << "\n";
    int *options = new int [5];
    int *partition = new int [numVertex+1];    
    int *xadj = new int [numVertex+2];
    int *adjncy = new int [2*numEdge];
    int *vwgts = 0;
    int *ewgts = 0;
    int numbering = 0;
    int weightflag = 0; // no weights on our graphs yet

    if (START_VERTEX_NUM == 0)
	numbering = 0;	
    else if (START_VERTEX_NUM == 1)
	numbering = 1;
    else {
	std::cerr << "WARNING Metis::partition - No partitioning done";
	std::cerr << " vertex numbering must start at 0 or 1\n";
	return (-2);
    }
    int edgecut;
    
    if ((options == 0) || (partition == 0) || (xadj == 0) || (adjncy == 0)) {
	std::cerr << "WARNING Metis::partition - No partitioning done";
	std::cerr << " as ran out of memory\n";
	return (-2);
    }


    // we build these data structures
    
    int indexEdge = 0;
    xadj[0] = 0;

    Vertex *vertexPtr;
    for (int vertex =0; vertex<numVertex; vertex++) {
	vertexPtr = theGraph.getVertexPtr(vertex+START_VERTEX_NUM);
	
	// check we don't have an invalid vertex numbering scheme
	// if so WARNING message, clean up and return -2

	if (vertexPtr == 0) {
	    std::cerr << "WARNING Metis::partition - No partitioning done";
	    std::cerr << " Metis requires consequtive Vertex Numbering\n";
	    
	    delete [] options;
	    delete [] partition;
	    delete [] xadj;
	    delete [] adjncy;
	    
	    return -2;
	}
	
	const std::set<int> &adjacency = vertexPtr->getAdjacency();
	for(std::set<int>::const_iterator i=adjacency.begin(); i!=adjacency.end(); i++)
          adjncy[indexEdge++]= *i-START_VERTEX_NUM;
	
	xadj[vertex+1] = indexEdge;
    }
    
    
    if (defaultOptions == true) 
	options[0] = 0;
    else {
	options[0] =1;
	options[1] = myCoarsenTo;
	options[2] = myMtype;
	options[3] = myIPtype;
	options[4] = myRtype;
    }
    
    
    // we now the metis routines
    //

    
    if (myPtype == 1) {
      //std::cerr << " Metis::partition PartGraphRecursive \n";
      METIS_PartGraphRecursive(&numVertex, xadj, adjncy, vwgts, ewgts, &weightflag,&numbering, &numPart,options, &edgecut, partition);
    }
    else {		
      //std::cerr << " Metis::partition PartGraphKway w/ \n";
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
      METIS_PartGraphKway(&numVertex, xadj, adjncy, vwgts, ewgts, &weightflag, &numbering, &numPart,options, &edgecut, partition);
    }
    //
    //std::cerr << " Metis::partition returned ok \n";
    
    // we set the vertex colors to correspond to the partitioned scheme
    for (int vert =0; vert<numVertex; vert++) {
	vertexPtr = theGraph.getVertexPtr(vert+START_VERTEX_NUM);
	vertexPtr->setColor(partition[vert]+1); // start colors at 1
    }

    // clean up the space and return
    
    delete [] options;
    delete [] partition;
    delete [] xadj;
    delete [] adjncy;
    
    return 0;
  }
int XC::Metis::partitionGraph(int *nvtxs, int *xadj, int *adjncy, int *vwgt, 
		      int *adjwgt, int *wgtflag, int *numflag, int *nparts, 
		      int *options, int *edgecut, int *part, bool whichToUse)
{
  // which to use -> if true use edge cut partitinioning, else uses communication
  // based partition, standard aplications , uniformly mesh regions need use the edge-cut
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
    // based partition, standard aplications , uniformly mesh regions need use the edge-cut
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

    if (checkOptions() == false) {
      std::cerr << "ERROR:  Metis::number - chek options failed\n";
      return theRefResult;
    }
    
    // now we get room for the data structures metis needs
    int numEdge = theGraph.getNumEdge();

    int *options = new int [5];
    int *partition = new int [numVertex+1];    
    int *xadj = new int [numVertex+2];
    int *adjncy = new int [2*numEdge];
    int *vwgts = 0;
    int *ewgts = 0;
    int numbering = 0;
    int weightflag = 0; // no weights on our graphs yet

    if (START_VERTEX_NUM == 0)
	numbering = 0;	
    else if (START_VERTEX_NUM == 1)
	numbering = 1;
    else {
	std::cerr << "WARNING Metis::partition - No partitioning done";
	std::cerr << " vertex numbering must start at 0 or 1\n";
	return theRefResult;
    }
    int edgecut;
    
    if ((options == 0) || (partition == 0) || (xadj == 0) || (adjncy == 0)) {
	std::cerr << "WARNING Metis::partition - No partitioning done";
	std::cerr << " as ran out of memory\n";
	return theRefResult;
    }


    // we build these data structures
    
    int indexEdge = 0;
    xadj[0] = 0;

    Vertex *vertexPtr;
    for (int vertex =0; vertex<numVertex; vertex++) {
	vertexPtr = theGraph.getVertexPtr(vertex+START_VERTEX_NUM);
	
	// check we don't have an invalid vertex numbering scheme
	// if so WARNING message, clean up and return -2

	if (vertexPtr == 0) {
	    std::cerr << "WARNING Metis::partition - No partitioning done";
	    std::cerr << " Metis requires consequtive Vertex Numbering\n";
	    
	    delete [] options;
	    delete [] partition;
	    delete [] xadj;
	    delete [] adjncy;
	    
	    return theRefResult;
	}
	
	const std::set<int> &adjacency= vertexPtr->getAdjacency();
	for(std::set<int>::const_iterator i=adjacency.begin(); i!=adjacency.end(); i++)
          adjncy[indexEdge++] = *i-START_VERTEX_NUM;
	xadj[vertex+1] = indexEdge;
    }


    if (defaultOptions == true) 
	options[0] = 0;
    else {
	options[0] =1;
	options[1] = myCoarsenTo;
	options[2] = myMtype;
	options[3] = myIPtype;
	options[4] = myRtype;
    }
    
    
    // we now the metis routines
    //
    if (myPtype == 1) 

      METIS_PartGraphRecursive(&numVertex, xadj, adjncy, vwgts, ewgts, &weightflag,&numbering, &numPartitions, options, &edgecut, partition);
    else		
      METIS_PartGraphKway(&numVertex, xadj, adjncy, vwgts, ewgts, &weightflag, 
	     &numbering, &numPartitions, options, &edgecut, partition);
    //
    /*
    if (myPtype == 1)

      PMETIS(&numVertex, xadj, adjncy, vwgts, ewgts, &weightflag, 
	     &numPartitions, options, &numbering, &edgecut, partition);
    else		
      KMETIS(&numVertex, xadj, adjncy, vwgts, ewgts, &weightflag, 
	     &numPartitions, options, &numbering, &edgecut, partition);
    */
std::cerr << "Metis::number -2\n";
    // we assign numbers now based on the partitions returned.
    // each vertex in partion i is assigned a number less than
    // thos in partion i+1: NOTE WE DON'T CARE WHAT THE NUMBERING IS
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
std::cerr << "Metis::number -3\n";
    // clean up the space and return
    delete [] options;
    delete [] partition;
    delete [] xadj;
    delete [] adjncy;
std::cerr << "Metis::number -4\n";    
    return theRefResult;
  }


const XC::ID &XC::Metis::number(Graph &theGraph, const XC::ID &lastVertices)
  { return this->number(theGraph); }

int XC::Metis::sendSelf(CommParameters &cp)
  { return 0; }

int XC::Metis::recvSelf(const CommParameters &cp)
  { return 0; }

