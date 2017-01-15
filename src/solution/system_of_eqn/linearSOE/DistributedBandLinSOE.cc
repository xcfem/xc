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
//DistributedBandLinSOE.cc

#include "DistributedBandLinSOE.h"
#include "solution/system_of_eqn/linearSOE/LinearSOEData.h"
#include "solution/system_of_eqn/linearSOE/bandGEN/BandGenLinSOE.h"
#include <utility/matrix/Matrix.h>
#include "solution/graph/graph/Graph.h"
#include <utility/actor/channel/Channel.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>

XC::DistributedBandLinSOE::DistributedBandLinSOE(void)
  : DistributedLinSOE() {}


void XC::DistributedBandLinSOE::setupMyB(const size_t &oldSize,const size_t &size)
  {
    if(myB.size()<size)
      myB.resize(size);

    // zero the vectors
    for(size_t j=0; j<size; j++)
      { myB[j]= 0.0; }
  
    if(oldSize!=size)
      myVectB.resize(size);
  }

int XC::DistributedBandLinSOE::addB(const int &size,const Vector &v, const ID &id,const double &fact)
  {
    
    // check for a quick return 
    if(fact == 0.0)  return 0;

    // check that m and id are of similar size
    int idSize = id.Size();        
    if(idSize != v.Size() )
      {
	std::cerr << "DistributedGenLinSOE::addB() - Vector and XC::ID not of similar sizes\n";
	return -1;
      }    
    
    if(fact == 1.0)
      { // do not need to multiply if fact == 1.0
	for(int i=0; i<idSize; i++)
          {
	    int pos= id(i);
	    if(pos <size && pos >= 0)
	      myB[pos] += v(i);
	  }
      }
    else if(fact == -1.0)
      {
	for(int i=0; i<idSize; i++)
          {
	    int pos= id(i);
	    if(pos <size && pos >= 0)
	      myB[pos]-= v(i);
	  }
      }
    else
      {
	for(int i=0; i<idSize; i++)
          {
	    int pos = id(i);
	    if(pos <size && pos >= 0)
	      myB[pos] += v(i) * fact;
	  }
      }	
    return 0;
  }

int XC::DistributedBandLinSOE::setB(const int &size,const Vector &v,const double &fact)
  {
    // check for a quick return 
    if(fact == 0.0)  return 0;

    if(v.Size() != size)
      {
	std::cerr << "WARNING XC::DistributedBandLinSOE::setB() -";
	std::cerr << " incomptable sizes " << size << " and " << v.Size() << std::endl;
	return -1;
      }
    
    if(fact == 1.0)
      { // do not need to multiply if fact == 1.0
	for(int i=0; i<size; i++)
          { myB[i] = v(i); }
      }
    else if(fact == -1.0)
      {
	for(int i=0; i<size; i++)
          { myB[i] = -v(i); }
      }
    else
      {
	for(int i=0; i<size; i++)
          { myB[i] = v(i) * fact; }
      }	
    return 0;
  }

void XC::DistributedBandLinSOE::zeroB(void)
  {
    for(std::vector<double>::iterator i=myB.begin(); i<myB.end(); i++)
      *i= 0.0;
  }



int XC::DistributedBandLinSOE::sendBArecvBX(LinearSOEData &soe,const bool &factored,double *A,const int &Asize,ID &result)
  {
    CommParameters cp(0,*theChannels[0]);
    inicComm(4);
    // send B
    int res= cp.sendVector(myVectB,getDbTagData(),CommMetaData(0));

    // send A in packets placed in vector X
    //    Vector vectA(A, Asize);    
    if(!factored)
      {
        Vector vectA(A, Asize);        
        res+= cp.sendVector(vectA,getDbTagData(),CommMetaData(1));//XXX asignar posición.
      }

    soe.receiveBX(cp);
    res+= cp.receiveID(result,getDbTagData(),CommMetaData(2));
    return res;
  }

int XC::DistributedBandLinSOE::recvBAsendBX(FactoredSOEBase &soe) const
  {
    //
    // recv B & A from all, solve and send back X, B & result
    //
    soe.setB(myVectB);

    const int numChannels= theChannels.size();
    int res= 0;
    // receive X and A contribution from subprocess & add them in
    for(int j=0; j<numChannels; j++)
      {
        CommParameters cp(0,*theChannels[j]);
        res+= cp.receiveVector(workArea,getDbTagData(),CommMetaData(0));//XXX asignar posición.
        soe.addB(Vector(workArea));
      }
  
    // send results back
    for(int j=0; j<numChannels; j++)
      {
        CommParameters cp(0,*theChannels[j]);
        soe.sendB(cp);
      }
    return res;
  }

int XC::DistributedBandLinSOE::sendResultsBack(FactoredSOEBase &soe,ID &result)
  {
    int res= 0;
    // send results back
    for(size_t j=0; j<theChannels.size(); j++)
      {
        CommParameters cp(0,*theChannels[j]);
        soe.sendBX(cp);
        res+= cp.sendID(result,getDbTagData(),CommMetaData(0));//XXX asignar posición.
      }
    return res;
  }

int XC::DistributedBandLinSOE::recvXA(FactoredSOEBase &soe,const bool &factored,double *A,const int &ldA,ID &result)
  {
    // add P0 contribution to B
    soe.setB(myVectB);

    // receive X and A contribution from subprocess & add them in
    const int numChannels= theChannels.size();
    int res= 0;
    for(int j=0; j<numChannels; j++)
      {
        // get X & add
        CommParameters cp(0,*theChannels[j]);
        soe.receiveX(cp);
        soe.addB(soe.getX());

        // get A & add using local map
        if(!factored)
          {
            const ID &localMap= localCol[j];
            const size_t localSize = localMap.Size() * ldA;
            workArea.resize(localSize);
            Vector vectA(workArea);
            res+= cp.receiveVector(vectA,getDbTagData(),CommMetaData(1));//XXX asignar posición.

            int loc = 0;
            for(int i=0; i<localMap.Size(); i++)
              {
                int pos = localMap(i)*ldA;
                for(int k=0; k<ldA; k++) 
                  A[pos++] += workArea[loc++];
	      }
          }
      }
    // solve
    result(0)= ((LinearSOE &)soe).solve();
    res+= sendResultsBack(soe,result);
    return res;
  }

