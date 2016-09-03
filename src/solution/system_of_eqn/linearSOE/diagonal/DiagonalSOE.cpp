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
                                                                        
// $Revision: 1.1 $
// $Date: 2005/01/27 22:22:50 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/diagonal/DiagonalSOE.cpp,v $

// Written: fmk 
// Created: Febuary 1997
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

XC::DiagonalSOE::DiagonalSOE(SoluMethod *owr)
  :FactoredSOEBase(owr,LinSOE_TAGS_DiagonalSOE) {}


XC::DiagonalSOE::DiagonalSOE(SoluMethod *owr,int N)
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
      std::cerr << "DiagonalSOE::setSolver; solver incompatible con sistema de ecuaciones." << std::endl;
    return retval;
  }
    
int XC::DiagonalSOE::setSize(Graph &theGraph)
  {
    const int oldSize = size;
    int result = 0;
    size = theGraph.getNumVertex();
    if(size==0)
      std::cerr << "¡OJO! error en " << nombre_clase() << "::setSize; el modelo no tiene ningún grado de libertad,"
                << " agrege algún nodo o cambie el gestor de coacciones." << std::endl;
  
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
    // check for a XC::quick return 
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


int XC::DiagonalSOE::sendSelf(CommParameters &cp)
  { return 0; }


int XC::DiagonalSOE::recvSelf(const CommParameters &cp)
  { return 0; }
