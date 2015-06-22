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
** file 'COPYRIGHT'  in XC::main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.1 $
// $Date: 2005/12/06 22:20:18 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/sparseGEN/DistributedSparseGenColLinSOE.cpp,v $
                                                                        
// Written: fmk 
// Revision: A
//
// Description: This file contains the implementation for SparseGenColLinSOE


#include <solution/system_of_eqn/linearSOE/sparseGEN/DistributedSparseGenColLinSOE.h>
#include <solution/system_of_eqn/linearSOE/sparseGEN/SparseGenColLinSolver.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include "solution/graph/graph/Graph.h"
#include <solution/graph/graph/Vertex.h>
#include <solution/graph/graph/VertexIter.h>
#include <utility/actor/channel/Channel.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>

//! @brief Constructor.
XC::DistributedSparseGenColLinSOE::DistributedSparseGenColLinSOE(SoluMethod *owr)
  :SparseGenColLinSOE(owr,LinSOE_TAGS_DistributedSparseGenColLinSOE), DistributedBandLinSOE() {}

//! @brief Calcula el valor de nnz.
void XC::DistributedSparseGenColLinSOE::calcNonZeros(Graph &theGraph)
  {
    size= theGraph.getNumVertex();
    // determine the number of non-zeros
    Vertex *theVertex;
    VertexIter &theVertices= theGraph.getVertices();
    nnz= 0;
    while((theVertex = theVertices()) != 0)
      {
        const std::set<int> &theAdjacency= theVertex->getAdjacency();
        nnz+= theAdjacency.size()+1; // the +1 is for the diag entry
      }
  }

int XC::DistributedSparseGenColLinSOE::setSize(Graph &theGraph)
  {
    int result = 0;
    int oldSize = size;
    //int maxNumSubVertex = 0;
    static ID data(2);

    // if subprocess, collect graph, send it off, 
    // vector back containing size of system, etc.
    if(processID != 0)
      {
        result= sendGraph(theGraph,data);
        size = data(0);
        nnz = data(1);

        if(nnz > A.Size())
          rowA.resize(nnz);

        if(size > B.Size())
          colStartA.resize(size+1);      

        ID rowAdata(rowA);
        ID colStartAdata(colStartA);
        FEM_ObjectBroker theBroker;
        CommParameters cp(0,*theChannels[0],theBroker);
        cp.receiveID(rowAdata,DistributedBandLinSOE::getDbTagData(),CommMetaData(3));
        cp.receiveID(colStartAdata,DistributedBandLinSOE::getDbTagData(),CommMetaData(4));
      }
    else
     {
       // if main domain, collect graphs from all subdomains,
       // merge into 1, number this one, send to subdomains the
       // id containing dof tags & start id's.
    
       // from each distributed soe recv it's graph
       // and merge them into master graph
       getSubGraphs(theGraph);
       calcNonZeros(theGraph);
       data(0) = size;
       data(1) = nnz;
       sendSizeData(data);

       if(nnz > A.Size())
         { // we have to get more space for A and rowA
           rowA.resize(nnz);
           workArea.resize(nnz);
         }

       if(size > Bsize)
         { // we have to get space for the vectors
           colStartA.resize(size+1);
         }

       // fill in colStartA and rowA
       if(size!=0)
         {
           colStartA(0)= 0;
           int startLoc = 0;
           int lastLoc = 0;
           for(int a=0; a<size; a++)
             {
               Vertex *theVertex = theGraph.getVertexPtr(a);
               if(theVertex == 0)
                 {
                   std::cerr << "WARNING:XC::SparseGenColLinSOE::setSize :";
                   std::cerr << " vertex " << a << " not in graph! - size set to 0\n";
                   size = 0;
                   return -1;
                 }
        
               rowA(lastLoc++) = theVertex->getTag(); // place diag in first
               const std::set<int> &theAdjacency = theVertex->getAdjacency();
        
               // now we have to place the entries in the XC::ID into order in rowA
               for(std::set<int>::const_iterator i=theAdjacency.begin(); i!=theAdjacency.end(); i++)
                 {
                   const int row= *i;
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
                    if(foundPlace == false) // put in at the end
                      rowA(lastLoc) = row;
                    lastLoc++;
                 }
               colStartA(a+1) = lastLoc;;            
               startLoc = lastLoc;
             }
         }
       ID rowAdata(rowA);
       ID colStartAdata(colStartA);
       for(size_t j=0; j<theChannels.size(); j++)
         {
           CommParameters cp(0,*theChannels[j]);
           cp.sendID(rowAdata,DistributedBandLinSOE::getDbTagData(),CommMetaData(3));//XXX asignar posición.
           cp.sendID(colStartAdata,DistributedBandLinSOE::getDbTagData(),CommMetaData(4));//XXX asignar posición.
         }
      }
    if(nnz > A.Size())
      { // we have to get more space for A and rowA
        A.resize(nnz);
      }
    A.Zero();  

    factored = false;

    if(size > B.Size())
      { // we have to get space for the vectors
    
        // delete the old
        inic(size);        
        myB.resize(size);
    
      }

    setupMyB(oldSize,size);
  
    // invoke setSize() on the Solver
    result= setSolverSize();
    return result;    
  }


