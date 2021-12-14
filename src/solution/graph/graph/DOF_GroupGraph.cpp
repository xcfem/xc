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
// $Date: 2005/11/03 23:11:55 $
// $Source: /usr/local/cvs/OpenSees/SRC/graph/graph/DOF_GroupGraph.cpp,v $
                                                                        
                                                                        
// Written: fmk 
// Revision: A
//
// Description: This file contains the class definition for XC::DOF_GroupGraph.
// DOF_GroupGraph is a graph of the DOF_Groups in the domain. It is used by the
// DOF_Numberer to assign equation numbers to the DOFs.
//
// What: "@(#) DOF_GroupGraph.C, revA"


#include <solution/graph/graph/DOF_GroupGraph.h>
#include <solution/graph/graph/Vertex.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <solution/analysis/model/fe_ele/FE_Element.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include <solution/analysis/model/FE_EleIter.h>
#include <solution/analysis/model/DOF_GrpIter.h>

//! @brief Constructor.
//!
//! The constructor is responsible for constructing the graph given {\em
//! theModel}. It creates the vertices of the graph, one for every
//! equation  (each DOF that has not been constrained out by the
//! constraint handler)  in the model and adds all edges based on the
//! FE\_Element connectivity. For this reason the model must be fully
//! populated with the DOF\_Group and FE\_Element objects before
// the constructor is called.
XC::DOF_GroupGraph::DOF_GroupGraph(const AnalysisModel &theModel)
  :ModelGraph(theModel.getNumDOF_Groups()+START_VERTEX_NUM,theModel)
  {
    assert(myModel);
    const int numVertex= myModel->getNumDOF_Groups();

    if(numVertex>0)
      {
	
        const DOF_Group *dofGroupPtr= nullptr;

        // now create the vertices with a reference equal to the DOF_Group number.
        // and a tag which ranges from 0 through numVertex-1
        DOF_GrpConstIter &dofIter2 = myModel->getConstDOFs();
        while((dofGroupPtr = dofIter2()) != 0)
          {
	    const int DOF_GroupTag = dofGroupPtr->getTag();
            const int DOF_GroupNodeTag = dofGroupPtr->getNodeTag();
	    const int numDOF = dofGroupPtr->getNumFreeDOF();
            Vertex vrt(DOF_GroupTag, DOF_GroupNodeTag, 0, numDOF);
            this->addVertex(vrt);
          }


        // now add the edges, by looping over the Elements, getting their
        // IDs and adding edges between DOFs for equation numbers >= START_EQN_NUM
    
        const FE_Element *elePtr= nullptr;
        FE_EleConstIter &eleIter = myModel->getConstFEs();
        while((elePtr = eleIter()) != 0)
          {
	    const ID &id= elePtr->getDOFtags();
	    const int size = id.Size();
            for(int i=0; i<size; i++)
              {
	        int dof1 = id(i);
	        for(int j=0; j<size; j++)
                  if(i != j)
                    {
		      const int dof2 = id(j);
		      this->addEdge(dof1,dof2);
		    }
	      }
          }
      }
  }


