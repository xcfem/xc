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
//MapSisRef.cc

#include "MapSisRef.h"
#include "modelador/Modelador.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"

#include "modelador/cad/SisRef.h"
#include "modelador/set_mgmt/Set.h"
#include "xc_utils/src/base/utils_any.h"
#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "SisRefCartesianas3d.h"

//! @brief Constructor.
XC::MapSisRef::MapSisRef(Cad *cad)
  : MapCadMember<SisRef>(cad) {}

//! @brief Lee un objeto MapSisRef desde el archivo de entrada.
bool XC::MapSisRef::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    const std::string str_err= "(MapSisRef) Procesando comando: " + cmd;
    if(verborrea>2)
      std::clog << str_err << std::endl;
    if(cmd == "tag_sis_ref")
      {
        setTag(interpretaSize_t(status.GetString()));
        return true;
      }
    else if(cmd == "sistema_referencia") //Crea un nuevo sistema de referencia.
      {
        Nuevo(status);
        return true;
      }
    else
      return MapCadMember<SisRef>::procesa_comando(status);
  }

//! @brief Crea un nuevo sistema de referencia.
XC::SisRef *XC::MapSisRef::Nuevo(const std::string &tipo)
  {
    SisRef *retval= busca(getTag());
    if(!retval) //El sistema de referencia es nuevo.
      {
        if(tipo == "cartesianas")
          {
            Modelador *mdlr= getModelador();
            retval= new SisRefCartesianas3d("r"+boost::lexical_cast<std::string>(getTag()),mdlr);
            (*this)[getTag()]= retval;
            tag++;
          }
        else
	  std::cerr << "MapSisRef::NuevoSisRef; el tipo de sistema de referencia: '" << tipo
                    << "' es desconocido." << std::endl;
      }
    return retval;
  }

//! @brief Lee una transformación geométrica.
XC::SisRef *XC::MapSisRef::Nuevo(CmdStatus &status)
  {
    std::deque<boost::any> fnc_indices= status.Parser().SeparaIndices(this);
    SisRef *retval= nullptr;
    bool nueva= true;
    size_t old_tag= getTag();
    if(fnc_indices.size()>0)
      {
        setTag(convert_to_size_t(fnc_indices[0])); //Identificador del sistema de referencia.
        if(existe(getTag()))
          nueva= false;
      }
    std::string tipo_sis_ref= "cartesianas";
    if(fnc_indices.size()>1)
      tipo_sis_ref= convert_to_string(fnc_indices[1]); //Identificador del tipo.
    if(getTag()>0) //El 0 se reserva para el sistema de coordenadas universal.
      {
        retval= Nuevo(tipo_sis_ref);
        retval->LeeCmd(status);
      }
    else
      std::clog << "No se puede redefinir el sistema de coordenadas de índice 0." << std::endl;
    if(!nueva)
      setTag(old_tag);
    return retval;
  }

//! Devuelve la propiedad del objeto cuyo código se pasa
//! como parámetro.
any_const_ptr XC::MapSisRef::GetProp(const std::string &cod) const
  {
    if(cod=="num_sis_ref")
      {
        tmp_gp_szt= size();
        return any_const_ptr(tmp_gp_szt);
      }
    else
      return MapCadMember<SisRef>::GetProp(cod);
  }
