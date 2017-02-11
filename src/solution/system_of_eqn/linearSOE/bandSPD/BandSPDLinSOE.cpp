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

// $Revision: 1.4 $
// $Date: 2005/12/06 22:03:48 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/bandSPD/BandSPDLinSOE.cpp,v $


// file: ~/system_of_eqn/linearSOE/bandSPD/BandSPDLinSOE.C
//
// Written: fmk
// Created: Febuary 1997
// Revision: A
//
// Description: This file contains the implementation for XC::BandSPDLinSOE


#include <solution/system_of_eqn/linearSOE/bandSPD/BandSPDLinSOE.h>
#include <solution/system_of_eqn/linearSOE/bandSPD/BandSPDLinSolver.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include "solution/graph/graph/Graph.h"
#include <solution/graph/graph/Vertex.h>
#include <solution/graph/graph/VertexIter.h>

void XC::BandSPDLinSOE::inicA(const size_t &hsz)
  {
    A.resize(hsz);
    A.Zero();
  }

XC::BandSPDLinSOE::BandSPDLinSOE(SoluMethod *owr)
  :FactoredSOEBase(owr,LinSOE_TAGS_BandSPDLinSOE), half_band(0){}


XC::BandSPDLinSOE::BandSPDLinSOE(SoluMethod *owr,int classTag)
  :FactoredSOEBase(owr,classTag), half_band(0) {}


XC::BandSPDLinSOE::BandSPDLinSOE(SoluMethod *owr,int N, int numSuper,BandSPDLinSolver *the_Solver)
  :FactoredSOEBase(owr,LinSOE_TAGS_BandSPDLinSOE,N), half_band(0)
  {
    half_band = numSuper+1;

    inicA(half_band*size);

    inic(size);

    if(the_Solver)
      setSolver(the_Solver);
    else
      std::cerr << "BandSPDLinSOE::BandSPDLinSOE; no se especificó el solver." << std::endl;
  }

bool XC::BandSPDLinSOE::setSolver(LinearSOESolver *newSolver)
  {
    bool retval= false;
    BandSPDLinSolver *tmp= dynamic_cast<BandSPDLinSolver *>(newSolver);
    if(tmp)
      retval= FactoredSOEBase::setSolver(tmp);
    else
      std::cerr << "BandSPDLinSOE::setSolver; solver incompatible con system of equations." << std::endl;
    return retval;
  }

int XC::BandSPDLinSOE::setSize(Graph &theGraph)
  {

    int result = 0;
    size = theGraph.getNumVertex();
    if(size==0)
      std::cerr << "¡OJO! error en " << nombre_clase() 
                << "::setSize; el modelo no tiene ningún grado de libertad,"
                << " agrege algún nodo o cambie el gestor de coacciones." << std::endl;
    half_band= theGraph.getVertexDiffMaxima();


    half_band+= 1; // include the diagonal


    if(half_band*size > A.Size())// we have to get another space for A
      inicA(half_band*size); 

    factored = false;

    if(size > B.Size())
      inic(size);


    // invoke setSize() on the Solver
    LinearSOESolver *the_Solver= this->getSolver();
    int solverOK = the_Solver->setSize();
    if(solverOK < 0)
      {
        std::cerr << "WARNING: BandSPDLinSOE::setSize :";
        std::cerr << " solver failed setSize()\n";
        return solverOK;
      }

    return result;
  }

int XC::BandSPDLinSOE::addA(const Matrix &m, const ID &id, double fact)
  {
    // check for a quick return
    if(fact == 0.0)  return 0;

    // check that m and id are of similar size
    int idSize = id.Size();
    if(idSize != m.noRows() && idSize != m.noCols())
      {
        std::cerr << "BandSPDLinSOE::addA() - Matrix and XC::ID not of similar sizes\n";
        return -1;
      }

    if(fact == 1.0)
      { // do not need to multiply
        for(int i=0; i<idSize; i++)
          {
            int col = id(i);
            if(col < size && col >= 0)
              {
                double *coliiPtr = A.getDataPtr() +(col+1)*half_band -1;
                int minColRow = col - half_band + 1;
                for(int j=0; j<idSize; j++)
                  {
                    int row = id(j);
                    if(row <size && row >= 0 &&
                        row <= col && row >= minColRow)
                      { // only add upper
                         double *APtr = coliiPtr + (row-col);
                         *APtr += m(j,i);
                      }
                  }  // for j
              }
          }  // for i
      }
    else
      {
        for(int i=0; i<idSize; i++)
          {
            int col = id(i);
            if(col < size && col >= 0)
              {
                double *coliiPtr = A.getDataPtr() +(col+1)*half_band -1;
                int minColRow = col - half_band +1;
                for(int j=0; j<idSize; j++)
                  {
                    int row = id(j);
                    if(row <size && row >= 0 &&
                        row <= col && row >= minColRow)
                      { // only add upper
                         double *APtr = coliiPtr + (row-col);
                         *APtr += m(j,i)*fact;
                      }
                  }  // for j
              }
          }  // for i
      }
    return 0;
  }


void XC::BandSPDLinSOE::zeroA(void)
  {
    A.Zero();
    factored = false;
  }

int XC::BandSPDLinSOE::sendSelf(CommParameters &cp)
  { return 0; }


int XC::BandSPDLinSOE::recvSelf(const CommParameters &cp)
  { return 0; }
