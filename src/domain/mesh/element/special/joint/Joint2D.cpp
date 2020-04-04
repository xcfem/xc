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

// $Revision: 1.8 $
// $Date: 2004/09/01 04:01:27 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/special/joint/Joint2D.cpp,v $

// Written: Arash & GGD
// Created: 03/02
// Revision: Arash

// Joint2D.cpp: implementation of the Joint2D class.
//
//////////////////////////////////////////////////////////////////////

#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <domain/mesh/element/utils/Information.h>
#include <domain/constraints/MFreedom_Joint2D.h>
#include <utility/recorder/response/ElementResponse.h>
#include <material/uniaxial/UniaxialMaterial.h>
#include <domain/mesh/element/special/joint/Joint2D.h>
#include <material/damage/DamageModel.h>
#include <domain/mesh/node/Node.h>


XC::Matrix XC::Joint2D::K(16,16);
XC::Vector XC::Joint2D::V(16);

//! @brief Default constructor.
XC::Joint2D::Joint2D(void)
  :Joint2dBase(0, ELE_TAG_Joint2D,Joint2DPhysicalProperties(5)),
  InternalConstraints(4), numDof(0), nodeDbTag(0), dofDbTag(0)
  { }


XC::Joint2D::Joint2D(int tag, int nd1, int nd2, int nd3, int nd4, int IntNodeTag,const SpringModels &springModels, Domain *theDomain, int LrgDisp)
  :Joint2dBase(tag, ELE_TAG_Joint2D,Joint2DPhysicalProperties(springModels)),
   InternalConstraints(4), numDof(0), nodeDbTag(0), dofDbTag(0),theLoadSens(0)
  {
    int i;
    numDof  = 16;

    K.Zero();
    V.Zero();


    
    theNodes.set_id_nodes(nd1,nd2,nd3,nd4,IntNodeTag); // Save external node id's
    setDomain(theDomain);

    // check for a two dimensional domain, since this element supports only two dimensions
    const Vector &end1Crd = theNodes[0]->getCrds();
    const Vector &end2Crd = theNodes[1]->getCrds();
    const Vector &end3Crd = theNodes[2]->getCrds();
    const Vector &end4Crd = theNodes[3]->getCrds();

    int dimNd1 = end1Crd.Size();
    int dimNd2 = end2Crd.Size();
    int dimNd3 = end3Crd.Size();
    int dimNd4 = end4Crd.Size();

    if(dimNd1 != 2 || dimNd2 != 2 || dimNd3 != 2 || dimNd4 != 2 )
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING has incorrect space dimension \n"
		  << "  space dimension not supported by Joint2D";
        return;
      }

    // now verify the number of dof at node ends
    int dofNd1 = theNodes[0]->getNumberDOF();
    int dofNd2 = theNodes[1]->getNumberDOF();
    int dofNd3 = theNodes[2]->getNumberDOF();
    int dofNd4 = theNodes[3]->getNumberDOF();

    if(dofNd1 != 3 || dofNd2 != 3 || dofNd3 != 3 || dofNd4 != 3 )
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING has incorrect degrees of freedom \n"
		  << "  DOF not supported by Joint2D";
        return;
      }

    // check the joint size. The joint size must be non-zero
    Vector Center1(end1Crd);
    Vector Center2(end2Crd);
    Center1 = Center1 - end3Crd;
    Center2 = Center2 - end4Crd;

    const double L1 = Center1.Norm();
    const double L2 = Center2.Norm();

    if(L1<1e-12  || L2<1e-12 )
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING zero length\n";
        return;
      }

    // check if nodes are not located on each other and they can construct
    // a parallelogram
    Center1 = end1Crd + end3Crd;
    Center2 = end2Crd + end4Crd;

    Center1 = 0.5 * Center1;
    Center2 = 0.5 * Center2;

    Vector Center3(Center2);
    Center3 = Center3 - Center1;

    if(Center3.Norm() > 1e-6 )
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; WARNING can not construct a parallelogram over external nodes\n";
        return;
      }

    // Generate internal node and add it up to domain
    Node *tmp= new Node(IntNodeTag , 4, Center1(0) , Center1(1) );
    theNodes.set_node(4,tmp);
    if(theNodes[4] == nullptr )
      { std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; unable to generate new nodes , out of memory\n" ; }
    else
      {
        if(getDomain()->addNode( theNodes[4] ) == false ) // add internal nodes to domain
          std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; unable to add internal node to domain\n";
      }


    // Generate and add constraints to domain

    // get the constraint numbers
    int startMPtag = getDomain()->getConstraints().getNumMPs();
    for( i=0 ; i<4 ; i++ ) InternalConstraints(i) = startMPtag + i ;

    // create MFreedom_Joint constraint node 1
    if(addMFreedom_Joint(getDomain(), InternalConstraints(0), theNodes.getTagNode(4), theNodes.getTagNode(0), 2, physicalProperties.getFixedEndInfo()(0), LrgDisp ) != 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING can not generate ForJoint MP at node 1\n";
        return;
      }

    // create MFreedom_Joint constraint node 2
    if(addMFreedom_Joint(getDomain(), InternalConstraints(1), theNodes.getTagNode(4), theNodes.getTagNode(1), 3, physicalProperties.getFixedEndInfo()(1), LrgDisp ) != 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING can not generate ForJoint MP at node 2\n";
        return;
      }

    // create MFreedom_Joint constraint node 3
    if( addMFreedom_Joint(getDomain(), InternalConstraints(2), theNodes.getTagNode(4), theNodes.getTagNode(2), 2, physicalProperties.getFixedEndInfo()(2), LrgDisp ) != 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING can not generate ForJoint MP at node 3\n";
        return;
      }

    // create MFreedom_Joint constraint node 4
    if(addMFreedom_Joint(getDomain(), InternalConstraints(3), theNodes.getTagNode(4), theNodes.getTagNode(3), 3, physicalProperties.getFixedEndInfo()(3), LrgDisp ) != 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING can not generate ForJoint MP at node 4\n";
        return;
      }
  }


