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
                                                                        
// $Revision: 1.1 $
// $Date: 2005/01/27 22:22:50 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/diagonal/DiagonalSOE.cpp,v $

// Written: fmk 
// Created: February 1997
// Revision: A
//
// Description: This file contains the implementation for XC::DiagonalSOE


#include <solution/system_of_eqn/linearSOE/diagonal/DiagonalSOE.h>
#include <solution/system_of_eqn/linearSOE/diagonal/DiagonalSolver.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include "solution/graph/graph/Graph.h"
#include <solution/graph/graph/Vertex.h>
#include <solution/graph/graph/VertexIter.h>

XC::DiagonalSOE::DiagonalSOE(AnalysisAggregation *owr)
  :FactoredSOEBase(owr,LinSOE_TAGS_DiagonalSOE) {}


XC::DiagonalSOE::DiagonalSOE(AnalysisAggregation *owr,int N)
  :FactoredSOEBase(owr,LinSOE_TAGS_DiagonalSOE,N)
  {
    if(size > 0)
      {
        A= Vector(size);
        inic(size);    
      }
  }

bool XC::DiagonalSOE::setSolver(LinearSOESolver *newSolver)
  {
    bool retval= false;
    DiagonalSolver *tmp= dynamic_cast<DiagonalSolver *>(newSolver);
    if(tmp)
      retval= FactoredSOEBase::setSolver(tmp);
    else
      std::cerr << "DiagonalSOE::setSolver; solver incompatible con system of equations." << std::endl;
    return retval;
  }
    
int XC::DiagonalSOE::setSize(Graph &theGraph)
  {
    const int oldSize = size;
    int result = 0;
    size= checkSize(theGraph);
  
    // check we have enough space in iDiagLoc and iLastCol
    // if not delete old and create new
    if(size > oldSize)
      { 
    
        A.resize(size);
        inic(size);
      }

    A.Zero();
    factored = false;

    
    // invoke setSize() on the XC::Solver
    LinearSOESolver *the_Solver = this->getSolver();
    int solverOK = the_Solver->setSize();
    if(solverOK < 0)
      {
        std::cerr << "WARNING XC::DiagonalSOE::setSize :";
        std::cerr << " solver failed setSize()\n";
        return solverOK;
      }    
    return result;
  }

int XC::DiagonalSOE::addA(const Matrix &m, const ID &id, double fact)
  {
    // check for a quick return 
    if (fact == 0.0)  return 0;
  
#ifdef _G3DEBUG
    // check that m and id are of similar size
    int idSize = id.Size();    
    if(idSize != m.noRows() && idSize != m.noCols())
      {
        std::cerr << "XC::FullGenLinSOE::addA()	- Matrix and XC::ID not of similar sizes\n";
        return -1;
      }
#endif

    if(fact == 1.0)
      { // do not need to multiply if fact == 1.0
        for(int i=0; i<id.Size(); i++)
          {
            int pos = id(i);
            if(pos <size && pos >= 0)
	      A[pos] += m(i,i);
          }
      }
    else if(fact == -1.0)
      { // do not need to multiply if fact == -1.0
        for(int i=0; i<id.Size(); i++)
          {
            int pos = id(i);
            if(pos <size && pos >= 0)
	      A[pos] -= m(i,i);
          }
      }
    else
      {
        for(int i=0; i<id.Size(); i++)
          {
            int pos = id(i);
            if(pos <size && pos >= 0)
	      A[pos] += m(i,i) * fact;
          }
      }	
    return 0;
  }
 
    
void XC::DiagonalSOE::zeroA(void)
  {
    A.Zero();
    factored = false;
  }


int XC::DiagonalSOE::sendSelf(Communicator &comm)
  { return 0; }


int XC::DiagonalSOE::recvSelf(const Communicator &comm)
  { return 0; }
