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
// $Date: 2005/12/06 22:06:04 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/profileSPD/ProfileSPDLinSOE.cpp,v $
                                                                        
                                                                        
// Written: fmk 
// Revision: A
//
// Description: This file contains the implementation for XC::ProfileSPDLinSOE


#include <solution/system_of_eqn/linearSOE/profileSPD/ProfileSPDLinSOE.h>
#include <solution/system_of_eqn/linearSOE/profileSPD/ProfileSPDLinSolver.h>
#include "solution/graph/graph/Graph.h"
#include <solution/graph/graph/Vertex.h>
#include <solution/graph/graph/VertexIter.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>

XC::ProfileSPDLinSOE::ProfileSPDLinSOE(SoluMethod *owr)
  :FactoredSOEBase(owr,LinSOE_TAGS_ProfileSPDLinSOE),
   profileSize(0), isAcondensed(false), numInt(0) {}

XC::ProfileSPDLinSOE::ProfileSPDLinSOE(SoluMethod *owr,int classTag)
  :FactoredSOEBase(owr,classTag),
   profileSize(0), isAcondensed(false), numInt(0) {}


XC::ProfileSPDLinSOE::ProfileSPDLinSOE(SoluMethod *owr,int N, int *iLoc,ProfileSPDLinSolver *the_Solver)
  :FactoredSOEBase(owr,LinSOE_TAGS_ProfileSPDLinSOE,N),
   profileSize(0), isAcondensed(false), numInt(0)
  {
    size = N;
    profileSize= iLoc[N-1];
    
    A= Vector(iLoc[N-1]);
        
    // zero the matrix
    A.Zero();
    
    inic(size);
    iDiagLoc= ID(size);
    
     // zero the vectors
     for(int i=0; i<size; i++)
       { iDiagLoc(i) = iLoc[i]; }

    if(the_Solver)
      setSolver(the_Solver);
    else
      std::cerr << "BandGenLinSOE::BandGenLinSOE; no se especificó el solver." << std::endl;
  }

    
bool XC::ProfileSPDLinSOE::setSolver(LinearSOESolver *newSolver)
  {
    bool retval= false;
    ProfileSPDLinSolver *tmp= dynamic_cast<ProfileSPDLinSolver *>(newSolver);
    if(tmp)
      retval= FactoredSOEBase::setSolver(tmp);
    else
      std::cerr << "XC::ProfileSPDLinSOE::setSolver; solver incompatible con sistema de ecuaciones." << std::endl;
    return retval;
  }

int XC::ProfileSPDLinSOE::setSize(Graph &theGraph)
  {
    int result = 0;
    size = theGraph.getNumVertex();
    if(size==0)
      std::cerr << "¡OJO! error en " << nombre_clase() << "::setSize; el modelo no tiene ningún grado de libertad,"
                << " agrege algún nodo o cambie el gestor de coacciones." << std::endl;

    // check we have enough space in iDiagLoc and iLastCol
    // if not delete old and create new
    if(size > B.Size())
       { iDiagLoc.resize(size); }

    // zero out iDiagLoc 
    iDiagLoc.Zero();

    // now we go through the vertices to find the height of each col and
    // width of each row from the connectivity information.
    
    Vertex *vertexPtr;
    VertexIter &theVertices = theGraph.getVertices();

    while ((vertexPtr = theVertices()) != 0)
      {
        int vertexNum = vertexPtr->getTag();
        const std::set<int> &theAdjacency = vertexPtr->getAdjacency();
        int iiDiagLoc = iDiagLoc(vertexNum);
        int *iiDiagLocPtr = &(iDiagLoc(vertexNum));

        for(std::set<int>::const_iterator i=theAdjacency.begin(); i!=theAdjacency.end(); i++)
          {
            const int otherNum= *i;
            const int diff= vertexNum-otherNum;
            if(diff > 0)
              {
                if(iiDiagLoc < diff)
                  {
                    iiDiagLoc = diff;
                    *iiDiagLocPtr = diff;
                  }
              } 
          }
      }


    // now go through iDiagLoc, adding 1 for the diagonal element
    // and then adding previous entry to give current location.
    if(!iDiagLoc.Nulo())
      iDiagLoc(0) = 1; // NOTE FORTRAN ARRAY LOCATION

    for(int j=1; j<size; j++)
      iDiagLoc(j)= iDiagLoc(j) + 1 + iDiagLoc(j-1);

    if(!iDiagLoc.Nulo())       
      profileSize = iDiagLoc[size-1];

    // check if we need more space to hold A
    // if so then go get it
    if(profileSize > A.Size())
      { A.resize(profileSize); }

    A.Zero();

    factored = false;
    isAcondensed = false;    

    if(size > B.Size())
      inic(size);

    // invoke setSize() on the XC::Solver
    LinearSOESolver *the_Solver = this->getSolver();
    int solverOK = the_Solver->setSize();
    if(solverOK < 0)
      {
        std::cerr << "WARNING XC::ProfileSPDLinSOE::setSize :";
        std::cerr << " solver failed setSize()\n";
        return solverOK;
      }    
    return result;
  }

int XC::ProfileSPDLinSOE::addA(const Matrix &m, const ID &id, double fact)
  {
    // check for a XC::quick return 
    if(fact == 0.0)  return 0;
    
    // check that m and id are of similar size
    int idSize = id.Size();    
    if(idSize != m.noRows() && idSize != m.noCols()) {
        std::cerr << "ProfileSPDLinSOE::addA() - Matrix and XC::ID not of similar sizes\n";
        return -1;
    }

    double *addrA= A.getDataPtr();
    if(fact == 1.0)
      { // do not need to multiply 
        for(int i=0; i<idSize; i++)
          {
            int col = id(i);
            if(col < size && col >= 0)
              {
                double *coliiPtr = &addrA[iDiagLoc(col)-1]; // -1 as fortran indexing 
                int minColRow;
                if(col == 0)
                    minColRow = 0;
                else
		  minColRow = col - (iDiagLoc(col) - iDiagLoc(col-1)) +1;
                for(int j=0; j<idSize; j++)
                  {
                    const int row = id(j);
                    if(row <size && row >= 0 && row <= col && row >= minColRow)
                      { 
                        // we only add upper and inside profile
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
            const int col = id(i);
            if(col < size && col >= 0)
              {
                double *coliiPtr = &addrA[iDiagLoc(col)-1]; // -1 as fortran indexing                 
                int minColRow;
                if(col == 0)
                    minColRow = 0;
                else
		  minColRow = col - (iDiagLoc(col) - iDiagLoc(col-1)) +1;
                for(int j=0; j<idSize; j++)
                  {
                    const int row = id(j);
                    if(row <size && row >= 0 && row <= col && row >= minColRow)
                      { 
                        // we only add upper and inside profile
                        double *APtr = coliiPtr + (row-col);
                         *APtr+= m(j,i) * fact;
                       }
                  }  // for j
              } 
          }  // for i
      }
    return 0;
  }

    
void XC::ProfileSPDLinSOE::zeroA(void)
  {
    A.Zero();
    factored = false;
  }


int XC::ProfileSPDLinSOE::sendSelf(CommParameters &cp)
  { return 0; }


int XC::ProfileSPDLinSOE::recvSelf(const CommParameters &cp)
  { return 0; }