XC::Joint2D::Joint2D(int tag, int nd1, int nd2, int nd3, int nd4, int IntNodeTag,const SpringModels &springModels, Domain *theDomain, int LrgDisp, const DamageModelVector &damageModels)
  : Joint2dBase(tag, ELE_TAG_Joint2D,Joint2DPhysicalProperties(springModels)), InternalConstraints(4), numDof(0), nodeDbTag(0), dofDbTag(0), theLoadSens(0)
  {
    numDof  = 16;

    K.Zero();
    V.Zero();


    // Save external node id's
    theNodes.set_id_nodes(nd1,nd2,nd3,nd4,IntNodeTag);
    setDomain(theDomain);

    // check for a two dimensional domain, since this element supports only two dimensions
    const Vector &end1Crd = theNodes[0]->getCrds();
    const Vector &end2Crd = theNodes[1]->getCrds();
    const Vector &end3Crd = theNodes[2]->getCrds();
    const Vector &end4Crd = theNodes[3]->getCrds();

    int dimNd1 = end1Crd.Size();
    int dimNd2 = end2Crd.Size();
    int dimNd3 = end3Crd.Size();
    int dimNd4 = end4Crd.Size();

    if(dimNd1 != 2 || dimNd2 != 2 || dimNd3 != 2 || dimNd4 != 2 )
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING has incorrect space dimension \n"
		  << "                                    space dimension not supported by XC::Joint2D";
        return;
      }

    // now verify the number of dof at node ends
    int dofNd1 = theNodes[0]->getNumberDOF();
    int dofNd2 = theNodes[1]->getNumberDOF();
    int dofNd3 = theNodes[2]->getNumberDOF();
    int dofNd4 = theNodes[3]->getNumberDOF();

    if(dofNd1 != 3 || dofNd2 != 3 || dofNd3 != 3 || dofNd4 != 3 )
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING has incorrect degrees of freedom \n"
		  << "                                    DOF not supported by XC::Joint2D";
        return;
      }

    // check the joint size. The joint size must be non-zero
    Vector Center1(end1Crd);
    Vector Center2(end2Crd);
    Center1 = Center1 - end3Crd;
    Center2 = Center2 - end4Crd;

    const double L1 = Center1.Norm();
    const double L2 = Center2.Norm();

    if(L1<1e-12  || L2<1e-12 )
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING zero length\n";
        return;
      }

    // check if nodes are not located on each other and they can construct
    // a parallelogram
    Center1 = end1Crd + end3Crd;
    Center2 = end2Crd + end4Crd;

    Center1 = 0.5 * Center1;
    Center2 = 0.5 * Center2;

    Vector Center3(Center2);
    Center3 = Center3 - Center1;

    if(Center3.Norm() > 1e-6 )
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING can not construct a parallelogram over external nodes\n";
        return;
      }

    // Generate internal node and add it up to domain
    Node *tmp= new Node( IntNodeTag , 4, Center1(0) , Center1(1) );
    theNodes.set_node(4,tmp);

    if( theNodes[4] == nullptr )
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; unable to generate new nodes , out of memory\n" ;
      }
    else
      {
        if(getDomain()->addNode( theNodes[4] ) == false )                // add intenal nodes to domain
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; unable to add internal nodeto domain\n";
      }


    // Generate and add constraints to domain

    // get the constraint numbers
    int startMPtag= getDomain()->getConstraints().getNumMPs();
    for(int i=0 ; i<4 ; i++ )
      InternalConstraints(i) = startMPtag + i ;

    // create MFreedom_Joint constraint node 1
    if( addMFreedom_Joint(getDomain(), InternalConstraints(0), theNodes.getTagNode(4), theNodes.getTagNode(0), 2, physicalProperties.getFixedEndInfo()(0), LrgDisp ) != 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING can not generate ForJoint MP at node 1\n";
        return;
      }

    // create MFreedom_Joint constraint node 2
    if( addMFreedom_Joint( getDomain(), InternalConstraints(1), theNodes.getTagNode(4), theNodes.getTagNode(1), 3, physicalProperties.getFixedEndInfo()(1), LrgDisp ) != 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING can not generate ForJoint MP at node 2\n";
                return;
      }

    // create MFreedom_Joint constraint node 3
    if( addMFreedom_Joint( getDomain(), InternalConstraints(2), theNodes.getTagNode(4), theNodes.getTagNode(2), 2, physicalProperties.getFixedEndInfo()(2), LrgDisp ) != 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING can not generate ForJoint MP at node 3\n";
        return;
      }

    // create MFreedom_Joint constraint node 4
    if( addMFreedom_Joint( getDomain(), InternalConstraints(3), theNodes.getTagNode(4), theNodes.getTagNode(3), 3, physicalProperties.getFixedEndInfo()(3), LrgDisp ) != 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING can not generate ForJoint MP at node 4\n";
        return;
      }
  }

