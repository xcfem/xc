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

// $Revision: 1.13 $
// $Date: 2005/11/29 23:31:10 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/model/dof_grp/DOF_Group.cpp,v $


// File: ~/analysis/model/dof_grp/DOF_Group.C
//
// Written: fmk
// Created: 11/96
// Revision: A
//
// Purpose: This file contains the code for implementing the methods
// of the XC::DOF_Group class interface.
//
// What: "@(#) DOF_Group.C, revA"


#include "DOF_Group.h"
#include <cstdlib>

#include "domain/mesh/node/Node.h"
#include "utility/matrix/Vector.h"
#include "utility/matrix/Matrix.h"
#include "solution/analysis/integrator/TransientIntegrator.h"

const int MAX_NUM_DOF= 256;


// static variables initialisation
XC::Matrix XC::DOF_Group::errMatrix(1,1);
XC::Vector XC::DOF_Group::errVect(1);
XC::UnbalAndTangentStorage XC::DOF_Group::unbalAndTangentArray(MAX_NUM_DOF+1);
int XC::DOF_Group::numDOF_Groups(0); // number of objects


void XC::DOF_Group::inicID(void)
  {
    const int sz= getNumDOF();
    // get number of DOF & verify valid
    if(sz <= 0)
      {
        std::cerr << "DOF_Group::inicID() ";
        std::cerr << " node must have at least 1 dof " << sz;
        exit(-1);
      }

    // initially set all the IDs to be -2
    for(int i=0; i<sz; i++)
      myID(i)= -2;
  }

//  DOF_Group(Node *);
//        constructor that take the corresponding model node.

XC::DOF_Group::DOF_Group(int tag, Node *node)
  :TaggedObject(tag), myID(node->getNumberDOF()),
   unbalAndTangent(node->getNumberDOF(),unbalAndTangentArray), myNode(node)
   
  {
    inicID();

    numDOF_Groups++;
  }


XC::DOF_Group::DOF_Group(int tag, int ndof)
  :TaggedObject(tag), myID(ndof),
   unbalAndTangent(ndof,unbalAndTangentArray), myNode(nullptr)
  {
    inicID();
    numDOF_Groups++;
  }

// ~DOF_Group();
//        destructor.

XC::DOF_Group::~DOF_Group(void)
  {
    numDOF_Groups--;

    //numDOF= unbalAndTangent.getNumDOF();

    // set the pointer in the associated Node to 0, to stop
    // segmentation fault if node tries to use this object after destroyed
    if(myNode)
      {
        myNode->setDOF_GroupPtr(nullptr);
        myNode= nullptr;
      }
  }

//! @brief to set the corresponding index of the ID to value.
void XC::DOF_Group::setID(int index, int value)
  {
    const int sz= getNumDOF(); 
    if((index >= 0) && (index < sz))
      myID(index) = value;
    else
      {
        std::cerr << "WARNING DOF_Group::setID - invalid location ";
        std::cerr << index << " in ID of size " << getNumDOF() << std::endl;
      }
  }

// void setID(const XC::ID &);
//        Method to set the XC::ID to be same as that given.

void XC::DOF_Group::setID(const ID &copy)
  { myID= copy; }

//! @brief Method to return the current ID.
const XC::ID &XC::DOF_Group::getID(void) const
  { return myID; }

int XC::DOF_Group::inicID(const int &value)
  {
    const ID &id= getID();
    const int sz= id.Size();
    for(int j=0;j<sz;j++)
      setID(j,value);
    return sz;
  }

int XC::DOF_Group::getNodeTag(void) const
  {
    if(myNode)
      return myNode->getTag();
    else
      return -1;
  }

int XC::DOF_Group::getNumFreeDOF(void) const
  {
    const int sz= getNumDOF();
    int numFreeDOF= sz;
    for(int i=0; i<sz; i++)
      if(myID(i) == -1)
        numFreeDOF--;
    return numFreeDOF;
  }

int XC::DOF_Group::getNumConstrainedDOF(void) const
  {
    int numConstr = 0;
    const int sz= getNumDOF();
    for(int i=0; i<sz; i++)
      if(myID(i) < 0)
        numConstr++;
    return numConstr;
  }



const XC::Matrix &XC::DOF_Group::getTangent(Integrator *theIntegrator)
  {
    if(theIntegrator)
      theIntegrator->formNodTangent(this);
    return unbalAndTangent.getTangent();
  }

