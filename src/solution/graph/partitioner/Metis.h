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
//! @brief Metis is a type of GraphPartitioner which uses 'METIS - Unstructured
//! Graph Partitioning And Sparse Matrix Ordering System', developed by
//! G. Karypis and V. Kumar at the University of Minnesota. The metis
//! files are found in metis-2.0 which were downloaded.
class Metis : public GraphPartitioner, public GraphNumberer
  {
  private:
    bool checkOptions(void);
    
    int myPtype ; 	// package type: 
                        //	pmetis = 1
                        //	kmetis = 2

    int myMtype;     	// type of matching scheme: 
			//	random = 1
			//  	heavy edge = 2
			//    	light edge = 3
			//	heavy clique = 4
			//	modified heavy edge = 5
			//    	sorted random = 11
			//	sorted heavy edge =21
			// 	sorted modified heavy edge = 51
   
   int myCoarsenTo; 	// the number of vertices the graph is coarsened down to
                        //   	if pmetis default is 100
			//	if kmetis default is 2000
	
    int myRtype;     	// type of refinement policy:  
			//	greedy = 1
			//	kernighan-lin = 2
			//    	combo greedy and K-L = 3
			// 	boundary greedy = 11
			//    	boundary K-L = 12
			//	combo of boundary greedy and boundary K-L = 13,
			//    	no-refinement = 20

    int myIPtype; 	// type of bisection algo:  
	                //    	graph growing partition = 1,
			//    	greedy graph growing partition = 2,
			//    	spectral bisection = 3,
			//    	graph growing followed by K-L = 4
	
    bool defaultOptions;			    
    
    int   numPartitions; // needed if to be used as a numberer
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

    // the follwing methods are if the object is to be used as a numberer
    const ID &number(Graph &theGraph, int lastVertex = -1);
    const ID &number(Graph &theGraph, const ID &lastVertices);

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
  };
} // fin namespace XC

#endif

