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
XC::CentralDifference::CentralDifference(SoluMethod *owr)
  : RayleighBase(owr,INTEGRATOR_TAGS_CentralDifference),
    c2(0.0), c3(0.0), Utm1(0), Ut(0), U(0) {}

//! @brief Constructor.
XC::CentralDifference::CentralDifference(SoluMethod *owr,const RayleighDampingFactors &rF)
  : RayleighBase(owr,INTEGRATOR_TAGS_CentralDifference,rF),
    c2(0.0), c3(0.0), Utm1(0), Ut(0), U(0) {}

int XC::CentralDifference::newStep(double _deltaT)
  {
    updateCount = 0;
    
    deltaT = _deltaT;
    if(deltaT <= 0.0)
      {
        std::cerr << "XC::CentralDifference::newStep() - error in variable\n";
        std::cerr << "dT = " << deltaT << std::endl;
        return -2;	
      }
    
    // get a pointer to the XC::AnalysisModel
    AnalysisModel *theModel = this->getAnalysisModelPtr();
    
    // set the constants
    c2 = 0.5/deltaT;
    c3 = 1.0/(deltaT*deltaT);
    
    if(Ut.get().Size() == 0)
      {
        std::cerr << "XC::CentralDifference::newStep() - domainChange() failed or hasn't been called\n";
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


int XC::CentralDifference::formEleTangent(FE_Element *theEle)
{
    theEle->zeroTangent();
    
    theEle->addCtoTang(c2);
    theEle->addMtoTang(c3);
    
    return 0;
}    


int XC::CentralDifference::formNodTangent(DOF_Group *theDof)
{
    theDof->zeroTangent();
    
    theDof->addCtoTang(c2);
    theDof->addMtoTang(c3);

    return(0);
}    


int XC::CentralDifference::domainChanged()
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
    
    std::cerr << "WARNING: CentralDifference::domainChanged() - assuming Ut-1 = Ut\n";
    
    return 0;
}


int XC::CentralDifference::update(const Vector &upU)
  {
    updateCount++;
    if(updateCount > 1)
      {
        std::cerr << "WARNING XC::CentralDifference::update() - called more than once -";
        std::cerr << " CentralDifference integration scheme requires a LINEAR solution algorithm\n";
        return -1;
      }
    
    AnalysisModel *theModel = this->getAnalysisModelPtr();
    if(theModel == 0)
      {
        std::cerr << "WARNING XC::CentralDifference::update() - no XC::AnalysisModel set\n";
        return -1;
      }
    
    // check domainChanged() has been called, i.e. Ut will not be zero
    if(Ut.get().Size() == 0)
      {
        std::cerr << "WARNING XC::CentralDifference::update() - domainChange() failed or not called\n";
        return -2;
      }	
    
    // check U is of correct size
    if(upU.Size() != Ut.get().Size())
      {
        std::cerr << "WARNING XC::CentralDifference::update() - Vectors of incompatible size ";
        std::cerr << " expecting " << Ut.get().Size() << " obtained " << upU.Size() << std::endl;
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
        std::cerr << "CentralDifference::update() - failed to update the domain\n";
        return -4;
      }
    return 0;
  }


int XC::CentralDifference::commit(void)
  {
    AnalysisModel *theModel = this->getAnalysisModelPtr();
    if(!theModel)
      {
        std::cerr << "WARNING XC::CentralDifference::commit() - no XC::AnalysisModel set\n";
        return -1;
      }	  
    
    // set the time to be t+deltaT
    double time = getCurrentModelTime();
    time+= deltaT;
    setCurrentModelTime(time);
    
    return commitModel();
  }

//! @brief Send members del objeto through the channel being passed as parameter.
int XC::CentralDifference::sendData(CommParameters &cp)
  {
    int res= RayleighBase::sendData(cp);
    res+= cp.sendInt(updateCount,getDbTagData(),CommMetaData(4));
    res+= cp.sendDoubles(c2,c3,getDbTagData(),CommMetaData(5));
    res+= cp.sendVector(Utm1,getDbTagData(),CommMetaData(6));
    res+= cp.sendMovable(Ut,getDbTagData(),CommMetaData(7));
    res+= cp.sendMovable(U,getDbTagData(),CommMetaData(8));
    return res;
  }

//! @brief Receives members del objeto through the channel being passed as parameter.
int XC::CentralDifference::recvData(const CommParameters &cp)
  {
    int res= RayleighBase::recvData(cp);
    res+= cp.receiveInt(updateCount,getDbTagData(),CommMetaData(4));
    res+= cp.receiveDoubles(c2,c3,getDbTagData(),CommMetaData(5));
    res+= cp.receiveVector(Utm1,getDbTagData(),CommMetaData(6));
    res+= cp.receiveMovable(Ut,getDbTagData(),CommMetaData(7));
    res+= cp.receiveMovable(U,getDbTagData(),CommMetaData(8));
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::CentralDifference::sendSelf(CommParameters &cp)
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


//! @brief Receives object through the channel being passed as parameter.
int XC::CentralDifference::recvSelf(const CommParameters &cp)
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


void XC::CentralDifference::Print(std::ostream &s, int flag)
  {
    RayleighBase::Print(s,flag);
  }