int XC::DistributedSparseGenColLinSOE::addA(const Matrix &m, const ID &id, double fact)
 {
    // check for a XC::quick return 
    if(fact == 0.0)  
        return 0;

    const int idSize = id.Size();
    
    // check that m and id are of similar size
    if(idSize != m.noRows() && idSize != m.noCols())
      {
        std::cerr << "SparseGenColLinSOE::addA() ";
        std::cerr << " - Matrix and XC::ID not of similar sizes\n";
        return -1;
      }
    
    if(fact == 1.0)
      { // do not need to multiply 
        for(int i=0; i<idSize; i++)
          {
            const int col = id(i);
            if(col < size && col >= 0)
              {
                const int startColLoc= colStartA(col);
                const int endColLoc= colStartA(col+1);
                for(int j=0; j<idSize; j++)
                  {
                    const int row = id(j);
                    if(row <size && row >= 0)
                      {
                        // find place in A using rowA
                        for(int k=startColLoc; k<endColLoc; k++)
                          if(rowA(k) == row)
                            {
                              A[k] += m(j,i);
                              k = endColLoc;
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
                const int startColLoc= colStartA(col);
                const int endColLoc= colStartA(col+1);
                for(int j=0; j<idSize; j++)
                  {
                    const int row = id(j);
                    if(row <size && row >= 0)
                      {
                        // find place in A using rowA
                        for(int k=startColLoc; k<endColLoc; k++)
			  if(rowA(k) == row)
                          {
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

int XC::DistributedSparseGenColLinSOE::addB(const Vector &v, const ID &id,const double &fact)
  { return DistributedBandLinSOE::addB(size,v,id,fact); }


int XC::DistributedSparseGenColLinSOE::setB(const Vector &v,const double &fact)
  { return DistributedBandLinSOE::setB(size,v,fact); }


int XC::DistributedSparseGenColLinSOE::solve(void)
  {
    static ID result(1);
    if(processID != 0)
      {
        // if subprocess send B and A and receive back result X, B & result
        sendBArecvBX(*this,factored,A.getDataPtr(),A.Size(),result);
        factored = true;
      } 
    else
      {
         //if main process, recv B & A from all, solve
         //and send back X, B & result

         // add P0 contribution to B
         SparseGenColLinSOE::setB(myVectB);

         // receive X and A contribution from subprocess & add them in
         for(size_t j=0; j<theChannels.size(); j++)
           {
             // get X & add
             CommParameters cp(0,*theChannels[j]);
             receiveX(cp);
             SparseGenColLinSOE::addB(getX());

             if(!factored)
               {
                 Vector vectA(workArea);
                 cp.receiveVector(vectA,CommMetaData(1));//XXX asignar posición.
                 for(int i=0; i<nnz; i++)
                   A[i]+= workArea[i];
               }

             /*
             // get A & add using local map
             const XC::ID &localMap = *(localCol[j]);
             int localSize = localMap.Size() * half_band;
             Vector vectA(workArea, localSize);    
             theChannel->recvVector(0, 0, vectA);

             int loc = 0;
             for(int i=0; i<localMap.Size(); i++) {
             int pos = localMap(i)*half_band;
             for(int k=0; k<half_band; k++) 
               A[pos++] += workArea[loc++];
               }    
             */
           }

        // solve
        result(0) = this->LinearSOE::solve();
        sendResultsBack(*this,result);
      } 
    return result(0);
  }        


const XC::Vector &XC::DistributedSparseGenColLinSOE::getB(void) const
  {

    DistributedSparseGenColLinSOE *this_no_const= const_cast<DistributedSparseGenColLinSOE *>(this);
    if(processID != 0)
      {
        CommParameters cp(0,*theChannels[0]);
        // send B & recv merged B
        cp.sendVector(myVectB,CommMetaData(0));//XXX asignar posición.
        this_no_const->receiveB(cp);
      }
    else
      {
        // if main process, recv B & A from all, solve
        //and send back X, B & result
        recvBAsendBX(*this_no_const);
      } 
    return SparseGenColLinSOE::getB();
  }

  
int XC::DistributedSparseGenColLinSOE::sendSelf(CommParameters &cp)
  {
    const int retval= send(cp);
    return retval;
  }


int XC::DistributedSparseGenColLinSOE::recvSelf(const CommParameters &cp)
  {
    const int retval= receive(cp);
    return retval;
  }

