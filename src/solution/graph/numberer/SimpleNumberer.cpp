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
// $Date: 2006/01/12 23:39:21 $
// $Source: /usr/local/cvs/OpenSees/SRC/graph/numberer/SimpleNumberer.cpp,v $
                                                                        
                                                                        

// Written: fmk 
// Revision: A
//
// Description: This file contains the class definition for XC::SimpleNumberer.
// SimpleNumberer is an object to perform a simple numbering of the vertices.
// It does this by using the graphs XC::VertexIter and assigning the numbers as
// it comes across the vertices.
//
// What: "@(#) SimpleNumberer.C, revA"

#include <solution/graph/numberer/SimpleNumberer.h>
#include "solution/graph/graph/Graph.h"
#include <solution/graph/graph/Vertex.h>
#include <solution/graph/graph/VertexIter.h>
#include <utility/matrix/ID.h>

// Constructor
XC::SimpleNumberer::SimpleNumberer(void)
 :BaseNumberer(GraphNUMBERER_TAG_SimpleNumberer) {}

XC::GraphNumberer *XC::SimpleNumberer::getCopy(void) const
  { return new SimpleNumberer(*this); }

const XC::ID &XC::SimpleNumberer::number(Graph &theGraph, int lastVertex)
  {
    // see if we can do quick return
    if(!checkSize(theGraph)) 
      return theRefResult;
	    

    // Now we go through the iter and assign the numbers

    if(lastVertex != -1)
      {
	std::cerr << "WARNING:  SimpleNumberer::number -";
	std::cerr << " - does not deal with lastVertex";
      }
    
    Vertex *vertexPtr= nullptr;
    VertexIter &vertexIter= theGraph.getVertices();
    int count = 0;
    
    while((vertexPtr = vertexIter()) != 0)
      {
	theRefResult(count++) = vertexPtr->getTag();
	vertexPtr->setTmp(count);
      }
    return theRefResult;
  }


int XC::SimpleNumberer::sendSelf(CommParameters &cp)
  { return 0; }

int XC::SimpleNumberer::recvSelf(const CommParameters &cp)
  { return 0; }

const XC::ID &XC::SimpleNumberer::number(Graph &theGraph, const XC::ID &startVertices)
  { return this->number(theGraph); }
