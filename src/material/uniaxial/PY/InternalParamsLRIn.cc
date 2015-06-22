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
//InternalParamsLRIn.cc

#include "material/uniaxial/PY/InternalParamsLRIn.h"

//! @brief Constructor with data
XC::InternalParamsLRIn::InternalParamsLRIn(const double &V,const double &r,const double &t,const double &yl,const double &yr,const double &pl,const double &pr)
  : InternalParamsLR(V,r,t,yl,yr), RLeft(pl), RRight(pr) {}

//! @brief Envía los miembros del objeto a través del canal que se pasa como parámetro.
int XC::InternalParamsLRIn::sendData(CommParameters &cp)
  {
    int res= InternalParamsLR::sendData(cp);
    res+= cp.sendDoubles(RLeft,RRight,getDbTagData(),CommMetaData(2));
    return res;
  }

//! @brief Recibe los miembros del objeto a través del canal que se pasa como parámetro.
int XC::InternalParamsLRIn::recvData(const CommParameters &cp)
  {
    int res= InternalParamsLR::recvData(cp);
    res+= cp.receiveDoubles(RLeft,RRight,getDbTagData(),CommMetaData(2));
    return res;
  }

//! @brief Envía el objeto a través del canal que se pasa como parámetro.
int XC::InternalParamsLRIn::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(2); 
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << "InternalParamsLRIn::sendSelf - failed to send data.\n";
    return res;
  }

//! @brief Recibe el objeto a través del canal que se pasa como parámetro.
int XC::InternalParamsLRIn::recvSelf(const CommParameters &cp)
  {
    inicComm(2);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "InternalParamsLRIn::recvSelf - failed to receive ids.\n";
    else
      {
        res+= recvData(cp);
        if(res<0)
           std::cerr << "InternalParamsLRIn::recvSelf - failed to receive data.\n";
      }
    return res;
  }
