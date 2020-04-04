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

// $Revision: 1.5 $
// $Date: 2005/12/19 22:43:36 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/CentralDifference.cpp,v $

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 02/05
// Revision: A
//
// Description: This file contains the implementation of the XC::CentralDifference class.
//
// What: "@(#) CentralDifference.cpp, revA"

#include <solution/analysis/integrator/transient/rayleigh/CentralDifference.h>
#include <solution/analysis/model/fe_ele/FE_Element.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <utility/matrix/Vector.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include <solution/analysis/model/DOF_GrpIter.h>
#include <solution/analysis/model/AnalysisModel.h>

//! @brief Constructor.
//!
//! @param owr: analysis aggregation that will own this object.
XC::CentralDifference::CentralDifference(AnalysisAggregation *owr)
  : RayleighBase(owr,INTEGRATOR_TAGS_CentralDifference),
    c2(0.0), c3(0.0), Utm1(0), Ut(0), U(0) {}

//! @brief Constructor.
//!
//! @param owr: analysis aggregation that will own this object.
//! @param rF: value of the Rayleigh damping factors.
XC::CentralDifference::CentralDifference(AnalysisAggregation *owr,const RayleighDampingFactors &rF)
  : RayleighBase(owr,INTEGRATOR_TAGS_CentralDifference,rF),
    c2(0.0), c3(0.0), Utm1(0), Ut(0), U(0) {}

//! The following are performed when this method is invoked:
//! \begin{enumerate}
//! \item First sets the values of the three constants {\em c1}, {\em c2}
//! and {\em c3} depending on the flag indicating whether incremental
//! displacements or accelerations are being solved for at each iteration.
//! If \p dispFlag was \p true, {\em c1} is set to \f$1.0\f$, {\em c2} to \f$
//! \gamma / (\beta * deltaT)\f$ and {\em c3} to \f$1/ (\beta * deltaT^2)\f$. If
//! the flag is \p false {\em c1} is set to \f$\beta * deltaT^2\f$, {\em c2}
//! to \f$ \gamma * deltaT\f$ and {\em c3} to \f$1.0\f$. 
//! \item Then the Vectors for response quantities at time \f$t\f$ are set
//! equal to those at time \f$t + \Delta t\f$.
//! \item Then the velocity and accelerations approximations at time \f$t +
//! \delta t\f$ are set using the difference approximations if {\em
//! dispFlag} was \p true. (displacement and velocity if \p false).
//! \item The response quantities at the DOF\_Group objects are updated
//! with the new approximations by invoking setResponse() on the
//! AnalysisModel with new quantities for time \f$t + \Delta t\f$.
//! \item current time is obtained from the AnalysisModel, incremented by
//! \f$\Delta t\f$, and {\em applyLoad(time, 1.0)} is invoked on the
//! AnalysisModel. 
//! \item Finally updateDomain() is invoked on the AnalysisModel.
//! \end{enumerate}
//! The method returns \f$0\f$ if successful, otherwise a negative number is
//! returned: \f$-1\f$ if \f$\gamma\f$ or \f$\beta\f$ are \f$0\f$, \f$-2\f$
//! if \p dispFlag was true and \f$\Delta t\f$ is \f$0\f$, and \f$-3\f$
//! if domainChanged() failed or has not been called.
int XC::CentralDifference::newStep(double _deltaT)
  {
    updateCount = 0;
    
    deltaT = _deltaT;
    if(deltaT <= 0.0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; error in variable.\n"
		  << "dT = " << deltaT << std::endl;
        return -2;	
      }
    
    // get a pointer to the AnalysisModel
    AnalysisModel *theModel= this->getAnalysisModelPtr();
    
    // set the constants
    c2 = 0.5/deltaT;
    c3 = 1.0/(deltaT*deltaT);
    
    if(Ut.get().Size() == 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; domainChange() failed or hasn't been called\n";
        return -3;	
      }
    
    // increment the time and apply the load
    double time = getCurrentModelTime();
    applyLoadModel(time);
    
    // determine the garbage velocities and accelerations at t
    Ut.getDot().addVector(0.0, Utm1, -c2);
    
    Ut.getDotDot().addVector(0.0, Ut.get(), -2.0*c3);
    Ut.getDotDot().addVector(1.0, Utm1, c3);
    
    // set the garbage response quantities for the nodes
    theModel->setVel(Ut.getDot());
    theModel->setAccel(Ut.getDotDot());
    
    // set response at t to be that at t+deltaT of previous step
    (Ut.getDot()) = U.getDot();
    (Ut.getDotDot()) = U.getDotDot();
    
    return 0;
  }

