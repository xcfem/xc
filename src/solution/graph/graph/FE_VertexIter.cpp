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
                                                                        
// $Revision: 1.1.1.1 $
// $Date: 2000/09/15 08:23:21 $
// $Source: /usr/local/cvs/OpenSees/SRC/graph/graph/FE_VertexIter.cpp,v $
                                                                        
                                                                        
// File: ~/analysis/model/simple/FE_VertexIter.C
//
// Written: fmk 
// Created: Fri Sep 20 15:27:47: 1996
// Revision: A
//
// Description: This file contains the method definitions for class 
// FE_VertexIter. FE_VertexIter is a class for iterating through the 
// vertices of an FE_Graph. 


#include "FE_Graph.h"
#include <solution/graph/graph/FE_VertexIter.h>
#include <solution/graph/graph/Vertex.h>

// FE_VertexIter(FE_Graph &theGraph):
//	constructor that takes the graph, just the basic iter

XC::FE_VertexIter::FE_VertexIter(FE_Graph &theGraph)
  :myGraph(theGraph), currIndex(0), numDone(0)
{
}


void XC::FE_VertexIter::reset(void)
{
    currIndex = 0;
    numDone = 0;
}

 XC::Vertex *XC::FE_VertexIter::operator()(void)
{
  // check if we still have vertices in the XC::Graph
  // if not return 0, indicating we are done
  if (numDone >= myGraph.numVertex)
    return 0;

  // search through domains ele list till we find the next element
  while ((currIndex < myGraph.sizeVertices) 
	 && (myGraph.theVertices[currIndex] == 0))
      currIndex++;

  // if not at the end of the list return the element
  if (currIndex < myGraph.sizeVertices) {
      Vertex *result = myGraph.theVertices[currIndex];
      numDone++; currIndex++;
      return(result);
  }
  return (0);
}

    
    