void XC::DOF_Group::zeroTangent(void)
  { unbalAndTangent.getTangent().Zero(); }


void  XC::DOF_Group::addMtoTang(double fact)
  {
    if(myNode != 0)
      {
        if(unbalAndTangent.getTangent().addMatrix(1.0, myNode->getMass(), fact) < 0)
          {
            std::cerr << "DOF_Group::addMtoTang(void) ";
            std::cerr << " invoking addMatrix() on the tangent failed\n";
          }
      }
    else
      {
        std::cerr << "DOF_Group::addMtoTang(void) - no XC::Node associated";
        std::cerr << " subclass should provide the method \n";
      }
  }


void XC::DOF_Group::addCtoTang(double fact)
  {
    if(myNode != 0)
      {
        if(unbalAndTangent.getTangent().addMatrix(1.0, myNode->getDamp(), fact) < 0)
          {
            std::cerr << "DOF_Group::addMtoTang(void) ";
            std::cerr << " invoking addMatrix() on the tangent failed\n";
          }
      }
    else
      {
        std::cerr << "DOF_Group::addMtoTang(void) - no XC::Node associated";
        std::cerr << " subclass should provide the method \n";
      }
  }



void XC::DOF_Group::zeroUnbalance(void)
  { unbalAndTangent.getResidual().Zero(); }

const XC::Vector &XC::DOF_Group::getUnbalance(Integrator *theIntegrator)
  {
    if(theIntegrator != 0)
      theIntegrator->formNodUnbalance(this);
    return unbalAndTangent.getResidual();
  }


void XC::DOF_Group::addPtoUnbalance(double fact)
  {
    if(myNode != 0)
      {
        if(unbalAndTangent.getResidual().addVector(1.0, myNode->getUnbalancedLoad(), fact) < 0)
          {
            std::cerr << "DOF_Group::addPIncInertiaToUnbalance() -";
            std::cerr << " invoking addVector() on the unbalance failed\n";
          }
      }
    else
      {
        std::cerr << "DOF_Group::addPtoUnbalance() - no XC::Node associated";
        std::cerr << " subclass should provide the method \n";
      }
  }


void XC::DOF_Group::addPIncInertiaToUnbalance(double fact)
  {
    if(myNode)
      {
        if(unbalAndTangent.getResidual().addVector(1.0, myNode->getUnbalancedLoadIncInertia(), fact) < 0)
          {
            std::cerr << "DOF_Group::addPIncInertiaToUnbalance() - ";
             std::cerr << " invoking addVector() on the unbalance failed\n";
          }
      }
    else
      {
        std::cerr << "DOF_Group::addPIncInertiaToUnbalance() - no XC::Node associated";
        std::cerr << " subclass should provide the method \n";
      }
  }


void XC::DOF_Group::addM_Force(const XC::Vector &Udotdot, double fact)
  {
    if(!myNode)
      {
        std::cerr << "DOF_Group::addM_Force() - no XC::Node associated";
        std::cerr << " subclass should not call this method \n";
        return;
      }

    const int sz= getNumDOF();
    Vector accel(sz);
    // get accel for the unconstrained dof
    for(int i=0; i<sz; i++)
      {
        const int loc= myID(i);
        if(loc >= 0)
          accel(i) = Udotdot(loc);
        else accel(i) = 0.0;
      }

    if(unbalAndTangent.getResidual().addMatrixVector(1.0, myNode->getMass(), accel, fact) < 0)
      {
        std::cerr << "DOF_Group::addM_Force() ";
        std::cerr << " invoking addMatrixVector() on the unbalance failed\n";
      }
  }



const XC::Vector &XC::DOF_Group::getTangForce(const XC::Vector &Udotdot, double fact)
  {
    std::cerr << "DOF_Group::getTangForce() - not yet implemented";
    return unbalAndTangent.getResidual();
  }


const XC::Vector &XC::DOF_Group::getM_Force(const XC::Vector &Udotdot, double fact)
  {
    if(!myNode)
      {
        std::cerr << "DOF_Group::getM_Force() - no XC::Node associated";
        std::cerr << " subclass should not call this method \n";
        return unbalAndTangent.getResidual();
      }

    const int sz= getNumDOF();
    Vector accel(sz);
    // get accel for the unconstrained dof
    for (int i=0; i<sz; i++)
      {
        const int loc= myID(i);
        if(loc >= 0)
            accel(i) = Udotdot(loc);
        else accel(i) = 0.0;
      }

    if(unbalAndTangent.getResidual().addMatrixVector(0.0, myNode->getMass(), accel, fact) < 0)
      {
        std::cerr << "DOF_Group::getM_Force() ";
        std::cerr << " invoking addMatrixVector() on the unbalance failed\n";
      }
    return unbalAndTangent.getResidual();
  }