//! @brief Virtual constructor.
XC::Element* XC::Joint2D::getCopy(void) const
  { return new Joint2D(*this); }


XC::Joint2D::~Joint2D(void)
  {
    if(getDomain())
      {
        MFreedom_Constraint *Temp_MF;
        for(int i=0;i<4;i++ )
          {
            Temp_MF= getDomain()->getConstraints().getMFreedom_Constraint( InternalConstraints(i) );
            if(Temp_MF)
              {
                getDomain()->removeMFreedom_Constraint( InternalConstraints(i) );
                delete Temp_MF;
              }
          }
        if(theNodes[4])
          {
            int intnodetag = theNodes[4]->getTag();
            getDomain()->removeNode( intnodetag );
            delete theNodes[4];
          }
      }
  }


void XC::Joint2D::setDomain(Domain *theDomain)
  {
    Joint2dBase::setDomain(theDomain);
    //Ckeck domain not null - invoked when object removed from a domain
    if(theDomain == nullptr)
      {
        theNodes.inic();
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING specified domain does not exist , Domain = 0\n";
      }
  }


int XC::Joint2D::addMFreedom_Joint(Domain *theDomain, int mpNum,
                                  int RnodeID, int CnodeID,
                                  int MainDOF, int FixedEnd, int LrgDispFlag )
  {

    // create MFreedom_ForJoint constraint
    MFreedom_Constraint *Temp_MF= new MFreedom_Joint2D(getDomain(), mpNum, RnodeID, CnodeID, MainDOF, FixedEnd, LrgDispFlag );

    if(Temp_MF == nullptr)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING ran out of memory for ForJoint XC::MFreedom_Constraint ";
        return -1;
      }
    // Add the multi-point constraint to the domain
    if(getDomain()->addMFreedom_Constraint(Temp_MF) == false)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING could not add equalDOF XC::MFreedom_Constraint to domain ";
        delete Temp_MF;
        return -2;
      }
    return 0;
  }

