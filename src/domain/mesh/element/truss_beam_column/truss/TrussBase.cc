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
//TrussBase.cpp

#include "domain/mesh/element/truss_beam_column/truss/TrussBase.h"
#include <domain/mesh/node/Node.h>

#include "utility/actor/actor/CommMetaData.h"


//! @brief Pone a cero los pointers to node, los cosenos directories y los sensibility parameters.
void XC::TrussBase::initialize(void)
  {
    // set node pointers to nullptr
    theNodes.inic();
    cosX[0]= 0.0;
    cosX[1]= 0.0;
    cosX[2]= 0.0;
    L= 0.0;
  }

void XC::TrussBase::set_load(const Vector &v)
  {
    // get a copy of the material and check we obtained a valid copy
    load= v;
  }

void XC::TrussBase::alloc_load(const size_t &sz)
  { load.resize(sz); }

//! @brief constructor:
//!  responsible for allocating the necessary space needed by each object
//!  and storing the tags of the truss end nodes.
XC::TrussBase::TrussBase(int classTag,int tag,int dim,int Nd1, int Nd2)
  :ProtoTruss(tag,classTag,Nd1,Nd2,0,dim), L(0.0)
  { initialize(); }

//! @brief constructor:
//!  responsible for allocating the necessary space needed by each object
//!  and storing the tags of the truss end nodes.
XC::TrussBase::TrussBase(int classTag,int tag,int dim)
  :ProtoTruss(tag,classTag,0,0,0,dim), L(0.0)
  { initialize(); }


//! constructor:
//!   invoked by a FEM_ObjectBroker - blank object that recvSelf needs
//!   to be invoked upon
XC::TrussBase::TrussBase(int classTag)
  :ProtoTruss(0,classTag,0,0,0,0), L(0.0)
  { initialize(); }

//! @brief Copy constructor.
XC::TrussBase::TrussBase(const TrussBase &other)
  :ProtoTruss(other), L(other.L) {}

//! @brief Assignment operator.
XC::TrussBase &XC::TrussBase::operator=(const TrussBase &other)
  {
    ProtoTruss::operator=(other);
    L= other.L;
    cosX[0]= other.cosX[0];
    cosX[1]= other.cosX[1];
    cosX[2]= other.cosX[2];
    return *this;
  }

//! @brief Determine the length, cosines and fill in the transformation
void XC::TrussBase::setup_L_cos_dir(void)
  {
    // NOTE t = -t(every one else uses for residual calc)
    const Vector &end1Crd = theNodes[0]->getCrds();
    const Vector &end2Crd = theNodes[1]->getCrds();	
    if(getNumDIM() == 1)
      {
	double dx = end2Crd(0)-end1Crd(0);	
	L= sqrt(dx*dx);
	
	if(L == 0.0)
          {
	    std::cerr << "WARNING XC::TrussBase::setDomain() - truss " << this->getTag() << " has zero length\n";
	    return;
	  }	
	cosX[0] = 1.0;

      }
    else if(getNumDIM() == 2)
      {
	double dx = end2Crd(0)-end1Crd(0);
	double dy = end2Crd(1)-end1Crd(1);	
    	L = sqrt(dx*dx + dy*dy);
    	if(L == 0.0)
          {
	    std::cerr << "WARNING XC::TrussBase::setDomain() - truss " << this->getTag() << " has zero length\n";
	    return;
	  }
	cosX[0] = dx/L;
	cosX[1] = dy/L;

      }
    else
      {
	double dx = end2Crd(0)-end1Crd(0);
	double dy = end2Crd(1)-end1Crd(1);	
	double dz = end2Crd(2)-end1Crd(2);		
	L = sqrt(dx*dx + dy*dy + dz*dz);
	if(L == 0.0)
          {
	    std::cerr << "WARNING XC::TrussBase::setDomain() - truss " << this->getTag() << " has zero length\n";
	    return;
	  }
	cosX[0] = dx/L;
	cosX[1] = dy/L;
	cosX[2] = dz/L;	
      }
  }

//! @brief Return the length of the element.
const double &XC::TrussBase::getL(void) const
  { return L; }

//! @brief Send members through the channel being passed as parameter.
int XC::TrussBase::sendData(Communicator &comm)
  {
    int res= ProtoTruss::sendData(comm);
    res+= comm.sendDoubles(L,cosX[0],cosX[1],cosX[2],getDbTagData(),CommMetaData(16));
    return res;
  }

//! @brief Receives members through the channel being passed as parameter.
int XC::TrussBase::recvData(const Communicator &comm)
  {
    int res= ProtoTruss::recvData(comm);
    res+= comm.receiveDoubles(L,cosX[0],cosX[1],cosX[2],getDbTagData(),CommMetaData(16));
    return res;
  }
