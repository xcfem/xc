//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
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

// $Revision: 1.1 $
// $Date: 2005/12/19 22:39:21 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/HHTGeneralized.cpp,v $

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 10/05
// Revision: A
//
// Description: This file contains the implementation of the XC::HHTGeneralized class.
//
// What: "@(#) HHTGeneralized.cpp, revA"

#include <solution/analysis/integrator/transient/rayleigh/HHTGeneralized.h>
#include <solution/analysis/model/fe_ele/FE_Element.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <utility/matrix/Vector.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include <solution/analysis/model/DOF_GrpIter.h>
#include <solution/analysis/model/AnalysisModel.h>

//! @brief Constructor.
XC::HHTGeneralized::HHTGeneralized(SoluMethod *owr)
    : RayleighBase(owr,INTEGRATOR_TAGS_HHTGeneralized),
    alphaI(1.0), alphaF(1.0), beta(0.0), gamma(0.0),c1(0.0), c2(0.0), c3(0.0) {}

//! @brief Constructor.
XC::HHTGeneralized::HHTGeneralized(SoluMethod *owr,double _rhoInf)
    : RayleighBase(owr,INTEGRATOR_TAGS_HHTGeneralized),
    alphaI((2.0-_rhoInf)/(1.0+_rhoInf)), alphaF(1.0/(1.0+_rhoInf)),
    beta(1.0/(1.0+_rhoInf)/(1.0+_rhoInf)), gamma(0.5*(3.0-_rhoInf)/(1.0+_rhoInf)),
    c1(0.0), c2(0.0), c3(0.0) {}

//! @brief Constructor.
XC::HHTGeneralized::HHTGeneralized(SoluMethod *owr,double _rhoInf,const RayleighDampingFactors &rF)
    : RayleighBase(owr,INTEGRATOR_TAGS_HHTGeneralized,rF), alphaI((2.0-_rhoInf)/(1.0+_rhoInf)), alphaF(1.0/(1.0+_rhoInf)),
    beta(1.0/(1.0+_rhoInf)/(1.0+_rhoInf)), gamma(0.5*(3.0-_rhoInf)/(1.0+_rhoInf)),
    c1(0.0), c2(0.0), c3(0.0) {}

//! @brief Constructor.
XC::HHTGeneralized::HHTGeneralized(SoluMethod *owr,double _alphaI, double _alphaF, double _beta, double _gamma)
    : RayleighBase(owr,INTEGRATOR_TAGS_HHTGeneralized),
    alphaI(_alphaI), alphaF(_alphaF),
    beta(_beta), gamma(_gamma), c1(0.0), c2(0.0), c3(0.0) {}

//! @brief Constructor.
XC::HHTGeneralized::HHTGeneralized(SoluMethod *owr,double _alphaI, double _alphaF,
    double _beta, double _gamma,const RayleighDampingFactors &rF)
    : RayleighBase(owr,INTEGRATOR_TAGS_HHTGeneralized,rF),
    alphaI(_alphaI), alphaF(_alphaF),
    beta(_beta), gamma(_gamma), c1(0.0), c2(0.0), c3(0.0) {}


