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
//ShellNLCrdTransf3d.cc

#include "ShellNLCrdTransf3d.h"
#include "utility/matrix/Matrix.h"
#include <domain/mesh/node/Node.h>

#include "utility/actor/objectBroker/FEM_ObjectBroker.h"
#include "utility/actor/actor/MovableVector.h"
#include "utility/matrix/ID.h"
#include "domain/mesh/element/utils/NodePtrs.h"
#include "domain/mesh/element/plane/shell/R3vectors.h"
#include "ShellMITC4Base.h"

//! @brief Default constructor
XC::ShellNLCrdTransf3d::ShellNLCrdTransf3d(void)
  : ShellCrdTransf3dBase() {}

//! @brief Constructor.
XC::ShellNLCrdTransf3d::ShellNLCrdTransf3d(const Vector &v1,const Vector &v2,const Vector &v3)
  : ShellCrdTransf3dBase(v1,v2,v3) {}

//! @brief Computes basic vectors from node coordinates.
XC::ShellNLCrdTransf3d::ShellNLCrdTransf3d(const NodePtrs &theNodes)
  : ShellCrdTransf3dBase()
  { initialize(theNodes); }


//! @brief Virtual constructor.
XC::ShellCrdTransf3dBase *XC::ShellNLCrdTransf3d::getCopy(void) const
  { return new ShellNLCrdTransf3d(*this); }

//! @brief Sets the transformation from the positions of the nodes.
int XC::ShellNLCrdTransf3d::initialize(const NodePtrs &ptrs)
  {
    ShellCrdTransf3dBase::initialize(ptrs);
    //could compute derivatives \frac{ \partial {\bf x} }{ \partial L_1 }
    //                     and  \frac{ \partial {\bf x} }{ \partial L_2 }
    //and use those as basis vectors but this is easier
    //and the shell is flat anyway.


    //get two vectors (v1, v2) in plane of shell by
    // nodal coordinate differences

    const Vector &coor0= (*theNodes)[0]->getCrds();
    const Vector &coor1= (*theNodes)[1]->getCrds();
    const Vector &coor2= (*theNodes)[2]->getCrds();
    const Vector &coor3= (*theNodes)[3]->getCrds();

    static Vector temp(3);
    static Vector v1(3);
    static Vector v2(3);
    static Vector v3(3);
    
    v1.Zero( );
    //v1= 0.5 * ( coor2 + coor1 - coor3 - coor0 );
    v1= coor2;
    v1+= coor1;
    v1-= coor3;
    v1-= coor0;
    v1*= 0.50;

    //normalize v1
    //double length= LovelyNorm( v1 );
    double length= v1.Norm( );
    v1/= length;

    v2.Zero( );
    //v2= 0.5 * ( coor3 + coor2 - coor1 - coor0 );
    v2= coor3;
    v2+= coor2;
    v2-= coor1;
    v2-= coor0;
    v2*= 0.50;

    //Gram-Schmidt process for v2

    //double alpha= LovelyInnerProduct( v2, v1 );
    const double alpha= v2^v1;

    //v2 -= alpha*v1;
    temp= v1;
    temp *= alpha;
    v2-= temp;


    //normalize v2
    //length= LovelyNorm( v2 );
    length= v2.Norm( );
    v2/= length;

    //cross product for v3
    v3= LovelyCrossProduct( v1, v2 );
    
    g1= v1; g2= v2; g3= v3;
    return 0;
  }

