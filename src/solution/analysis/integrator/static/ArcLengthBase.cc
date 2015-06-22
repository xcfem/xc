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
//ArcLengthBase.cpp


#include <solution/analysis/integrator/static/ArcLengthBase.h>
#include <utility/matrix/Vector.h>
#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "utility/actor/actor/CommMetaData.h"

//! @brief Constructor.
XC::ArcLengthBase::ArcLengthBase(SoluMethod *owr,int classTag,double arcLength, double alpha)
  :ProtoArcLength(owr,classTag,arcLength), alpha2(alpha*alpha) {}

//! @brief Devuelve el valor de dLambda para el método newStep.
double XC::ArcLengthBase::getDLambdaNewStep(void) const
  {
    const Vector &dUhat= vectores.getDeltaUhat();
    // determine delta lambda(1) == dlambda
    double retval = sqrt(arcLength2/((dUhat^dUhat)+alpha2));
    retval *= signLastDeltaLambdaStep; // base sign of load change
                                        // on what was happening last step
    return retval;
  }

//! @brief Lee un objeto ArcLengthBase desde archivo
bool XC::ArcLengthBase::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(ArcLengthBase) Procesando comando: " << cmd << std::endl;
    if(cmd=="alpha")
      {
        const double tmp= interpretaDouble(status.GetString());
        alpha2= tmp*tmp;
        return true;
      }
    else
      return ProtoArcLength::procesa_comando(status);
  }

//! @brief Envía los miembros del objeto a través del canal que se pasa como parámetro.
int XC::ArcLengthBase::sendData(CommParameters &cp)
  {
    int res= ProtoArcLength::sendData(cp);
    res+= cp.sendDouble(alpha2,getDbTagData(),CommMetaData(18));
    return res;
  }

//! @brief Recibe los miembros del objeto a través del canal que se pasa como parámetro.
int XC::ArcLengthBase::recvData(const CommParameters &cp)
  {
    int res= ProtoArcLength::recvData(cp);
    res+= cp.receiveDouble(alpha2,getDbTagData(),CommMetaData(18));
    return res;
  }

int XC::ArcLengthBase::sendSelf(CommParameters &cp)
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


int XC::ArcLengthBase::recvSelf(const CommParameters &cp)
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

//! \brief Devuelve la propiedad del objeto cuyo código (de la propiedad) se pasa
//! como parámetro.
//!
//! Soporta los códigos:
any_const_ptr XC::ArcLengthBase::GetProp(const std::string &cod) const
  {
    if(cod=="alpha")
      {
        tmp_gp_dbl= sqrt(alpha2);
        return any_const_ptr(tmp_gp_dbl);
      }
    else
      return ProtoArcLength::GetProp(cod);
  }
