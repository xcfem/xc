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

// $Revision: 1.2 $
// $Date: 2004/09/01 04:01:27 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/special/joint/Joint3D.cpp,v $

// Written: Arash Altoontash, Gregory Deierlein
// Created: 03/02
// Revision: Arash

// Joint3D.cpp: implementation of the XC::Joint3D class.
//
//////////////////////////////////////////////////////////////////////

#include <domain/mesh/element/special/joint/Joint3D.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <domain/mesh/element/utils/Information.h>
#include <domain/constraints/MFreedom_Joint3D.h>
#include <domain/mesh/node/Node.h>
#include <utility/recorder/response/ElementResponse.h>
#include <material/uniaxial/UniaxialMaterial.h>


XC::Matrix XC::Joint3D::K(45,45);
XC::Vector XC::Joint3D::V(45);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


XC::Joint3D::Joint3D(void)
  :ElemWithMaterial<7,Joint3DPhysicalProperties>(0, ELE_TAG_Joint3D,Joint3DPhysicalProperties(3,nullptr)), InternalConstraints(6),
  numDof(0), nodeDbTag(0), dofDbTag(0) {}


XC::Joint3D::Joint3D(int tag, int nd1, int nd2, int nd3, int nd4, int nd5, int nd6, int IntNodeTag, const SpringModels &springModels, Domain *TheDomain, int LrgDisp)
  :ElemWithMaterial<7,Joint3DPhysicalProperties>(tag, ELE_TAG_Joint3D,Joint3DPhysicalProperties(springModels)), InternalConstraints(6), numDof(0), nodeDbTag(0), dofDbTag(0)
  {
    int i;
    numDof  = 45;

    K.Zero();
    V.Zero();



    // Save external node id's
    theNodes.set_id_nodes(nd1,nd2,nd3,nd4,nd5,nd6,IntNodeTag);
    setDomain(TheDomain);

    // check for a two dimensional domain, since this element supports only two dimensions
    const Vector &end1Crd = theNodes[0]->getCrds();
    const Vector &end2Crd = theNodes[1]->getCrds();
    const Vector &end3Crd = theNodes[2]->getCrds();
    const Vector &end4Crd = theNodes[3]->getCrds();
    const Vector &end5Crd = theNodes[4]->getCrds();
    const Vector &end6Crd = theNodes[5]->getCrds();

    int dimNd1 = end1Crd.Size();
    int dimNd2 = end2Crd.Size();
    int dimNd3 = end3Crd.Size();
    int dimNd4 = end4Crd.Size();
    int dimNd5 = end5Crd.Size();
    int dimNd6 = end6Crd.Size();

    if(dimNd1 != 3 || dimNd2 != 3 || dimNd3 != 3 || dimNd4 != 3 || dimNd5 != 3 || dimNd6 != 3 )
      {
        std::cerr << "WARNING XC::Joint3D::setDomain(): has incorrect space dimension \n";
        std::cerr << "                                    space dimension not supported by XC::Joint3D";
        return;
      }

    // now verify the number of dof at node ends
    const int dofNd1 = theNodes[0]->getNumberDOF();
    const int dofNd2 = theNodes[1]->getNumberDOF();
    const int dofNd3 = theNodes[2]->getNumberDOF();
    const int dofNd4 = theNodes[3]->getNumberDOF();
    const int dofNd5 = theNodes[4]->getNumberDOF();
    const int dofNd6 = theNodes[5]->getNumberDOF();

    if(dofNd1 != 6 || dofNd2 != 6 || dofNd3 != 6 || dofNd4 != 6 || dofNd5 != 6 || dofNd6 != 6 )
      {
        std::cerr << "WARNING XC::Joint3D::Joint3D: has incorrect degrees of freedom \n";
        std::cerr << "                                    DOF not supported by XC::Joint3D";
        return;
      }

    // check the joint size. The joint size must be non-zero
    Vector Center1(end1Crd);
    Vector Center2(end3Crd);
    Vector Center3(end5Crd);
    Center1 = Center1 - end2Crd;
    Center2 = Center2 - end4Crd;
    Center3 = Center3 - end6Crd;

    const double L1 = Center1.Norm();
    const double L2 = Center2.Norm();
    const double L3 = Center3.Norm();

    if( L1<1e-12  || L2<1e-12  || L3<1e-12 )
      {
        std::cerr << "WARNING XC::Joint3D::(): zero length\n";
        return;
      }

    // check if nodes are not located on each other and they can construct
    // a parallelogram
    Center1 = end1Crd + end2Crd;
    Center2 = end3Crd + end4Crd;
    Center3 = end5Crd + end6Crd;

    Center1 = 0.5 * Center1;
    Center2 = 0.5 * Center2;
    Center3 = 0.5 * Center3;

    Vector CenterTemp(Center2);
    CenterTemp = CenterTemp - Center1;
    if(CenterTemp.Norm() > 1e-6 )
      {
        std::cerr << "WARNING XC::Joint3D::(): can not construct a shear block over external nodes\n";
        std::cerr << "check the coordinates\n";
        return;
      }

    CenterTemp = Center3 - Center1;
    if(CenterTemp.Norm() > 1e-6 )
      {
        std::cerr << "WARNING XC::Joint3D::(): can not construct a shear block over external nodes\n";
        std::cerr << "check the coordinates\n";
        return;
      }

    // Generate internal node and add it up to domain
    Node *tmp= new Node( IntNodeTag , 9, Center1(0) , Center1(1) , Center1(2) );
    theNodes.set_node(6,tmp);
    if(theNodes[6] == nullptr )
      { std::cerr << "XC::Joint3D::Joint3D - Unable to generate new nodes , out of memory\n" ; }
    else
      {
        if(getDomain()->addNode( theNodes[6] ) == false )                // add intenal nodes to domain
        std::cerr << "XC::Joint3D::Joint3D - unable to add internal nodeto domain\n";
      }


    // Generate and add constraints to domain

    // get the constraint numbers
    int startMPtag = getDomain()->getConstraints().getNumMPs();
    for(i=0;i<6;i++)
      InternalConstraints(i) = startMPtag + i ;

    // create MFreedom_Joint constraint node 1
    if(addMFreedom_Joint(getDomain(), InternalConstraints(0), theNodes.getTagNode(6), theNodes.getTagNode(0), theNodes.getTagNode(5), 8, theNodes.getTagNode(3), 7, LrgDisp ) != 0)
      {
        std::cerr << "WARNING XC::Joint3D::Joint3D(): can not generate ForJoint MP at node 1\n";
        return;
      }

    // create MFreedom_Joint constraint node 2
    if(addMFreedom_Joint(getDomain(), InternalConstraints(1), theNodes.getTagNode(6), theNodes.getTagNode(1), theNodes.getTagNode(5), 8, theNodes.getTagNode(3), 7, LrgDisp ) != 0)
      {
        std::cerr << "WARNING XC::Joint3D::Joint3D(): can not generate ForJoint MP at node 2\n";
        return;
      }

    // create MFreedom_Joint constraint node 3
    if(addMFreedom_Joint(getDomain(), InternalConstraints(2), theNodes.getTagNode(6), theNodes.getTagNode(2), theNodes.getTagNode(1), 6, theNodes.getTagNode(5), 8, LrgDisp ) != 0)
      {
        std::cerr << "WARNING XC::Joint3D::Joint3D(): can not generate ForJoint MP at node 3\n";
        return;
      }

    // create MFreedom_Joint constraint node 4
    if(addMFreedom_Joint(getDomain(), InternalConstraints(3), theNodes.getTagNode(6), theNodes.getTagNode(3), theNodes.getTagNode(1), 6, theNodes.getTagNode(5), 8, LrgDisp ) != 0)
      {
        std::cerr << "WARNING XC::Joint3D::Joint3D(): can not generate ForJoint MP at node 4\n";
        return;
      }

    // create MFreedom_Joint constraint node 5
    if(addMFreedom_Joint(getDomain(), InternalConstraints(4), theNodes.getTagNode(6), theNodes.getTagNode(4), theNodes.getTagNode(3), 7, theNodes.getTagNode(1), 6, LrgDisp ) != 0)
      {
        std::cerr << "WARNING XC::Joint3D::Joint3D(): can not generate ForJoint MP at node 3\n";
        return;
      }

    // create MFreedom_Joint constraint node 6
    if(addMFreedom_Joint(getDomain(), InternalConstraints(5), theNodes.getTagNode(6), theNodes.getTagNode(5), theNodes.getTagNode(3), 7, theNodes.getTagNode(1), 6, LrgDisp ) != 0)
      {
        std::cerr << "WARNING XC::Joint3D::Joint3D(): can not generate ForJoint MP at node 3\n";
        return;
      }
  }

