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

// $Revision: 1.2 $
// $Date: 2005/12/21 00:31:57 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/AlphaOS.cpp,v $

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 02/05
// Revision: A
//
// Description: This file contains the implementation of the XC::AlphaOS class.
//
// What: "@(#)E XC::AlphaOS.cpp, revA"

#include <solution/analysis/integrator/transient/rayleigh/AlphaOS.h>
#include <solution/analysis/model/fe_ele/FE_Element.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <utility/matrix/Vector.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include <solution/analysis/model/DOF_GrpIter.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <solution/analysis/model/FE_EleIter.h>

//! @brief Constructor.
XC::AlphaOS::AlphaOS(AnalysisAggregation *owr)
    : AlphaOSBase(owr,INTEGRATOR_TAGS_AlphaOS), alpha(1.0) {}

//! @brief Constructor.
XC::AlphaOS::AlphaOS(AnalysisAggregation *owr,double _alpha)
  : AlphaOSBase(owr,INTEGRATOR_TAGS_AlphaOS,(2-_alpha)*(2-_alpha)*0.25,1.5-_alpha),
    alpha(_alpha) {}

//! @brief Constructor.
XC::AlphaOS::AlphaOS(AnalysisAggregation *owr,double _alpha,const RayleighDampingFactors &rF)
  : AlphaOSBase(owr,INTEGRATOR_TAGS_AlphaOS,(2-_alpha)*(2-_alpha)*0.25,1.5-_alpha,rF), alpha(_alpha) {}

//! @brief Constructor.
XC::AlphaOS::AlphaOS(AnalysisAggregation *owr,double _alpha, double _beta, double _gamma)
  : AlphaOSBase(owr,INTEGRATOR_TAGS_AlphaOS,_beta,_gamma), alpha(_alpha) {}

//! @brief Constructor.
XC::AlphaOS::AlphaOS(AnalysisAggregation *owr,double _alpha, double _beta, double _gamma,const RayleighDampingFactors &rF)
  : AlphaOSBase(owr,INTEGRATOR_TAGS_AlphaOS,_beta,_gamma,rF), alpha(_alpha) {}


int XC::AlphaOS::newStep(double _deltaT)
  {
    const int retval= AlphaOSBase::newStep(_deltaT);
    if(retval!=0)
      return retval;
            
    // determine the displacements and velocities at t+alpha*deltaT
    Ualpha= Upt;
    Ualpha.get().addVector((1.0-alpha), U.get(), alpha);
    Ualpha.getDot().addVector((1.0-alpha), U.getDot(), alpha);
        
    // set the trial response quantities for the elements
    AnalysisModel *mdl= getAnalysisModelPtr();
    mdl->setDisp(Ualpha.get());
    mdl->setVel(Ualpha.getDot());
    
    // increment the time to t+alpha*deltaT and apply the load
    double time = getCurrentModelTime();
    time += alpha*deltaT;
    if(updateModel(time, deltaT) < 0)
      {
        std::cerr << "XC::AlphaOS::newStep() - failed to update the domain\n";
        return -4;
      }

    // determine the velocities and accelerations at t+alpha*deltaT
    (Ualpha.getDot()) = Ut.getDot();
    Ualpha.get().addVector((1.0-alpha), U.getDot(), alpha);

    U.getDotDot().Zero();

    // set the trial response quantities for the nodes
    mdl->setVel(Ualpha.getDot());
    mdl->setAccel(U.getDotDot());
    
    return 0;
}


int XC::AlphaOS::revertToLastStep(void)
  {
    // set response at t+deltaT to be that at t .. for next step
    if(U.get().Size()>0)
      U= Ut;
    return 0;
  }


int XC::AlphaOS::formEleTangent(FE_Element *theEle)
  {
    theEle->zeroTangent();
    
    theEle->addKiToTang(alpha*c1);
    theEle->addCtoTang(alpha*c2);
    theEle->addMtoTang(c3);
    
    return 0;
  }    


int XC::AlphaOS::formNodTangent(DOF_Group *theDof)
  {
    theDof->zeroTangent();
    
    theDof->addCtoTang(alpha*c2);
    theDof->addMtoTang(c3);
    
    return 0;
  }


int XC::AlphaOS::domainChanged(void)
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
        Ualpha.resize(size);
        Upt.resize(size);
      }
    
    // now go through and populate U, Udot and Udotdot by iterating through
    // the DOF_Groups and getting the last committed velocity and accel
    DOF_GrpIter &theDOFGroups = myModel->getDOFGroups();
    DOF_Group *dofGroupPtr= nullptr;
    
    while ((dofGroupPtr = theDOFGroups()) != 0)
      {
        const XC::ID &id = dofGroupPtr->getID();
        const Vector &disp = dofGroupPtr->getCommittedDisp();	
        U.setDisp(id,disp);
        const Vector &vel = dofGroupPtr->getCommittedVel();
        U.setVel(id,vel);
        const Vector &accel = dofGroupPtr->getCommittedAccel();
        U.setAccel(id,accel);
      }    
    
    return 0;
  }


