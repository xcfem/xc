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
                                                                        
                                                                        
// Written: Chris McGann
//          04.2009
// Modified: Chris McGann, 11.2010
//           Chris McGann, 02.2011 -> added elemental load 

// Description: This file contains the implementation for the BrickSurfaceLoad class.

#include "BrickSurfaceLoad.h"
#include "domain/load/ElementalLoad.h"
#include "domain/mesh/node/Node.h"
#include "vtkCellType.h"
#include "utility/utils/misc_utils/colormod.h"


double XC::BrickSurfaceLoad::GsPts[SL_NUM_NODE][2];
XC::Matrix XC::BrickSurfaceLoad::tangentStiffness(SL_NUM_DOF, SL_NUM_DOF);

//! @brief Constructor.
XC::BrickSurfaceLoad::BrickSurfaceLoad(int tag, int Nd1, int Nd2, int Nd3, int Nd4, double pressure)
  : SurfaceLoadBase<SL_NUM_NODE>(tag, ELE_TAG_BrickSurfaceLoad, pressure, 1.0),
    internalForces(SL_NUM_DOF),
    theCoordTransf(),
    myNI(SL_NUM_NODE),
    dcrd1(SL_NUM_NDF),
    dcrd2(SL_NUM_NDF),
    dcrd3(SL_NUM_NDF),
    dcrd4(SL_NUM_NDF)
  {
    theNodes.set_id_nodes(Nd1,Nd2,Nd3,Nd4);
    theCoordTransf.initialize(theNodes);
    GsPts[0][0] = -oneOverRoot3;
    GsPts[0][1] = -oneOverRoot3;
    GsPts[1][0] = oneOverRoot3;
    GsPts[1][1] = -oneOverRoot3;
    GsPts[2][0] = oneOverRoot3;
    GsPts[2][1] = oneOverRoot3;
    GsPts[3][0] = -oneOverRoot3;
    GsPts[3][1] = oneOverRoot3;
  }

//! @brief Default constructor.
XC::BrickSurfaceLoad::BrickSurfaceLoad(int tag)
  :SurfaceLoadBase<SL_NUM_NODE>(tag, ELE_TAG_BrickSurfaceLoad, 0.0, 1.0),     
   internalForces(SL_NUM_DOF),
   theCoordTransf(),
   myNI(SL_NUM_NODE),
   dcrd1(SL_NUM_NDF),
   dcrd2(SL_NUM_NDF),
   dcrd3(SL_NUM_NDF),
   dcrd4(SL_NUM_NDF)
  {}

XC::BrickSurfaceLoad::~BrickSurfaceLoad(void)
  {}

//! @brief Virtual constructor.
XC::Element *XC::BrickSurfaceLoad::getCopy(void) const
  { return new BrickSurfaceLoad(*this); }
    
void XC::BrickSurfaceLoad::setDomain(Domain *theDomain)
  {
    SurfaceLoadBase<SL_NUM_NODE>::setDomain(theDomain);
    theNodes.checkNumDOF(3,getTag());

    dcrd1= theNodes[0]->getCrds();
    dcrd2= theNodes[1]->getCrds();
    dcrd3= theNodes[2]->getCrds();
    dcrd4= theNodes[3]->getCrds();

  }

//! @brief return number of dofs
int XC::BrickSurfaceLoad::getNumDOF(void) const
  { return SL_NUM_DOF; }

//! @brief this function calculates g1, g2, NI, and nHat for given Xi and Eta
int XC::BrickSurfaceLoad::UpdateBase(double Xi, double Eta) const
  {
    const double oneMinusEta= 1 - Eta;
    const double onePlusEta= 1 + Eta;
    const double oneMinusXi= 1 - Xi;
    const double onePlusXi= 1 + Xi;

    // calculate vectors g1 and g2
    // g1 = d(x_Xi)/dXi, g2 = d(x_Xi)/dEta
    const Vector g1= (oneMinusEta * (dcrd2 - dcrd1) + onePlusEta  * (dcrd3 - dcrd4)) * 0.25;
    const Vector g2= (onePlusXi   * (dcrd3 - dcrd2) + oneMinusXi  * (dcrd4 - dcrd1)) * 0.25;

    // shape functions
    myNI(0) = 0.25 * oneMinusXi * oneMinusEta;
    myNI(1) = 0.25 * onePlusXi  * oneMinusEta;
    myNI(2) = 0.25 * onePlusXi  * onePlusEta;
    myNI(3) = 0.25 * oneMinusXi * onePlusEta;

    // normal vector to primary surface as cross product of g1 and g2
    Vector myNhat(SL_NUM_NDF);
    myNhat(0) = g1(1)*g2(2) - g1(2)*g2(1);
    myNhat(1) = g1(2)*g2(0) - g1(0)*g2(2);
    myNhat(2) = g1(0)*g2(1) - g1(1)*g2(0);
    
    theCoordTransf.initialize(theNodes);
    theCoordTransf.setUnitVectors(g1, g2, myNhat);
    return 0;
  }

//! @brief Return the element dimension (0, 1, 2 or 3).
size_t XC::BrickSurfaceLoad::getDimension(void) const
  { return 2; }

//! @brief Returns a pointer to the coordinate transformation.
XC::ShellLinearCrdTransf3d *XC::BrickSurfaceLoad::getCoordTransf(void)
  { 
    return &theCoordTransf;
  }

//! @brief Returns (if possible) a pointer to the coordinate transformation.
const XC::ShellLinearCrdTransf3d *XC::BrickSurfaceLoad::getCoordTransf(void) const
  {
    return &theCoordTransf;
  }

