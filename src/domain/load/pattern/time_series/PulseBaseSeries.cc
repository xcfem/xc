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
//PulseBaseSeries.cpp

#include "PulseBaseSeries.h"
#include <utility/matrix/Vector.h>
#include <classTags.h>
#include <cmath>
#include "xc_utils/src/base/CmdStatus.h"
#include "utility/actor/actor/MovableVector.h"
#include "utility/matrix/ID.h"
#include "xc_utils/src/base/any_const_ptr.h"


//! @brief Constructor.
XC::PulseBaseSeries::PulseBaseSeries(int classTag,const double &startTime,const double &finishTime,const double &factor)
  : CFactorSeries(classTag,factor), tStart(startTime),tFinish(finishTime) {}

//! @brief Lee un objeto XC::PulseBaseSeries desde archivo
bool XC::PulseBaseSeries::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(PulseBaseSeries) Procesando comando: " << cmd << std::endl;

    if(cmd == "start")
      {
        tStart= interpretaDouble(status.GetString());
        return true;
      }
    else if(cmd == "finish")
      {
        tFinish= interpretaDouble(status.GetString());
        return true;
      }
    else
      return CFactorSeries::procesa_comando(status);
  }

//! @brief Envía los miembros del objeto a través del canal que se pasa como parámetro.
int XC::PulseBaseSeries::sendData(CommParameters &cp)
  {
    int res= CFactorSeries::sendData(cp);
    res+= cp.sendDoubles(tStart,tFinish,getDbTagData(),CommMetaData(1));
    return res;
  }

//! @brief Recibe los miembros del objeto a través del canal que se pasa como parámetro.
int XC::PulseBaseSeries::recvData(const CommParameters &cp)
  {
    int res= CFactorSeries::recvData(cp);
    res+= cp.receiveDoubles(tStart,tFinish,getDbTagData(),CommMetaData(1));
    return res;
  }


//! @brief Envía el objeto a través del canal que se pasa como parámetro.
int XC::PulseBaseSeries::sendSelf(CommParameters &cp)
  {
    inicComm(2);
    int result= sendData(cp);

    const int dataTag= getDbTag();
    result+= cp.sendIdData(getDbTagData(),dataTag);
    if(result < 0)
      std::cerr << "PulseBaseSeries::sendSelf() - ch failed to send data\n";
    return result;
  }

//! @brief Recibe el objeto a través del canal que se pasa como parámetro.
int XC::PulseBaseSeries::recvSelf(const CommParameters &cp)
  {
    inicComm(2);

    const int dataTag = this->getDbTag();  
    int result = cp.receiveIdData(getDbTagData(),dataTag);
    if(result<0)
      std::cerr << "PulseBaseSeries::sendSelf() - ch failed to receive data\n";
    else
      result+= recvData(cp);
    return result;    
  }

//! \brief Devuelve la propiedad del objeto cuyo código (de la propiedad) se pasa
//! como parámetro.
any_const_ptr XC::PulseBaseSeries::GetProp(const std::string &cod) const
  {
    if(cod == "getStart")
      return any_const_ptr(tStart);
    else if(cod=="getFinish")
      return any_const_ptr(tFinish);
    else
      return CFactorSeries::GetProp(cod);
  }

//! @brief Imprime el objeto.
void XC::PulseBaseSeries::Print(std::ostream &s, int flag) const
  {
    s << "Pulse Series" << std::endl;
    s << "\tFactor: " << cFactor << std::endl;
    s << "\ttStart: " << tStart << std::endl;
    s << "\ttFinish: " << tFinish << std::endl;
  }
