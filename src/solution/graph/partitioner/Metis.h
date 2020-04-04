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
// $Date: 2006/01/12 23:37:19 $
// $Source: /usr/local/cvs/OpenSees/SRC/graph/partitioner/Metis.h,v $
                                                                        
                                                                        
// Written: fmk 
//
// Description: This file contains the class definition for Metis.
// Metis is a type of GraphPartitioner which uses 'METIS - Unstructured
// Graph Partitioning And Sparse Matrix Ordering System', developed by
// G. Karypis and V. Kumar at the University of Minnesota. The metis
// files are found in metis-2.0 which were downloaded.
//     This class provides the C++ interface for metis which will allow
// it to fit seamlessly into our system.
//
// What: "@(#) Metis.h, revA"

#ifndef Metis_h
#define Metis_h

#include "solution/graph/partitioner/GraphPartitioner.h"
#include "solution/graph/numberer/GraphNumberer.h"

namespace XC {
//! @ingroup Graph
//
//! @brief Metis is a type of GraphPartitioner which uses METIS library.
//!
//! The Metis graph partitioner calls procedures defined in the METIS
//! library to partition the graph. METIS is currently being developed by
//! G.~Karypis and V.~Kumar at the University of Minnesota. At the present
//! time the Graph to be partitioned MUST have the vertices
//! labeled \f$0\f$ through \f$numVertex-1\f$.

//! The METIS library uses two integer arrays to represent the graph, {\em
//! xadj} and \p adjncy. \f$xadj(i)\f$ stores the location in \p adjncy
//! of the start of the \f$i\f$'th Vertices adjacent Vertices. \p adjncy
//! contains the tags of all the adjacent vertices. For example, the graph
//! which is represented by the following matrix \f$A\f$:
//! 'METIS - Unstructured Graph Partitioning And Sparse Matrix Ordering
//! System', developed by G. Karypis and V. Kumar at the University of
//! Minnesota. The metis files are found in metis-2.0 which were downloaded.
//!
// \f[
// \begin{displaymath}
// A = \left[
//   \begin{array}{ccccc}
//   1 & 0 & 1 & 1 & 0
//   1 & 1 & 0 & 0 & 0
//   0 & 1 & 1 & 0 & 0
//   0 & 0 & 0 & 1 & 1
//   1 & 1 & 0 & 0 & 1
//   \end{array}
// \right] 
// \end{displaymath}
// \f]
// 
//  is represented by:
// 
// \f[
// \begin{displaymath}
// \f$\f$
//  xadj =
// \left[
// \begin{array}{cccccccccccccc}
// 0 & 2 & 3 & 4 & 5 & 7
// \end{array}
// \right] 
// \f$\f$
// \end{displaymath}
// \f]
// 
//  and
// 
// \f[
// \begin{displaymath}
// \f$\f$
//  adjncy =
// \left[
// \begin{array}{cccccccccccccc}
// 2 & 3 & 0 & 1 & 4 & 0 & 1
// \end{array}
// \right] 
// \f$\f$
// \end{displaymath}
// \f]
//
// \noindent note that there is no space allocated for the diagonal
// components.
class Metis: public GraphPartitioner, public GraphNumberer
  {
  private:
    bool checkOptions(void);
    
    int myPtype; 	//!< package type: 
                        //	pmetis = 1
                        //	kmetis = 2

    int myMtype;     	//!< type of matching scheme: 
			//	random = 1
			//  	heavy edge = 2
			//    	light edge = 3
			//	heavy clique = 4
			//	modified heavy edge = 5
			//    	sorted random = 11
			//	sorted heavy edge =21
			// 	sorted modified heavy edge = 51
   
   int myCoarsenTo; 	//!< the number of vertices the graph is coarsened down to
                        //   	if pmetis default is 100
			//	if kmetis default is 2000
	
    int myRtype;     	//!< type of refinement policy:  
			//	greedy = 1
			//	kernighan-lin = 2
			//    	combo greedy and K-L = 3
			// 	boundary greedy = 11
			//    	boundary K-L = 12
			//	combo of boundary greedy and boundary K-L = 13,
			//    	no-refinement = 20

    int myIPtype; 	//!< type of bisection algo:  
	                //    	graph growing partition = 1,
			//    	greedy graph growing partition = 2,
			//    	spectral bisection = 3,
			//    	graph growing followed by K-L = 4
	
    bool defaultOptions;			    
    
    int numPartitions; //!< needed if to be used as a numberer
    ID theRefResult;

    Metis(int numParts =1);
    Metis(int Ptype, 
	  int Mtype, 
	  int coarsenTo,
	  int Rtype, 
	  int IPtype,
	  int numParts =1);
  public:
    bool setOptions(int Ptype, 
		    int Mtype,
		    int coarsenTo,
		    int Rtype, 
		    int IPtype);
    bool setDefaultOptions(void);
    
    int partitionHexMesh(int* elmnts, int* epart, int* npart, int ne, int nn, int nparts, bool whichToUse);
    int partition(Graph &theGraph, int numPart);
    int partitionGraph(int *nvtxs, int *xadj, int *adjncy, int *vwgt, 
		       int *adjwgt, int *wgtflag, int *numflag, int *nparts, 
		       int *options, int *edgecut, int *part, bool whichToUse);

    // the following methods are if the object is to be used as a numberer
    const ID &number(Graph &theGraph, int lastVertex = -1);
    const ID &number(Graph &theGraph, const ID &lastVertices);

    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);
  };
} // end of XC namespace

#endif

