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
                                                                        
// $Revision: 1.4 $
// $Date: 2003/02/14 23:00:49 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/WilsonTheta.cpp,v $
                                                                        
                                                                        
// File: ~/analysis/integrator/WilsonTheta.C
// 
// Written: fmk 
// Created: 11/98
// Revision: A
//
// Description: This file contains the implementation of XC::WilsonTheta.
//
// What: "@(#) WilsonTheta.C, revA"

#include <solution/analysis/integrator/transient/rayleigh/WilsonTheta.h>
#include <solution/analysis/model/fe_ele/FE_Element.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <utility/matrix/Vector.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include <solution/analysis/model/DOF_GrpIter.h>
#include <solution/analysis/model/AnalysisModel.h>

//! @brief Constructor. \f$\theta\f$ is set to zero.
//!
//! @param owr: analysis aggregation that will own this object.
XC::WilsonTheta::WilsonTheta(SolutionStrategy *owr)
:RayleighBase(owr,INTEGRATOR_TAGS_WilsonTheta),
 theta(0), c1(0.0), c2(0.0), c3(0.0) {}

//! @brief Constructor.
//!
//! Sets \f$\theta\f$ to \p _theta, \f$\gamma\f$ to \f$(1.5 - \alpha)\f$
//! and \f$\beta\f$ to \f$0.25*\alpha^2\f$. In addition, a flag is set
//! indicating that Rayleigh damping will not be used.
//!
//! @param owr: analysis aggregation that will own this object.
//! @param _theta: value for the theta parameteros.
XC::WilsonTheta::WilsonTheta(SolutionStrategy *owr,double _theta)
:RayleighBase(owr,INTEGRATOR_TAGS_WilsonTheta),
 theta(_theta), c1(0.0), c2(0.0), c3(0.0) {}

//! @brief Constructor.
//!
//! This constructor is invoked if Rayleigh damping is to be used, 
//! i.e. \f$D = \alpha_M M + \beta_K K\f$. Sets \f$\theta\f$ to \p theta,
//! \f$\gamma\f$ to \f$(1.5 - \alpha)\f$, \f$\beta\f$
//! to \f$0.25*\alpha^2\f$, \f$\alpha_M\f$ to \p alphaM and \f$\beta_K\f$
//! to \p betaK. Sets a flag indicating whether the incremental solution is
//! done in terms of displacement or acceleration to \p dispFlag and a flag
//! indicating that Rayleigh damping will be used.
//! 
//! @param owr: analysis aggregation that will own this object.
//! @param _theta: value for the theta parameteros.
//! @param rF: value of the Rayleigh damping factors.
XC::WilsonTheta::WilsonTheta(SolutionStrategy *owr,double _theta,const RayleighDampingFactors &rF)
  :RayleighBase(owr,INTEGRATOR_TAGS_WilsonTheta,rF), theta(_theta),
   c1(0.0), c2(0.0), c3(0.0) {}

//! @brief Virtual constructor.
XC::Integrator *XC::WilsonTheta::getCopy(void) const
  { return new WilsonTheta(*this); }