//! This tangent for each FE\_Element is defined to be \f$K_e = c1 K + c2
//! D + c3 M\f$, where c1,c2 and c3 were determined in the last invocation
//! of the newStep() method.  The method returns \f$0\f$ after
//! performing the following operations:
//! \begin{tabbing}
//! while \= \+ while \= while \= \kill
//! theEle-\f$>\f$zeroTang()
//! theEle-\f$>\f$addKtoTang(c1)
//! theEle-\f$>\f$addCtoTang(c2)
//! theEle-\f$>\f$addMtoTang(c3) 
//! \end{tabbing}
int XC::CentralDifference::formEleTangent(FE_Element *theEle)
  {
    theEle->zeroTangent();
    
    theEle->addCtoTang(c2);
    theEle->addMtoTang(c3);
    
    return 0;
  }    


//! The method returns \f$0\f$ after performing the following operations:
//! \begin{tabbing}
//! while \= \+ while \= while \= \kill
//! theDof-\f$>\f$zeroUnbalance()
//! theDof-\f$>\f$addMtoTang(c3) 
//! \end{tabbing}
int XC::CentralDifference::formNodTangent(DOF_Group *theDof)
  {
    theDof->zeroTangent();
    
    theDof->addCtoTang(c2);
    theDof->addMtoTang(c3);

    return 0;
  }    

//! If the size of the LinearSOE has changed, the object deletes any old Vectors
//! created and then creates \f$6\f$ new Vector objects of size equal to {\em
//! theLinearSOE-\f$>\f$getNumEqn()}. There is a Vector object created to store
//! the current displacement, velocity and accelerations at times \f$t\f$ and
//! \f$t + \Delta t\f$. The response quantities at time \f$t + \Delta t\f$ are
//! then set by iterating over the DOF\_Group objects in the model and
//! obtaining their committed values. 
//! Returns \f$0\f$ if successful, otherwise a warning message and a negative
//! number is returned: \f$-1\f$ if no memory was available for constructing
//! the Vectors.
int XC::CentralDifference::domainChanged(void)
  {
    AnalysisModel *myModel = this->getAnalysisModelPtr();
    LinearSOE *theLinSOE = this->getLinearSOEPtr();
    const XC::Vector &x = theLinSOE->getX();
    int size = x.Size();
    
    setRayleighDampingFactors();

    if(Ut.get().Size() != size)
      {
        Utm1.resize(size);
        Ut.resize(size);
        U.resize(size);
      }
    
    // now go through and populate U, Udot and Udotdot by iterating through
    // the DOF_Groups and getting the last committed velocity and accel
    DOF_GrpIter &theDOFGroups = myModel->getDOFGroups();
    DOF_Group *dofGroupPtr= nullptr;
    
    while((dofGroupPtr = theDOFGroups()) != 0)
      {
        const ID &id = dofGroupPtr->getID();
        const int idSize = id.Size();
        
        const Vector &disp = dofGroupPtr->getCommittedDisp();	
        for(int i=0;i<idSize;i++)
          {
            const int loc = id(i);
            if(loc >= 0)
              { Utm1(loc) = disp(i); }
          }
        Ut.setDisp(id,disp);
        
        const Vector &vel = dofGroupPtr->getCommittedVel();
        U.setVel(id,vel);
        
        const XC::Vector &accel = dofGroupPtr->getCommittedAccel();	
        U.setAccel(id,accel);
        /** NOTE WE CAN't DO TOGETHER BECAUSE DOF_GROUPS USING SINGLE VECTOR ******
        for (int i=0; i < id.Size(); i++) {
          int loc = id(i);
          if (loc >= 0) {
 	    (*U)(loc) = disp(i);		
             (U.getDot())(loc) = vel(i);
          (U.getDotDot())(loc) = accel(i);
           }
         }
          ***/
      }
    
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; WARNING: assuming Ut-1 = Ut\n";
    
    return 0;
  }

