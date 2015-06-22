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
** file 'COPYRIGHT'  in XC::main directory for information on usage and   **
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
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/AlphaOSGeneralized.cpp,v $

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 10/05
// Revision: A
//
// Description: This file contains the implementation of the XC::AlphaOSGeneralized class.
//
// What: "@(#)E XC::AlphaOSGeneralized.cpp, revA"

#include <solution/analysis/integrator/transient/rayleigh/AlphaOSGeneralized.h>
#include <solution/analysis/model/fe_ele/FE_Element.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <utility/matrix/Vector.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include <solution/analysis/model/DOF_GrpIter.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <solution/analysis/model/FE_EleIter.h>

//! @brief Constructor.
XC::AlphaOSGeneralized::AlphaOSGeneralized(SoluMethod *owr)
  : AlphaOSBase(owr,INTEGRATOR_TAGS_AlphaOSGeneralized),
    alphaI(1.0), alphaF(1.0) {}

//! @brief Constructor.
XC::AlphaOSGeneralized::AlphaOSGeneralized(SoluMethod *owr,double _rhoInf)
  : AlphaOSBase(owr,INTEGRATOR_TAGS_AlphaOSGeneralized,1.0/(1.0+_rhoInf)/(1.0+_rhoInf),0.5*(3.0-_rhoInf)/(1.0+_rhoInf)),
    alphaI((2.0-_rhoInf)/(1.0+_rhoInf)), alphaF(1.0/(1.0+_rhoInf)) {}

//! @brief Constructor.
XC::AlphaOSGeneralized::AlphaOSGeneralized(SoluMethod *owr,double _rhoInf,const RayleighDampingFactors &rF)
  : AlphaOSBase(owr,INTEGRATOR_TAGS_AlphaOSGeneralized,1.0/(1.0+_rhoInf)/(1.0+_rhoInf),
                0.5*(3.0-_rhoInf)/(1.0+_rhoInf),rF),
    alphaI((2.0-_rhoInf)/(1.0+_rhoInf)), alphaF(1.0/(1.0+_rhoInf)) {}

//! @brief Constructor.
XC::AlphaOSGeneralized::AlphaOSGeneralized(SoluMethod *owr,double _alphaI, double _alphaF, double _beta, double _gamma)
  : AlphaOSBase(owr,INTEGRATOR_TAGS_AlphaOSGeneralized,_beta,_gamma),
    alphaI(_alphaI), alphaF(_alphaF) {}

//! @brief Constructor.
XC::AlphaOSGeneralized::AlphaOSGeneralized(SoluMethod *owr,double _alphaI, double _alphaF,double _beta, double _gamma,const RayleighDampingFactors &rF)
  : AlphaOSBase(owr,INTEGRATOR_TAGS_AlphaOSGeneralized,_beta,_gamma,rF), alphaI(_alphaI), alphaF(_alphaF)
  {}

int XC::AlphaOSGeneralized::newStep(double _deltaT)
  {
    int retval= AlphaOSBase::newStep(_deltaT);
    if(retval!=0)
      return retval;

    // determine the displacements and velocities at t+alphaF*deltaT
    (Ualpha.get()) = Upt.get();
    Ualpha.get().addVector((1.0-alphaF), U.get(), alphaF);
    
    (Ualpha.getDot()) = Upt.getDot();
    Ualpha.get().addVector((1.0-alphaF), U.getDot(), alphaF);
        
    // set the trial response quantities for the elements
    AnalysisModel *mdl= getAnalysisModelPtr();
    mdl->setDisp(Ualpha.get());
    mdl->setVel(Ualpha.getDot());
    
    // increment the time to t+alphaF*deltaT and apply the load
    double time = getCurrentModelTime();
    time += alphaF*deltaT;
    if(updateModel(time, deltaT) < 0)
      {
        std::cerr << "AlphaOSGeneralized::newStep() - failed to update the domain\n";
        return -4;
      }

    // determine the velocities and accelerations at t+alpha*deltaT
    (Ualpha.getDot()) = Ut.getDot();
    Ualpha.get().addVector((1.0-alphaF), U.getDot(), alphaF);

    (Ualpha.getDotDot()) = (1.0-alphaI)*(Ut.getDotDot());

    // set the trial response quantities for the nodes
    mdl->setVel(Ualpha.getDot());
    mdl->setAccel(Ualpha.getDotDot());
    
    return 0;
  }


int XC::AlphaOSGeneralized::domainChanged()
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
        Upt.resize(size);
        U.resize(size);
      }
    
    // now go through and populate U, Udot and Udotdot by iterating through
    // the DOF_Groups and getting the last committed velocity and accel
    DOF_GrpIter &theDOFGroups = myModel->getDOFGroups();
    DOF_Group *dofGroupPtr= nullptr;
    
    while((dofGroupPtr = theDOFGroups()) != 0)
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


