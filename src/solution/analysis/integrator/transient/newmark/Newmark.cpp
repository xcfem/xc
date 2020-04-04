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
// $Date: 2006/01/10 00:34:55 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/Newmark.cpp,v $

// Written : fmk
// Created : 11/98
// Modified: 02/05 ahs
// Revision: A
//
// Description: This file contains the implementation of the Newmark class.
//
// What: "@(#) Newmark.C, revA"

#include <solution/analysis/integrator/transient/newmark/Newmark.h>
#include <solution/analysis/model/fe_ele/FE_Element.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <utility/matrix/Vector.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include <solution/analysis/model/DOF_GrpIter.h>
#include <solution/analysis/model/AnalysisModel.h>

#include "utility/actor/actor/ArrayCommMetaData.h"

//! @brief Constructor.
//!
//! @param owr: analysis aggregation that will own this object.
XC::Newmark::Newmark(AnalysisAggregation *owr)
  : NewmarkBase2(owr,INTEGRATOR_TAGS_Newmark),
    displ(true), determiningMass(false) {}

//! @brief Constructor.
//!
//! @param owr: analysis aggregation that will own this object.
//! @param _gammma: gamma factor for Newmark method.
//! @param _beta: beta factor for Newmark method.
//! @param dispFlag: if true, the incremental solution is done in terms
//!                  of displacement otherwise it's done in terms
//!                  of acceleration (and then, in addition, a flag is set
//!                  indicating that Rayleigh damping will not be used.
XC::Newmark::Newmark(AnalysisAggregation *owr,double _gamma, double _beta, bool dispFlag)
  : NewmarkBase2(owr,INTEGRATOR_TAGS_Newmark,_gamma,_beta),
    displ(dispFlag), determiningMass(false) {}

//! @brief Constructor.
//!
//! @param owr: analysis aggregation that will own this object.
//! @param _gammma: gamma factor for Newmark method.
//! @param _beta: beta factor for Newmark method.
//! @param fF: Rayleigh damping factors.
//! @param dispFlag: if true, the incremental solution is done in terms
//!                  of displacement otherwise it's done in terms
//!                  of acceleration (and then, in addition, a flag is set
//!                  indicating that Rayleigh damping will not be used.
XC::Newmark::Newmark(AnalysisAggregation *owr,double _gamma, double _beta,const RayleighDampingFactors &rF,bool dispFlag)
  : NewmarkBase2(owr,INTEGRATOR_TAGS_Newmark,_gamma,_beta,rF),
    displ(dispFlag), determiningMass(false) {}

