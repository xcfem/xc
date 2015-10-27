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
//MapCadMemberBase.cc

#include "MapCadMemberBase.h"
#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "preprocessor/cad/Cad.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"

//! @brief Constructor.
XC::MapCadMemberBase::MapCadMemberBase(Cad *cad)
  : EntCmd(cad), tag(0) {}

//! @brief Lee un objeto Pnt desde el archivo de entrada.
bool XC::MapCadMemberBase::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    const std::string str_err= "(MapCadMemberBase) Procesando comando: " + cmd;
    if(verborrea>2)
      std::clog << str_err << std::endl;
    if(cmd == "set_tag")
      {
        tag= interpretaSize_t(status.GetString()); //Nuevo identificador del punto.
        return true;
      }
    else
      return EntCmd::procesa_comando(status);
  }

//! @brief Devuelve un apuntador al objeto Cad.
const XC::Cad *XC::MapCadMemberBase::getCad(void) const
  {
    const Cad *retval= dynamic_cast<const Cad *>(Owner());
    return retval;
  }

//! @brief Devuelve un apuntador al objeto Cad.
XC::Cad *XC::MapCadMemberBase::getCad(void)
  { 
    Cad *retval= const_cast<Cad *>(dynamic_cast<const Cad *>(Owner()));
    return retval;
  }

//! @brief Devuelve un apuntador al modelador.
const XC::Preprocessor *XC::MapCadMemberBase::getPreprocessor(void) const
  { return getCad()->getPreprocessor(); }

//! @brief Devuelve un apuntador al modelador.
XC::Preprocessor *XC::MapCadMemberBase::getPreprocessor(void)
  { return getCad()->getPreprocessor(); }

//! Devuelve la propiedad del objeto cuyo código se pasa
//! como parámetro.
any_const_ptr XC::MapCadMemberBase::GetProp(const std::string &cod) const
  {
    if(cod=="tag")
      return any_const_ptr(tag);
    else if(cod=="last_tag")
      {
        tmp_gp_szt= tag-1;
        return any_const_ptr(tmp_gp_szt);
      }
    else
      return EntCmd::GetProp(cod);
  }