//////////////////////////////////////////////////////////////////////
// Public methods called, taken care of for 2D element subclasses
//////////////////////////////////////////////////////////////////////

int XC::Joint2D::update(void)
  {
        const Vector &disp1 = theNodes[0]->getTrialDisp();
        const Vector &disp2 = theNodes[1]->getTrialDisp();
        const Vector &disp3 = theNodes[2]->getTrialDisp();
        const Vector &disp4 = theNodes[3]->getTrialDisp();
        const Vector &dispC = theNodes[4]->getTrialDisp();
        double Delta[5];
        Delta[0] = disp1(2) - dispC(3);
        Delta[1] = disp2(2) - dispC(2);
        Delta[2] = disp3(2) - dispC(3);
        Delta[3] = disp4(2) - dispC(2);
        Delta[4] = dispC(3) - dispC(2);
        int result = 0;

        for( int i=0 ; i<5 ; i++ )
          {
            if( physicalProperties[i] != nullptr ) result = physicalProperties[i]->setTrialStrain(Delta[i]);
            if( result != 0 ) break;
          }

        return result;
}

int XC::Joint2D::commitState()
  {

    // setting the trial state for the damage models

    int result= Joint2dBase::commitState();
    if(result==0)
      result+= physicalProperties.commitState();
    return result;
  }

int XC::Joint2D::revertToLastCommit()
  {
    int result= Joint2dBase::revertToLastCommit();
    if(result==0)
      result+= physicalProperties.revertToLastCommit();
    return result;
  }

int XC::Joint2D::revertToStart(void)
  {
    int result= Joint2dBase::revertToStart();
    if(result==0)
      result+= physicalProperties.revertToStart();
    return result;
  }


int XC::Joint2D::getNumDOF(void) const
  { return numDof; }

const XC::Matrix &XC::Joint2D::getTangentStiff(void) const
  {
        double Ktangent[5] ;
        for( int i=0 ; i<5 ; i++ )
        {
                Ktangent[i] = 0;
                if( physicalProperties[i] != nullptr ) Ktangent[i] = physicalProperties[i]->getTangent();
        }

        K.Zero();

        K(2,2)  =  Ktangent[0];
        K(2,15) = -Ktangent[0];
        K(5,5)  =  Ktangent[1];
        K(5,14) = -Ktangent[1];
        K(8,8)  =  Ktangent[2];
        K(8,15) = -Ktangent[2];
        K(11,11)=  Ktangent[3];
        K(11,14)= -Ktangent[3];
        K(14,5) = -Ktangent[1];
        K(14,11)= -Ktangent[3];
        K(14,14)=  Ktangent[1] + Ktangent[3] + Ktangent[4];
        K(14,15)= -Ktangent[4];
        K(15,2) = -Ktangent[0];
        K(15,8) = -Ktangent[2];
        K(15,14)= -Ktangent[4];
        K(15,15)=  Ktangent[0] + Ktangent[2] + Ktangent[4];

    if(isDead())
      K*=dead_srf;
        return K;
  }


const XC::Matrix &XC::Joint2D::getInitialStiff(void) const
  {
        double Kintial[5] ;
        for( int i=0 ; i<5 ; i++ )
        {
                Kintial[i] = 0;
                if( physicalProperties[i] != nullptr ) Kintial[i] = physicalProperties[i]->getTangent();
        }

        K.Zero();

        K(2,2)  =  Kintial[0];
        K(2,15) = -Kintial[0];
        K(5,5)  =  Kintial[1];
        K(5,14) = -Kintial[1];
        K(8,8)  =  Kintial[2];
        K(8,15) = -Kintial[2];
        K(11,11)=  Kintial[3];
        K(11,14)= -Kintial[3];
        K(14,5) = -Kintial[1];
        K(14,11)= -Kintial[3];
        K(14,14)=  Kintial[1] + Kintial[3] + Kintial[4];
        K(14,15)= -Kintial[4];
        K(15,2) = -Kintial[0];
        K(15,8) = -Kintial[2];
        K(15,14)= -Kintial[4];
        K(15,15)=  Kintial[0] + Kintial[2] + Kintial[4];

    if(isDead())
      K*=dead_srf;
        return K;
  }


