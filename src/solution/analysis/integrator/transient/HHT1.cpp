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
                                                                        
// $Revision: 1.7 $
// $Date: 2003/02/14 23:00:48 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/HHT1.cpp,v $
                                                                        
                                                                        
// File: ~/analysis/integrator/HHT1.C
// 
// Written: fmk 
// Created: 11/98
// Revision: A
//
// Description: This file contains the implementation of the XC::HHT1 class.
//
// What: "@(#) HHT1.C, revA"

#include <solution/analysis/integrator/transient/HHT1.h>
#include <solution/analysis/model/fe_ele/FE_Element.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <utility/matrix/Vector.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include <solution/analysis/model/DOF_GrpIter.h>
#include <solution/analysis/model/AnalysisModel.h>
#include "utility/actor/actor/ArrayCommMetaData.h"

//! @brief Constructor.
XC::HHT1::HHT1(SoluMethod *owr)
:DampingFactorsIntegrator(owr,INTEGRATOR_TAGS_HHT1),
 alpha(0.5), gamma(1.0), beta(0), 
 c1(0.0), c2(0.0), c3(0.0), Ut(0), U(0), Ualpha(0) {}

//! @brief Constructor.
XC::HHT1::HHT1(SoluMethod *owr,double _alpha)
:DampingFactorsIntegrator(owr,INTEGRATOR_TAGS_HHT1),
 alpha(_alpha), gamma(1.5-_alpha), beta((2-_alpha)*(2-_alpha)*0.25),
 c1(0.0), c2(0.0), c3(0.0), Ut(0),  U(0), Ualpha(0) {}

//! @brief Constructor.
XC::HHT1::HHT1(SoluMethod *owr,double _alpha,const RayleighDampingFactors &rF)
  :DampingFactorsIntegrator(owr,INTEGRATOR_TAGS_HHT1,rF),
 alpha(_alpha), gamma(1.5-_alpha), beta((2-_alpha)*(2-_alpha)*0.25), 
 c1(0.0), c2(0.0), c3(0.0), Ut(0),  U(0), Ualpha(0) {}


int XC::HHT1::initialize(void)
  { return 0; }


int XC::HHT1::newStep(double deltaT)
  {

    if(beta == 0 || gamma == 0 ) {
    std::cerr << "XC::HHT1::newStep() - error in variable\n";
    std::cerr << "gamma = " << gamma << " beta= " << beta << std::endl;
    return -1;
  }
    
  if (deltaT <= 0.0) {
    std::cerr << "XC::HHT1::newStep() - error in variable\n";
    std::cerr << "dT = " << deltaT << std::endl;
    return -2;	
  }
  c1 = 1.0;
  c2 = gamma/(beta*deltaT);
  c3 = 1.0/(beta*deltaT*deltaT);


  AnalysisModel *theModel = this->getAnalysisModelPtr();

  // set response at t to be that at t+delta t of previous step
  Ut= U;
    
  // set new_ velocity and accelerations at t + delta t
  double a1 = (1.0 - gamma/beta); 
  double a2 = (deltaT)*(1.0 - 0.5*gamma/beta);

  //  (U.getDot()) *= a1;
  U.getDot().addVector(a1, Ut.getDotDot(),a2);

  double a3 = -1.0/(beta*deltaT);
  double a4 = 1 - 0.5/beta;
  //  (U.getDotDot()) *= a4;  
  U.getDotDot().addVector(a4, Ut.getDot(),a3);

  Ualpha= Ut;
  //  (Ualpha.getDot()) *= (1 - alpha);
  Ualpha.getDot().addVector((1-alpha), U.getDot(), alpha);

  // set the new_ trial response quantities

  theModel->setResponse(Ualpha.get(),Ualpha.getDot(),U.getDotDot());        

  // increment the time and apply the load
  double time = getCurrentModelTime();
  time +=deltaT;
  if(updateModel(time, deltaT) < 0)
    {
      std::cerr << "XC::HHT::newStep() - failed to update the domain\n";
      return -4;
    }
   return 0;
  }



int XC::HHT1::formEleTangent(FE_Element *theEle)
{
  theEle->zeroTangent();
  if (statusFlag == CURRENT_TANGENT) {
    theEle->addKtToTang(c1);
    theEle->addCtoTang(c2);
    theEle->addMtoTang(c3);
  } else if (statusFlag == INITIAL_TANGENT) {
    theEle->addKiToTang(c1);
    theEle->addCtoTang(c2);
    theEle->addMtoTang(c3);
  }

  return 0;
}    