int XC::HHTGeneralized::newStep(double _deltaT)
  {
    deltaT = _deltaT;
    if(beta == 0 || gamma == 0 )
      {
        std::cerr << "HHTGeneralized::newStep() - error in variable\n";
        std::cerr << "gamma = " << gamma << " beta = " << beta << std::endl;
        return -1;
      }
    
    if(deltaT <= 0.0)
      {
        std::cerr << "XC::HHTGeneralized::newStep() - error in variable\n";
        std::cerr << "dT = " << deltaT << std::endl;
        return -2;	
      }

    // get a pointer to the XC::AnalysisModel
    AnalysisModel *theModel = this->getAnalysisModelPtr();
    
    // set the constants
    c1 = 1.0;
    c2 = gamma/(beta*deltaT);
    c3 = 1.0/(beta*deltaT*deltaT);
       
    if(U.get().Size() == 0)  {
        std::cerr << "XC::HHTGeneralized::newStep() - domainChange() failed or hasn't been called\n";
        return -3;	
    }
    
    // set response at t to be that at t+deltaT of previous step
    Ut= U;        

    // increment the time to t+alpha*deltaT and apply the load
    double time = getCurrentModelTime();
    time += alphaF*deltaT;
//    theModel->applyLoadDomain(time);
    if(updateModel(time, deltaT) < 0)
      {
        std::cerr << "XC::HHTGeneralized::newStep() - failed to update the domain\n";
        return -4;
      }

    // determine new_ velocities and accelerations at t+deltaT
    double a1 = (1.0 - gamma/beta);
    double a2 = deltaT*(1.0 - 0.5*gamma/beta);
    U.getDot().addVector(a1, Ut.getDotDot(), a2);
    
    double a3 = -1.0/(beta*deltaT);
    double a4 = 1.0 - 0.5/beta;  
    U.getDotDot().addVector(a4, Ut.getDot(), a3);
    
    // determine the velocities and accelerations at t+alpha*deltaT
    (Ualpha.getDot()) = Ut.getDot();
    Ualpha.get().addVector((1.0-alphaF), U.getDot(), alphaF);
    
    (Ualpha.getDotDot()) = Ut.getDotDot();
    Ualpha.getDotDot().addVector((1.0-alphaI), U.getDotDot(), alphaI);

    // set the trial response quantities for the nodes
    theModel->setVel(Ualpha.getDot());
    theModel->setAccel(Ualpha.getDotDot());
        
    return 0;
}


int XC::HHTGeneralized::revertToLastStep()
  {
    // set response at t+deltaT to be that at t .. for next step
    if(U.get().Size()>0)
      U= Ut;
    return 0;
  }


int XC::HHTGeneralized::formEleTangent(FE_Element *theEle)
{
    theEle->zeroTangent();
    if (statusFlag == CURRENT_TANGENT)  {
        theEle->addKtToTang(alphaF*c1);
        theEle->addCtoTang(alphaF*c2);
        theEle->addMtoTang(alphaI*c3);
    } else if (statusFlag == INITIAL_TANGENT)  {
        theEle->addKiToTang(alphaF*c1);
        theEle->addCtoTang(alphaF*c2);
        theEle->addMtoTang(alphaI*c3);
    }
    
    return 0;
}    


int XC::HHTGeneralized::formNodTangent(DOF_Group *theDof)
{
    theDof->zeroTangent();

    theDof->addCtoTang(alphaF*c2);
    theDof->addMtoTang(alphaI*c3);
    
    return 0;
}


