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

// $Revision: 1.10 $
// $Date: 2005/12/19 22:43:36 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/HHT.cpp,v $

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 02/05
// Revision: A
//
// Description: This file contains the implementation of the XC::HHT class.
//
// What: "@(#) HHT.cpp, revA"

#include <solution/analysis/integrator/transient/rayleigh/HHT.h>
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
XC::HHT::HHT(SolutionStrategy *owr)
  : HHTBase(owr,INTEGRATOR_TAGS_HHT) {}

//! @brief Constructor.
//!
//! @param owr: analysis aggregation that will own this object.
XC::HHT::HHT(SolutionStrategy *owr,double _alpha)
  : HHTBase(owr,INTEGRATOR_TAGS_HHT,_alpha) {}

//! @brief Constructor.
//!
//! Sets \f$\alpha\f$ to \p alpha, \f$\gamma\f$ to \f$(1.5 - \alpha)\f$
//! and \f$\beta\f$ to \f$0.25*\alpha^2\f$.
//!
//! @param owr: analysis aggregation that will own this object.
//! @param rF: value of the Rayleigh damping factors.
XC::HHT::HHT(SolutionStrategy *owr,double _alpha,const RayleighDampingFactors &rF)
  : HHTBase(owr,INTEGRATOR_TAGS_HHT,_alpha,rF) {}

//! @brief Constructor.
//!
//! @param owr: analysis aggregation that will own this object.
//! @param _alpha: value for the alpha parameter.
//! @param _beta: value for the beta parameter.
//! @param _gamma: value for the gamma parameter.
XC::HHT::HHT(SolutionStrategy *owr,double _alpha, double _beta, double _gamma)
  : HHTBase(owr,INTEGRATOR_TAGS_HHT,_alpha,_beta,_gamma) {}

//! @brief Constructor.
//!
//! @param owr: analysis aggregation that will own this object.
//! @param _alpha: value for the alpha parameter.
//! @param _beta: value for the beta parameter.
//! @param _gamma: value for the gamma parameter.
//! @param rF: value of the Rayleigh damping factors.
XC::HHT::HHT(SolutionStrategy *owr,double _alpha, double _beta, double _gamma,const RayleighDampingFactors &rF)
  : HHTBase(owr,INTEGRATOR_TAGS_HHT,_alpha,_beta,_gamma,rF) {}

XC::Integrator *XC::HHT::getCopy(void) const
  { return new HHT(*this); }

