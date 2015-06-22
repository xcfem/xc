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
//ProtoArcLength.cpp


#include <solution/analysis/integrator/static/ProtoArcLength.h>
#include "solution/analysis/model/AnalysisModel.h"
#include "solution/system_of_eqn/linearSOE/LinearSOE.h"
#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "utility/actor/actor/ArrayCommMetaData.h"

//! @brief Constructor.
XC::ProtoArcLength::ProtoArcLength(SoluMethod *owr,int classTag,double arcLength)
  :StaticIntegrator(owr,classTag), arcLength2(arcLength*arcLength), signLastDeltaLambdaStep(1) {}

//! @brief Lee un objeto ProtoArcLength desde archivo
bool XC::ProtoArcLength::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(ProtoArcLength) Procesando comando: " << cmd << std::endl;
    if(cmd=="arc_length")
      {
        const double tmp= interpretaDouble(status.GetString());
        arcLength2= tmp*tmp;
        return true;
      }
    else
      return StaticIntegrator::procesa_comando(status);
  }

int XC::ProtoArcLength::newStep(void)
  {
    // get pointers to AnalysisModel and LinearSOE
    LinearSOE *theLinSOE= this->getLinearSOEPtr();
    AnalysisModel *mdl= getAnalysisModelPtr();
    if(!mdl || !theLinSOE)
      {
        std::cerr << "WARNING ArcLength::newStep() ";
        std::cerr << "No AnalysisModel or LinearSOE has been set\n";
        return -1;
      }

    // get the current load factor
    vectores.setCurrentLambda(getCurrentModelTime());

    if(vectores.getDeltaLambdaStep() < 0)
      signLastDeltaLambdaStep= -1;
    else
      signLastDeltaLambdaStep= +1;

    // determine dUhat
    this->formTangent();
    vectores.determineUhat(*theLinSOE);

    // determine delta lambda(1) == dlambda
    const double dLambda = getDLambdaNewStep();

    vectores.newStep(dLambda,vectores.getDeltaUhat());

    // update model with delta lambda and delta U
    mdl->incrDisp(vectores.getDeltaU());
    applyLoadModel(vectores.getCurrentLambda());
    if(updateModel() < 0)
      {
        std::cerr << "DisplacementControl::newStep - falló la actualización para el nuevo dU\n";
        return -1;
      }

    return 0;
  }

//! @brief Actualiza el estado del sistema.
int XC::ProtoArcLength::update(const Vector &dU)
  {
    LinearSOE *theLinSOE = this->getLinearSOEPtr();    
    AnalysisModel *mdl= getAnalysisModelPtr();
    if(!mdl || !theLinSOE)
      {
	std::cerr << "WARNING XC::ArcLength1::update() ";
	std::cerr << "No XC::AnalysisModel or LinearSOE has been set\n";
	return -1;
      }

    vectores.solve(dU,*theLinSOE);

    const double dLambda= getDLambdaUpdate();

    vectores.update(dLambda);

    // update the model
    mdl->incrDisp(vectores.getDeltaU());    
    applyLoadModel(vectores.getCurrentLambda());    
    if(updateModel() < 0)
      {
        std::cerr << "ProtoArcLength::update - falló la actualización para el nuevo dU.\n";
      }
    
    // set the X soln in linearSOE to be deltaU for convergence Test
    theLinSOE->setX(vectores.getDeltaU());

    return 0;
  }

//! @brief Respuesta a un cambio en el dominio.
int XC::ProtoArcLength::domainChanged(void)
  {
    // we first create the Vectors needed
    LinearSOE *theLinSOE = this->getLinearSOEPtr();    
    AnalysisModel *mdl= getAnalysisModelPtr();
    if(!mdl || !theLinSOE)
      {
	std::cerr << "WARNING ArcLengthBase::domainChanged() ";
	std::cerr << "No AnalysisModel or LinearSOE has been set\n";
	return -1;
      }    
    const size_t sz= mdl->getNumEqn(); // ask model in case N+1 space

    vectores.domainChanged(sz,*this,*theLinSOE);
    
    return 0;
  }

//! @brief Envía los miembros del objeto a través del canal que se pasa como parámetro.
int XC::ProtoArcLength::sendData(CommParameters &cp)
  {
    int res= StaticIntegrator::sendData(cp);
    res+= cp.sendDouble(arcLength2,getDbTagData(),CommMetaData(1));
    res+= cp.sendMovable(vectores,getDbTagData(),CommMetaData(2));
    res+= cp.sendInt(signLastDeltaLambdaStep,getDbTagData(),CommMetaData(3));
    return res;
  }

//! @brief Recibe los miembros del objeto a través del canal que se pasa como parámetro.
int XC::ProtoArcLength::recvData(const CommParameters &cp)
  {
    int res= StaticIntegrator::recvData(cp);
    res+= cp.receiveDouble(arcLength2,getDbTagData(),CommMetaData(1));
    res+= cp.receiveMovable(vectores,getDbTagData(),CommMetaData(2));
    res+= cp.receiveInt(signLastDeltaLambdaStep,getDbTagData(),CommMetaData(7));
    return res;
  }

int XC::ProtoArcLength::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(8);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << nombre_clase() << "sendSelf() - failed to send data\n";
    return res;
  }


int XC::ProtoArcLength::recvSelf(const CommParameters &cp)
  {
    inicComm(8);
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

//! \brief Devuelve la propiedad del objeto cuyo código (de la propiedad) se pasa
//! como parámetro.
//!
//! Soporta los códigos:
any_const_ptr XC::ProtoArcLength::GetProp(const std::string &cod) const
  {
    if(cod=="arc_length")
      {
        tmp_gp_dbl= sqrt(arcLength2);
        return any_const_ptr(tmp_gp_dbl);
      }
    else if(cod=="getDeltaLambdaStep")
      return any_const_ptr(vectores.getDeltaLambdaStep());
    else if(cod== "getCurrentLambda")
      return any_const_ptr(vectores.getCurrentLambda());
    else if(cod=="signLastDeltaLambdaStep")
      return any_const_ptr(signLastDeltaLambdaStep);
    else
      return StaticIntegrator::GetProp(cod);
  }