//! @brief Returns the positions of the element nodes
//! (without duplicates corresponding to degenerated elements).
std::deque<Pos3d> XC::BrickSurfaceLoad::getNodePositions(bool initialGeometry) const
  {
    const std::deque<Pos3d> tmp= this->getPosNodes(initialGeometry);
    // Filter collapsed sides.
    std::deque<Pos3d> retval;
    std::deque<Pos3d>::const_iterator i= tmp.begin();
    Pos3d p0= *i;
    i++;
    retval.push_back(p0);
    for(;i!= tmp.end();i++)
      {
	Pos3d p1= *i;
	if(dist2(p0,p1)>1e-12)
	  retval.push_back(p1);
	p0= p1;
      }
    // Repeated nodes removed (if any).
    return retval;
  }

//! @brief Returns the element contour as a polygon.
//! @param initialGeometry: if true returns the area of the undeformed geometry.
Polygon3d XC::BrickSurfaceLoad::getPolygon(bool initialGeometry) const
  {
    const std::deque<Pos3d> positions= getNodePositions(initialGeometry);
    Polygon3d retval(positions.begin(),positions.end());
    return retval;
  }

//! @brief Returns element area.
//!
//! @param initialGeometry: if true returns the area of the undeformed geometry.
double XC::BrickSurfaceLoad::getArea(bool initialGeometry) const
  { return getPolygon(initialGeometry).getArea(); }

//! @brief VTK interface.
int XC::BrickSurfaceLoad::getVtkCellType(void) const
  { return VTK_QUAD; }

const XC::Matrix &XC::BrickSurfaceLoad::getTangentStiff(void) const
  { return tangentStiffness; }

const XC::Matrix & XC::BrickSurfaceLoad::getInitialStiff(void) const
  { return getTangentStiff(); }
    

const XC::Vector &XC::BrickSurfaceLoad::getResistingForce(void) const
  {
    internalForces.Zero();

    const double factoredPressure= mLoadFactor*my_pressure;
    // loop over Gauss points
    for(int i = 0; i < SL_NUM_NODE; i++)
      {
	this->UpdateBase(GsPts[i][0],GsPts[i][1]);

	// loop over nodes
	for(int j = 0; j < SL_NUM_NODE; j++)
	  {
	    const double mynij= myNI(j);
	    // loop over dof
	    for(int k = 0; k < 3; k++)
	      {
		const Vector &myNhat= theCoordTransf.G3();
		internalForces[j*3+k]-= factoredPressure*myNhat(k)*mynij;
	      }
	  }
      }
    return internalForces;
  }

const XC::Vector &XC::BrickSurfaceLoad::getResistingForceIncInertia(void) const
  { return getResistingForce(); }

//! @brief Returns a vector to store the dbTags
//! of the class members.
XC::DbTagData &XC::BrickSurfaceLoad::getDbTagData(void) const
  {
    static DbTagData retval(18); 
    return retval;
  }

//! @brief Send object members through the communicator argument.
int XC::BrickSurfaceLoad::sendData(Communicator &comm)
  {
    int res= SurfaceLoadBase<SL_NUM_NODE>::sendData(comm);
    res+=comm.sendDoubles(mLoadFactor,my_pressure,getDbTagData(),CommMetaData(7));
    res+= comm.sendVector(internalForces,getDbTagData(),CommMetaData(8));
    //res+= comm.sendVector(theVector,getDbTagData(),CommMetaData(9));
    res+= comm.sendMovable(theCoordTransf,getDbTagData(),CommMetaData(9));
    res+= comm.sendVector(myNI,getDbTagData(),CommMetaData(10));
    res+= comm.sendVector(dcrd1,getDbTagData(),CommMetaData(11));
    res+= comm.sendVector(dcrd2,getDbTagData(),CommMetaData(12));
    res+= comm.sendVector(dcrd3,getDbTagData(),CommMetaData(13));
    res+= comm.sendVector(dcrd4,getDbTagData(),CommMetaData(14));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::BrickSurfaceLoad::recvData(const Communicator &comm)
  {
    int res= SurfaceLoadBase<SL_NUM_NODE>::recvData(comm);
    res+=comm.receiveDoubles(mLoadFactor,my_pressure,getDbTagData(),CommMetaData(7));
    res+= comm.receiveVector(internalForces,getDbTagData(),CommMetaData(8));
    //res+= comm.receiveVector(theVector,getDbTagData(),CommMetaData(9));
    res+= comm.receiveMovable(theCoordTransf,getDbTagData(),CommMetaData(9));
    res+= comm.receiveVector(myNI,getDbTagData(),CommMetaData(10));
    res+= comm.receiveVector(dcrd1,getDbTagData(),CommMetaData(11));
    res+= comm.receiveVector(dcrd2,getDbTagData(),CommMetaData(12));
    res+= comm.receiveVector(dcrd3,getDbTagData(),CommMetaData(13));
    res+= comm.receiveVector(dcrd4,getDbTagData(),CommMetaData(14));
    return res;
  }

//! @brief Sends object through the communicator argument.
int XC::BrickSurfaceLoad::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(18);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << "; - failed to send data."
	        << Color::def << std::endl;
    return res;
  }

//! @brief Receives object through the communicator argument.
int XC::BrickSurfaceLoad::recvSelf(const Communicator &comm)
  {
    inicComm(18);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< "; data could not be received"
		<< Color::def << std::endl;
    else
      {
        setTag(getDbTagDataPos(0));
        res+= recvData(comm);
        if(res<0)
          std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		    << "; failed to receive data."
		    << Color::def << std::endl;
      }
    return res;
  }

void XC::BrickSurfaceLoad::Print(std::ostream &os, int flag)
  {
    os << getClassName() << ", element id:  " << this->getTag() << std::endl
       << "   Connected external nodes:  " << theNodes << std::endl; 
  }