//! @brief Virtual constructor.
XC::Element* XC::Joint3D::getCopy(void) const
  { return new Joint3D(*this); }

XC::Joint3D::~Joint3D(void)
  {

    if(getDomain())
      {
        MFreedom_Constraint *Temp_MF;
        for(int i=0 ; i < 6 ; i++ )
          {
            Temp_MF = getDomain()->getConstraints().getMFreedom_Constraint(InternalConstraints(i) );

            if(Temp_MF != nullptr )
              {
                getDomain()->removeMFreedom_Constraint( InternalConstraints(i) );
                delete Temp_MF;
              }
          }
        if(theNodes[7])
          {
            int intnodetag = theNodes[7]->getTag();
            getDomain()->removeNode( intnodetag );
            delete theNodes[7];
          }
      }
  }



void XC::Joint3D::setDomain(Domain *TheDomain)
  {
    ElemWithMaterial<7,Joint3DPhysicalProperties>::setDomain(TheDomain);
    //Ckeck domain not null - invoked when object removed from a domain
    if(!TheDomain)
      {
        std::cerr << "WARNING XC::Joint3D(): Specified domain does not exist , Domain = 0\n";
        theNodes.inic();
      }
  }


int XC::Joint3D::addMFreedom_Joint(Domain *theDomain, int mpNum, int RetNodeID, int ConNodeID, int RotNodeID, int Rdof, int DspNodeID, int Ddof, int LrgDispFlag )
  {
    // create MFreedom_ForJoint constraint
    MFreedom_Constraint *Temp_MF= new MFreedom_Joint3D( theDomain, mpNum, RetNodeID, ConNodeID, RotNodeID, Rdof, DspNodeID, Ddof, LrgDispFlag );

    if(Temp_MF == nullptr)
      {
        std::cerr << "XC::Joint3D::addMFreedom_Joint - WARNING ran out of memory for XC::MFreedom_Joint3D XC::MFreedom_Constraint ";
        return -1;
      }
    // Add the multi-point constraint to the domain
    if(theDomain->addMFreedom_Constraint(Temp_MF) == false)
      {
        std::cerr << "XC::Joint3D::addMFreedom_Joint - WARNING could not add equalDOF XC::MFreedom_Constraint to domain ";
        delete Temp_MF;
        return -2;
      }
    return 0;
  }