const XC::Vector &XC::DOF_Group::getC_Force(const XC::Vector &Udotdot, double fact)
  {
    if(!myNode)
      {
        std::cerr << "DOF_Group::getC_Force() - no XC::Node associated";
        std::cerr << " subclass should not call this method \n";
        return unbalAndTangent.getResidual();
      }

    const int sz= getNumDOF();
    Vector accel(sz);
    // get accel for the unconstrained dof
    for(int i=0; i<sz; i++)
      {
        const int loc= myID(i);
        if(loc >= 0)
          accel(i) = Udotdot(loc);
        else
          accel(i) = 0.0;
      }

    if(unbalAndTangent.getResidual().addMatrixVector(0.0, myNode->getDamp(), accel, fact) < 0)
      {
        std::cerr << "DOF_Group::getC_Force() ";
        std::cerr << " invoking addMatrixVector() on the unbalance failed\n";
      }
    return unbalAndTangent.getResidual();
  }


const XC::Vector &XC::DOF_Group::getCommittedDisp(void)
  {
    if(!myNode)
      {
        std::cerr << "DOF_Group::getCommittedDisp: no associated XC::Node ";
        std::cerr << " returning the error XC::Vector\n";
        return errVect;
      }
    return myNode->getDisp();
  }


const XC::Vector &XC::DOF_Group::getCommittedVel(void)
  {
    if(!myNode)
      {
        std::cerr << "DOF_Group::getCommittedVel: no associated XC::Node ";
        std::cerr << " returning the error XC::Vector\n";
        return errVect;
      }
    return myNode->getVel();
  }


const XC::Vector &XC::DOF_Group::getCommittedAccel(void)
  {
    if(!myNode)
      {
        std::cerr << "DOF_Group::getCommittedAccel: no associated XC::Node ";
        std::cerr << " returning the error XC::Vector\n";
        return errVect;
      }
    return myNode->getAccel();
  }

// void setNodeDisp(const XC::Vector &u);
//        Method to set the corresponding nodes displacements to the
//        values in u, components identified by myID;

void XC::DOF_Group::setNodeDisp(const Vector &u)
  {
    if(!myNode)
      {
        std::cerr << "DOF_Group::setNodeDisp: no associated XC::Node\n";
        return;
      }

    Vector &disp = unbalAndTangent.getResidual();
    disp= myNode->getTrialDisp();

    // get disp for my dof out of vector u
    const int sz= getNumDOF();
    for(int i=0; i<sz; i++)
      {
        const int loc= myID(i);
        if(loc >= 0)
          disp(i) = u(loc);
      }
    myNode->setTrialDisp(disp);
  }


// void setNodeVel(const XC::Vector &udot);
//        Method to set the corresponding nodes velocities to the
//        values in udot, components identified by myID;

void XC::DOF_Group::setNodeVel(const Vector &udot)
  {

    if(!myNode)
      {
        std::cerr << "DOF_Group::setNodeVel: 0 XC::Node Pointer\n";
        return;
      }

    Vector &vel = unbalAndTangent.getResidual();
    vel = myNode->getTrialVel();

    // get vel for my dof out of vector udot
    const int sz= getNumDOF();
    for(int i=0; i<sz; i++)
      {
        const int loc= myID(i);
        if(loc >= 0)
            vel(i) = udot(loc);
      }
    myNode->setTrialVel(vel);
  }



// void setNodeAccel(const Vector &udotdot);
//        Method to set the corresponding nodes accelerations to the
//        values in udotdot, components identified by myID;

void XC::DOF_Group::setNodeAccel(const Vector &udotdot)
  {

    if(!myNode)
      {
        std::cerr << "DOF_Group::setNodeAccel: 0 XC::Node Pointer\n";
        return;
      }

    Vector &accel = unbalAndTangent.getResidual();;
    accel = myNode->getTrialAccel();

    // get disp for the unconstrained dof
    const int sz= getNumDOF();
    for(int i=0; i<sz; i++)
      {
        const int loc= myID(i);
        if(loc >= 0)
          accel(i) = udotdot(loc);
      }

    myNode->setTrialAccel(accel);
  }