int XC::AlphaOS::update(const XC::Vector &deltaU)
{
    updateCount++;
    if (updateCount > 1)  {
        std::cerr << "WARNING XC::AlphaOS::update() - called more than once -";
        std::cerr << " AlphaOS integration scheme requires a LINEAR solution algorithm\n";
        return -1;
    }
    
    AnalysisModel *mdl= getAnalysisModelPtr();
    if(!mdl)
      {
        std::cerr << "WARNING AlphaOS::update() - no AnalysisModel set\n";
        return -1;
      }
    
    // check domainChanged() has been called, i.e. Ut will not be zero
    if(Ut.get().Size() == 0)  {
        std::cerr << "WARNING XC::AlphaOS::update() - domainChange() failed or not called\n";
        return -2;
    }	
    
    // check deltaU is of correct size
    if (deltaU.Size() != U.get().Size())  {
        std::cerr << "WARNING XC::AlphaOS::update() - Vectors of incompatible size ";
        std::cerr << " expecting " << U.get().Size() << " obtained " << deltaU.Size() << "\n";
        return -3;
    }
    
    // save the predictor displacements and velocities
    Upt= U;

    //  determine the response at t+deltaT
    U.get()+= deltaU;

    U.getDot().addVector(1.0, deltaU, c2);

    U.getDotDot().addVector(1.0, deltaU, c3);
    
    // update the response at the DOFs
    mdl->setResponse(U.get(),U.getDot(),U.getDotDot());
//    if(mdl->updateDomain() < 0)
//      {
//        std::cerr << "XC::AlphaOS::update() - failed to update the domain\n";
//        return -4;
//      }
    return 0;
  }


int XC::AlphaOS::commit(void)
  {
    AnalysisModel *mdl= this->getAnalysisModelPtr();
    if(!mdl)
      {
        std::cerr << "WARNING AlphaOS::commit() - no AnalysisModel set\n";
        return -1;
      }
    else
      {
        double time = getCurrentModelTime(); //set the time to be t+deltaT
        time+= (1.0-alpha)*deltaT;
        setCurrentModelTime(time);
        return commitModel();
      }
  }    

//! @brief Send object members through the channel being passed as parameter.
int XC::AlphaOS::sendData(CommParameters &cp)
  {
    int res= AlphaOSBase::sendData(cp);
    res+= cp.sendDouble(alpha,getDbTagData(),CommMetaData(11));
    return res;
  }

//! @brief Receives object members through the channel being passed as parameter.
int XC::AlphaOS::recvData(const CommParameters &cp)
  {
    int res= AlphaOSBase::recvData(cp);
    res+= cp.receiveDouble(alpha,getDbTagData(),CommMetaData(11));
    return res;
  }

int XC::AlphaOS::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(9);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "sendSelf() - failed to send data\n";
    return res;
  }


int XC::AlphaOS::recvSelf(const CommParameters &cp)
  {
    inicComm(9);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << getClassName() << "::recvSelf - failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
          std::cerr << getClassName() << "::recvSelf - failed to receive data.\n";
      }
    return res;
  }


void XC::AlphaOS::Print(std::ostream &s, int flag)
  {
    AlphaOSBase::Print(s,flag);
    s << "  alpha: " << alpha << " beta: " << beta  << " gamma: " << gamma << std::endl;
    s << "  c1: " << c1 << " c2: " << c2 << " c3: " << c3 << std::endl;
  }


int XC::AlphaOS::formElementResidual(void)
  {
    // calculate Residual Force     
    LinearSOE *theSOE = this->getLinearSOEPtr();
    
    // loop through the FE_Elements and add the residual
    FE_Element *elePtr;
    
    int res = 0;    
    
    AnalysisModel *mdl= getAnalysisModelPtr();
    FE_EleIter &theEles= mdl->getFEs();
    while((elePtr = theEles()) != 0)
      {
        // calculate R-F(d)
        if(theSOE->addB(elePtr->getResidual(this),elePtr->getID()) < 0)
          {
            std::cerr << "WARNING IncrementalIntegrator::formElementResidual -";
            std::cerr << " failed in addB for ID " << elePtr->getID();
            res = -2;
          }        
        // add Ki*d -> R-F(d)+Ki*d
        const double tmp_c2= c2;
        const double tmp_c3= c3;
        alpha = alpha-1.0;
        c2 = c3 = 0.0; // no contribution of C and M to tangent
        const Vector Ki_d = elePtr->getTangForce(Ut.get() - Upt.get());
        if(theSOE->addB(Ki_d, elePtr->getID())<0)
          {
            std::cerr << "WARNING AlphaOS::formElementResidual -";
            std::cerr << " failed in addB for XC::ID " << elePtr->getID();
            res = -2;
          }
        alpha = alpha+1.0;
        c2= tmp_c2;
        c3= tmp_c3;
      }
    return res;
  }