//////////////////////////////////////////////////////////////////////
// Public methods called, taken care of for 2D element subclasses
//////////////////////////////////////////////////////////////////////

int XC::Joint3D::update(void)
  {
    const Vector &dispC= theNodes[6]->getTrialDisp();
    return physicalProperties.update(dispC,6);
  }

int XC::Joint3D::getNumDOF(void) const
  { return numDof; }

const XC::Matrix &XC::Joint3D::getTangentStiff(void) const
  {
    const Vector Ktangent= physicalProperties.getTangent();

    K.Zero();
    K(42,42)  =  Ktangent[0];
    K(43,43)  =  Ktangent[1];
    K(44,44)  =  Ktangent[2];
    if(isDead())
      K*=dead_srf;
    return K;
  }


const XC::Matrix &XC::Joint3D::getInitialStiff(void) const
  {
    const Vector Kinitial= physicalProperties.getTangent();
        K.Zero();

        K(42,42)  =  Kinitial[0];
        K(43,43) = -Kinitial[0];
        K(44,44)  =  Kinitial[1];

    if(isDead())
      K*=dead_srf;
        return K;
  }


const XC::Matrix &XC::Joint3D::getDamp(void) const
  {
        K.Zero();
        return K;
  }

const XC::Matrix &XC::Joint3D::getMass(void) const
{
        K.Zero();
        return K;
}

void XC::Joint3D::Print(std::ostream &s, int flag ) const
{
  s << "\nElement: " << getTag() << " type: Joint3D iNode: "
    << theNodes.getTagNode(0) << " jNode: " << theNodes.getTagNode(1) << "\n"
    << " kNode: " << theNodes.getTagNode(2) << " lNode: " << theNodes.getTagNode(3) << "\n"
        << " mNode: " << theNodes.getTagNode(4) << " nNode: " << theNodes.getTagNode(5) << "\n"
        << " Internal node: " << theNodes.getTagNode(6) << "\n";
}

/////////////////////////////////////////////////////////////////////
// methods for applying and returning loads
//////////////////////////////////////////////////////////////////////

int XC::Joint3D::addLoad(ElementalLoad *theLoad, double loadFactor)
  { return 0; }

int XC::Joint3D::addInertiaLoadToUnbalance(const Vector &accel)
  { return 0; }



const XC::Vector &XC::Joint3D::getResistingForce(void) const
  {
    const Vector Force= physicalProperties.getStress();

    V.Zero();
    V(42) = Force[0];
    V(43) = Force[1];
    V(44) = Force[2];
    if(isDead())
      V*=dead_srf;
    return V;
  }

const XC::Vector &XC::Joint3D::getResistingForceIncInertia(void) const
  { return this->getResistingForce(); }


