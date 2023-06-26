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

double XC::BrickSurfaceLoad::GsPts[SL_NUM_NODE][2];

//! @brief Constructor.
XC::BrickSurfaceLoad::BrickSurfaceLoad(int tag, int Nd1, int Nd2, int Nd3, int Nd4, double pressure)
 : ElementBase<SL_NUM_NODE>(tag, ELE_TAG_BrickSurfaceLoad),     
   tangentStiffness(SL_NUM_DOF, SL_NUM_DOF),
   internalForces(SL_NUM_DOF),
   my_pressure(pressure),
   g1(SL_NUM_NDF), 
   g2(SL_NUM_NDF),
   myNhat(SL_NUM_NDF), 
   myNI(SL_NUM_NODE),
   dcrd1(SL_NUM_NDF),
   dcrd2(SL_NUM_NDF),
   dcrd3(SL_NUM_NDF),
   dcrd4(SL_NUM_NDF),
   mLoadFactor(1.0)
  {
    theNodes.set_id_nodes(Nd1,Nd2,Nd3,Nd4);

    GsPts[0][0] = -oneOverRoot3;
    GsPts[0][1] = -oneOverRoot3;
    GsPts[1][0] = oneOverRoot3;
    GsPts[1][1] = -oneOverRoot3;
    GsPts[2][0] = oneOverRoot3;
    GsPts[2][1] = oneOverRoot3;
    GsPts[3][0] = -oneOverRoot3;
    GsPts[3][1] = oneOverRoot3;

    tangentStiffness.Zero();
  }

//! @brief Default constructor.
XC::BrickSurfaceLoad::BrickSurfaceLoad(int tag)
  :ElementBase<SL_NUM_NODE>(tag, ELE_TAG_BrickSurfaceLoad),     
   tangentStiffness(SL_NUM_DOF, SL_NUM_DOF),
   internalForces(SL_NUM_DOF),
   my_pressure(0.0),
   g1(SL_NUM_NDF), 
   g2(SL_NUM_NDF),
   myNhat(SL_NUM_NDF), 
   myNI(SL_NUM_NODE),
   dcrd1(SL_NUM_NDF),
   dcrd2(SL_NUM_NDF),
   dcrd3(SL_NUM_NDF),
   dcrd4(SL_NUM_NDF),
   mLoadFactor(1.0)
  {}

XC::BrickSurfaceLoad::~BrickSurfaceLoad(void)
  {}

//! @brief Virtual constructor.
XC::Element *XC::BrickSurfaceLoad::getCopy(void) const
  { return new BrickSurfaceLoad(*this); }
    
void XC::BrickSurfaceLoad::setDomain(Domain *theDomain)
  {
    ElementBase<SL_NUM_NODE>::setDomain(theDomain);
    theNodes.checkNumDOF(3,getTag());

    dcrd1= theNodes[0]->getCrds();
    dcrd2= theNodes[1]->getCrds();
    dcrd3= theNodes[2]->getCrds();
    dcrd4= theNodes[3]->getCrds();

  }

//! @brief return number of dofs
int XC::BrickSurfaceLoad::getNumDOF(void) const
  { return SL_NUM_DOF; }

int XC::BrickSurfaceLoad::commitState(void)
  {
    int retVal = 0;
    // call element commitState to do any base class stuff
    if((retVal = this->ElementBase<SL_NUM_NODE>::commitState()) != 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; failed in base class";
      }

    return retVal; 
  }

int XC::BrickSurfaceLoad::revertToLastCommit(void)
  { return 0; }

int XC::BrickSurfaceLoad::revertToStart(void)
  { return 0; }

int XC::BrickSurfaceLoad::update(void)
  { return 0; }

//! @brief this function calculates g1, g2, NI, and nHat for given Xi and Eta
int XC::BrickSurfaceLoad::UpdateBase(double Xi, double Eta) const
  {
    const double oneMinusEta= 1 - Eta;
    const double onePlusEta= 1 + Eta;
    const double oneMinusXi= 1 - Xi;
    const double onePlusXi= 1 + Xi;

    // calculate vectors g1 and g2
    // g1 = d(x_Xi)/dXi, g2 = d(x_Xi)/dEta
    g1 = (oneMinusEta * (dcrd2 - dcrd1) + onePlusEta  * (dcrd3 - dcrd4)) * 0.25;
    g2 = (onePlusXi   * (dcrd3 - dcrd2) + oneMinusXi  * (dcrd4 - dcrd1)) * 0.25;

    // shape functions
    myNI(0) = 0.25 * oneMinusXi * oneMinusEta;
    myNI(1) = 0.25 * onePlusXi  * oneMinusEta;
    myNI(2) = 0.25 * onePlusXi  * onePlusEta;
    myNI(3) = 0.25 * oneMinusXi * onePlusEta;

    // normal vector to primary surface as cross product of g1 and g2
    myNhat(0) = g1(1)*g2(2) - g1(2)*g2(1);
    myNhat(1) = g1(2)*g2(0) - g1(0)*g2(2);
    myNhat(2) = g1(0)*g2(1) - g1(1)*g2(0);

    return 0;
  }