//! @brief New analysis step.
//!
//! The following are performed when this method is invoked:
//! \begin{enumerate}
//! \item First sets the values of the three constants {\em c1}, {\em c2}
//! and {\em c3}: {\em c1} is set to \f$1.0\f$, 
//! {\em c2} to \f$3 / (\Theta
//! \Delta t)\f$ and {\em c3} to \f$6 / (\Theta \Delta t)^2)\f$. 
//! \item Then the Vectors for response quantities at time \f$t\f$ are set
//! equal to those at time \f$t + \Delta t\f$.
//! \begin{tabbing}
//! while \= while \= while \= while \= \kill
//! \>\> \f$ U_t = U_{t + \Delta t}\f$
//! \>\> \f$ Ud_t = Ud_{t + \Delta t} \f$
//! \>\> \f$ Udd_t = Udd_{t + \Delta t} \f$ 
//! \end{tabbing}
//! \item Then the velocity and accelerations approximations
//! at time \f$t + \Theta \Delta t\f$ are set using the difference
//! approximations,
//! \begin{tabbing}
//! while \= while \= while \= while \= \kill
//! \>\> \f$ U_{t + \theta \Delta t} = U_t \f$
//! \>\> \f$ \dot U_{t + \theta \Delta t} = - 2 \dot U_t + \frac{\theta
//! \Delta t}{2} \ddot U_t  \f$
//! \>\> \f$ \ddot U_{t + \theta \Delta t} = - \frac{6}{\theta \Delta t}
//! \dot U_t -2 Udd_t \f$ 
//! \end{tabbing}
//! \item The response quantities at the DOF\_Group objects are updated
//! with the new approximations by invoking setResponse() on the
//! AnalysisModel with quantities at time \f$t + \Theta \Delta t\f$.
//! \begin{tabbing}
//! while \= while \= while \= while \= \kill
//! \>\> theModel-\f$>\f$setResponse\f$(U_{t + \theta \Delta t}, Ud_{t+\theta
//! \Delta t}, Udd_{t+ \theta \Delta t})\f$ 
//! \end{tabbing}
//! \item current time is obtained from the AnalysisModel, incremented by
//! \f$\Theta \Delta t\f$, and {\em applyLoad(time, 1.0)} is invoked on the
//! AnalysisModel. 
//! \item Finally updateDomain() is invoked on the AnalysisModel.
//! \end{enumerate}
//! The method returns \f$0\f$ if successful, otherwise a negative number is
//! returned: \f$-1\f$ if \f$\gamma\f$ or \f$\beta\f$ are \f$0\f$, \f$-2\f$
//! if \p dispFlag was true and \f$\Delta t\f$ is \f$0\f$, and \f$-3\f$ if
//! domainChanged() failed or has not been called.
int XC::WilsonTheta::newStep(double _deltaT)
  {
    if(theta <= 0.0 )
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; error in variable\n"
		  << "theta: " << theta << " <= 0.0\n";
        return -1;
      }

  
    if(_deltaT <= 0.0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; error in variable\n"
		  << "dT = " << deltaT << std::endl;
        return -2;	
      }

  deltaT = _deltaT;
  c1 = 1.0;
  c2 = 3.0/(theta*deltaT);
  c3 = 2*c2/(theta*deltaT);
    
  if (U.get().Size() == 0)
    {
      std::cerr << getClassName() << "::" << __FUNCTION__
  	        << "; domainChange() failed or hasn't been called\n"; 
      return -3;	
    }

  // set response at t to be that at t+delta t of previous step
  Ut= U;        
    
  // set new_ velocity and accelerations at t + theta delta t
  //  (U.getDot()) *= -2.0;
  double a1 = -0.5*theta*deltaT; 
  U.getDot().addVector(-2.0,Ut.getDotDot(),a1);

  //  (U.getDotDot()) *= -2.0;  
  double a2 = -6.0/theta/deltaT; 
  U.getDotDot().addVector(-2.0, Ut.getDot(), a2);
    

  // set the new_ trial response quantities
  AnalysisModel *theModel = this->getAnalysisModelPtr();
  theModel->setResponse(U.get(),U.getDot(),U.getDotDot());        

  // increment the time and apply the load
  double time = getCurrentModelTime();
  time+= theta * deltaT;
  applyLoadModel(time);
  updateModel();
  
  return 0;
}

