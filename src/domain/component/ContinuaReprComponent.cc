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
//ContinuaReprComponent.cc

#include "ContinuaReprComponent.h"
#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "utility/matrix/ID.h"

XC::ContinuaReprComponent::ContinuaReprComponent(int classTag)
  : DomainComponent(0,classTag), dead(false){}

XC::ContinuaReprComponent::ContinuaReprComponent(int tag, int classTag)
  : DomainComponent(tag,classTag), dead(false){}

//! @brief Envía los miembros a través del canal que se pasa como parámetro.
int XC::ContinuaReprComponent::sendData(CommParameters &cp)
  {
    setDbTagDataPos(0,getTag());
    int res= cp.sendBool(dead,getDbTagData(),CommMetaData(1));
    return res;
  }

//! @brief Recibe los miembros a través del canal que se pasa como parámetro.
int XC::ContinuaReprComponent::recvData(const CommParameters &cp)
  {
    setTag(getDbTagDataPos(0));
    int res= cp.receiveBool(dead,getDbTagData(),CommMetaData(1));
    return res;
  }

//! @brief Lee un objeto XC::Element desde archivo
bool XC::ContinuaReprComponent::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(ContinuaReprComponent) Procesando comando: " << cmd << std::endl;
    if(cmd == "kill")
      {
        status.GetBloque(); //Ignoramos argumentos.
        kill();
        return true;
      }
    else if(cmd == "alive")
      {
        status.GetBloque(); //Ignoramos argumentos.
        alive();
        return true;
      }
    else
      return DomainComponent::procesa_comando(status);
  }

//! Devuelve la propiedad del objeto cuyo código se pasa
//! como parámetro.
any_const_ptr XC::ContinuaReprComponent::GetProp(const std::string &cod) const
  {
    if(verborrea>4)
      std::clog << "ContinuaReprComponent::GetProp (" << nombre_clase() 
                << "::GetProp) Buscando propiedad: " << cod << std::endl;
    if(cod == "isAlive")
      {
        tmp_gp_bool= isAlive();
        return any_const_ptr(tmp_gp_bool);
      }
    else if(cod == "isDead")
      {
        tmp_gp_bool= isDead();
        return any_const_ptr(tmp_gp_bool);
      }
    else
      return DomainComponent::GetProp(cod);
  }