int XC::HHTGeneralized::domainChanged()
{
    AnalysisModel *myModel = this->getAnalysisModelPtr();
    LinearSOE *theLinSOE = this->getLinearSOEPtr();
    const XC::Vector &x = theLinSOE->getX();
    int size = x.Size();
    
    setRayleighDampingFactors();
    
    if(Ut.get().Size() != size)
      {
        Ualpha.resize(size);
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


int XC::HHTGeneralized::update(const Vector &deltaU)
{
    AnalysisModel *theModel = this->getAnalysisModelPtr();
    if (theModel == 0)  {
        std::cerr << "WARNING XC::HHTGeneralized::update() - no XC::AnalysisModel set\n";
        return -1;
    }	
    
    // check domainChanged() has been called, i.e. Ut will not be zero
    if (Ut.get().Size() == 0)  {
        std::cerr << "WARNING XC::HHTGeneralized::update() - domainChange() failed or not called\n";
        return -2;
    }	
    
    // check deltaU is of correct size
    if (deltaU.Size() != U.get().Size())  {
        std::cerr << "WARNING XC::HHTGeneralized::update() - Vectors of incompatible size ";
        std::cerr << " expecting " << U.get().Size() << " obtained " << deltaU.Size() << std::endl;
        return -3;
    }
    
    //  determine the response at t+deltaT
    (U.get()) += deltaU;

    U.getDot().addVector(1.0, deltaU, c2);
    
    U.getDotDot().addVector(1.0, deltaU, c3);

    // determine displacement and velocity at t+alpha*deltaT
    (Ualpha.get()) = Ut.get();
    Ualpha.get().addVector((1.0-alphaF), U.get(), alphaF);

    (Ualpha.getDot()) = Ut.getDot();
    Ualpha.get().addVector((1.0-alphaF), U.getDot(), alphaF);
    
    (Ualpha.getDotDot()) = Ut.getDotDot();
    Ualpha.getDotDot().addVector((1.0-alphaI), U.getDotDot(), alphaI);

    // update the response at the DOFs
    theModel->setResponse(Ualpha.get(),Ualpha.getDot(),Ualpha.getDotDot());
    if(updateModel()<0)
      {
        std::cerr << "XC::HHTGeneralized::update() - failed to update the domain\n";
        return -4;
      }
    return 0;
  }


int XC::HHTGeneralized::commit(void)
  {
    AnalysisModel *theModel = this->getAnalysisModelPtr();
    if(theModel == 0)
      {
        std::cerr << "WARNING XC::HHTGeneralized::commit() - no XC::AnalysisModel set\n";
        return -1;
      }
    
    // update the response at the DOFs
    theModel->setResponse(U.get(),U.getDot(),U.getDotDot());
//    if (theModel->updateDomain() < 0)  {
//        std::cerr << "XC::HHTGeneralized::commit() - failed to update the domain\n";
//        return -4;
//    }
    
    // set the time to be t+deltaT
    double time = getCurrentModelTime();
    time+= (1.0-alphaF)*deltaT;
    setCurrentModelTime(time);
    return commitModel();
  }

//! @brief Envía los miembros del objeto a través del canal que se pasa como parámetro.
int XC::HHTGeneralized::sendData(CommParameters &cp)
  {
    int res= RayleighBase::sendData(cp);
    res+= cp.sendDoubles(alphaI,alphaF,beta,gamma,getDbTagData(),CommMetaData(4));
    res+= cp.sendDoubles(c1,c2,c3,getDbTagData(),CommMetaData(5));
    res+= cp.sendMovable(Ut,getDbTagData(),CommMetaData(6));
    res+= cp.sendMovable(U,getDbTagData(),CommMetaData(7));
    res+= cp.sendMovable(Ualpha,getDbTagData(),CommMetaData(8));
    return res;
  }

//! @brief Recibe los miembros del objeto a través del canal que se pasa como parámetro.
int XC::HHTGeneralized::recvData(const CommParameters &cp)
  {
    int res= RayleighBase::recvData(cp);
    res+= cp.receiveDoubles(alphaI,alphaF,beta,gamma,getDbTagData(),CommMetaData(4));
    res+= cp.receiveDoubles(c1,c2,c3,getDbTagData(),CommMetaData(5));
    res+= cp.receiveMovable(Ut,getDbTagData(),CommMetaData(6));
    res+= cp.receiveMovable(U,getDbTagData(),CommMetaData(7));
    res+= cp.receiveMovable(Ualpha,getDbTagData(),CommMetaData(8));
    return res;
  }

int XC::HHTGeneralized::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(9);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << nombre_clase() << "sendSelf() - failed to send data\n";
    return res;
  }

int XC::HHTGeneralized::recvSelf(const CommParameters &cp)
  {
    inicComm(9);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << nombre_clase() << "::recvSelf - failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
          std::cerr << nombre_clase() << "::recvSelf - failed to receive data.\n";
      }
    return res;
  }

void XC::HHTGeneralized::Print(std::ostream &s, int flag)
  {
    RayleighBase::Print(s,flag);
    s << "  alphaI: " << alphaI << " alphaF: " << alphaF  << " beta: " << beta  << " gamma: " << gamma << std::endl;
    s << "  c1: " << c1 << " c2: " << c2 << " c3: " << c3 << std::endl;
  }