const XC::Matrix &XC::Joint2D::getDamp(void) const
  {
    K.Zero();
    return K;
  }

const XC::Matrix &XC::Joint2D::getMass(void) const
  {
    K.Zero();
    return K;
  }

void XC::Joint2D::Print(std::ostream &s, int flag ) const
{
  s << "\nElement: " << getTag() << " type: Joint2D iNode: "
    << theNodes.getTagNode(0) << " jNode: " << theNodes.getTagNode(1) << "\n"
    << " kNode: " << theNodes.getTagNode(2) << " lNode: " << theNodes.getTagNode(3) << "\n"
        << " Internal node: " << theNodes.getTagNode(4) << "\n";
}

/////////////////////////////////////////////////////////////////////
// methods for applying and returning loads
//////////////////////////////////////////////////////////////////////

int XC::Joint2D::addLoad(ElementalLoad *theLoad, double loadFactor)
  { return 0; }

int XC::Joint2D::addInertiaLoadToUnbalance(const Vector &accel)
  { return 0; }



const XC::Vector &XC::Joint2D::getResistingForce(void) const
  {
    Vector Force= physicalProperties.getStresses();

    V.Zero();

    V(2) = Force[0];
    V(5) = Force[1];
    V(8) = Force[2];
    V(11)= Force[3];
    V(14)= -Force[4] - Force[1] - Force[3];
    V(15)= Force[4] - Force[0] - Force[2];
    if(isDead())
      V*=dead_srf;
    return V;
  }

const XC::Vector &XC::Joint2D::getResistingForceIncInertia(void) const
  { return this->getResistingForce(); }

//most-probably requires to be overridden
XC::Response* XC::Joint2D::setResponse(const std::vector<std::string> &argv, Information &eleInformation)
  {
//
// we compare argv[0] for known response types for the XC::Truss
//
    if(argv[0] == "node" || argv[0] == "internalNode" )
      return new ElementResponse(this, 1, Vector(4));
    else if(argv[0] == "size" || argv[0] == "jointSize" )
      return new ElementResponse(this, 2, Vector(2));
    else if(argv[0] == "moment" || argv[0] == "-moment" || argv[0] == "force" || argv[0] == "-force" )
      return new ElementResponse(this, 3, Vector(5));
    else if(argv[0] == "defo" || argv[0] == "deformations" || argv[0] == "deformation" )
      return new ElementResponse(this, 4, Vector(5));
    else if(argv[0] == "defoANDforce" || argv[0] == "deformationANDforce" || argv[0] == "deformationsANDforces" )
      return new ElementResponse(this, 5, Vector(10));
    else if(argv[0] == "stiff" || argv[0] == "stiffness" )
      return new ElementResponse(this, 6, Matrix(16,16) );
    else if(argv[0] == "plasticRotation" || argv[0] == "plasticDeformation")
      return new ElementResponse(this, 7, Vector(5));
    else if(argv[0] == "damage" || argv[0] == "damages" || argv[0] == "-damage" || argv[0] == "-damages")
      return new ElementResponse(this, 8, Vector(5));
        // material response
    else if((argv[0] == "spring") || (argv[0] == "-spring") || (argv[0] == "material") || (argv[0] == "-material") )
      {
        int materialNum = atoi(argv[1]) - 1;
        if(materialNum >= 0 && materialNum < 5)
          if(physicalProperties[materialNum] != 0)
            return setMaterialResponse(physicalProperties[materialNum],argv,2,eleInformation);
          else
            return nullptr;
        else
          return nullptr;
      }
    else
      return nullptr;
  }

