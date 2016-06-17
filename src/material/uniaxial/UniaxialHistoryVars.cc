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

#include "UniaxialHistoryVars.h"

XC::UniaxialHistoryVars::UniaxialHistoryVars(void)
  :MovableObject(0), minStrain(0.0), unloadSlope(0.0), endStrain(0.0) {}


//! @brief Devuelve el estado del material al inicial.
int XC::UniaxialHistoryVars::revertToStart(const double &E)
  {
    minStrain= 0.0;
    endStrain= 0.0;
    unloadSlope= E;
    return 0;
  }

void XC::UniaxialHistoryVars::zero(void)
  {
    minStrain= 0.0;
    unloadSlope= 0.0;
    endStrain= 0.0;
  }

//! @brief Send members del objeto through the channel being passed as parameter.
int XC::UniaxialHistoryVars::sendData(CommParameters &cp)
  {
    int res= cp.sendDoubles(minStrain,unloadSlope,endStrain,getDbTagData(),CommMetaData(0));
    return res;
  }

//! @brief Receives members del objeto through the channel being passed as parameter.
int XC::UniaxialHistoryVars::recvData(const CommParameters &cp)
  {
    int res= cp.receiveDoubles(minStrain,unloadSlope,endStrain,getDbTagData(),CommMetaData(0));
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::UniaxialHistoryVars::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(10); 
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << "UniaxialHistoryVars::sendSelf - failed to send data.\n";
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::UniaxialHistoryVars::recvSelf(const CommParameters &cp)
  {
    inicComm(10);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "UniaxialHistoryVars::recvSelf - failed to receive ids.\n";
    else
      {
        res+= recvData(cp);
        if(res<0)
           std::cerr << "UniaxialHistoryVars::recvSelf - failed to receive data.\n";
      }
    return res;
  }

void XC::UniaxialHistoryVars::Print(std::ostream &s, int flag)
  {
    s << "UniaxialHistoryVars, min. strain: " << minStrain << std::endl;
    s << "  unload slope: " << unloadSlope << std::endl;
    s << "  end strain: " << endStrain << std::endl;
  }


