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
//RayleighDampingFactors.cpp

#include "RayleighDampingFactors.h"
#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "utility/matrix/Vector.h"
#include <domain/mesh/element/Information.h>

//! @brief constructor
XC::RayleighDampingFactors::RayleighDampingFactors(void)
  :EntCmd(), MovableObject(0), alphaM(0.0), betaK(0.0), betaK0(0.0), betaKc(0.0) {}

//! @brief constructor
XC::RayleighDampingFactors::RayleighDampingFactors(const double &aM,const double &bK,const double &bK0,const double &bKc)
  :EntCmd(), MovableObject(0), alphaM(aM), betaK(bK), betaK0(bK0), betaKc(bKc) {}

//! @brief constructor
XC::RayleighDampingFactors::RayleighDampingFactors(const Vector &v)
  : EntCmd(), MovableObject(0), alphaM(v[0]), betaK(v[1]), betaK0(v[2]), betaKc(v[3]) {}

//! @brief Lee un objeto XC::RayleighDampingFactors desde archivo
bool XC::RayleighDampingFactors::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(Element) Procesando comando: " << cmd << std::endl;
    if(cmd == "alphaM")
      {
        alphaM= interpretaDouble(status.GetString());
        return true;
      }
    else if(cmd == "betaK")
      {
        betaK= interpretaDouble(status.GetString());
        return true;
      }
    else if(cmd == "betaK0")
      {
        betaK0= interpretaDouble(status.GetString());
        return true;
      }
    else if(cmd == "betaKc")
      {
        betaKc= interpretaDouble(status.GetString());
        return true;
      }
    else
      return EntCmd::procesa_comando(status);
  }

void XC::RayleighDampingFactors::Print(std::ostream &s, int flag) const
  {
    s << "rayleigh damping factors: alphaM: " << alphaM << " betaK: ";
    s << betaK << " betaK0: " << betaK0 << std::endl;
  }


int XC::RayleighDampingFactors::updateParameter(int parameterID, Information &info)
  {
    switch (parameterID)
      {
      case 1:
        alphaM= info.theDouble;
        return 0;
      case 2:
        betaK = info.theDouble;
        return 0;
      default:
        return 0;
      }
  }

//! Devuelve la propiedad del objeto cuyo código se pasa
//! como parámetro.
any_const_ptr XC::RayleighDampingFactors::GetProp(const std::string &cod) const
  {
    static m_double tmp;
    if(cod == "getAlphaM")
      return any_const_ptr(alphaM);
    else if(cod == "getBetaK")
      return any_const_ptr(betaK);
    else if(cod == "getBetaK0")
      return any_const_ptr(betaK0);
    else if(cod == "getBetaKc")
      return any_const_ptr(betaKc);
    else
      return EntCmd::GetProp(cod);
  }

//! @brief Envía los miembros del objeto a través del canal que se pasa como parámetro.
int XC::RayleighDampingFactors::sendData(CommParameters &cp)
  {
    int res=cp.sendDoubles(alphaM,betaK,betaK0,betaKc,getDbTagData(),CommMetaData(1));
    return res;
  }

//! @brief Recibe los miembros del objeto a través del canal que se pasa como parámetro.
int XC::RayleighDampingFactors::recvData(const CommParameters &cp)
  {
    int res= cp.receiveDoubles(alphaM,betaK,betaK0,betaKc,getDbTagData(),CommMetaData(1));
    return res;
  }

//! @brief Envía el objeto a través del canal que se pasa como parámetro.
int XC::RayleighDampingFactors::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(2);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << nombre_clase() << "sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Recibe el objeto a través del canal que se pasa como parámetro.
int XC::RayleighDampingFactors::recvSelf(const CommParameters &cp)
  {
    inicComm(2);
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

std::ostream &XC::operator<<(std::ostream &os,const XC::RayleighDampingFactors &rF)
  {
    rF.Print(os);
    return os;
  }