int XC::Joint2D::getResponse(int responseID, Information &eleInformation)
{
        switch (responseID) {
        case -1:
                return -1;

        case 1:
                if(eleInformation.theVector!=0)
                {
                        const XC::Vector& disp = theNodes[4]->getTrialDisp();
                        (*(eleInformation.theVector))(0) = disp(0);
                        (*(eleInformation.theVector))(1) = disp(1);
                        (*(eleInformation.theVector))(2) = disp(2);
                        (*(eleInformation.theVector))(3) = disp(3);
                }
                return 0;

        case 2:
                if(eleInformation.theVector!=0)
                {
                        const XC::Vector &node1Crd = theNodes[0]->getCrds();
                        const XC::Vector &node2Crd = theNodes[1]->getCrds();
                        const XC::Vector &node3Crd = theNodes[2]->getCrds();
                        const XC::Vector &node4Crd = theNodes[3]->getCrds();

                        const XC::Vector &node1Disp = theNodes[0]->getDisp();
                        const XC::Vector &node2Disp = theNodes[1]->getDisp();
                        const XC::Vector &node3Disp = theNodes[2]->getDisp();
                        const XC::Vector &node4Disp = theNodes[3]->getDisp();

                        Vector v1(2);
                        Vector v2(2);
                        Vector v3(2);
                        Vector v4(2);

                        // calculate the current coordinates of four external nodes
                        for(int i=0; i<2; i++)
                    {
                                v1(i) = node1Crd(i)+node1Disp(i);
                                v2(i) = node2Crd(i)+node2Disp(i);
                                v3(i) = node3Crd(i)+node3Disp(i);
                                v4(i) = node4Crd(i)+node4Disp(i);
                        }

                        v3 = v3 - v1;
                        v4 = v4 - v2;

                        v1(0) = v3.Norm();
                        v1(1) = v4.Norm();

                        *(eleInformation.theVector) = v1;
                }
                return 0;

        case 3:
                if( eleInformation.theVector != 0 )
                {
                        for( int i =0 ; i<5 ; i++ )
                        {
                                (*(eleInformation.theVector))(i) = 0.0;
                                if( physicalProperties[i] != nullptr )
                                        (*(eleInformation.theVector))(i) = physicalProperties[i]->getStress();
                        }
                }
                return 0;

        case 4:
                if(eleInformation.theVector!=0)
                {
                        for( int i =0 ; i<5 ; i++ )
                        {
                                (*(eleInformation.theVector))(i) = 0.0;
                                if( physicalProperties[i] != nullptr )
                                        (*(eleInformation.theVector))(i) = physicalProperties[i]->getStrain();
                        }
                }
                return 0;

        case 5:
                if(eleInformation.theVector!=0)
                {
                        for( int i =0 ; i<5 ; i++ )
                        {
                                (*(eleInformation.theVector))(i) = 0.0;
                                (*(eleInformation.theVector))(i+5) = 0.0;
                                if( physicalProperties[i] != nullptr )
                                {
                                        (*(eleInformation.theVector))(i) = physicalProperties[i]->getStrain();
                                        (*(eleInformation.theVector))(i+5) = physicalProperties[i]->getStress();
                                }
                        }
                }
                return 0;

        case 6:
                return eleInformation.setMatrix(this->getTangentStiff());

        case 7:
                if(eleInformation.theVector!=0)
                {
                        for( int i=0 ; i<5 ; i++ )
                        {
                                (*(eleInformation.theVector))(i) = 0.0;
                                if( physicalProperties[i] != nullptr && physicalProperties[i]->getInitialTangent() != 0.0 )
                                {
                                        (*(eleInformation.theVector))(i) =
                                                physicalProperties[i]->getStrain() - physicalProperties[i]->getStress()/physicalProperties[i]->getInitialTangent();
                                }

                        }
                }
                return 0;

        case 8:
	  if(eleInformation.theVector!=0)
  	    {
	      for( int i=0 ; i<5 ; i++ )
		{
		  (*(eleInformation.theVector))(i) = 0.0;
		  const DamageModelVector &theDamages= physicalProperties.getDamageModelVector();
		  if(theDamages[i] != nullptr )
		    { (*(eleInformation.theVector))(i) = theDamages[i]->getDamage(); }
		}
	    }
	  return 0;

        default:
                return -1;
        }
}


