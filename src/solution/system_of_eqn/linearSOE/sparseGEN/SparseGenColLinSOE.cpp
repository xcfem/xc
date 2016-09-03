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
                                                                        
// $Revision: 1.6 $
// $Date: 2005/12/06 22:11:37 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/sparseGEN/SparseGenColLinSOE.cpp,v $
                                                                        
                                                                        
// Written: fmk 
// Created: 04/98
// Revision: A
//
// Description: This file contains the implementation for XC::SparseGenColLinSOE

#include <solution/system_of_eqn/linearSOE/sparseGEN/SparseGenColLinSOE.h>
#include <solution/system_of_eqn/linearSOE/sparseGEN/SparseGenColLinSolver.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include "solution/graph/graph/Graph.h"
#include <solution/graph/graph/Vertex.h>
#include <solution/graph/graph/VertexIter.h>
#include <cmath>

XC::SparseGenColLinSOE::SparseGenColLinSOE(SoluMethod *owr)
  :SparseGenSOEBase(owr,LinSOE_TAGS_SparseGenColLinSOE) {}

XC::SparseGenColLinSOE::SparseGenColLinSOE(SoluMethod *owr,int classTag)
  :SparseGenSOEBase(owr,classTag) {}

bool XC::SparseGenColLinSOE::setSolver(LinearSOESolver *newSolver)
  {
    bool retval= false;
    SparseGenColLinSolver *tmp= dynamic_cast<SparseGenColLinSolver *>(newSolver);
    if(tmp)
      retval= SparseGenSOEBase::setSolver(tmp);
    else
      std::cerr << "SparseGenColLinSOE::setSolver; solver incompatible con sistema de ecuaciones." << std::endl;
    return retval;
  }

int XC::SparseGenColLinSOE::setSize(Graph &theGraph)
  {
    int result = 0;
    size = theGraph.getNumVertex();
    if(size==0)
      std::cerr << "¡OJO! error en " << nombre_clase() << "::setSize; el modelo no tiene ningún grado de libertad,"
                << " agrege algún nodo o cambie el gestor de coacciones." << std::endl;

    // fist itearte through the vertices of the graph to get nnz
    Vertex *theVertex;
    int newNNZ = 0;
    VertexIter &theVertices = theGraph.getVertices();
    while((theVertex = theVertices()) != 0)
      {
	const std::set<int> &theAdjacency = theVertex->getAdjacency();
	newNNZ += theAdjacency.size() +1; // the +1 is for the diag entry
      }
    nnz = newNNZ;

    if(newNNZ > A.Size())
      { // we have to get more space for A and rowA
	A.resize(newNNZ);
        rowA.resize(newNNZ);
      }
    A.Zero();
	
    factored = false;
    
    if(size > B.Size())
      { // we have to get space for the vectors
	
        inic(size);
	colStartA.resize(size+1);	
      }

    // fill in colStartA and rowA
    if(size != 0)
      {
        colStartA(0)= 0;
        int startLoc = 0;
        int lastLoc = 0;
        for(int a=0;a<size;a++)
          {
            theVertex = theGraph.getVertexPtr(a);
	    if(theVertex == 0)
              {
	        std::cerr << "WARNING:XC::SparseGenColLinSOE::setSize :";
	        std::cerr << " vertex " << a << " not in graph! - size set to 0\n";
	        size = 0;
	        return -1;
	      }
            rowA(lastLoc++) = theVertex->getTag(); // place diag in first
	    const std::set<int> &theAdjacency = theVertex->getAdjacency();
	
	    // now we have to place the entries in the ID into order in rowA
            for(std::set<int>::const_iterator i=theAdjacency.begin(); i!=theAdjacency.end(); i++)
              {
                int row = *i;
	        bool foundPlace = false;
	        // find a place in rowA for current col
	        for(int j=startLoc; j<lastLoc; j++)
	          if(rowA(j) > row)
                    { 
	              // move the entries already there one further on
	              // and place col in current location
	              for(int k=lastLoc; k>j; k--)
			rowA(k) = rowA(k-1);
	              rowA(j) = row;
	              foundPlace = true;
	              j = lastLoc;
	            }
	        if(!foundPlace) // put in at the end
	          rowA(lastLoc)= row;
                lastLoc++;
	      }
	    colStartA(a+1)= lastLoc;;	    
	    startLoc = lastLoc;
          }
      }
    // invoke setSize() on the Solver    
    LinearSOESolver *the_Solver = this->getSolver();
    int solverOK = the_Solver->setSize();
    if(solverOK < 0)
      {
	std::cerr << "WARNING:XC::SparseGenColLinSOE::setSize :";
	std::cerr << " solver failed setSize()\n";
	return solverOK;
      }   
    return result;
  }

int XC::SparseGenColLinSOE::addA(const Matrix &m, const ID &id, double fact)
  {
    // check for a XC::quick return 
    if (fact == 0.0)  
	return 0;

    int idSize = id.Size();
    
    // check that m and id are of similar size
    if (idSize != m.noRows() && idSize != m.noCols()) {
	std::cerr << "SparseGenColLinSOE::addA() ";
	std::cerr << " - Matrix and XC::ID not of similar sizes\n";
	return -1;
    }
    
    if (fact == 1.0) { // do not need to multiply 
      for (int i=0; i<idSize; i++) {
	int col = id(i);
	if (col < size && col >= 0) {
	  int startColLoc = colStartA(col);
	  int endColLoc = colStartA(col+1);
	  for (int j=0; j<idSize; j++) {
	    int row = id(j);
	    if (row <size && row >= 0) {
	      // find place in A using rowA
	      for (int k=startColLoc; k<endColLoc; k++)
		if(rowA(k) == row) {
		  A[k] += m(j,i);
		  k = endColLoc;
		}
	    }
	  }  // for j		
	} 
      }  // for i
    } else {
      for (int i=0; i<idSize; i++) {
	int col = id(i);
	if (col < size && col >= 0) {
	  int startColLoc = colStartA(col);
	  int endColLoc = colStartA(col+1);
	  for (int j=0; j<idSize; j++) {
	    int row = id(j);
	    if (row <size && row >= 0) {
	      // find place in A using rowA
	      for (int k=startColLoc; k<endColLoc; k++)
		if(rowA(k) == row) {
		  A[k] += fact * m(j,i);
		  k = endColLoc;
		}
	    }
	  }  // for j		
	} 
      }  // for i
    }
    return 0;
  }

    
int XC::SparseGenColLinSOE::sendSelf(CommParameters &cp)
  { return 0; }

int XC::SparseGenColLinSOE::recvSelf(const CommParameters &cp)  
  { return 0; }