//! The following are performed when this method is invoked:
//! \begin{enumerate}
//! \item First sets the values of the three constants {\em c1}, {\em c2}
//! and {\em c3} depending on the flag indicating whether incremental
//! displacements or accelerations are being solved for at each iteration.
//! If \p dispFlag was \p true, {\em c1} is set to \f$1.0\f$, {\em c2} to \f$
//! \gamma / (\beta \Delta t)\f$ and {\em c3} to \f$1/ (\beta \Delta t^2)\f$.
//! If the flag is \p false {\em c1} is set to \f$\beta \Delta t^2\f$, {\em c2} //! to \f$ \gamma \Delta t\f$ and {\em c3} to \f$1.0\f$. 
//! \item Then the Vectors for response quantities at time \f$t\f$ are set
//! equal to those at time \f$t + \Delta t\f$.
//! \begin{tabbing}
//! while \= while \= while \= while \= \kill
//! \>\> \f$ U_t = U_{t + \Delta t}\f$
//! \>\> \f$ Ud_t = Ud_{t + \Delta t} \f$
//! \>\> \f$ Udd_t = Udd_{t + \Delta t} \f$ 
//! \end{tabbing}
//! \item Then the velocity and accelerations approximations at time \f$t +
//! \Delta t\f$ are set using the difference approximations if {\em
//! dispFlag} was \p true. (displacement and velocity if \p false).
//! \begin{tabbing}
//! while \= while \= while \= while \= \kill
//! \>\> if (displIncr == true) \{
//! \>\>\> \f$ \dot U_{t + \Delta t} = 
//! \left( 1 - \frac{\gamma}{\beta}\right) \dot U_t + \Delta t \left(1
//! - \frac{\gamma}{2 \beta}\right) \ddot U_t \f$
//! \>\>\> \f$ \ddot U_{t + \Delta t} = 
//!  - \frac{1}{\beta \Delta t} \dot U_t + \left( 1 - \frac{1}{2
//! \beta} \right) \ddot U_t  \f$
//! \>\> \} else \{
//! \>\>\> \f$ U_{t + \Delta t} = U_t + \Delta t Ud_t + \frac{\Delta
//! t^2}{2}Udd_t\f$
//! \>\>\> \f$ Ud_{t + \Delta t} = Ud_t +  \Delta t Udd_t \f$
//! \>\> \} 
//! \end{tabbing}
//! \item The response quantities at the DOF\_Group objects are updated
//! with the new approximations by invoking setResponse() on the
//! AnalysisModel with new quantities for time \f$t + \Delta t\f$.
//! \begin{tabbing}
//! while \= while \= while \= while \= \kill
//! \>\> theModel-\f$>\f$setResponse\f$(U_{t + \Delta t}, Ud_{t+\Delta t},
//! Udd_{t+\Delta t})\f$ 
//! \end{tabbing}
//! \item current time is obtained from the AnalysisModel, incremented by
//! \f$\Delta t\f$, and {\em applyLoad(time, 1.0)} is invoked on the
//! AnalysisModel. 
//! \item Finally updateDomain() is invoked on the AnalysisModel.
//! \end{enumerate}
//! The method returns \f$0\f$ if successful, otherwise a negative number is
//! returned: \f$-1\f$ if \f$\gamma\f$ or \f$\beta\f$ are \f$0\f$, \f$-2\f$ if
//! \p dispFlag was true and \f$\Delta t\f$ is \f$0\f$, and \f$-3\f$ if
//! domainChanged() failed or has not been called.
int XC::Newmark::newStep(double deltaT)
  {
    if(beta == 0 || gamma == 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; error in variable\n"
		  << "gamma = " << gamma << " beta = " << beta << std::endl;
        return -1;
      }
    
    if(deltaT <= 0.0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; error in variable\n"
		  << "dT = " << deltaT << std::endl;
        return -2;	
      }

    // get a pointer to the XC::AnalysisModel
    AnalysisModel *theModel = this->getAnalysisModelPtr();
    
    // set the constants
    if(displ == true)
      {
        c1 = 1.0;
        c2 = gamma/(beta*deltaT);
        c3 = 1.0/(beta*deltaT*deltaT);
      }
    else
      {
        c1 = beta*deltaT*deltaT;
        c2 = gamma*deltaT;
        c3 = 1.0;
      }
    
    if(U.get().Size() == 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; domainChange() failed or hasn't been called\n";
        return -3;	
      }
    
    // set response at t to be that at t+deltaT of previous step
    Ut= U;        
    
    if(displ == true)
      {        
        // determine new_ velocities and accelerations at t+deltaT
        double a1= (1.0 - gamma/beta); 
        double a2= (deltaT)*(1.0 - 0.5*gamma/beta);
        U.getDot().addVector(a1, Ut.getDotDot(), a2);
        
        double a3= -1.0/(beta*deltaT);
        double a4= 1.0 - 0.5/beta;
        U.getDotDot().addVector(a4, Ut.getDot(), a3);

        // set the trial response quantities
        theModel->setVel(U.getDot());
        theModel->setAccel(U.getDotDot());
      }
    else
      {
        // determine new_ displacements at t+deltaT      
        double a1 = (deltaT*deltaT/2.0);
        U.get().addVector(1.0,Ut.getDot(), deltaT);
        U.get().addVector(1.0,Ut.getDotDot(), a1);
        
        U.getDot().addVector(1.0, Ut.getDotDot(), deltaT);
    
        // set the trial response quantities
        theModel->setDisp(U.get());
        theModel->setVel(U.getDot());
      }   
    // increment the time and apply the load
    double time= getCurrentModelTime();
    time += deltaT;
    if(updateModel(time, deltaT) < 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; failed to update the domain\n";
        return -4;
      }
    return 0;
  }


int XC::Newmark::revertToLastStep(void)
  {
    // set response at t+deltaT to be that at t .. for next newStep
    if(U.get().Size()>0)
      {
        U= Ut;        
      }
    return 0;
  }

//! This tangent for each FE\_Element is defined to be \f$K_e = c1 K + c2
//! D + c3 M\f$, where c1,c2 and c3 were determined in the last invocation
//! of the newStep() method.  The method returns \f$0\f$ after
//! performing the following operations:
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
int XC::Newmark::formEleTangent(FE_Element *theEle)
  {
    if(determiningMass == true)
      return 0;
    
    theEle->zeroTangent();
    
    if(statusFlag == CURRENT_TANGENT)
      {
        theEle->addKtToTang(c1);
        theEle->addCtoTang(c2);
        theEle->addMtoTang(c3);
      }
    else if(statusFlag == INITIAL_TANGENT)
      {
        theEle->addKiToTang(c1);
        theEle->addCtoTang(c2);
        theEle->addMtoTang(c3);
      }
    return 0;
  }

//! The method returns \f$0\f$ after performing the following operations:
//! \begin{tabbing}
//! while \= \+ while \= while \= \kill
//! theDof-\f$>\f$zeroUnbalance()
//! if (RayleighDamping == false)  \+
//! theDof-\f$>\f$addMtoTang(c3)  \-
//! else \+
//! theDof-\f$>\f$addMtoTang(c3 + c2 * \f$\alpha_M\f$)  \- 
//! \end{tabbing}
int XC::Newmark::formNodTangent(DOF_Group *theDof)
  {
    if(determiningMass == true)
      return 0;
    
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
int XC::Newmark::domainChanged(void)
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
    PopulateUs(myModel);
    return 0;
  }

