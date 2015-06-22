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
      std::cerr << "BandSPDLinSOE::setSolver; solver incompatible con sistema de ecuaciones." << std::endl;
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
