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
                                                                        
                                                                        
// Written: Chris McGann
//          04.2009
// Modified: Chris McGann, 11.2010
//           Chris McGann, 02.2011 -> added elemental load 

// Description: This file contains the implementation for the QuadSurfaceLoad class.

#include "QuadSurfaceLoad.h"
#include "domain/load/ElementalLoad.h"
#include "domain/mesh/node/Node.h"

double XC::QuadSurfaceLoad::GsPts[QSL_NUM_NODE][1];

//! @brief Constructor.
XC::QuadSurfaceLoad::QuadSurfaceLoad(int tag, int Nd1, int Nd2, double pressure)
  : SurfaceLoadBase<QSL_NUM_NODE>(tag, ELE_TAG_QuadSurfaceLoad, pressure, 1.0),     
   tangentStiffness(QSL_NUM_DOF, QSL_NUM_DOF),
   internalForces(QSL_NUM_DOF),
   g(QSL_NUM_NDF), 
   myNhat(QSL_NUM_NDF), 
   myNI(QSL_NUM_NODE),
   dcrd1(QSL_NUM_NDF),
   dcrd2(QSL_NUM_NDF)
  {
    theNodes.set_id_nodes(Nd1,Nd2);

    GsPts[0][0]= 0.5;
    
    tangentStiffness.Zero();
  }

//! @brief Default constructor.
XC::QuadSurfaceLoad::QuadSurfaceLoad(int tag)
  :SurfaceLoadBase<QSL_NUM_NODE>(tag, ELE_TAG_QuadSurfaceLoad, 0.0, 1.0),     
   tangentStiffness(QSL_NUM_DOF, QSL_NUM_DOF),
   internalForces(QSL_NUM_DOF),
   g(QSL_NUM_NDF), 
   myNhat(QSL_NUM_NDF), 
   myNI(QSL_NUM_NODE),
   dcrd1(QSL_NUM_NDF),
   dcrd2(QSL_NUM_NDF)
  {}

XC::QuadSurfaceLoad::~QuadSurfaceLoad(void)
  {}

//! @brief Virtual constructor.
XC::Element *XC::QuadSurfaceLoad::getCopy(void) const
  { return new QuadSurfaceLoad(*this); }
    
void XC::QuadSurfaceLoad::setDomain(Domain *theDomain)
  {
    SurfaceLoadBase<QSL_NUM_NODE>::setDomain(theDomain);
    theNodes.checkNumDOF(QSL_NUM_NDF,getTag());

    dcrd1= theNodes[0]->getCrds();
    dcrd2= theNodes[1]->getCrds();
  }

//! @brief return number of dofs
int XC::QuadSurfaceLoad::getNumDOF(void) const
  { return QSL_NUM_DOF; }

//! @brief this function calculates g1, g2, NI, and nHat for given Xi and Eta
int XC::QuadSurfaceLoad::UpdateBase(double Xi) const
  {
    const double oneMinusXi= 1 - Xi;
    const double onePlusXi= 1 + Xi;

    // calculate vectors g1 and g2
    // g1 = d(x_Xi)/dXi, g2 = d(x_Xi)/dEta
    g= (dcrd2 - dcrd1).Normalized();

    // shape functions
    myNI(0) = 0.5 * oneMinusXi;
    myNI(1) = 0.5 * onePlusXi;

    // normal vector to primary surface as cross product of g1 and g2
    myNhat(0)= -g(1);
    myNhat(1)= g(0);

    return 0;
  }

const XC::Matrix &XC::QuadSurfaceLoad::getTangentStiff(void) const
  { return tangentStiffness; }

const XC::Matrix & XC::QuadSurfaceLoad::getInitialStiff(void) const
  { return getTangentStiff(); }
    

const XC::Vector &XC::QuadSurfaceLoad::getResistingForce(void) const
  {
    internalForces.Zero();

    // loop over Gauss points
    for(int i = 0; i < QSL_NUM_NODE; i++)
      {
	this->UpdateBase(GsPts[i][0]);

	// loop over nodes
	for(int j = 0; j < QSL_NUM_NODE; j++)
	  {
	    // loop over dof
	    for(int k = 0; k < 3; k++)
	      {
		internalForces[j*3+k]-=  mLoadFactor*my_pressure*myNhat(k)*myNI(j);
	      }
	  }
      }
    return internalForces;
  }

const XC::Vector &XC::QuadSurfaceLoad::getResistingForceIncInertia(void) const
  { return getResistingForce(); }

//! @brief Returns a vector to store the dbTags
//! of the class members.
XC::DbTagData &XC::QuadSurfaceLoad::getDbTagData(void) const
  {
    static DbTagData retval(18); 
    return retval;
  }

//! @brief Send object members through the communicator argument.
int XC::QuadSurfaceLoad::sendData(Communicator &comm)
  {
    int res= SurfaceLoadBase<QSL_NUM_NODE>::sendData(comm);
    res+=comm.sendDoubles(mLoadFactor,my_pressure,getDbTagData(),CommMetaData(7));
    res+= comm.sendVector(internalForces,getDbTagData(),CommMetaData(8));
    res+= comm.sendVector(theVector,getDbTagData(),CommMetaData(9));
    res+= comm.sendVector(g,getDbTagData(),CommMetaData(10));
    res+= comm.sendVector(myNhat,getDbTagData(),CommMetaData(11));
    res+= comm.sendVector(myNI,getDbTagData(),CommMetaData(12));
    res+= comm.sendVector(dcrd1,getDbTagData(),CommMetaData(13));
    res+= comm.sendVector(dcrd2,getDbTagData(),CommMetaData(14));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::QuadSurfaceLoad::recvData(const Communicator &comm)
  {
    int res= SurfaceLoadBase<QSL_NUM_NODE>::recvData(comm);
    res+=comm.receiveDoubles(mLoadFactor,my_pressure,getDbTagData(),CommMetaData(7));
    res+= comm.receiveVector(internalForces,getDbTagData(),CommMetaData(8));
    res+= comm.receiveVector(theVector,getDbTagData(),CommMetaData(9));
    res+= comm.receiveVector(g,getDbTagData(),CommMetaData(10));
    res+= comm.receiveVector(myNhat,getDbTagData(),CommMetaData(11));
    res+= comm.receiveVector(myNI,getDbTagData(),CommMetaData(12));
    res+= comm.receiveVector(dcrd1,getDbTagData(),CommMetaData(13));
    res+= comm.receiveVector(dcrd2,getDbTagData(),CommMetaData(14));
    return res;
  }

//! @brief Sends object through the communicator argument.
int XC::QuadSurfaceLoad::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(18);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; failed to send data.\n";
    return res;
  }

//! @brief Receives object through the communicator argument.
int XC::QuadSurfaceLoad::recvSelf(const Communicator &comm)
  {
    inicComm(18);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; failed to receive ids.\n";
    else
      {
        setTag(getDbTagDataPos(0));
        res+= recvData(comm);
        if(res<0)
          std::cerr << getClassName() << "::" << __FUNCTION__
		    << "failed to receive data.\n";
      }
    return res;
  }

void XC::QuadSurfaceLoad::Print(std::ostream &os, int flag)
  {
    os << getClassName() << ", element id:  " << this->getTag() << std::endl
       << "   Connected external nodes:  " << theNodes << std::endl; 
  }