//most-probably requires to be overridden
 XC::Response* XC::Joint3D::setResponse(const std::vector<std::string> &argv, Information &eleInformation)
{
//
// we compare argv[0] for known response types for the XC::Truss
//

        if(argv[0] == "node" || argv[0] == "internalNode" )
    return new ElementResponse(this, 1, Vector(9));

        else if(argv[0] == "size" || argv[0] == "jointSize" )
    return new ElementResponse(this, 2, Vector(3));

        else if(argv[0] == "moment" || argv[0] == "moments"
                || argv[0] == "force" || argv[0] == "forces" )
    return new ElementResponse(this, 3, Vector(3));

        else if(argv[0] == "defo" || argv[0] == "deformations" ||
           argv[0] == "deformation" )
    return new ElementResponse(this, 4, Vector(3));

        else if(argv[0] == "defoANDforce" || argv[0] == "deformationANDforce" ||
           argv[0] == "deformationsANDforces" )
    return new ElementResponse(this, 5, Vector(6));

        else if( argv[0] == "stiff" || argv[0] == "stiffness" )
    return new ElementResponse(this, 6, Matrix(45,45) );

        else if(argv[0] == "plasticRotation" || argv[0] == "plasticDeformation")
                return new ElementResponse(this, 7, Vector(3));

        else
                return 0;

}

int XC::Joint3D::getResponse(int responseID, Information &eleInformation)
  {
        switch (responseID) {
        case -1:
                return -1;

        case 1:
                if(eleInformation.theVector!=0)
                {
                        const Vector &disp= theNodes[6]->getTrialDisp();
                        for( int i = 0 ; i<9 ; i++ )
                                (*(eleInformation.theVector))(i) = disp(i);
                }
                return 0;

        case 2:
                if(eleInformation.theVector!=0)
                {
                        const XC::Vector &node1Crd = theNodes[0]->getCrds();
                        const XC::Vector &node2Crd = theNodes[1]->getCrds();
                        const XC::Vector &node3Crd = theNodes[2]->getCrds();
                        const XC::Vector &node4Crd = theNodes[3]->getCrds();
                        const XC::Vector &node5Crd = theNodes[4]->getCrds();
                        const XC::Vector &node6Crd = theNodes[5]->getCrds();

                        const XC::Vector &node1Disp = theNodes[0]->getDisp();
                        const XC::Vector &node2Disp = theNodes[1]->getDisp();
                        const XC::Vector &node3Disp = theNodes[2]->getDisp();
                        const XC::Vector &node4Disp = theNodes[3]->getDisp();
                        const XC::Vector &node5Disp = theNodes[4]->getDisp();
                        const XC::Vector &node6Disp = theNodes[5]->getDisp();

                        Vector v1(3);
                        Vector v2(3);
                        Vector v3(3);
                        Vector v4(3);
                        Vector v5(3);
                        Vector v6(3);

                        // calculate the current coordinates of four external nodes
                        for(int i=0; i<3; i++)
                    {
                                v1(i) = node1Crd(i)+node1Disp(i);
                                v2(i) = node2Crd(i)+node2Disp(i);
                                v3(i) = node3Crd(i)+node3Disp(i);
                                v4(i) = node4Crd(i)+node4Disp(i);
                                v5(i) = node5Crd(i)+node5Disp(i);
                                v6(i) = node6Crd(i)+node6Disp(i);
                        }
                        v2 = v2 - v1;
                        v4 = v4 - v3;
                        v6 = v6 - v5;

                        v1(0) = sqrt( v2(0)*v2(0) + v2(1)*v2(1) + v2(2)*v2(2) );
                        v1(1) = sqrt( v4(0)*v4(0) + v4(1)*v4(1) + v4(2)*v4(2) );
                        v1(2) = sqrt( v6(0)*v6(0) + v6(1)*v6(1) + v6(2)*v6(2) );

                        *(eleInformation.theVector) = v1;
                }
                return 0;

        case 3:
                if( eleInformation.theVector != 0 )
                  (*(eleInformation.theVector))= physicalProperties.getStress();
                return 0;

        case 4:
                if(eleInformation.theVector!=0)
                  (*(eleInformation.theVector))= physicalProperties.getStrain();
                return 0;

        case 5:
                if(eleInformation.theVector!=0)
                  (*(eleInformation.theVector))= physicalProperties.getStrainStress();
                return 0;

        case 6:
                return eleInformation.setMatrix(this->getTangentStiff());

        case 7:
                if(eleInformation.theVector!=0)
                  (*(eleInformation.theVector))= physicalProperties.getResponse7();
                return 0;

        default:
                return -1;
        }
}


int XC::Joint3D::sendSelf(Communicator &comm)
  { return 0; }

int XC::Joint3D::recvSelf(const Communicator &comm)
  { return 0; }