// void setNodeIncrDisp(const XC::Vector &u);
//        Method to set the corresponding nodes displacements to the
//        values in u, components identified by myID;

void XC::DOF_Group::incrNodeDisp(const Vector &u)
  {
    if(!myNode)
      {
        std::cerr << "DOF_Group::setNodeDisp: 0 XC::Node Pointer\n";
        exit(-1);
      }

    Vector &disp = unbalAndTangent.getResidual();;

    if(disp.Size() == 0)
      {
        std::cerr << "DOF_Group::setNodeIncrDisp - out of space\n";
        return;
      }

    // get disp for my dof out of vector u
    const int sz= getNumDOF();
    for(int i=0; i<sz; i++)
      {
        const int loc= myID(i);
        if(loc >= 0)
          disp(i) = u(loc);
        else
          disp(i) = 0.0;
      }
    myNode->incrTrialDisp(disp);
  }


// void setNodeincrVel(const XC::Vector &udot);
//        Method to set the corresponding nodes velocities to the
//        values in udot, components identified by myID;

void XC::DOF_Group::incrNodeVel(const Vector &udot)
  {

    if(!myNode)
      {
        std::cerr << "DOF_Group::setNodeVel: 0 XC::Node Pointer\n";
        exit(-1);
      }

    Vector &vel = unbalAndTangent.getResidual();

    // get vel for my dof out of vector udot
    const int sz= getNumDOF();
    for(int i=0; i<sz; i++)
      {
        const int loc= myID(i);
        if(loc >= 0)
          vel(i) = udot(loc);  // -1 for dof labelled 1 through ndof
        else
          vel(i) = 0.0;
      }
    myNode->incrTrialVel(vel);
  }



// void setNodeIncrAccel(const XC::Vector &udotdot);
//        Method to set the corresponding nodes accelerations to the
//        values in udotdot, components identified by myID;

void XC::DOF_Group::incrNodeAccel(const Vector &udotdot)
  {

    if(!myNode)
      {
        std::cerr << "DOF_Group::setNodeAccel: 0 XC::Node Pointer\n";
        exit(-1);
      }

    Vector &accel = unbalAndTangent.getResidual();

    // get disp for the unconstrained dof
    const int sz= getNumDOF();
    for(int i=0; i<sz; i++)
      {
        const int loc= myID(i);
        if(loc >= 0)
          accel(i) = udotdot(loc);
        else accel(i) = 0.0;
      }
    myNode->incrTrialAccel(accel);
  }



void XC::DOF_Group::setEigenvector(int mode, const Vector &theVector)
  {

    if(!myNode)
      {
        std::cerr << "DOF_Group::setNodeAccel: 0 XC::Node Pointer\n";
        exit(-1);
      }

    Vector &eigenvector = unbalAndTangent.getResidual();

    // get disp for the unconstrained dof
    const int sz= getNumDOF();
    for(int i=0; i<sz; i++)
      {
        const int loc= myID(i);
        if(loc >= 0)
          eigenvector(i) = theVector(loc);
        else
           eigenvector(i) = 0.0;
      }
    myNode->setEigenvector(mode, eigenvector);
  }


XC::Matrix *XC::DOF_Group::getT(void)
  { return nullptr; }



void XC::DOF_Group::addLocalM_Force(const Vector &accel, double fact)
  {
    if(myNode != 0)
      {
        if(unbalAndTangent.getResidual().addMatrixVector(1.0, myNode->getMass(), accel, fact) < 0)
          {
            std::cerr << "DOF_Group::addLocalM_Force() ";
            std::cerr << " invoking addMatrixVector() on the unbalance failed\n";
          }
      }
    else
      {
        std::cerr << "DOF_Group::addM_Force() - no XC::Node associated";
        std::cerr << " subclass should not call this method \n";
      }
  }


// AddingSensitivity:BEGIN ////////////////////////////////////////
const XC::Vector &XC::DOF_Group::getDispSensitivity(int gradNumber)
  {
    Vector &result = unbalAndTangent.getResidual();
    const int sz= getNumDOF();
    for(int i=0; i<sz; i++)
      { result(i) = myNode->getDispSensitivity(i+1,gradNumber);        }
    return result;
  }

const XC::Vector &XC::DOF_Group::getVelSensitivity(int gradNumber)
  {
    Vector &result = unbalAndTangent.getResidual();
    const int sz= getNumDOF();
    for(int i=0; i<sz; i++)
      { result(i) = myNode->getVelSensitivity(i+1,gradNumber); }
    return result;
  }

