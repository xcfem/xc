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
XC::HHT::HHT(SoluMethod *owr)
  : HHTBase(owr,INTEGRATOR_TAGS_HHT) {}

//! @brief Constructor.
XC::HHT::HHT(SoluMethod *owr,double _alpha)
  : HHTBase(owr,INTEGRATOR_TAGS_HHT,_alpha) {}

//! @brief Constructor.
XC::HHT::HHT(SoluMethod *owr,double _alpha,const RayleighDampingFactors &rF)
  : HHTBase(owr,INTEGRATOR_TAGS_HHT,_alpha,rF) {}

//! @brief Constructor.
XC::HHT::HHT(SoluMethod *owr,double _alpha, double _beta, double _gamma)
  : HHTBase(owr,INTEGRATOR_TAGS_HHT,_alpha,_beta,_gamma) {}

//! @brief Constructor.
XC::HHT::HHT(SoluMethod *owr,double _alpha, double _beta, double _gamma,const RayleighDampingFactors &rF)
  : HHTBase(owr,INTEGRATOR_TAGS_HHT,_alpha,_beta,_gamma,rF) {}


int XC::HHT::newStep(double _deltaT)
  {
    deltaT = _deltaT;
    if (beta == 0 || gamma == 0 )  {
        std::cerr << "XC::HHT::newStep() - error in variable\n";
        std::cerr << "gamma = " << gamma << " beta = " << beta << std::endl;
        return -1;
    }
    
    if (deltaT <= 0.0)  {
        std::cerr << "XC::HHT::newStep() - error in variable\n";
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
        std::cerr << "HHT::newStep() - domainChange() failed or hasn't been called\n";
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
        std::cerr << "XC::HHT::newStep() - failed to update the domain\n";
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


int XC::HHT::formEleTangent(FE_Element *theEle)
  {
    theEle->zeroTangent();
    if (statusFlag == CURRENT_TANGENT)  {
        theEle->addKtToTang(alpha*c1);
        theEle->addCtoTang(alpha*c2);
        theEle->addMtoTang(c3);
    } else if (statusFlag == INITIAL_TANGENT)  {
        theEle->addKiToTang(alpha*c1);
        theEle->addCtoTang(alpha*c2);
        theEle->addMtoTang(c3);
    }
    
    return 0;
}   
 

int XC::HHT::formNodTangent(DOF_Group *theDof)
{
    theDof->zeroTangent();

    theDof->addCtoTang(alpha*c2);
    theDof->addMtoTang(c3);
    
    return 0;
}


int XC::HHT::domainChanged()
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


int XC::HHT::update(const Vector &deltaU)
{
    AnalysisModel *theModel = this->getAnalysisModelPtr();
    if (theModel == 0)  {
        std::cerr << "WARNING XC::HHT::update() - no XC::AnalysisModel set\n";
        return -1;
    }
    
    // check domainChanged() has been called, i.e. Ut will not be zero
    if(Ut.get().Size() == 0)
      {
        std::cerr << "WARNING XC::HHT::update() - domainChange() failed or not called\n";
        return -2;
      }
    
    // check deltaU is of correct size
    if(deltaU.Size() != U.get().Size())
      {
        std::cerr << "WARNING XC::HHT::update() - Vectors of incompatible size ";
        std::cerr << " expecting " << U.get().Size() << " obtained " << deltaU.Size() << std::endl;
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
        std::cerr << "XC::HHT::update() - failed to update the domain\n";
        return -4;
      }
    return 0;
  }


int XC::HHT::commit(void)
  {
    AnalysisModel *theModel = this->getAnalysisModelPtr();
    if(theModel == 0)
      {
        std::cerr << "WARNING XC::HHT::commit() - no XC::AnalysisModel set\n";
        return -1;
      }
    
    // update the response at the DOFs
    theModel->setResponse(U.get(),U.getDot(),U.getDotDot());
//    if (theModel->updateDomain() < 0)  {
//        std::cerr << "XC::HHT::commit() - failed to update the domain\n";
//        return -4;
//    }
    
    // set the time to be t+deltaT
    double time= getCurrentModelTime();
    time += (1.0-alpha)*deltaT;
    setCurrentModelTime(time);

    return commitModel();
  }

//! @brief Send members del objeto through the channel being passed as parameter.
int XC::HHT::sendData(CommParameters &cp)
  {
    int res= HHTBase::sendData(cp);
    return res;
  }

//! @brief Receives members del objeto through the channel being passed as parameter.
int XC::HHT::recvData(const CommParameters &cp)
  {
    int res= HHTBase::recvData(cp);
    return res;
  }

int XC::HHT::sendSelf(CommParameters &cp)
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

int XC::HHT::recvSelf(const CommParameters &cp)
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


void XC::HHT::Print(std::ostream &s, int flag)
  {
    HHTBase::Print(s,flag);
    s << "  alpha: " << alpha << " beta: " << beta  << " gamma: " << gamma << std::endl;
    s << "  c1: " << c1 << " c2: " << c2 << " c3: " << c3 << std::endl;
  }