//! @brief Send object members through the channel being passed as parameter.
int XC::Joint2D::sendData(Communicator &comm)
  {
    int res= Joint2dBase::sendData(comm);
    res+=comm.sendID(InternalConstraints,getDbTagData(),CommMetaData(8));
    res+= comm.sendInts(numDof, nodeDbTag, dofDbTag,getDbTagData(),CommMetaData(10));
    return res;
  }

//! @brief Receives object members through the channel being passed as parameter.
int XC::Joint2D::recvData(const Communicator &comm)
  {
    int res= Joint2dBase::recvData(comm);
    res+=comm.receiveID(InternalConstraints,getDbTagData(),CommMetaData(8));
    res+= comm.receiveInts(numDof, nodeDbTag, dofDbTag,getDbTagData(),CommMetaData(10));
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::Joint2D::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(10);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to send data\n";
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::Joint2D::recvSelf(const Communicator &comm)
  {
    inicComm(15);
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
		    << "; failed to receive data.\n";
      }
    return res;
  }

// AddingSensitivity:BEGIN ///////////////////////////////////
int XC::Joint2D::addInertiaLoadSensitivityToUnbalance(const XC::Vector &accel, bool somethingRandomInMotions)
{

  if(theLoadSens == 0) {
    theLoadSens = new Vector(numDof);
  }
  else {
    theLoadSens->Zero();
  }

  return 0;
}


int
XC::Joint2D::setParameter(const std::vector<std::string> &argv, Parameter &param)
{//
        // From the parameterID value it should be possible to extract
        // information about:
        //  1) Which parameter is in question. The parameter could
        //     be at element, section, or material level.
        //  2) Which section and material number (tag) it belongs to.
        //
        // To accomplish this the parameterID is given the following value:
        //     parameterID = type + 1000*matrTag + 100000*sectionTag
        // ...where 'type' is an integer in the range (1-99) and added 100
        // for each level (from material to section to element).
        //
        // Example:
        //    If 'E0' (case 2) is random in material #3 of section #5
        //    the value of the parameterID at this (element) level would be:
        //    parameterID = 2 + 1000*3 + 100000*5 = 503002
        //    As seen, all given information can be extracted from this number.
        //

        int ok = -1;
        if(argv.size() < 3)
        return ok;

    // a material parameter
    if(argv[0] == "-material" || argv[0] == "material") {
                // Get material tag numbers from user input
                int paramMaterialTag = atoi(argv[1]);
                if( paramMaterialTag<0 || paramMaterialTag>4 ) {
                        std::cerr << getClassName() << "::" << __FUNCTION__
				  << "; material number out of range, must be 0 to 4." << std::endl;
                return -1;
                }

                // Find the material and call its setParameter method
                if( physicalProperties[paramMaterialTag] != nullptr )
		  ok = setMaterialParameter(physicalProperties[paramMaterialTag],argv,2,param);

                // Check if the ok is valid
                if(ok < 0) {
                        std::cerr << getClassName() << "::" << __FUNCTION__
				  << "; could not set parameter. " << std::endl;
                        return -1;
                }
                else {
                        // Return the ok value (according to the above comments)
                        return 1000*paramMaterialTag + ok;
                }
        }

    // otherwise parameter is unknown for the XC::Joint2D class
      return -1;
}

int
XC::Joint2D::updateParameter (int parameterID, Information &info)
{
        // Extract section and material tags from the passedParameterID
        int MaterialTag = (int)( floor( (double)parameterID / 1000.0 ) );
        int MaterialParameterID = parameterID-1000*MaterialTag;
        if( MaterialTag<0 || MaterialTag>4 ) {
                std::cerr << getClassName() << "::" << __FUNCTION__
			  << "; material number out of range, must be 0 to 4." << std::endl;
                return -1;
        }

        // Go down to the material and set appropriate flags
        if( physicalProperties[MaterialTag] != nullptr )
                return physicalProperties[MaterialTag]->updateParameter(MaterialParameterID,info);

        return -1;
}