//! @brief Form tangent stiffness for FE\_Element.
//! 
//! This tangent for each FE\_Element is defined to be \f$K_e = c1 K
//! + c2  D + c3 M\f$, where c1,c2 and c3 were determined in the last
//! invocation of the newStep() method. Returns \f$0\f$ after performing the
//! following operations:  
//! \begin{tabbing}
//! while \= \+ while \= while \= \kill
//! if (RayleighDamping == false) \{ \+
//! theEle-\f$>\f$zeroTang()
//! theEle-\f$>\f$addKtoTang(c1)
//! theEle-\f$>\f$addCtoTang(c2)
//! theEle-\f$>\f$addMtoTang(c3)  \-
//! \} else \{ \+
//! theEle-\f$>\f$zeroTang()
//! theEle-\f$>\f$addKtoTang(c1 + c2 * \f$\beta_K\f$)
//! theEle-\f$>\f$addMtoTang(c3 + c2 * \f$\alpha_M\f$)  \- 
//! \}
//! \end{tabbing}
int XC::WilsonTheta::formEleTangent(FE_Element *theEle)
  {
    theEle->zeroTangent();
    theEle->addKtToTang(c1);
    theEle->addCtoTang(c2);
    theEle->addMtoTang(c3);
    return 0;
  }    

//! @brief Form tangent stiffness for DOF_Group.
//! 
//! This performs the following:
//! \begin{tabbing}
//! while \= \+ while \= while \= \kill
//! theDof-\f$>\f$zeroUnbalance()
//! if (RayleighDamping == false)  \+
//! theDof-\f$>\f$addMtoTang(c3)  \-
//! else \+
//! theDof-\f$>\f$addMtoTang(c3 + c2 * \f$\alpha_M\f$)  \- 
//! \end{tabbing}
int XC::WilsonTheta::formNodTangent(DOF_Group *theDof)
  {
    theDof->zeroTangent();
    theDof->addMtoTang(c3);
    theDof->addCtoTang(c2);
    return(0);
  }    

//! If the size of the LinearSOE has changed, the object deletes any old Vectors
//! created and then creates \f$6\f$ new Vector objects of size equal to {\em
//! theLinearSOE-\f$>\f$getNumEqn()}. There is a Vector object created to store
//! the current displacement, velocity and accelerations at times \f$t\f$ and
//! \f$t + \Delta t\f$ (between newStep()} and {\em commit() the \f$t +
//! \Delta t\f$ quantities store \f$t + \Theta \Delta t\f$ quantities).
//! The response quantities at time \f$t + \Delta t\f$ are
//! then set by iterating over the DOF\_Group objects in the model and
//! obtaining their committed values. 
//! Returns \f$0\f$ if successful, otherwise a warning message and a negative
//! number is returned: \f$-1\f$ if no memory was available for constructing
//! the Vectors.
int XC::WilsonTheta::domainChanged()
  {
    AnalysisModel *myModel = this->getAnalysisModelPtr();
    LinearSOE *theLinSOE = this->getLinearSOEPtr();
    const XC::Vector &x = theLinSOE->getX();
    int size = x.Size();

    setRayleighDampingFactors();
  
    if(Ut.get().Size() != size)
      {
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
        
        const Vector &disp = dofGroupPtr->getCommittedDisp();	
        U.setDisp(id,disp);
        
        const Vector &vel = dofGroupPtr->getCommittedVel();
        U.setVel(id,vel);
        
        const XC::Vector &accel = dofGroupPtr->getCommittedAccel();	
        U.setAccel(id,accel);
        // NOTE WE CAN't DO TOGETHER BECAUSE DOF_GROUPS USING SINGLE VECTOR
      }
    return 0;
  }

