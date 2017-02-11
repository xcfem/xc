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
                                                                        
// $Revision: 1.3 $
// $Date: 2003/02/14 23:02:01 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/fullGEN/FullGenLinSOE.cpp,v $
                                                                        
                                                                        
// File: ~/system_of_eqn/linearSOE/fullGEN/FullGenLinSOE.C
//
// Written: fmk 
// Created: Febuary 1997
// Revision: A
//
// Description: This file contains the implementation for XC::FullGenLinSOE


#include <solution/system_of_eqn/linearSOE/fullGEN/FullGenLinSOE.h>
#include <solution/system_of_eqn/linearSOE/fullGEN/FullGenLinSolver.h>
#include <utility/matrix/Matrix.h>
#include "solution/graph/graph/Graph.h"
#include <solution/graph/graph/Vertex.h>
#include <solution/graph/graph/VertexIter.h>

XC::FullGenLinSOE::FullGenLinSOE(SoluMethod *owr)
  :FactoredSOEBase(owr,LinSOE_TAGS_FullGenLinSOE) {}


XC::FullGenLinSOE::FullGenLinSOE(SoluMethod *owr,int N)
  :FactoredSOEBase(owr,LinSOE_TAGS_FullGenLinSOE,N)
  {
    A= Vector(size*size);
        
    // zero the matrix
    A.Zero();
    inic(size);        
  }

    
bool XC::FullGenLinSOE::setSolver(LinearSOESolver *newSolver)
  {
    bool retval= false;
    FullGenLinSolver *tmp= dynamic_cast<FullGenLinSolver *>(newSolver);
    if(tmp)
      retval= FactoredSOEBase::setSolver(tmp);
    else
      std::cerr << "FullGenLinSOE::setSolver; solver incompatible con system of equations." << std::endl;
    return retval;
  }

int XC::FullGenLinSOE::setSize(Graph &theGraph)
  {
    int result = 0;
    size = theGraph.getNumVertex();
    if(size==0)
      std::cerr << "¡OJO! error en " << nombre_clase()
                << "::setSize; el modelo no tiene ningún grado de libertad,"
                << " agrege algún nodo o cambie el gestor de coacciones." << std::endl;

    const int size2= size*size;
    A.resize(size2);
    A.Zero();
        
    factored = false;
    
    if(size > B.Size())
      inic(size);

    // invoke setSize() on the XC::Solver    
    LinearSOESolver *theSolvr = this->getSolver();
    int solverOK = theSolvr->setSize();
    if(solverOK < 0)
      {
        std::cerr << "WARNING:XC::FullGenLinSOE::setSize :";
        std::cerr << " solver failed setSize()\n";
        return solverOK;
      }    
    return result;
  }

int XC::FullGenLinSOE::addA(const Matrix &m, const ID &id, double fact)
  {
    // check for a XC::quick return 
    if(fact == 0.0)  return 0;

    int idSize = id.Size();
    
    // check that m and id are of similar size
    if(idSize != m.noRows() && idSize != m.noCols()) {
        std::cerr << "FullGenLinSOE::addA() - Matrix and ID not of similar sizes\n";
        return -1;
    }
    
    double *addrA= A.getDataPtr();
    if(fact == 1.0)
      { // do not need to multiply 
        for(int i=0; i<idSize; i++)
          {
            const int col = id(i);
            if(col < size && col >= 0)
              {
                double *startColiPtr= addrA + col*size;
                for(int j=0; j<idSize; j++)
                  {
                    const int row = id(j);
                    if(row <size && row >= 0)
                      {
                        double *APtr= startColiPtr + row;
                        *APtr+= m(j,i);
                      }
                  }  // for j
              }
          }  // for i
      }
    else
      {
        for(int i=0; i<idSize; i++)
          {
            const int col= id(i);
            if(col < size && col >= 0)
              {
                double *startColiPtr= addrA + col*size;
                for(int j=0; j<idSize; j++)
                  {
                    const int row= id(j);
                    if(row <size && row >= 0)
                      {
                         double *APtr= startColiPtr + row;
                         *APtr += m(j,i) * fact;
                      }
                  }  // for j
              } 
          }  // for i
      }    
    return 0;
  }

void XC::FullGenLinSOE::zeroA(void)
  {
    A.Zero();
    factored = false;
  }
        
int XC::FullGenLinSOE::sendSelf(CommParameters &cp)
  {
    return 0;
  }

int XC::FullGenLinSOE::recvSelf(const CommParameters &cp)
  {
    return 0;
  }