int
XC::Joint2D::activateParameter(int passedParameterID)
{
        // Note that the parameteID that is stored here at the
        // element level contains all information about section
        // and material tag number:
        parameterID = passedParameterID;

        if(passedParameterID == 0 ) {
                // "Zero out" all flags downwards through sections/materials
                for(int i=0; i<5; i++) {
                        if( physicalProperties[i] !=0 )
                                physicalProperties[i]->activateParameter(passedParameterID);
                }
        }
        else {

                // Extract section and material tags from the passedParameterID
                int activeMaterialTag = (int)( floor( (double)passedParameterID / 1000.0 ) );

                if( activeMaterialTag<0 || activeMaterialTag>4 ) {
                        std::cerr << getClassName() << "::" << __FUNCTION__
				  << "; material number out of range, must be 0 to 4." << std::endl;
                        return -1;
                }

                // Go down to the material and set appropriate flags
                if( physicalProperties[activeMaterialTag] != nullptr ) {
                        physicalProperties[activeMaterialTag]->activateParameter(passedParameterID-1000*activeMaterialTag);
                }
        }

        return 0;
}


const XC::Matrix &XC::Joint2D::getKiSensitivity(int gradNumber)
{
        K.Zero();

        if(parameterID == 0) {
                // // Nothing here
        }

        else {
                double KtangentSensitivity[5] ;
                for( int i=0 ; i<5 ; i++ )
                {
                        KtangentSensitivity[i] = 0;
                        if( physicalProperties[i] != nullptr )
                                KtangentSensitivity[i] = physicalProperties[i]->getInitialTangentSensitivity(gradNumber);
                }

                K(2,2)  =  KtangentSensitivity[0];
                K(2,15) = -KtangentSensitivity[0];
                K(5,5)  =  KtangentSensitivity[1];
                K(5,14) = -KtangentSensitivity[1];
                K(8,8)  =  KtangentSensitivity[2];
                K(8,15) = -KtangentSensitivity[2];
                K(11,11)=  KtangentSensitivity[3];
                K(11,14)= -KtangentSensitivity[3];
                K(14,5) = -KtangentSensitivity[1];
                K(14,11)= -KtangentSensitivity[3];
                K(14,14)=  KtangentSensitivity[1] + KtangentSensitivity[3] + KtangentSensitivity[4];
                K(14,15)= -KtangentSensitivity[4];
                K(15,2) = -KtangentSensitivity[0];
                K(15,8) = -KtangentSensitivity[2];
                K(15,14)= -KtangentSensitivity[4];
                K(15,15)=  KtangentSensitivity[0] + KtangentSensitivity[2] + KtangentSensitivity[4];
        }

        return K;
}


const XC::Matrix &XC::Joint2D::getMassSensitivity(int gradNumber)
{
          K.Zero();
        return K;
}


const XC::Vector &XC::Joint2D::getResistingForceSensitivity(int gradNumber)
{
        this->update();
        V.Zero();

        // Compute sensitivity depending on the material
        double ForceSensitivity[5] ;
        for( int i=0 ; i<5 ; i++ )
        {
                ForceSensitivity[i] = 0;
                if( physicalProperties[i] != nullptr ) ForceSensitivity[i] = physicalProperties[i]->getStressSensitivity(gradNumber,true);
        }

        V(2) = ForceSensitivity[0];
        V(5) = ForceSensitivity[1];
        V(8) = ForceSensitivity[2];
        V(11)= ForceSensitivity[3];
        V(14)= -ForceSensitivity[4] - ForceSensitivity[1] - ForceSensitivity[3];
        V(15)= ForceSensitivity[4] - ForceSensitivity[0] - ForceSensitivity[2];

        return V;
}


int XC::Joint2D::commitSensitivity(int gradNumber, int numGrads)
  {
    double strainSensitivity = 0.0;
    // Pass it down to the material
    for( int i=0 ; i<5 ; i++ )
      if( physicalProperties[i] != nullptr )
        physicalProperties[i]->commitSensitivity(strainSensitivity, gradNumber, numGrads);
    return 0;
  }

// AddingSensitivity:END /////////////////////////////////////////////
