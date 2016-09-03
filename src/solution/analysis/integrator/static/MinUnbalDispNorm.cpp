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
                                                                        
// $Revision: 1.8 $
// $Date: 2005/10/19 21:53:57 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/MinUnbalDispNorm.cpp,v $
                                                                        
                                                                        
// File: ~/analysis/integrator/MinUnbalDispNorm.C
// 
// Written: fmk 
// Created: 07/99
// Revision: A
//
// What: "@(#) MinUnbalDispNorm.C, revA"


#include <solution/analysis/integrator/static/MinUnbalDispNorm.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <utility/matrix/Vector.h>
#include <cmath>

//! @brief Constructor.
XC::MinUnbalDispNorm::MinUnbalDispNorm(SoluMethod *owr,double lambda1, int specNumIter,double min, double max, int signFirstStep)
  :DispBase(owr,INTEGRATOR_TAGS_MinUnbalDispNorm,specNumIter),
 dLambda1LastStep(lambda1), signLastDeltaLambdaStep(1),
 dLambda1min(min), dLambda1max(max), signLastDeterminant(1), signFirstStepMethod(signFirstStep)
  {}

//! @brief Devuelve el valor de dLambda para el método newStep
double XC::MinUnbalDispNorm::getDLambdaNewStep(void) const
  {
    // determine delta lambda(1) == dlambda
    double retval = dLambda1LastStep*factor();

    // check aaint min and max values specified in constructor
    if(retval < dLambda1min)
      retval= dLambda1min;
    else if(retval > dLambda1max)
      retval= dLambda1max;

    dLambda1LastStep= retval;


    if(signFirstStepMethod == SIGN_LAST_STEP)
      {
        if(vectores.getDeltaLambdaStep() < 0)
          signLastDeltaLambdaStep = -1;
        else
	  signLastDeltaLambdaStep = +1;
        retval*= signLastDeltaLambdaStep; // base sign of load change
                                        // on what was happening last step
      }
    else
      {
        //XXX Lo de eliminar la «constancia» no queda muy bien.
        LinearSOE *theLinSOE= const_cast<LinearSOE *>(getLinearSOEPtr());
        double det= theLinSOE->getDeterminant();
        int signDeterminant = 1;
        if(det < 0)
	  signDeterminant = -1;
        retval*= signDeterminant * signLastDeterminant;
        signLastDeterminant = signDeterminant;
      }
    return retval;
  }

int XC::MinUnbalDispNorm::newStep(void)
  {
    // get pointers to XC::AnalysisModel and XC::LinearSOE
    AnalysisModel *theModel = this->getAnalysisModelPtr();
    LinearSOE *theLinSOE = this->getLinearSOEPtr();    
    if(theModel == 0 || theLinSOE == 0)
      {
	std::cerr << "WARNING XC::MinUnbalDispNorm::newStep() ";
	std::cerr << "No XC::AnalysisModel or XC::LinearSOE has been set\n";
	return -1;
      }

    // get the current load factor
    vectores.setCurrentLambda(getCurrentModelTime());


    // determine dUhat
    this->formTangent();
    vectores.determineUhat(*theLinSOE);

    const double dLambda= getDLambdaNewStep();

    /*
    double work = (*phat)^(dUhat);
    int signCurrentWork = 1;
    if (work < 0) signCurrentWork = -1;

    if (signCurrentWork != signLastDeltaStep)
    */

    vectores.newStep(dLambda,vectores.getDeltaUhat());
    numIncrLastStep= 0;

    // update model with delta lambda and delta U
    theModel->incrDisp(vectores.getDeltaU());    
    applyLoadModel(vectores.getCurrentLambda());    
    if(updateModel() < 0)
      {
        std::cerr << "MinUnbalDispNorm::newStep- falló la actualización para el nuevo dU\n";
        return -1;
      }
    return 0;
  }

//! @brief Devuelve el valor de dLambda para el método update.
double XC::MinUnbalDispNorm::getDLambdaUpdate(void) const
  {
    const Vector &dUhat= vectores.getDeltaUhat();
    const Vector &dUbar= vectores.getDeltaUbar();
    // determine delta lambda(i)
    double a = dUhat^dUbar;
    double b = dUhat^dUhat;
    if(b == 0)
      {
        std::cerr << "XC::MinUnbalDispNorm::getDLambdaUpdate() - zero denominator\n";
        return -1;
      }
    return -a/b;
  }

int XC::MinUnbalDispNorm::update(const Vector &dU)
  {
    AnalysisModel *theModel = this->getAnalysisModelPtr();
    LinearSOE *theLinSOE = this->getLinearSOEPtr();    
    if (theModel == 0 || theLinSOE == 0) {
	std::cerr << "WARNING XC::MinUnbalDispNorm::update() ";
	std::cerr << "No AnalysisModel or LinearSOE has been set\n";
	return -1;
    }

    vectores.setDeltaUbar(dU); // have to do this as the SOE is gonna change

    vectores.determineUhat(*theLinSOE);

    const double dLambda= getDLambdaUpdate();

    
    vectores.update(dLambda);

    // update the model
    theModel->incrDisp(vectores.getDeltaU());    
    applyLoadModel(vectores.getCurrentLambda());    

    if(updateModel() < 0)
      {
        std::cerr << "XC::MinUnbalDispNorm::update- falló la actualización para el nuevo dU\n";
        return -1;
      }

    // set the X soln in linearSOE to be deltaU for convergence Test
    theLinSOE->setX(vectores.getDeltaU());

    numIncrLastStep++;
    return 0;
  }



int XC::MinUnbalDispNorm::domainChanged(void)
  {
    // we first create the Vectors needed
    AnalysisModel *theModel = this->getAnalysisModelPtr();
    LinearSOE *theLinSOE = this->getLinearSOEPtr();    
    if(theModel == 0 || theLinSOE == 0)
      {
	std::cerr << "WARNING XC::MinUnbalDispNorm::update() ";
	std::cerr << "No XC::AnalysisModel or XC::LinearSOE has been set\n";
	return -1;
      }    
    const size_t sz= theModel->getNumEqn(); // ask model in case N+1 space

    vectores.domainChanged(sz,*this,*theLinSOE);

    return 0;
  }

//! @brief Send members del objeto through the channel being passed as parameter.
int XC::MinUnbalDispNorm::sendData(CommParameters &cp)
  {
    int res= DispBase::sendData(cp);
    res+= cp.sendDoubles(dLambda1LastStep,dLambda1min,dLambda1max,signLastDeterminant,getDbTagData(),CommMetaData(17));
    res+= cp.sendInts(signLastDeltaLambdaStep,signFirstStepMethod,getDbTagData(),CommMetaData(18));
    return res;
  }

//! @brief Receives members del objeto through the channel being passed as parameter.
int XC::MinUnbalDispNorm::recvData(const CommParameters &cp)
  {
    int res= DispBase::recvData(cp);
    res+= cp.receiveDoubles(dLambda1LastStep,dLambda1min,dLambda1max,signLastDeterminant,getDbTagData(),CommMetaData(17));
    res+= cp.receiveInts(signLastDeltaLambdaStep,signFirstStepMethod,getDbTagData(),CommMetaData(18));
    return res;
  }

int XC::MinUnbalDispNorm::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(19);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << nombre_clase() << "sendSelf() - failed to send data\n";
    return res;
  }


int XC::MinUnbalDispNorm::recvSelf(const CommParameters &cp)
  {
    inicComm(19);
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