//! The following are performed when this method is invoked:
//! \begin{enumerate}
//! \item First sets the values of the three constants {\em c1}, {\em c2}
//! and {\em c3}, {\em c1} is set to \f$1.0\f$, {\em c2} to \f$
//! \gamma / (\beta * \Delta t)\f$ and {\em c3} to \f$1/ (\beta * \Delta t^2)\f$.
//! \item Then the Vectors for response quantities at time \f$t\f$ are set
//! equal to those at time \f$t + \Delta t\f$.
//! \begin{tabbing}
//! while \= while \= while \= while \= \kill
//! \>\> \f$ U_t = U_{t + \Delta t}\f$
//! \>\> \f$ Ud_t = Ud_{t + \Delta t} \f$
//! \>\> \f$ Udd_t = Udd_{t + \Delta t} \f$ 
//! \end{tabbing}
//! \item Then the velocity and accelerations approximations at time \f$t +
//! \Delta t\f$ and the displacement and velocity at time \f$t + \alpha \Delta t\f$
//! are set using the difference approximations.
//! \begin{tabbing}
//! while \= while \= while \= while \= \kill
//! \>\> \f$ U_{t + \alpha \Delta t} = U_t\f$
//! \>\> \f$ \dot U_{t + \Delta t} = 
//!  \left( 1 - \frac{\gamma}{\beta}\right) \dot U_t + \Delta t \left(1
//! - \frac{\gamma}{2 \beta}\right) \ddot U_t \f$
//! \>\> \f$ Ud_{t + \alpha \Delta t} = (1 - \alpha) Ud_t + \alpha Ud_{t +
//! \Delta t}\f$
//! \>\> \f$ \ddot U_{t + \Delta t} = 
//!  - \frac{1}{\beta \Delta t} \dot U_t + \left( 1 - \frac{1}{2
//! \beta} \right) \ddot U_t  \f$
//! \>\> theModel-\f$>\f$setResponse\f$(U_{t + \alpha \Delta t}, Ud_{t+\alpha
//! \Delta t}, Udd_{t+\Delta t})\f$ 
//! \end{tabbing}
//! \item The response quantities at the DOF\_Group objects are updated
//! with the new approximations by invoking setResponse() on the
//! AnalysisModel with displacements and velocities at time \f$t + \alpha
//! \Delta t\f$ and the accelerations at time \f$t + \Delta t\f$.
//! \begin{tabbing}
//! while \= while \= while \= while \= \kill
//! \>\> theModel-\f$>\f$setResponse\f$(U_{t + \alpha \Delta t}, Ud_{t+\alpha
//! \Delta t}, Udd_{t+\Delta t})\f$ 
//! \end{tabbing}
//! \item current time is obtained from the AnalysisModel, incremented by
//! \f$\Delta t\f$, and {\em applyLoad(time, 1.0)} is invoked on the
//! AnalysisModel. 
//! \item Finally updateDomain() is invoked on the AnalysisModel.
//! \end{enumerate}
//! The method returns \f$0\f$ if successful, otherwise a negative number is
//! returned: \f$-1\f$ if \f$\gamma\f$ or \f$\beta\f$ are \f$0\f$, \f$-2\f$
//! if \p dispFlag was true and \f$\Delta t\f$ is \f$0\f$, and \f$-3\f$ if
//! domainChanged() failed or has not been called.
int XC::HHT::newStep(double _deltaT)
  {
    deltaT = _deltaT;
    if (beta == 0 || gamma == 0 )
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; error in variable\n"
		  << "gamma = " << gamma << " beta = " << beta << std::endl;
        return -1;
    }
    
    if (deltaT <= 0.0)  {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; error in variable\n";
        std::cerr << "dT = " << deltaT << std::endl;
        return -2;	
    }

    // get a pointer to the XC::AnalysisModel
    AnalysisModel *theModel = this->getAnalysisModelPtr();
    
    // set the constants
    c1 = 1.0;
    c2 = gamma/(beta*deltaT);
    c3 = 1.0/(beta*deltaT*deltaT);
       
    if(U.get().Size() == 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; failed or hasn't been called\n";
        return -3;
      }
    
    // set response at t to be that at t+deltaT of previous step
    Ut= U;

    // increment the time to t+alpha*deltaT and apply the load
    double time = getCurrentModelTime();
    time += alpha*deltaT;
//    theModel->applyLoadDomain(time);
    if(updateModel(time, deltaT) < 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; failed to update the domain\n";
        return -4;
      }

    // determine new_ velocities and accelerations at t+deltaT
    double a1 = (1.0 - gamma/beta);
    double a2 = deltaT*(1.0 - 0.5*gamma/beta);
    U.getDot().addVector(a1, Ut.getDotDot(), a2);
    
    double a3 = -1.0/(beta*deltaT);
    double a4 = 1.0 - 0.5/beta;
    U.getDotDot().addVector(a4, Ut.getDot(), a3);
    
    // determine the velocities at t+alpha*deltaT
    (Ualpha.getDot())= Ut.getDot();
    Ualpha.getDot().addVector((1.0-alpha), U.getDot(), alpha);
    
    // set the trial response quantities for the nodes
    theModel->setVel(Ualpha.getDot());
    theModel->setAccel(U.getDotDot());
        
    return 0;
  }


int XC::HHT::revertToLastStep()
  {
    // set response at t+deltaT to be that at t .. for next step
    U= Ut;
    return 0;
  }

//! This tangent for each FE\_Element is defined to be \f$K_e = c1\alpha K
//! + c2\alpha D + c3 M\f$, where c1,c2 and c3 were determined in the last
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
int XC::HHT::formEleTangent(FE_Element *theEle)
  {
    theEle->zeroTangent();
    if(statusFlag == CURRENT_TANGENT)
      {
        theEle->addKtToTang(alpha*c1);
        theEle->addCtoTang(alpha*c2);
        theEle->addMtoTang(c3);
      }
    else if(statusFlag == INITIAL_TANGENT)
      {
        theEle->addKiToTang(alpha*c1);
        theEle->addCtoTang(alpha*c2);
        theEle->addMtoTang(c3);
      }
    return 0;
  }
 