int XC::AlphaOSGeneralized::update(const XC::Vector &deltaU)
  {
    updateCount++;
    if (updateCount > 1)  {
        std::cerr << "WARNING XC::AlphaOSGeneralized::update() - called more than once -";
        std::cerr << " AlphaOSGeneralized integration scheme requires a LINEAR solution algorithm\n";
        return -1;
    }
    
    AnalysisModel *mdl= getAnalysisModelPtr();
    if(!mdl)
      {
        std::cerr << "WARNING AlphaOSGeneralized::update() - no AnalysisModel set\n";
        return -1;
      }
    
    // check domainChanged() has been called, i.e. Ut will not be zero
    if (Ut.get().Size() == 0)  {
        std::cerr << "WARNING XC::AlphaOSGeneralized::update() - domainChange() failed or not called\n";
        return -2;
    }	
    
    // check deltaU is of correct size
    if (deltaU.Size() != U.get().Size())  {
        std::cerr << "WARNING XC::AlphaOSGeneralized::update() - Vectors of incompatible size ";
        std::cerr << " expecting " << U.get().Size() << " obtained " << deltaU.Size() << "\n";
        return -3;
    }
    
    // save the predictor displacements and velocities
    Upt.get()= U.get();
    Upt.getDot()= U.getDot();

    //  determine the response at t+deltaT
    (U.get()) += deltaU;

    U.getDot().addVector(1.0, deltaU, c2);

    (U.getDotDot()) = c3*deltaU;
    
    // update the response at the DOFs
    mdl->setResponse(U.get(),U.getDot(),U.getDotDot());
//    if(mdl->updateDomain() < 0)
//      {
//        std::cerr << "XC::AlphaOSGeneralized::update() - failed to update the domain\n";
//        return -4;
//      }
    return 0;
  }


int XC::AlphaOSGeneralized::commit(void)
  {
    AnalysisModel *mdl= getAnalysisModelPtr();
    if(!mdl)
      {
        std::cerr << "WARNING AlphaOSGeneralized::commit() - no AnalysisModel set\n";
        return -1;
      }
    else
      {
        // set the time to be t+deltaT
        double time= getCurrentModelTime();
        time+= (1.0-alphaF)*deltaT;
        setCurrentModelTime(time);
        return commitModel();
      }
  }    

//! @brief Envía los miembros del objeto a través del canal que se pasa como parámetro.
int XC::AlphaOSGeneralized::sendData(CommParameters &cp)
  {
    int res= AlphaOSBase::sendData(cp);
    res+= cp.sendDoubles(alphaI,alphaF,getDbTagData(),CommMetaData(11));
    return res;
  }

//! @brief Recibe los miembros del objeto a través del canal que se pasa como parámetro.
int XC::AlphaOSGeneralized::recvData(const CommParameters &cp)
  {
    int res= AlphaOSBase::recvData(cp);
    res+= cp.receiveDoubles(alphaI,alphaF,getDbTagData(),CommMetaData(11));
    return res;
  }

int XC::AlphaOSGeneralized::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(12);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << nombre_clase() << "sendSelf() - failed to send data\n";
    return res;
  }


int XC::AlphaOSGeneralized::recvSelf(const CommParameters &cp)
  {
    inicComm(12);
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


void XC::AlphaOSGeneralized::Print(std::ostream &s, int flag)
  {
    AlphaOSBase::Print(s,flag);
    s << "  alphaI: " << alphaI << " alphaF: " << alphaF  << " beta: " << beta  << " gamma: " << gamma << std::endl;
    s << "  c1: " << c1 << " c2: " << c2 << " c3: " << c3 << std::endl;
  }

int XC::AlphaOSGeneralized::formEleTangent(FE_Element *theEle)
  {
    theEle->zeroTangent();
    
    theEle->addKiToTang(alphaF*c1);
    theEle->addCtoTang(alphaF*c2);
    theEle->addMtoTang(alphaI*c3);
    
    return 0;
  }    


int XC::AlphaOSGeneralized::formNodTangent(DOF_Group *theDof)
  {
    theDof->zeroTangent();
    
    theDof->addCtoTang(alphaF*c2);
    theDof->addMtoTang(alphaI*c3);
    
    return 0;
  }    

int XC::AlphaOSGeneralized::formElementResidual(void)
  {
    // calculate Residual Force     
    LinearSOE *theSOE = this->getLinearSOEPtr();
    
    // loop through the FE_Elements and add the residual
    FE_Element *elePtr= nullptr;
    int res = 0;    
    AnalysisModel *mdl= getAnalysisModelPtr();
    FE_EleIter &theEles= mdl->getFEs();
    while((elePtr = theEles()) != 0)
      {
        // calculate R-F(d)
        if(theSOE->addB(elePtr->getResidual(this),elePtr->getID()) < 0)
          {
            std::cerr << "WARNING XC::IncrementalIntegrator::formElementResidual -";
            std::cerr << " failed in addB for ID " << elePtr->getID();
            res = -2;
          }        
        // add Ki*d -> R-F(d)+Ki*d
        const double tmp_c2 = c2;
        const double tmp_c3 = c3;
        alphaF = alphaF-1.0;
        c2 = c3 = 0.0; // no contribution of C and M to tangent
        const Vector Ki_d = elePtr->getTangForce(Ut.get() - Upt.get());
        if(theSOE->addB(Ki_d, elePtr->getID())<0)
          {
            std::cerr << "WARNING AlphaOSGeneralized::formElementResidual -";
            std::cerr << " failed in addB for ID " << elePtr->getID();
            res = -2;
          }
        alphaF = alphaF+1.0;
        c2 = tmp_c2;
        c3 = tmp_c3;
      }
    return res;
  }