const XC::Matrix &XC::BrickSurfaceLoad::getTangentStiff(void) const
  { return tangentStiffness; }

const XC::Matrix & XC::BrickSurfaceLoad::getInitialStiff(void) const
  { return getTangentStiff(); }
    
void XC::BrickSurfaceLoad::zeroLoad(void)
  {}

double XC::BrickSurfaceLoad::getLoadFactor(void) const
  { return this->mLoadFactor; }

void XC::BrickSurfaceLoad::setLoadFactor(const double &d)
  { this->mLoadFactor= d; }

double XC::BrickSurfaceLoad::getPressure(void) const
  { return this->my_pressure; }

void XC::BrickSurfaceLoad::setPressure(const double &d)
  { this->my_pressure= d; }

int XC::BrickSurfaceLoad::addLoad(ElementalLoad *theLoad, double loadFactor)
  {
    int type(0);
    //const Vector &data= theLoad->getData(type, loadFactor);
    theLoad->getData(type, loadFactor);

    if(type == LOAD_TAG_SurfaceLoader)
      {
	mLoadFactor = loadFactor;
	return 0;
      }
    else
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; ele with tag: " << this->getTag()
		  << " does not accept load type: " << type << std::endl;
	return -1;
      }
    return -1;
  }

int XC::BrickSurfaceLoad::addInertiaLoadToUnbalance(const Vector &accel)
  { return 0; }

const XC::Vector &XC::BrickSurfaceLoad::getResistingForce(void) const
  {
    internalForces.Zero();

    // loop over Gauss points
    for(int i = 0; i < SL_NUM_NODE; i++)
      {
	this->UpdateBase(GsPts[i][0],GsPts[i][1]);

	// loop over nodes
	for(int j = 0; j < SL_NUM_NODE; j++)
	  {
	    // loop over dof
	    for(int k = 0; k < 3; k++)
	      {
		internalForces[j*3+k] = internalForces[j*3+k] - mLoadFactor*my_pressure*myNhat(k)*myNI(j);
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
    int res= ElementBase<SL_NUM_NODE>::sendData(comm);
    res+=comm.sendDoubles(mLoadFactor,my_pressure,getDbTagData(),CommMetaData(7));
    res+= comm.sendVector(internalForces,getDbTagData(),CommMetaData(8));
    res+= comm.sendVector(theVector,getDbTagData(),CommMetaData(9));
    res+= comm.sendVector(g1,getDbTagData(),CommMetaData(10));
    res+= comm.sendVector(g2,getDbTagData(),CommMetaData(11));
    res+= comm.sendVector(myNhat,getDbTagData(),CommMetaData(12));
    res+= comm.sendVector(myNI,getDbTagData(),CommMetaData(13));
    res+= comm.sendVector(dcrd1,getDbTagData(),CommMetaData(14));
    res+= comm.sendVector(dcrd2,getDbTagData(),CommMetaData(15));
    res+= comm.sendVector(dcrd3,getDbTagData(),CommMetaData(16));
    res+= comm.sendVector(dcrd4,getDbTagData(),CommMetaData(17));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::BrickSurfaceLoad::recvData(const Communicator &comm)
  {
    int res= ElementBase<SL_NUM_NODE>::recvData(comm);
    res+=comm.receiveDoubles(mLoadFactor,my_pressure,getDbTagData(),CommMetaData(7));
    res+= comm.receiveVector(internalForces,getDbTagData(),CommMetaData(8));
    res+= comm.receiveVector(theVector,getDbTagData(),CommMetaData(9));
    res+= comm.receiveVector(g1,getDbTagData(),CommMetaData(10));
    res+= comm.receiveVector(g2,getDbTagData(),CommMetaData(11));
    res+= comm.receiveVector(myNhat,getDbTagData(),CommMetaData(12));
    res+= comm.receiveVector(myNI,getDbTagData(),CommMetaData(13));
    res+= comm.receiveVector(dcrd1,getDbTagData(),CommMetaData(14));
    res+= comm.receiveVector(dcrd2,getDbTagData(),CommMetaData(15));
    res+= comm.receiveVector(dcrd3,getDbTagData(),CommMetaData(16));
    res+= comm.receiveVector(dcrd4,getDbTagData(),CommMetaData(17));
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
      std::cerr << getClassName() << "sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Receives object through the communicator argument.
int XC::BrickSurfaceLoad::recvSelf(const Communicator &comm)
  {
    inicComm(18);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << getClassName() << "::recvSelf - failed to receive ids.\n";
    else
      {
        setTag(getDbTagDataPos(0));
        res+= recvData(comm);
        if(res<0)
          std::cerr << getClassName() << "::recvSelf - failed to receive data.\n";
      }
    return res;
  }

void XC::BrickSurfaceLoad::Print(std::ostream &os, int flag)
  {
    os << getClassName() << ", element id:  " << this->getTag() << std::endl
       << "   Connected external nodes:  " << theNodes << std::endl; 
  }