//! Invoked this causes the object to increment the DOF\_Group
//! response quantities at time \f$t + \Delta t\f$. The displacement Vector is  
//! incremented by \f$ c1 * \Delta U\f$, the velocity Vector by \f$
//! c2 * \Delta U\f$, and the acceleration Vector by \f$c3 * \Delta U\f$. 
//! The response at the DOF\_Group objects are then updated by invoking
//! setResponse() on the AnalysisModel with quantities at time \f$t +
//! \Delta t\f$. Finally updateDomain() is invoked on the
//! AnalysisModel. Returns
//! \f$0\f$ if successful. A warning message is printed and a negative number
//! returned if an error occurs: \f$-1\f$ if no associated AnalysisModel,
//! \f$-2\f$ if the Vector objects have not been created, \f$-3\f$ if the Vector
//! objects and \f$\delta U\f$ are of different sizes.
int XC::CentralDifference::update(const Vector &upU)
  {
    updateCount++;
    if(updateCount > 1)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING - called more than once -"
		  << " CentralDifference integration scheme"
	          << " requires a LINEAR solution algorithm\n";
        return -1;
      }
    
    AnalysisModel *theModel = this->getAnalysisModelPtr();
    if(theModel == 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING - no XC::AnalysisModel set\n";
        return -1;
      }
    
    // check domainChanged() has been called, i.e. Ut will not be zero
    if(Ut.get().Size() == 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING - domainChange() failed or not called\n";
        return -2;
      }	
    
    // check U is of correct size
    if(upU.Size() != Ut.get().Size())
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING vectors of incompatible size "
		  << " expecting " << Ut.get().Size()
		  << " obtained " << upU.Size() << std::endl;
        return -3;
      }
    
    //  determine the response at t+deltaT
    U.getDot().addVector(0.0, upU, 3.0);
    U.getDot().addVector(1.0, Ut.get(), -4.0);
    U.getDot().addVector(1.0, Utm1, 1.0);
    (U.getDot())*= c2;
    
    U.getDotDot().addVector(0.0, U.getDot(), 1.0);
    U.getDotDot().addVector(1.0, Ut.getDot(), -1.0);
    (U.getDotDot()) /= deltaT;
    
    // set response at t to be that at t+deltaT of previous step
    Utm1= Ut.get();
    Ut.get()= upU;
   
    // update the response at the DOFs
    theModel->setResponse(upU, U.getDot(), U.getDotDot());
    if(updateModel() < 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; failed to update the domain\n";
        return -4;
      }
    return 0;
  }


int XC::CentralDifference::commit(void)
  {
    AnalysisModel *theModel = this->getAnalysisModelPtr();
    if(!theModel)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "WARNING no AnalysisModel set\n";
        return -1;
      }	  
    
    // set the time to be t+deltaT
    double time = getCurrentModelTime();
    time+= deltaT;
    setCurrentModelTime(time);
    
    return commitModel();
  }

//! @brief Send object members through the channel being passed as parameter.
int XC::CentralDifference::sendData(Communicator &comm)
  {
    int res= RayleighBase::sendData(comm);
    res+= comm.sendInt(updateCount,getDbTagData(),CommMetaData(4));
    res+= comm.sendDoubles(c2,c3,getDbTagData(),CommMetaData(5));
    res+= comm.sendVector(Utm1,getDbTagData(),CommMetaData(6));
    res+= comm.sendMovable(Ut,getDbTagData(),CommMetaData(7));
    res+= comm.sendMovable(U,getDbTagData(),CommMetaData(8));
    return res;
  }

//! @brief Receives object members through the channel being passed as parameter.
int XC::CentralDifference::recvData(const Communicator &comm)
  {
    int res= RayleighBase::recvData(comm);
    res+= comm.receiveInt(updateCount,getDbTagData(),CommMetaData(4));
    res+= comm.receiveDoubles(c2,c3,getDbTagData(),CommMetaData(5));
    res+= comm.receiveVector(Utm1,getDbTagData(),CommMetaData(6));
    res+= comm.receiveMovable(Ut,getDbTagData(),CommMetaData(7));
    res+= comm.receiveMovable(U,getDbTagData(),CommMetaData(8));
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::CentralDifference::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(9);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to send data\n";
    return res;
  }


//! @brief Receives object through the channel being passed as parameter.
int XC::CentralDifference::recvSelf(const Communicator &comm)
  {
    inicComm(9);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(comm);
        if(res<0)
          std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; failed to receive data.\n";
      }
    return res;
  }

//! @brief The object sends to \f$s\f$ its type, the current time,
//! \f$\gamma\f$ and \f$\beta\f$. 
void XC::CentralDifference::Print(std::ostream &s, int flag) const
  {
    RayleighBase::Print(s,flag);
  }
