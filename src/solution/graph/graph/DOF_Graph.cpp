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
                                                                        
// $Revision: 1.5 $
// $Date: 2005/11/14 20:47:11 $
// $Source: /usr/local/cvs/OpenSees/SRC/graph/graph/DOF_Graph.cpp,v $
                                                                        
                                                                        
// File: ~/graph/graph/DOF_Graph.C
// 
// Written: fmk 
// Created: Sun Sept 15 11:47:47: 1996
// Revision: A
//
// Description: This file contains the class definition for XC::DOF_Graph.
// DOF_Graph is a graph of the DOFs in the analysis model. It is used
// by the SysOfEqn to determine its size.
//
// What: "@(#) DOF_Graph.C, revA"


#include <solution/graph/graph/DOF_Graph.h>
#include <solution/graph/graph/Vertex.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include <solution/analysis/model/DOF_GrpIter.h>
#include <solution/analysis/model/fe_ele/FE_Element.h>
#include <solution/analysis/model/FE_EleIter.h>

#define START_EQN_NUM 0

//! @brief Constructor.
//!
//! The constructor is responsible for constructing the graph given {\em
//! theModel}. It creates the vertices of the graph, one for every
//! DOF in the model and adds all edges based on the FE\_Element
//! connectivity. For this reason the model must be fully populated with
//! the DOF\_Group and FE\_Element objects before the constructor is
//! called.
XC::DOF_Graph::DOF_Graph(const AnalysisModel &theModel)
  :ModelGraph(theModel.getNumEqn(),theModel)
  {
    //
    // create a vertex for each dof
    //
    assert(myModel);
    const DOF_Group *dofPtr= nullptr;
    DOF_GrpConstIter &theDOFs= myModel->getConstDOFs();
    while((dofPtr= theDOFs()) != 0)
      {
        const ID &id= dofPtr->getID();
        const int size= id.Size();
        for(int i=0; i<size; i++)
          {
            int dofTag= id(i);
            if(dofTag >= START_EQN_NUM)
              {
	        Vertex *vertexPtr= this->getVertexPtr(dofTag);
	        if(!vertexPtr)
                  {
	            Vertex vrt(dofTag, dofTag);      
	            if(this->addVertex(vrt, false) == false)
                      { std::cerr << "WARNING DOF_Graph::DOF_Graph - error adding vertex\n"; }
	          }
              }
          }
      }

    // now add the edges, by looping over the FE_elements, getting their
    // IDs and adding edges between DOFs for equation numbers >= START_EQN_NUM
  
    const FE_Element *elePtr= nullptr;
    FE_EleConstIter &eleIter= myModel->getConstFEs();
    int cnt= 0;

    while((elePtr= eleIter()) != 0)
      {
        const ID &id= elePtr->getID();
        cnt++;
        const int size= id.Size();
        for(int i=0; i<size; i++)
          {
            const int eqn1= id(i);
	    
            // if eqnNum of DOF is a valid eqn number add an edge
            // to all other DOFs with valid eqn numbers.
            if(eqn1 >=START_EQN_NUM)
              {
	        for(int j=i+1; j<size; j++)
                  {
	            const int eqn2= id(j);
	            if(eqn2 >=START_EQN_NUM)
	            this->addEdge(eqn1-START_EQN_NUM+START_VERTEX_NUM,eqn2-START_EQN_NUM+START_VERTEX_NUM);
	          }
              }
          }
      }
  }