const XC::Vector &XC::DOF_Group::getAccSensitivity(int gradNumber)
  {
    Vector &result = unbalAndTangent.getResidual();
    const int sz= getNumDOF();
    for(int i=0; i<sz; i++)
      { result(i) = myNode->getAccSensitivity(i+1,gradNumber); }
    return result;
  }

int XC::DOF_Group::saveSensitivity(Vector *v,Vector *vdot,Vector *vdotdot,int gradNum,int numGrads)
  {
    // Displacement sensitivities
    const int sz= getNumDOF();
    Vector myV(sz);
    for(int i=0; i<sz; i++)
      {
        const int loc= myID(i);
        if(loc >= 0)
         { myV(i) = (*v)(loc); }
        else
         { myV(i) = 0.0; }
      }


    // Vel and Acc sensitivities only if they are being delivered
    if( (vdot != 0) && (vdotdot != 0) )
      {
        Vector myVdot(sz);
        for(int i=0; i<sz; i++)
          {
            const int loc= myID(i);
            if(loc >= 0)
              { myVdot(i) = (*vdot)(loc); }
            else
              { myVdot(i) = 0.0; }
          }
        Vector myVdotdot(sz);
        for(int i=0; i<sz; i++)
          {
            const int loc= myID(i);
            if(loc >= 0)
              { myVdotdot(i) = (*vdotdot)(loc); }
            else
              { myVdotdot(i) = 0.0; }
          }
        myNode->saveSensitivity(&myV, &myVdot, &myVdotdot, gradNum, numGrads);
      }
    else
      myNode->saveSensitivity(&myV, nullptr, nullptr, gradNum, numGrads);

    return 0;
  }

void XC::DOF_Group::addM_ForceSensitivity(const XC::Vector &Udotdot, double fact)
  {
    if(!myNode)
      {
        std::cerr << "DOF_Group::addM_Force() - no XC::Node associated";
        std::cerr << " subclass should not call this method \n";
        return;
      }

    const int sz= getNumDOF();
    Vector accel(sz);
    // get accel for the unconstrained dof
    for(int i=0; i<sz; i++)
      {
        const int loc= myID(i);
        if(loc >= 0)
            accel(i) = Udotdot(loc);
        else accel(i) = 0.0;
      }

    if(unbalAndTangent.getResidual().addMatrixVector(1.0, myNode->getMassSensitivity(), accel, fact) < 0)
      {
        std::cerr << "DOF_Group::addM_Force() ";
        std::cerr << " invoking addMatrixVector() on the unbalance failed\n";
      }
  }

void XC::DOF_Group::addD_Force(const XC::Vector &Udot, double fact)
  {
    if(!myNode)
      {
        std::cerr << "DOF_Group::addD_Force() - no XC::Node associated";
        std::cerr << " subclass should not call this method \n";
        return;
      }

    const int sz= getNumDOF();
    Vector vel(sz);
    // get velocity for the unconstrained dof
    for(int i=0; i<sz; i++)
      {
        const int loc= myID(i);
        if(loc >= 0)
          vel(i) = Udot(loc);
        else vel(i) = 0.0;
      }

    if(unbalAndTangent.getResidual().addMatrixVector(1.0, myNode->getDamp(), vel, fact) < 0)
      {
        std::cerr << "DOF_Group::addD_Force() ";
        std::cerr << " invoking addMatrixVector() on the unbalance failed\n";
      }
  }

void XC::DOF_Group::addD_ForceSensitivity(const Vector &Udot, double fact)
  {
    if(!myNode)
      {
        std::cerr << "DOF_Group::addD_ForceSensitivity() - no XC::Node associated";
        std::cerr << " subclass should not call this method \n";
        return;
      }

    const int sz= getNumDOF();
    Vector vel(sz);
    // get velocity for the unconstrained dof
    for(int i=0; i<sz; i++)
      {
        const int loc= myID(i);
        if(loc >= 0)
          vel(i) = Udot(loc);
        else
          vel(i) = 0.0;
      }

    if(unbalAndTangent.getResidual().addMatrixVector(1.0, myNode->getDampSensitivity(), vel, fact) < 0)
      {
        std::cerr << "DOF_Group::addD_ForceSensitivity() ";
        std::cerr << " invoking addMatrixVector() on the unbalance failed\n";
      }
  }

// AddingSensitivity:END //////////////////////////////////////////
void XC::DOF_Group::resetNodePtr(void)
  { myNode= nullptr; }