int XC::HHT1::formNodTangent(DOF_Group *theDof)
{
  theDof->zeroTangent();
  theDof->addMtoTang(c3);
  theDof->addCtoTang(c2);        
  
  return(0);
}    



int XC::HHT1::domainChanged()
{
  AnalysisModel *myModel = this->getAnalysisModelPtr();
  LinearSOE *theLinSOE = this->getLinearSOEPtr();
  const XC::Vector &x = theLinSOE->getX();
  int size = x.Size();

  setRayleighDampingFactors();

  // create the new_ Vector objects
  if(Ut.get().Size() != size)
    {
      // create the new
      Ut.resize(size);
      U.resize(size);
      Ualpha.resize(size);
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


int XC::HHT1::update(const Vector &deltaU)
  {
    AnalysisModel *theModel = this->getAnalysisModelPtr();
    if(theModel == 0)
      {
        std::cerr << "WARNING HHT1::update() - no XC::AnalysisModel set\n";
        return -1;
      }	

    // check domainChanged() has been called, i.e. Ut will not be zero
    if(Ut.get().Size() == 0)
      {
        std::cerr << "WARNING HHT1::update() - domainChange() failed or not called\n";
        return -2;
      }

    // check deltaU is of correct size
    if(deltaU.Size() != U.get().Size())
      {
        std::cerr << "WARNING HHT1::update() - Vectors of incompatable size ";
        std::cerr << " expecting " << U.get().Size() << " obtained " << deltaU.Size() << std::endl;
        return -3;
      }
    
     //  determine the response at t+delta t
     U.get()+= deltaU;
     U.getDot().addVector(1.0, deltaU,c2);
     U.getDotDot().addVector(1.0, deltaU,c3);
     Ualpha.get().addVector(1.0, deltaU, alpha);
     Ualpha.getDot().addVector(1.0, deltaU, c2*alpha);
  
     // update the responses at the DOFs
     theModel->setResponse(Ualpha.get(),Ualpha.getDot(),U.getDotDot());        
     if(updateModel() < 0)
       {
         std::cerr << "HHT1::update() - failed to update the domain\n";
         return -4;
       }
     return 0;
  }    

int XC::HHT1::commit(void)
  {
    AnalysisModel *theModel = this->getAnalysisModelPtr();
    if(theModel == 0)
      {
        std::cerr << "WARNING XC::HHT1::commit() - no XC::AnalysisModel set\n";
        return -1;
      }	  

     // update the responses at the DOFs
    theModel->setResponse(U.get(),U.getDot(),U.getDotDot());        
    updateModel();
    return commitModel();
  }

//! @brief Send members del objeto through the channel being passed as parameter.
int XC::HHT1::sendData(CommParameters &cp)
  {
    int res= DampingFactorsIntegrator::sendData(cp);
    res+= cp.sendDoubles(alpha,beta,gamma,c1,c2,c3,getDbTagData(),CommMetaData(3));
    res+= cp.sendMovable(Ut,getDbTagData(),CommMetaData(4));
    res+= cp.sendMovable(U,getDbTagData(),CommMetaData(5));
    res+= cp.sendMovable(Ualpha,getDbTagData(),CommMetaData(6));
    return res;
  }

//! @brief Receives members del objeto through the channel being passed as parameter.
int XC::HHT1::recvData(const CommParameters &cp)
  {
    int res= DampingFactorsIntegrator::recvData(cp);
    res+= cp.receiveDoubles(alpha,beta,gamma,c1,c2,c3,getDbTagData(),CommMetaData(3));
    res+= cp.receiveMovable(Ut,getDbTagData(),CommMetaData(4));
    res+= cp.receiveMovable(U,getDbTagData(),CommMetaData(5));
    res+= cp.receiveMovable(Ualpha,getDbTagData(),CommMetaData(6));
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::HHT1::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(27);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << nombre_clase() << "sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::HHT1::recvSelf(const CommParameters &cp)
  {
    inicComm(27);
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

void XC::HHT1::Print(std::ostream &s, int flag)
  {
    DampingFactorsIntegrator::Print(s,flag);
    s << alpha << " gamma: " << gamma << "  beta: " << beta << std::endl;
    s << "  Rayleigh Damping: " << rayFactors << std::endl;	    
  }