//! Invoked this first causes the object to increment the DOF\_Group
//! response quantities at time \f$t + \Theta \Delta t\f$. The displacement Vector is  
//! incremented by \f$ c1 * \Delta U\f$, the velocity Vector by \f$
//! c2 * \Delta U\f$, and the acceleration Vector by \f$c3 * \Delta U\f$. 
//! The response quantities at the DOF\_Group objects are then updated
//! with the new approximations by invoking setResponse() on the
//! AnalysisModel with displacements, velocities and accelerations at time
//! \f$t + \Theta \Delta t\f$.
//! Finally updateDomain() is invoked on the AnalysisModel. 
//! \begin{tabbing}
//! while \= while \= while \= while \= \kill
//! \>\> \f$ U_{t + \theta \Delta t} += \Delta U\f$
//! \>\> \f$ \dot U_{t + \theta \Delta t} += \frac{3}{\theta \Delta t}
//! \Delta U  \f$
//! \>\> \f$ \ddot U_{t + \theta \Delta t} += \frac{6}{\theta^2 \Delta
//! t^2} \Delta U \f$ 
//! \>\> theModel-\f$>\f$setResponse\f$(U_{t + \alpha \theta t}, Ud_{t+\theta
//! \Delta t}, Udd_{t+ \theta \Delta t})\f$
//! \>\> theModel-\f$>\f$updateDomain()
//! \end{tabbing}
//! Returns \f$0\f$ if successful. A warning message is printed and a negative number
//! returned if an error occurs: \f$-1\f$ if no associated AnalysisModel, \f$-2\f$
//! if the Vector objects have not been created, \f$-3\f$ if the Vector
//! objects and \f$\Delta U\f$ are of different sizes.
int XC::WilsonTheta::update(const Vector &deltaU)
  {
    AnalysisModel *theModel = this->getAnalysisModelPtr();
    if (theModel == 0)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING - no AnalysisModel set.\n";
	return -1;
      }	

    // check domainChanged() has been called, i.e. Ut will not be zero
    if (Ut.get().Size() == 0)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING - domainChange() failed or not called.\n";
	return -2;
      }	

    // check deltaU is of correct size
    if (deltaU.Size() != U.get().Size())
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING - Vectors of incompatible size "
		  << " expecting " << U.get().Size()
		  << " obtained " << deltaU.Size() << std::endl;
	return -3;
      }

    // determine the response at t + theta * deltaT
    (U.get())+= deltaU;
    U.getDot().addVector(1.0, deltaU,c2);
    U.getDotDot().addVector(1.0, deltaU,c3);

    // update the responses at the DOFs
    theModel->setResponse(U.get(),U.getDot(),U.getDotDot());        
    updateModel();

    return 0;
  }    