//! @brief Updates the transformation.
int XC::ShellNLCrdTransf3d::update(void)
  {
    //could compute derivatives \frac{ \partial {\bf x} }{ \partial L_1 }
    //                     and  \frac{ \partial {\bf x} }{ \partial L_2 }
    //and use those as basis vectors but this is easier
    //and the shell is flat anyway.

    static Vector temp(3);

    static Vector v1(3);
    static Vector v2(3);
    static Vector v3(3);


    const Vector &coor0= (*theNodes)[0]->getCrds() + (*theNodes)[0]->getTrialDisp();
    const Vector &coor1= (*theNodes)[1]->getCrds() + (*theNodes)[1]->getTrialDisp();
    const Vector &coor2= (*theNodes)[2]->getCrds() + (*theNodes)[2]->getTrialDisp();
    const Vector &coor3= (*theNodes)[3]->getCrds() + (*theNodes)[3]->getTrialDisp();

    v1.Zero();
    //v1= 0.5 * ( coor2 + coor1 - coor3 - coor0 );
    v1= coor2;
    v1+= coor1;
    v1-= coor3;
    v1-= coor0;
    v1*= 0.50;

    v2.Zero();
    //v2= 0.5 * ( coor3 + coor2 - coor1 - coor0 );
    v2= coor3;
    v2+= coor2;
    v2-= coor1;
    v2-= coor0;
    v2*= 0.50;
    /*//normalize v1
    //double length= LovelyNorm( v1);
    double length= v1.Norm();
    v1/= length;

    //Gram-Schmidt process for v2

    //double alpha= LovelyInnerProduct( v2, v1);
    double alpha= v2^v1;

    //v2-= alpha*v1;
    temp= v1;
    temp *= alpha;
    v2-= temp;

    //normalize v2
    //length= LovelyNorm( v2);
    length= v2.Norm();
    v2/= length;

    //cross product for v3
    v3= LovelyCrossProduct( v1, v2);*/

    // this procesure is simplified by Lisha Wang
    v3= LovelyCrossProduct(v1,v2);
    v2= LovelyCrossProduct(v3,v1);
    temp(0)= v1.Norm();
    temp(1)= v2.Norm();
    temp(2)= v3.Norm();

    v1/= temp(0);
    v2/= temp(1);
    v3/= temp(2);

    //basis vectors stored as array of doubles
    g1= v1; g2= v2; g3= v3;
    
    //local nodal coordinates in plane of shell
    setup_nodal_local_coordinates();
    return 0;
  }

//! @brief Commits state (normally after convergence is achieved).
int XC::ShellNLCrdTransf3d::commitState(void)
  { return 0; }


//! @brief Returns to the las committed state.
int XC::ShellNLCrdTransf3d::revertToLastCommit(void)
  { return 0; }


//! @brief Returns to the initial state.
int XC::ShellNLCrdTransf3d::revertToStart(void)
  { return 0; }

//! @brief Returns the displacements vector expressed on the basic system.
XC::Vector XC::ShellNLCrdTransf3d::getBasicTrialDisp(const int &i) const
  { return (*theNodes)[i]->getTrialDisp(); }

//! @brief Returns the velocities vector expressed on the basic system.
XC::Vector XC::ShellNLCrdTransf3d::getBasicTrialVel(const int &i) const
  { return (*theNodes)[i]->getTrialVel(); }

//! @brief Returns the accelerations vector expressed on the basic system.
XC::Vector XC::ShellNLCrdTransf3d::getBasicTrialAccel(const int &i) const
  { return (*theNodes)[i]->getTrialAccel(); }

//! @brief Returns the load vector expressend in global coordinates.
const XC::Vector &XC::ShellNLCrdTransf3d::getGlobalResistingForce(const Vector &p0) const
  {
    static Vector retval;
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; not implemented yet."
              << std::endl;
    return retval;
  }

//! @brief Send the object through the communicator argument.
int XC::ShellNLCrdTransf3d::sendSelf(Communicator &comm)
  {
    inicComm(3);
    int res= sendData(comm);

    const int dataTag= getDbTag();
    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; data could not be sent.\n";
    return res;
  }


//! @brief Receives object through the communicator argument.
int XC::ShellNLCrdTransf3d::recvSelf(const Communicator &comm)
  {
    inicComm(3);
    int res= comm.receiveIdData(getDbTagData(),getDbTag());
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; data could not be received.\n";
    else
      res+= recvData(comm);
    return res;
  }


