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
                                                                        
// $Revision: 1.1.1.1 $
// $Date: 2000/09/15 08:23:21 $
// $Source: /usr/local/cvs/OpenSees/SRC/graph/numberer/MetisNumberer.h,v $
                                                                        
                                                                        
// File: ~/graph/partitioner/Metis.h
// 
// Written: fmk 
// Created: Sun Sept 15 11:47:47: 1996
// Revision: A
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

#include <GraphPartitioner.h>

namespace XC {
//! @ingroup Graph
//
//! @brief type of GraphPartitioner which uses 'METIS - Unstructured
//! Graph Partitioning And Sparse Matrix Ordering System', developed by
//! G. Karypis and V. Kumar at the University of Minnesota. The metis
//! files are found in metis-2.0 which were downloaded.
class Metis: public GraphPartitioner
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
  protected:	
    friend class DOF_Numberer;
    Metis(void);
    Metis(int Ptype, int Mtype, int coarsenTo, int Rtype, int IPtype);
    GraphNumberer *getCopy(void) const;
  public:
    bool setOptions(int Ptype, 
		    int Mtype,
		    int coarsenTo,
		    int Rtype, 
		    int IPtype);

    bool setDefaultOptions(void);
    
    int partition(Graph &theGraph, int numPart);
  };
} // end of XC namespace

#endif

