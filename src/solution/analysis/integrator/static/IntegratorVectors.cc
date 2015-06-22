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
//IntegratorVectors.cpp


#include "IntegratorVectors.h"
#include "solution/analysis/integrator/IncrementalIntegrator.h"
#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "solution/system_of_eqn/linearSOE/LinearSOE.h"
#include "utility/actor/actor/CommMetaData.h"

//! @brief Constructor.
XC::IntegratorVectors::IntegratorVectors(void)
  :MovableObject(0), deltaLambdaStep(0.0), currentLambda(0.0) {}


//! @brief Cambia el tamaño de los vectores.
void XC::IntegratorVectors::resize(const int &size)
  {
    if(deltaUhat.Size() != size)
      deltaUhat.resize(size);

    if(deltaUbar.Size() != size)
      deltaUbar.resize(size);

    if(deltaU.Size() != size)
      deltaU.resize(size);

    if(deltaUstep.Size() != size)
      deltaUstep.resize(size);

    if(phat.Size() != size)
      phat.resize(size);
  }

//! @brief Realiza las operaciones correspondientes a un nuevo paso.
void XC::IntegratorVectors::newStep(const double &dLambda,const Vector &dUhat)
  {
    deltaLambdaStep= dLambda;
    currentLambda+= dLambda;

    // determine delta U(1) == dU
    deltaU= dUhat;
    deltaU*= dLambda;
    deltaUstep= deltaU;
  }

//! @brief Obtiene la solución para el nuevo vector de cargas empleando
//! procesamiento paralelo.
void XC::IntegratorVectors::distribDetermineUhat(const int &pID,LinearSOE &theLinSOE)
  {
    if(pID == 0)
      theLinSOE.setB(phat);
    else
      theLinSOE.zeroB();

    theLinSOE.solve();
    deltaUhat= theLinSOE.getX();
  }

//! @brief Obtiene la solución para el nuevo vector de cargas.
void XC::IntegratorVectors::determineUhat(LinearSOE &theLinSOE)
  {
    // determine dUhat    
    theLinSOE.setB(phat);
    if(theLinSOE.solve() < 0)
      std::cerr << "IntegratorVectors::determineUhat - failed in solver\n";
    deltaUhat= theLinSOE.getX();
  }

//! @brief Obtiene la solución para el nuevo vector de cargas.
void XC::IntegratorVectors::solve(const Vector &dU,LinearSOE &theLinSOE)
  {
    deltaUbar= dU; // have to do this as the SOE is gonna change
    determineUhat(theLinSOE);
  }

//! @brief Actualiza los vectores.
void XC::IntegratorVectors::update(const double &dLambda)
  {
    // determine delta U(i)
    deltaU= deltaUbar;    
    deltaU.addVector(1.0, deltaUhat,dLambda);
    
    // update dU and dlambda
    deltaUstep+= deltaU;
    deltaLambdaStep+= dLambda;
    currentLambda+= dLambda;
  }

//! @brief Respuesta a un cambio en el dominio.
void XC::IntegratorVectors::domainChanged(const size_t &sz,IncrementalIntegrator &I,LinearSOE &theLinSOE)
  {
    resize(sz);

    // now we have to determine phat
    // do this by incrementing lambda by 1, applying load
    // and getting phat from unbalance.
    currentLambda= I.getCurrentModelTime()+1.0;
    I.applyLoadModel(currentLambda);
    I.formUnbalance(); // NOTE: this assumes unbalance at last was 0
    phat= theLinSOE.getB();
    currentLambda-= 1.0;
    I.setCurrentModelTime(currentLambda);

    // check there is a reference load
    bool haveLoad= false;
    for(size_t i=0; i<sz; i++)
      if(phat(i)!=0.0)
        {
          haveLoad= true;
          break;
        }
    if(!haveLoad)
      {
        std::cerr << "WARNING IntegratorVectors::domainChanged() - zero reference load";
      }
  }

//! @brief Envía los miembros del objeto a través del canal que se pasa como parámetro.
int XC::IntegratorVectors::sendData(CommParameters &cp)
  {
    int res= cp.sendDoubles(deltaLambdaStep,currentLambda,getDbTagData(),CommMetaData(1));
    res+= cp.sendVector(deltaUhat,getDbTagData(),CommMetaData(2));
    res+= cp.sendVector(deltaUbar,getDbTagData(),CommMetaData(3));
    res+= cp.sendVector(deltaU,getDbTagData(),CommMetaData(4));
    res+= cp.sendVector(deltaUstep,getDbTagData(),CommMetaData(5));
    res+= cp.sendVector(phat,getDbTagData(),CommMetaData(6));
    return res;
  }

//! @brief Recibe los miembros del objeto a través del canal que se pasa como parámetro.
int XC::IntegratorVectors::recvData(const CommParameters &cp)
  {
    int res= cp.receiveDoubles(deltaLambdaStep,currentLambda,getDbTagData(),CommMetaData(1));
    res+= cp.receiveVector(deltaUhat,getDbTagData(),CommMetaData(2));
    res+= cp.receiveVector(deltaUbar,getDbTagData(),CommMetaData(3));
    res+= cp.receiveVector(deltaU,getDbTagData(),CommMetaData(4));
    res+= cp.receiveVector(deltaUstep,getDbTagData(),CommMetaData(5));
    res+= cp.receiveVector(phat,getDbTagData(),CommMetaData(6));
    return res;
  }

int XC::IntegratorVectors::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(7);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << "IntegratorVectors::sendSelf() - failed to send data\n";
    return res;
  }


int XC::IntegratorVectors::recvSelf(const CommParameters &cp)
  {
    inicComm(7);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << "IntegratorVectors::recvSelf - failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
          std::cerr << "IntegratorVectors::recvSelf - failed to receive data.\n";
      }
    return res;
  }


