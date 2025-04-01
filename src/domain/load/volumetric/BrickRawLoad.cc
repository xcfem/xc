//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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
//BrickRawLoad.cpp

#include "BrickRawLoad.h"
#include "domain/mesh/element/utils/fvectors/FVectorBrick.h"
#include "domain/domain/Domain.h"
#include "domain/mesh/element/Element.h"
#include "utility/matrix/Matrix.h"
#include "utility/matrix/ID.h"
#include "utility/geom/pos_vec/SlidingVectorsSystem2d.h"
#include "utility/geom/pos_vec/SlidingVector2d.h"

//! @brief Default constructor.
XC::BrickRawLoad::BrickRawLoad(int tag)
  :BrickMecLoad(tag, LOAD_TAG_BrickRawLoad) {}

//! @brief Constructor.
//!
//! @param tag: load identifier.
//! @param nLoads: nodal loads.
//! @param theElementTags: tags of the loaded elements.
XC::BrickRawLoad::BrickRawLoad(int tag, const std::vector<Vector> &nLoads, const ID &theElementTags)
  : BrickMecLoad(tag, LOAD_TAG_BrickRawLoad, theElementTags),
    nodalLoads(nLoads) {}

//! @brief Return the category of this kind of loads.
std::string XC::BrickRawLoad::Category(void) const
  { return "raw"; }

//! @brief Returns force expressed in local coordinates.
XC::Vector XC::BrickRawLoad::getLocalForce(void) const
  {
    Vector retval(2);
    for(std::vector<Vector>::const_iterator i= nodalLoads.begin(); i!= nodalLoads.end();i++)
      {
	const Vector &nLoad= *i;
	retval(0)+= nLoad[0]; retval(1)+= nLoad[1];
      }
    return retval;
  }

//! @brief Returns the components of the force vectors.
const XC::Matrix &XC::BrickRawLoad::getLocalForces(void) const
  {
    static Matrix retval;
    const size_t sz= numElements();
    retval= Matrix(sz,3);
    const Vector f= getLocalForce();
    for(size_t i=0; i<sz; i++)
      { retval(i,0)= f[0]; retval(i,1)= f[1]; retval(i,2)= f[2]; }
    return retval;
  }

const XC::Vector &XC::BrickRawLoad::getData(int &type, const double &loadFactor) const
  {
    type = getClassTag();
    std::cerr << getClassName() << "::" << __FUNCTION__
              << " not implemented yet." << std::endl;
    static const Vector trash;
    return trash;
  }

//! @brief Adds the load al consistent load vector (see page 108 libro Eugenio Oñate).
//! @param areas tributary areas for each node.
//! @param loadFactor Load factor.
//! @param p0 element load vector.
void XC::BrickRawLoad::addReactionsInBasicSystem(const std::vector<double> &,const double &loadFactor, FVectorBrick &p0) const
  {
    const size_t sz= nodalLoads.size();
    // Reactions in basic system
    for(size_t i= 0;i<sz;i++)
      {
	const Vector f= nodalLoads[i]*loadFactor; // Load on node i.
        p0.addForce(i, f[0], f[1], f[2]); // i-th node.
      }
  }

//! @brief ??
//! @param areas tributary areas for each node.
//! @param loadFactor Load factor.
//! @param q0 
void XC::BrickRawLoad::addFixedEndForcesInBasicSystem(const std::vector<double> &,const double &,FVectorBrick &) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; not implemented." << std::endl;
  }

//! @brief Returns a vector to store the dbTags
//! of the class members.
XC::DbTagData &XC::BrickRawLoad::getDbTagData(void) const
  {
    static DbTagData retval(6);
    return retval;
  }

//! @brief Send data through the communicator argument.
int XC::BrickRawLoad::sendData(Communicator &comm)
  {
    int res= ThreedimLoad::sendData(comm);
    res+= comm.sendVectors(nodalLoads,getDbTagData(),CommMetaData(5));
    return res;
  }

//! @brief Receive data through the communicator argument.
int XC::BrickRawLoad::recvData(const Communicator &comm)
  {
    int res= ThreedimLoad::recvData(comm);
    res+= comm.receiveVectors(nodalLoads,getDbTagData(),CommMetaData(5));
    return res;
  }

//! @brief Send the object through the communicator argument.
int XC::BrickRawLoad::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(6);
    int result= sendData(comm);

    result+= comm.sendIdData(getDbTagData(),dataTag);
    if(result < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; failed to send extra data.\n";
    return result;
  }

//! @brief Receive the object through the communicator argument.
int XC::BrickRawLoad::recvSelf(const Communicator &comm)
  {
    inicComm(6);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; data could not be received.\n" ;
    else
      res+= recvData(comm);
    return res;
  }

void  XC::BrickRawLoad::Print(std::ostream &s, int flag) const
  {
    s << "BrickRawLoad - Reference load" << std::endl;
    int count= 0;
    for(std::vector<Vector>::const_iterator i= nodalLoads.begin(); i!= nodalLoads.end();i++)
      {
        s << "  load node( " << count << "): " << *i << std::endl;
      }
    s << "  Elements acted on: " << getElementTags();
  }