//! Invoked this causes the object to increment the DOF\_Group
//! response quantities at time \f$t + \Delta t\f$. The displacement Vector is  
//! incremented by \f$ c1 * \Delta U\f$, the velocity Vector by \f$
//! c2 * \Delta U\f$, and the acceleration Vector by \f$c3 * \Delta U\f$.
//! The response at the DOF\_Group objects are then updated by invoking
//! setResponse() on the AnalysisModel with quantities at time \f$t +
//! \Delta t\f$. Finally updateDomain() is invoked on the 
//! AnalysisModel. 
//! \begin{tabbing}
//! while \= while \= while \= while \= \kill
//! \>\> if (displIncr == true) \{
//! \>\>\> \f$ U_{t + \Delta t} += \Delta U\f$
//! \>\>\> \f$ \dot U_{t + \Delta t} += \frac{\gamma}{\beta \Delta t}
//! Delta U \f$
//! \>\>\> \f$ \ddot U_{t + \Delta t} += \frac{1}{\beta {\Delta t}^2} \Delta
//! U \f$
//! \>\> \} else \{
//! \>\>\> \f$ Udd_{t + \Delta t} += \Delta Udd\f$
//! \>\>\> \f$ U_{t + \Delta t} += \beta \Delta t^2 \Delta Udd \f$
//! \>\>\> \f$ Ud_{t + \Delta t} += \gamma \Delta t \Delta Udd \f$
//! \>\> \}
//! \>\> theModel-\f$>\f$setResponse\f$(U_{t + \Delta t}, Ud_{t+\Delta t},
//! Udd_{t+\Delta t})\f$
//! \>\> theModel-\f$>\f$setUpdateDomain()
//! \end{tabbing}
//! Returns \f$0\f$ if successful. A warning message is printed and a negative
//! number returned if an error occurs: \f$-1\f$ if no associated
//! AnalysisModel, \f$-2\f$ if the Vector objects have not been created,
//! \f$-3\f$ if the Vector objects and \f$\delta U\f$ are of different sizes.
int XC::Newmark::update(const XC::Vector &deltaU)
  {
    AnalysisModel *theModel = this->getAnalysisModelPtr();
    if(theModel == 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING - no AnalysisModel set.\n";
        return -1;
      }	
    
    // check domainChanged() has been called, i.e. Ut will not be zero
    if(Ut.get().Size() == 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING - domainChange() failed or not called\n";
        return -2;
      }
    
    // check deltaU is of correct size
    if(deltaU.Size() != U.get().Size())
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING - Vectors of incompatible size "
		  << " expecting " << U.get().Size() << " obtained "
		  << deltaU.Size() << std::endl;
        return -3;
      }
    
    //  determine the response at t+deltaT
    if(displ == true)
      {
        U.get()+= deltaU;
        U.getDot().addVector(1.0, deltaU, c2);
        U.getDotDot().addVector(1.0, deltaU, c3);
      }
    else
      {
        U.get().addVector(1.0, deltaU, c1);
        U.getDot().addVector(1.0, deltaU, c2);
        (U.getDotDot())+= deltaU;
      }
    
    // update the response at the DOFs
    theModel->setResponse(U.get(),U.getDot(),U.getDotDot());
    if(updateModel() < 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; failed to update the domain\n";
        return -4;
      }
    return 0;
  }    

//! @brief Send object members through the channel being passed as parameter.
int XC::Newmark::sendData(Communicator &comm)
  {
    int res= NewmarkBase2::sendData(comm);
    res+= comm.sendBool(displ,getDbTagData(),CommMetaData(14));
    res+= comm.sendMovable(Ut,getDbTagData(),CommMetaData(15));
    res+= comm.sendBool(determiningMass,getDbTagData(),CommMetaData(16));
    return res;
  }

//! @brief Receives object members through the channel being passed as parameter.
int XC::Newmark::recvData(const Communicator &comm)
  {
    int res= NewmarkBase2::recvData(comm);
    res+= comm.receiveBool(displ,getDbTagData(),CommMetaData(14));
    res+= comm.receiveMovable(Ut,getDbTagData(),CommMetaData(15));
    res+= comm.receiveBool(determiningMass,getDbTagData(),CommMetaData(16));
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::Newmark::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(25);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to send data\n";
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::Newmark::recvSelf(const Communicator &comm)
  {
    inicComm(25);
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

//! The object sends to \f$s\f$ its type, the current time, \f$\gamma\f$ and
//! \f$\beta\f$. If Rayleigh damping is specified, the constants \f$\alpha_M\f$
//! and \f$\beta_K\f$ are also printed.
void XC::Newmark::Print(std::ostream &s, int flag) const
  {
    NewmarkBase2::Print(s,flag);
    s << "  gamma: " << gamma << "  beta: " << beta << std::endl;
    s << " c1: " << c1 << " c2: " << c2 << " c3: " << c3 << std::endl;
  }


// AddingSensitivity:BEGIN //////////////////////////////
int XC::Newmark::revertToStart(void)
  {
    Ut.Zero();
    U.Zero();
    return 0;
  }
// AddingSensitivity:END ////////////////////////////////