//! This performs the following:
//! \begin{tabbing}
//! while \= \+ while \= while \= \kill
//! if (RayleighDamping == false)  \+
//! theDof-\f$>\f$addMtoTang(c3)  \-
//! else \+
//! theDof-\f$>\f$addMtoTang(c3 + c2 * \f$\alpha_M\f$)  \- 
//! \end{tabbing}
int XC::HHT::formNodTangent(DOF_Group *theDof)
  {
    theDof->zeroTangent();

    theDof->addCtoTang(alpha*c2);
    theDof->addMtoTang(c3);
    
    return 0;
  }


//! If the size of the LinearSOE has changed, the object deletes any old Vectors
//! created and then creates \f$8\f$ new Vector objects of size equal to {\em
//! theLinearSOE-\f$>\f$getNumEqn()}. There is a Vector object created to store
//! the current displacement, velocity and accelerations at times \f$t\f$ and
//! \f$t + \Delta t\f$, and the displacement and velocity at time \f$t + \alpha
//! \Delta t\f$. The response quantities at time \f$t + \Delta t\f$ are
//! then set by iterating over the DOF\_Group objects in the model and
//! obtaining their committed values. 
//! Returns \f$0\f$ if successful, otherwise a warning message and a negative
//! number is returned: \f$-1\f$ if no memory was available for constructing
//! the Vectors.
int XC::HHT::domainChanged(void)
  {
    AnalysisModel *myModel = this->getAnalysisModelPtr();
    LinearSOE *theLinSOE = this->getLinearSOEPtr();
    const Vector &x = theLinSOE->getX();
    int size = x.Size();
    
    setRayleighDampingFactors();
    
    if(Ut.get().Size() != size)
      {
        
        Ut.resize(size);
        U.resize(size);
        Ualpha.resize(size);
      }
    
    // now go through and populate U, Udot and Udotdot by iterating through
    // the DOF_Groups and getting the last committed velocity and accel
    DOF_GrpIter &theDOFGroups = myModel->getDOFGroups();
    DOF_Group *dofGroupPtr= nullptr;
    
    while ((dofGroupPtr = theDOFGroups()) != 0)
      {
        const ID &id = dofGroupPtr->getID();
        
        const Vector &disp = dofGroupPtr->getCommittedDisp();	
        U.setDisp(id,disp);
        const Vector &vel = dofGroupPtr->getCommittedVel();
        U.setVel(id,vel);
        const Vector &accel = dofGroupPtr->getCommittedAccel();
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
    return 0;
  }

//! Invoked this first causes the object to increment the DOF\_Group
//! response quantities at time \f$t + \Delta t\f$. The displacement Vector is  
//! incremented by \f$ c1 * \Delta U\f$, the velocity Vector by \f$
//! c2 * \Delta U\f$, and the acceleration Vector by \f$c3 * \Delta U\f$. 
//! The displacement Vector at time \f$t + \alpha \Delta t\f$ is incremented
//! by \f$c1 \alpha \Delta U\f$ and the velocity Vector by \f$c2 \alpha
//! \Delta U\f$.
//! The response quantities at the DOF\_Group objects are then updated
//! with the new approximations by invoking setResponse() on the
//! AnalysisModel with displacement and velocity at time \f$t + \alpha
//! \Delta t\f$ and the accelerations at time \f$t + \Delta t\f$. 
//! Finally updateDomain() is invoked on the AnalysisModel. 
//! \begin{tabbing}
//! while \= \+ while \= while \= \kill
//! \>\> \f$ U_{t + \Delta t} += \Delta U\f$
//! \>\> \f$ \dot U_{t + \Delta t} += \frac{\gamma}{\beta \Delta t} \Delta U \f$
//! \>\> \f$ \ddot U_{t + \Delta t} += \frac{1}{\beta {\Delta t}^2} \Delta U \f$
//! \>\> \f$ U_{t + \alpha \Delta t} += \alpha \Delta U \f$
//! \>\> \f$ Ud_{t + \alpha \Delta t} += \frac{\alpha \gamma}{\beta \Delta t}
//! \Delta U \f$ 
//! \>\> theModel-\f$>\f$setResponse\f$(U_{t + \alpha \Delta t}, Ud_{t+\alpha
//! \Delta t}, Udd_{t+\Delta t})\f$
//! \>\> theModel-\f$>\f$updateDomain()
//! \end{tabbing}
//! Returns
//! \f$0\f$ if successful. A warning message is printed and a negative number
//! returned if an error occurs: \f$-1\f$ if no associated AnalysisModel,
//! \f$-2\f$ if the Vector objects have not been created, \f$-3\f$ if the Vector
//! objects and \f$\Delta U\f$ are of different sizes.
int XC::HHT::update(const Vector &deltaU)
  {
    AnalysisModel *theModel= this->getAnalysisModelPtr();
    if(theModel == nullptr)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; no AnalysisModel set.\n";
        return -1;
      }
    
    // check domainChanged() has been called, i.e. Ut will not be zero
    if(Ut.get().Size() == 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; domainChange() failed or not called\n";
        return -2;
      }
    
    // check deltaU is of correct size
    if(deltaU.Size() != U.get().Size())
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; vectors of incompatible size "
		  << " expecting " << U.get().Size()
		  << " obtained " << deltaU.Size() << std::endl;
        return -3;
      }
    
    //  determine the response at t+deltaT
    U.get()+= deltaU;
    U.getDot().addVector(1.0, deltaU, c2);
    U.getDotDot().addVector(1.0, deltaU, c3);

    // determine displacement and velocity at t+alpha*deltaT
    Ualpha.get()= Ut.get();
    Ualpha.get().addVector((1.0-alpha), U.get(), alpha);
    Ualpha.getDot()= Ut.getDot();
    Ualpha.getDot().addVector((1.0-alpha), U.getDot(), alpha);
    
    // update the response at the DOFs
    theModel->setResponse(Ualpha.get(),Ualpha.getDot(),U.getDotDot());
    if(updateModel() < 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; failed to update the domain.\n";
        return -4;
      }
    return 0;
  }

