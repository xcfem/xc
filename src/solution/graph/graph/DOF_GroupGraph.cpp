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

// constructs the XC::Graph
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


