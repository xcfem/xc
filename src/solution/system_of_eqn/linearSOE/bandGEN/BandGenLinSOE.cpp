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
// $Date: 2005/12/06 21:56:41 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/bandGEN/BandGenLinSOE.cpp,v $


// Written: fmk
//
// Description: This file contains the implementation for XC::BandGenLinSOE


#include <solution/system_of_eqn/linearSOE/bandGEN/BandGenLinSOE.h>
#include <solution/system_of_eqn/linearSOE/bandGEN/BandGenLinSolver.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include "solution/graph/graph/Graph.h"
#include <solution/graph/graph/Vertex.h>
#include <solution/graph/graph/VertexIter.h>

XC::BandGenLinSOE::BandGenLinSOE(SoluMethod *owr)
  :FactoredSOEBase(owr,LinSOE_TAGS_BandGenLinSOE), numSuperD(0), numSubD(0) {}

XC::BandGenLinSOE::BandGenLinSOE(SoluMethod *owr,int classTag)
  :FactoredSOEBase(owr,classTag), numSuperD(0), numSubD(0) {}


XC::BandGenLinSOE::BandGenLinSOE(SoluMethod *owr,int N, int numSuperDiag, int numSubDiag,BandGenLinSolver *theSolvr)
  :FactoredSOEBase(owr,LinSOE_TAGS_BandGenLinSOE,N),
   numSuperD(numSuperDiag), numSubD(numSubDiag)
  {
    const size_t Asize= N * (2*numSubD + numSuperD +1);
    A= Vector(Asize);
    A.Zero();

    inic(size);

    if(theSolvr)
      {
        setSolver(theSolvr);

        int solverOK = theSolvr->setSize();
        if(solverOK < 0)
          {
            std::cerr << "WARNING XC::BandGenLinSOE::BandGenLinSOE :";
            std::cerr << " solver failed setSize() in constructor\n";
          }
      }
    else
      std::cerr << "XC::BandGenLinSOE::BandGenLinSOE; no se especificó el solver." << std::endl;
  }

bool XC::BandGenLinSOE::setSolver(LinearSOESolver *newSolver)
  {
    bool retval= false;
    BandGenLinSolver *tmp= dynamic_cast<BandGenLinSolver *>(newSolver);
    if(tmp)
      retval= FactoredSOEBase::setSolver(tmp);
    else
      std::cerr << "XC::BandGenLinSOE::setSolver; solver incompatible con sistema de ecuaciones." << std::endl;
    return retval;
  }

int XC::BandGenLinSOE::setSize(Graph &theGraph)
  {
    int result = 0;
    size = theGraph.getNumVertex();
    if(size==0)
      std::cerr << "¡OJO! error en " << nombre_clase() << "::setSize; el modelo no tiene ningún grado de libertad,"
                << " agrege algún nodo o cambie el gestor de coacciones." << std::endl;

    /*
     * determine the number of superdiagonals and subdiagonals
     */
    theGraph.getBand(numSubD,numSuperD);

    int newSize = size * (2*numSubD + numSuperD +1);
    if(newSize > A.Size())
      { // we have to get another space for A
        A.resize(newSize);
      }
    A.Zero();

    factored = false;

    if(size > B.Size())
      { // we have to get space for the vectors

        // create the new
        inic(size);
      }

    // invoke setSize() on the Solver
    LinearSOESolver *theSolvr = this->getSolver();
    int solverOK = theSolvr->setSize();
    if(solverOK < 0)
      {
        std::cerr << "WARNING:XC::BandGenLinSOE::setSize :";
        std::cerr << " solver failed setSize().\n";
        return solverOK;
      }
    return result;
  }

int XC::BandGenLinSOE::addA(const XC::Matrix &m, const XC::ID &id, double fact)
  {
  //  std::cerr << "BAND- addA() " << m << id;

    // check for a XC::quick return
    if(fact == 0.0)  return 0;


    // check that m and id are of similar size
    const int idSize = id.Size();
    if(idSize != m.noRows() && idSize != m.noCols())
      {
        std::cerr << "XC::BandGenLinSOE::addA() - Matrix and XC::ID not of similar sizes.\n";
        return -1;
      }

    int ldA = 2*numSubD + numSuperD + 1;


    if(fact == 1.0)
      { // do not need to multiply
        for(int i=0; i<idSize; i++)
          {
            int col = id(i);
            if(col < size && col >= 0)
              {
                double *coliiPtr = A.getDataPtr() + col*ldA + numSubD + numSuperD;
                for(int j=0; j<idSize; j++)
                  {
                    int row = id(j);
                    if(row <size && row >= 0)
                      {
                        int diff = col - row;
                        if(diff > 0)
                          {
                            if(diff <= numSuperD)
                              {
                                double *APtr = coliiPtr - diff;
                                *APtr += m(j,i);
                              }
                          }
                        else
                          {
                            diff *= -1;
                            if(diff <= numSubD)
                              {
                                double *APtr = coliiPtr + diff;
                                *APtr += m(j,i);
                              }
                          }
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
                double *coliiPtr = A.getDataPtr() + col*ldA + numSubD + numSuperD;
                for(int j=0; j<idSize; j++)
                  {
                    int row = id(j);
                    if(row <size && row >= 0)
                      {
                        int diff = col - row;
                        if(diff > 0)
                          {
                            if(diff <= numSuperD)
                              {
                                double *APtr = coliiPtr - diff;
                                *APtr += m(j,i) *fact;
                              }
                          }
                        else
                          {
                            diff *= -1;
                            if(diff <= numSubD)
                              {
                                double *APtr = coliiPtr + diff;
                                *APtr += m(j,i) *fact;
                              }
                          }
                      }
                  }  // for j
              }
          }  // for i
      }
    //    std::cerr << A;
    // std::cerr << std::endl;
    return 0;
  }

void XC::BandGenLinSOE::zeroA(void)
  {
    A.Zero();
    factored = false;
  }

int XC::BandGenLinSOE::sendSelf(CommParameters &cp)
  { return 0; }


int XC::BandGenLinSOE::recvSelf(const CommParameters &cp)
  { return 0; }


