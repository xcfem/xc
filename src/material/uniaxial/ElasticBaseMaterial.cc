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
//ElasticBaseMaterial.cc

#include <material/uniaxial/ElasticBaseMaterial.h>
#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "utility/matrix/Vector.h"


//! @brief Constructor.
XC::ElasticBaseMaterial::ElasticBaseMaterial(int tag, int classtag, double e,double e0)
  :UniaxialMaterial(tag,classtag), trialStrain(0.0),E(e),ezero(e0) {}

//! @brief Lee un objeto XC::ElasticBaseMaterial desde archivo
bool XC::ElasticBaseMaterial::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(ElasticBaseMaterial) Procesando comando: " << cmd << std::endl;
    if(cmd == "E")
      {
        E= interpretaDouble(status.GetString());
        return true;
      }
    else if(cmd == "trialStrain")
      {
        trialStrain= interpretaDouble(status.GetString());
        return true;
      }
    else if(cmd == "e0")
      {
        ezero= interpretaDouble(status.GetString());
        return true;
      }
    else
      return UniaxialMaterial::procesa_comando(status);
  }

//! @brief Establece el valor de la tensión inicial.
int XC::ElasticBaseMaterial::setInitialStrain(double strain)
  {
    ezero= strain;
    return 0;
  }

//! @brief Envía los miembros del objeto a través del canal que se pasa como parámetro.
int XC::ElasticBaseMaterial::sendData(CommParameters &cp)
  {
    int res= UniaxialMaterial::sendData(cp);
    res+= cp.sendDoubles(trialStrain,E,ezero,getDbTagData(),CommMetaData(2));
    return res;
  }

//! @brief Recibe los miembros del objeto a través del canal que se pasa como parámetro.
int XC::ElasticBaseMaterial::recvData(const CommParameters &cp)
  {
    int res= UniaxialMaterial::recvData(cp);
    res+= cp.receiveDoubles(trialStrain,E,ezero,getDbTagData(),CommMetaData(2));
    return res;
  }

//! @brief Envía el objeto a través del canal que se pasa como parámetro.
int XC::ElasticBaseMaterial::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(3); 
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << "ElasticBaseMaterial::sendSelf - failed to send data.\n";
    return res;
  }

//! @brief Recibe el objeto a través del canal que se pasa como parámetro.
int XC::ElasticBaseMaterial::recvSelf(const CommParameters &cp)
  {
    inicComm(3);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "ElasticBaseMaterial::recvSelf - failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
           std::cerr << "ElasticBaseMaterial::recvSelf - failed to receive data.\n";
      }
    return res;
  }

//! \brief Devuelve la propiedad del objeto cuyo código (de la propiedad) se pasa
//! como parámetro.
//!
//! Soporta los códigos:
//! nnod: Devuelve el número de nodos del dominio.
any_const_ptr XC::ElasticBaseMaterial::GetProp(const std::string &cod) const
  {
    if(cod=="E")
      return any_const_ptr(E);
    if(cod=="trial_strain")
      return any_const_ptr(trialStrain);
    if(cod=="e0")
      return any_const_ptr(ezero);
    else
      return UniaxialMaterial::GetProp(cod);
  }
