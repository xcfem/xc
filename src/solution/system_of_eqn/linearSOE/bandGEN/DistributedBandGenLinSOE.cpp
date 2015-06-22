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
// $Date: 2005/12/06 22:02:08 $
// $Source: /usr/local/cvs/OpenSees/SRC/system_of_eqn/linearSOE/bandGEN/DistributedBandGenLinSOE.cpp,v $
                                                                        
// Written: fmk 
//
// Description: This file contains the implementation for XC::BandGenLinSOE


#include <solution/system_of_eqn/linearSOE/bandGEN/DistributedBandGenLinSOE.h>
#include <solution/system_of_eqn/linearSOE/bandGEN/BandGenLinSolver.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include "solution/graph/graph/Graph.h"
#include <solution/graph/graph/Vertex.h>
#include <solution/graph/graph/VertexIter.h>
#include <utility/actor/channel/Channel.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>

//! @brief Constructor.
XC::DistributedBandGenLinSOE::DistributedBandGenLinSOE(SoluMethod *owr)
  :BandGenLinSOE(owr,LinSOE_TAGS_DistributedBandGenLinSOE), DistributedBandLinSOE() {}


//! @brief Calcula los valores de numSubD y numSuperD.
void XC::DistributedBandGenLinSOE::calcBand(Graph &theGraph)
  {
    size= theGraph.getNumVertex();
    theGraph.getBand(numSubD,numSuperD);
  }

int XC::DistributedBandGenLinSOE::setSize(Graph &theGraph)
  {
    int result = 0;
    int oldSize = size;
    static ID data(3);
    // if subprocess, collect graph, send it off, 
    // vector back containing size of system, etc.
    if(processID != 0)
      {
        result= sendGraph(theGraph,data);
        size= data(0);
        numSubD= data(1);
        numSuperD= data(2);
      } 
    // if main domain, collect graphs from all subdomains,
    // merge into 1, number this one, send to subdomains the
    // id containing dof tags & start id's.
    else
      {

        // from each distributed soe recv it's graph
        // and merge them into master graph

        getSubGraphs(theGraph);
        calcBand(theGraph);
    
        // to each distributed soe send the size data
        // and merge them into master graph
        data(0) = size;
        data(1) = numSubD;
        data(2) = numSuperD;
        sendSizeData(data);

      }

    const int numCols = theGraph.getNumVertex();
    calcLocalMap(size);


    int newSize;
    if(processID != 0)
      newSize = numCols * (2*numSubD + numSuperD + 1);
    else
      newSize = size * (2*numSubD + numSuperD + 1);

    // newSize = size * (2*numSubD + numSuperD + 1);

    if(newSize != A.Size())
      { // we have to get another space for A

        if(processID == 0)
          {
            workArea.resize(newSize);
          }

        A.resize(newSize);
      }
    A.Zero();
    factored = false;

  
    if(size > B.Size())
      { // we have to get space for the vectors
        inic(size);
        myB.resize(size);    
      }
    setupMyB(oldSize,size);
  
     // invoke setSize() on the Solver
     result= setSolverSize();
     return result;    
  }


int XC::DistributedBandGenLinSOE::addA(const Matrix &m, const ID &id, double fact)
  {
    // check for a XC::quick return 
    if(fact == 0.0)  return 0;
  
    // check that m and id are of similar size
    int idSize = id.Size();    
    if(idSize != m.noRows() && idSize != m.noCols())
      {
        std::cerr << "XC::BandGenLinSOE::addA()	- Matrix and XC::ID not of similar sizes\n";
        return -1;
      }
  
    int ldA = 2*numSubD + numSuperD + 1;
    ID *theMap= nullptr;
    const int numChannels= theChannels.size();
    if(numChannels > 0)
      theMap= &localCol[0];

    if(fact == 1.0)
      { // do not need to multiply 
        for(int i=0; i<idSize; i++)
          {
            int col = id(i);
            if(col < size && col >= 0)
              {
	        // double *coliiPtr = A.getDataPtr() + col*ldA + numSubD + numSuperD;
	
	        double *coliiPtr;
	        if(processID == 0)
	          coliiPtr = A.getDataPtr() + col*ldA + numSubD + numSuperD;
	        else
	          coliiPtr = A.getDataPtr() + ((*theMap)(col))*ldA + numSubD + numSuperD;
	
	        for(int j=0; j<idSize; j++)
                  {
                    const int row = id(j);
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
            const int col = id(i);
            if(col < size && col >= 0)
              {
                // double *coliiPtr = A.getDataPtr() + col*ldA + numSubD + numSuperD;
	
	        double *coliiPtr;
	        if(processID == 0)
	          coliiPtr = A.getDataPtr() + col*ldA + numSubD + numSuperD;
	        else
	          coliiPtr = A.getDataPtr() + (*theMap)(col)*ldA + numSubD + numSuperD;

	        for(int j=0; j<idSize; j++)
                  {
                    const int row = id(j);
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
    return 0;
  }

int XC::DistributedBandGenLinSOE::solve(void)
  {
    static ID result(1);

    if(processID != 0) 
      {
        // if subprocess send B and A and receive back result X, B & result
        sendBArecvBX(*this,factored,A.getDataPtr(),workArea.size(),result);
        factored = true;
      } 
    else //if main process, recv B & A from all, solve and send back X, B & result
      {
        const int ldA = 2*numSubD + numSuperD + 1;    
        recvXA(*this,factored,A.getDataPtr(),ldA,result);
      } 
    return result(0);
  }


int XC::DistributedBandGenLinSOE::addB(const Vector &v, const ID &id,const double &fact)
  { return DistributedBandLinSOE::addB(size,v,id,fact); }
    

int XC::DistributedBandGenLinSOE::setB(const Vector &v,const double &fact)
  { return DistributedBandLinSOE::setB(size,v,fact); }

XC::Vector &XC::DistributedBandGenLinSOE::getB(void)
  { return BandGenLinSOE::getB(); }

const XC::Vector &XC::DistributedBandGenLinSOE::getB(void) const
  {
    DistributedBandGenLinSOE *this_no_const= const_cast<DistributedBandGenLinSOE *>(this);
    if(processID != 0)
      {
        CommParameters cp(0,*theChannels[0]);
        // send B & recv merged B
        cp.sendVector(myVectB,CommMetaData(0));//XXX asignar posición.
        this_no_const->receiveB(cp);
      }
    else
      {
        //
        // if main process, recv B & A from all, solve and send back X, B & result
        //
        recvBAsendBX(*this_no_const);
      }     
    return BandGenLinSOE::getB();
  }	

  
int XC::DistributedBandGenLinSOE::sendSelf(CommParameters &cp)
  {
    const int retval= send(cp);
    return retval;
  }


int XC::DistributedBandGenLinSOE::recvSelf(const CommParameters &cp)
  {
    const int retval= receive(cp);
    return retval;
  }