//! First the quantities at time \f$t + \Delta t\f$ are determined using the
//! quantities at time \f$t\f$ and \f$t + \Theta \Delta t\f$.
//! Then the response quantities at the DOF\_Group objects are updated
//! with the new approximations by invoking setResponse() on the
//! AnalysisModel with displacement, velocity and accelerations at time \f$t +
//! \Delta t\f$. The time is obtained from the AnalysisModel and \f$(\Theta
//! -1) \Delta t\f$ is subtracted from the value. The time is set in the
//! Domain by invoking {\em setCurrentDomainTime(time)} on the
//! AnalysisModel. Finally updateDomain()} and {\em commitDomain()
//! are invoked on the AnalysisModel. 
//! \begin{tabbing}
//! while \= while \= while \= while \= \kill
//! \>\> \f$Udd_{t + \Delta t} = Udd_t + \frac{1}{\theta} \left( Udd_{t +
//! \theta \Delta t} - Udd_t \right)\f$
//! \>\> \f$ Ud_{t + \Delta t} = Ud_t + \frac{\Delta t}{2}\left( Udd_{t +
//! \Delta t} + Udd_t \right) \f$
//! \>\> \f$ U_{t + \Delta t} = U_t + \Delta tUd_t +
//! \frac{\Delta t^2}{6}\left( Udd_{t + \Delta t} + 2 Udd_t \right) \f$
//! \>\> theModel-\f$>\f$setResponse\f$(U_{t + \Delta t}, Ud_{t+
//! \Delta t}, Udd_{t+\Delta t})\f$
//! \>\> time = theModel-\f$>\f$getDomainTime()
//! \>\> time -= \f$(\theta -1) * \Delta t\f$
//! \>\> theModel-\f$>\f$setTime(time)
//! \>\> theModel-\f$>\f$commitDomain()
//! \end{tabbing}
//! Returns \f$0\f$ if successful, a warning
//! message and a negative number if not: \f$-1\f$ if no AnalysisModel
//! associated with the object and \f$-2\f$ if commitDomain() failed.
int XC::WilsonTheta::commit(void)
  {

    AnalysisModel *theModel= this->getAnalysisModelPtr();
    if(theModel == nullptr)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; WARNING - no AnalysisModel set\n";
        return -1;
      }	  

    // determine the quantities at t + delta t 
    /* THIS IS WHAT IS IN BATHE'S BOOK - SEE BELOW FOR QUICKER SOLN
    double a1 = c3/theta;
    double a2 = c3*deltaT;	
    double a3 = (1 - 3.0/theta);
    (U.getDotDot()) = *U;
    (U.getDotDot()) -= Ut.get();
    (U.getDotDot()) *= a1;
    U.getDotDot().addVector(Ut.getDot(), -a2);
    U.getDotDot().addVector(Ut.getDotDot(), a3);
    */

    /* REVISION BASED ON WHAT IS IN CHOPRA's BOOK - MAKES SENSE - LINEAR ACCEL */

    // determine response quantities at Ut+dt given Ut and Ut + theta dt
    double a1,a2;
    (U.getDotDot()) -= Ut.getDotDot();
    (U.getDotDot()) *= 1/theta;
    (U.getDotDot()) += Ut.getDotDot();

    (U.getDot()) = Ut.getDot();
    a1 = 0.5*deltaT;
    U.getDot().addVector(1.0, U.getDotDot(), a1);
    U.getDot().addVector(1.0, Ut.getDotDot(), a1);


    (U.get()) = Ut.get();
    U.get().addVector(1.0, Ut.getDot(), deltaT);
    a2 = deltaT*deltaT/6.0;
    U.get().addVector(1.0, U.getDotDot(), a2);
    U.get().addVector(1.0, Ut.getDotDot(), 2*a2);
    // update the t + delta t responses at the DOFs 
    theModel->setResponse(U.get(),U.getDot(),U.getDotDot());        
    updateModel();

    // set the time to be t+delta t
    double time = getCurrentModelTime();
    time -= (theta -1) * deltaT;
    setCurrentModelTime(time);

    // now invoke commit on the XC::AnalysisModel
    return commitModel();
  }

//! @brief Send object members through the communicator argument.
int XC::WilsonTheta::sendData(Communicator &comm)
  {
    int res= RayleighBase::sendData(comm);
    res+= comm.sendDoubles(theta,c1,c2,c3,getDbTagData(),CommMetaData(4));
    res+= comm.sendMovable(Ut,getDbTagData(),CommMetaData(5));
    res+= comm.sendMovable(U,getDbTagData(),CommMetaData(6));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::WilsonTheta::recvData(const Communicator &comm)
  {
    int res= RayleighBase::recvData(comm);
    res+= comm.receiveDoubles(theta,c1,c2,c3,getDbTagData(),CommMetaData(4));
    res+= comm.receiveMovable(Ut,getDbTagData(),CommMetaData(5));
    res+= comm.receiveMovable(U,getDbTagData(),CommMetaData(6));
    return res;
  }

//! @brief Sends object through the communicator argument.
int XC::WilsonTheta::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(7);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to send data\n";
    return res;
  }

//! @brief Receives object through the communicator argument.
int XC::WilsonTheta::recvSelf(const Communicator &comm)
  {
    inicComm(7);
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

//! @brief Printing stuff.
//! 
//! The object sends to \f$s\f$ its type, the current time, \f$\alpha\f$,
//! \f$\gamma\f$ and \f$\beta\f$. 
void XC::WilsonTheta::Print(std::ostream &os, int flag) const
  {
    RayleighBase::Print(os,flag);
    os << " theta: " << theta << std::endl;
  }

