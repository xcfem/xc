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
//ConcreteBase.cpp

#include <material/uniaxial/concrete/ConcreteBase.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>

#include <domain/mesh/element/Information.h>
#include <cmath>
#include <cfloat>
#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/base/any_const_ptr.h"

//! @brief Constructor.
XC::ConcreteBase::ConcreteBase(int tag, int classTag, double FPC, double EPSC0, double EPSCU)
  :UniaxialMaterial(tag, classTag), fpc(FPC), epsc0(EPSC0), epscu(EPSCU) {}

//! @brief Constructor
XC::ConcreteBase::ConcreteBase(int tag, int classTag)
  :UniaxialMaterial(tag, classTag), fpc(0.0), epsc0(0.0), epscu(0.0) {}

void XC::ConcreteBase::setFpc(const double &d)
  {
    fpc= d;
    if(fpc > 0.0)
      {
        fpc= -fpc;
        std::clog << "!Ojo!, los parámetros del hormigón deben ser negativos." << std::endl;
      }
    setup_parameters();
  }

double XC::ConcreteBase::getFpc(void) const
  { return fpc; }

void XC::ConcreteBase::setEpsc0(const double &d)
  {
    epsc0= d;
    if(epsc0 > 0.0)
      {
        epsc0= -epsc0;
        std::clog << "!Ojo!, los parámetros del hormigón deben ser negativos." << std::endl;
      }
    setup_parameters();
  }

double XC::ConcreteBase::getEpsc0(void) const
  { return epsc0; }

void XC::ConcreteBase::setEpscu(const double &d)
  {
    epscu= d;
    if(epscu > 0.0)
      {
        epscu= -epscu;
        std::clog << "!Ojo!, los parámetros del hormigón deben ser negativos." << std::endl;
      }
  }

double XC::ConcreteBase::getEpscu(void) const
  { return epscu; }

//! @brief Lee un objeto XC::ConcreteBase desde archivo
bool XC::ConcreteBase::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(ConcreteBase) Procesando comando: " << cmd << std::endl;
    if(cmd == "fpc")
      {
        setFpc(interpretaDouble(status.GetString()));
        return true;
      }
    else if(cmd == "epsc0")
      {
        setEpsc0(interpretaDouble(status.GetString()));
        return true;
      }
    else if(cmd == "epscu")
      {
        setEpscu(interpretaDouble(status.GetString()));
        return true;
      }
    else
      return UniaxialMaterial::procesa_comando(status);
  }

//! @brief Devuelve la tensión en el material.
double XC::ConcreteBase::getStress(void) const
  { return trialState.getStress(); }

//! @breif Devuelve la deformación del material.
double XC::ConcreteBase::getStrain(void) const
  { return trialState.getStrain(); }

//! @breif Devuelve la tangente al diagrama tensión - deformación.
double XC::ConcreteBase::getTangent(void) const
  { return trialState.getTangent(); }

//! @brief Envía los miembros del objeto a través del canal que se pasa como parámetro.
int XC::ConcreteBase::sendData(CommParameters &cp)
  {
    int res= UniaxialMaterial::sendData(cp);
    res+= cp.sendMovable(convergedState,getDbTagData(),CommMetaData(2));
    res+= cp.sendMovable(trialState,getDbTagData(),CommMetaData(3));
    res+= cp.sendMovable(convergedHistory,getDbTagData(),CommMetaData(4));
    res+= cp.sendMovable(trialHistory,getDbTagData(),CommMetaData(5));
    res+= cp.sendDoubles(fpc,epsc0,epscu,getDbTagData(),CommMetaData(6));
    return res;
  }

//! @brief Recibe los miembros del objeto a través del canal que se pasa como parámetro.
int XC::ConcreteBase::recvData(const CommParameters &cp)
  {
    int res= UniaxialMaterial::recvData(cp);
    res+= cp.receiveMovable(convergedState,getDbTagData(),CommMetaData(2));
    res+= cp.receiveMovable(trialState,getDbTagData(),CommMetaData(3));
    res+= cp.receiveMovable(convergedHistory,getDbTagData(),CommMetaData(4));
    res+= cp.receiveMovable(trialHistory,getDbTagData(),CommMetaData(5));
    res+= cp.receiveDoubles(fpc,epsc0,epscu,getDbTagData(),CommMetaData(6));
    return res;
  }

//! \brief Devuelve la propiedad del objeto cuyo código (de la propiedad) se pasa
//! como parámetro.
//!
//! Soporta los códigos:
//! nnod: Devuelve el número de nodos del dominio.
any_const_ptr XC::ConcreteBase::GetProp(const std::string &cod) const
  {
    if(verborrea>4)
      std::clog << "ConcreteBase::GetProp (" << nombre_clase() << "::GetProp) Buscando propiedad: " << cod << std::endl;
    if(cod=="fpc")
      return any_const_ptr(fpc);
    else if(cod=="epsc0")
      return any_const_ptr(epsc0);
    else if(cod=="epscu")
      return any_const_ptr(epscu);
    else if(cod=="CminStrain")
      return any_const_ptr(convergedHistory.getMinStrain());
    else if(cod=="CunloadSlope")
      return any_const_ptr(convergedHistory.getUnloadSlope());
    else if(cod=="CendStrain")
      return any_const_ptr(convergedHistory.getEndStrain());
    else if(cod=="Ctangent")
      return any_const_ptr(convergedState.getTangent());
    else if(cod=="Cstrain")
      return any_const_ptr(convergedState.getStrain());
    else if(cod=="Cstress")
      return any_const_ptr(convergedState.getStress());
    else if(cod=="TminStrain")
      return any_const_ptr(trialHistory.getMinStrain());
    else if(cod=="TunloadSlope")
      return any_const_ptr(trialHistory.getUnloadSlope());
    else if(cod=="TendStrain")
      return any_const_ptr(trialHistory.getEndStrain());
    else if(cod=="Ttangent")
      return any_const_ptr(trialState.getTangent());
    else if(cod=="Tstrain")
      return any_const_ptr(trialState.getStrain());
    else if(cod=="Tstress")
      return any_const_ptr(trialState.getStress());
    else
      return UniaxialMaterial::GetProp(cod);
  }