//! First the response quantities at the DOF\_Group objects are updated
//! with the new approximations by invoking setResponse() on the
//! AnalysisModel with displacement, velocity and accelerations at time \f$t +
//! \Delta t\f$. Finally updateDomain()} and {\em commitDomain() are
//! invoked on the AnalysisModel. 
//! Returns \f$0\f$ if successful, a warning
//! message and a negative number if not: \f$-1\f$ if no AnalysisModel
//! associated with the object and \f$-2\f$ if commitDomain() failed.
int XC::HHT::commit(void)
  {
    AnalysisModel *theModel = this->getAnalysisModelPtr();
    if(theModel == 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING no AnalysisModel set.\n";
        return -1;
      }
    
    // update the response at the DOFs
    theModel->setResponse(U.get(),U.getDot(),U.getDotDot());
//    if (theModel->updateDomain() < 0)  {
//        std::cerr << getClassName() << "::" << __FUNCTION__ <
//                  << ": failed to update the domain\n";
//        return -4;
//    }
    
    // set the time to be t+deltaT
    double time= getCurrentModelTime();
    time += (1.0-alpha)*deltaT;
    setCurrentModelTime(time);

    return commitModel();
  }

//! @brief Send object members through the communicator argument.
int XC::HHT::sendData(Communicator &comm)
  {
    int res= HHTBase::sendData(comm);
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::HHT::recvData(const Communicator &comm)
  {
    int res= HHTBase::recvData(comm);
    return res;
  }

//! @brief Send object through the communicator argument.
int XC::HHT::sendSelf(Communicator &comm)
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

//! @brief Receives object through the communicator argument.
int XC::HHT::recvSelf(const Communicator &comm)
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


//! The object sends to \f$s\f$ its type, the current time, \f$\alpha\f$,
//! \f$\gamma\f$ and \f$\beta\f$. If Rayleigh damping is specified, the
//! constants \f$\alpha_M\f$ and \f$\beta_K\f$ are also printed.
void XC::HHT::Print(std::ostream &s, int flag) const
  {
    HHTBase::Print(s,flag);
    s << "  alpha: " << alpha << " beta: " << beta
      << " gamma: " << gamma << std::endl;
    s << "  c1: " << c1 << " c2: " << c2
      << " c3: " << c3 << std::endl;
  }
