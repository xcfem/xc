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
//EPPBaseMaterial.cc

#include <material/uniaxial/EPPBaseMaterial.h>
#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/base/any_const_ptr.h"

//! @brief Constructor.
XC::EPPBaseMaterial::EPPBaseMaterial(int tag, int classtag, double e,double e0)
  :ElasticBaseMaterial(tag,classtag,e,e0), trialStress(0.0), trialTangent(e), commitStrain(0.0) {}

//! @brief Lee un objeto XC::EPPBaseMaterial desde archivo
bool XC::EPPBaseMaterial::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(EPPBaseMaterial) Procesando comando: " << cmd << std::endl;

    return ElasticBaseMaterial::procesa_comando(status);
  }

//! \brief Devuelve la propiedad del objeto cuyo código (de la propiedad) se pasa
//! como parámetro.
any_const_ptr XC::EPPBaseMaterial::GetProp(const std::string &cod) const
  {
    return ElasticBaseMaterial::GetProp(cod);
  }

//! @brief Envía los miembros del objeto a través del canal que se pasa como parámetro.
int XC::EPPBaseMaterial::sendData(CommParameters &cp)
  {
    int res= ElasticBaseMaterial::sendData(cp);
    res+= cp.sendDoubles(trialStress,trialTangent,commitStrain,getDbTagData(),CommMetaData(3));
    return res;
  }

//! @brief Recibe los miembros del objeto a través del canal que se pasa como parámetro.
int XC::EPPBaseMaterial::recvData(const CommParameters &cp)
  {
    int res= ElasticBaseMaterial::recvData(cp);
    res+= cp.receiveDoubles(trialStress,trialTangent,commitStrain,getDbTagData(),CommMetaData(3));
    return res;
  }
