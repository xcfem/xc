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
//MapEsquemas2d.cc

#include "MapEsquemas2d.h"
#include "MatrizPtrPnt.h"
#include "modelador/Modelador.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"

#include "modelador/cad/entidades/Pnt.h"
#include "modelador/set_mgmt/Set.h"
#include "xc_utils/src/base/utils_any.h"
#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/base/any_const_ptr.h"

//! @brief Constructor.
XC::MapEsquemas2d::MapEsquemas2d(Cad *cad)
  : MapCadMember<MatrizPtrPnt>(cad) {}

//! @brief Lee un objeto MapEsquemas2d desde el archivo de entrada.
bool XC::MapEsquemas2d::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    const std::string str_err= "(MapEsquemas2d) Procesando comando: " + cmd;
    if(verborrea>2)
      std::clog << str_err << std::endl;

    if(cmd == "tag_esquema2d")
      {
        setTag(interpretaSize_t(status.GetString()));
        return true;
      }
    else if(cmd == "esquema2d") //Crea una nueva rotación.
      {
        Nuevo(status);
        return true;
      }
    else
      return MapCadMember<MatrizPtrPnt>::procesa_comando(status);
  }

//! @brief Crea un nuevo punto.
XC::MatrizPtrPnt *XC::MapEsquemas2d::Crea(void)
  {
    MatrizPtrPnt *retval= new MatrizPtrPnt();
    retval->set_owner(this);
    (*this)[getTag()]= retval;
    //UpdateSets(retval);
    tag++;
    return retval;
  }

//! @brief Crea un nuevo esquema bidimensional.
XC::MatrizPtrPnt *XC::MapEsquemas2d::makeNew(void)
  {
    MatrizPtrPnt *retval= busca(getTag());
    if(!retval) //El esquema es nuevo.
      retval= Crea();
    return retval;
  }

//! @brief Crea un nuevo esquema bidimensional.
XC::MatrizPtrPnt *XC::MapEsquemas2d::Nuevo(CmdStatus &status)
  {
    std::deque<boost::any> fnc_indices= status.Parser().SeparaIndices(this);
    bool nuevo= true;
    size_t old_tag= getTag();
    MatrizPtrPnt *retval= nullptr;
    if(fnc_indices.size()>0)
      {
        setTag(convert_to_size_t(fnc_indices[0])); //Identificador del punto.
        retval= busca(getTag());
      }
    if(retval)
      nuevo= false;
    else
      retval= Crea();
    if(!nuevo)
      setTag(old_tag);
    retval->LeeCmd(status); //Lee el punto
    return retval;
  }

//! Devuelve la propiedad del objeto cuyo código se pasa
//! como parámetro.
any_const_ptr XC::MapEsquemas2d::GetProp(const std::string &cod) const
  {
    return MapCadMember<MatrizPtrPnt>::GetProp(cod);
  }
