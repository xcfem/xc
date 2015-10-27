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
//MapFields.cc

#include "MapFields.h"
#include "xc_utils/src/base/CmdStatus.h"
#include "preprocessor/Preprocessor.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "xc_utils/src/base/utils_any.h"
#include "domain/mesh/element/Element.h"
#include "domain/mesh/node/Node.h"


//! @brief Devuelve verdadero si el conjunto existe.
bool XC::MapFields::existe(const std::string &nmb) const
  {
    const_iterator i= buscaField(nmb);
    return (i!=end());
  }

//! @brief Devuelve un puntero al conjunto cuyo nombre se pasa como parámetro.
XC::MapFields::iterator XC::MapFields::buscaField(const std::string &nmb)
  {
    iterator i=begin();
    for(;i!=end();i++)
      if(i->GetNombre()==nmb) break;
    return i;
  }

//! @brief Devuelve un puntero al conjunto cuyo nombre se pasa como parámetro.
XC::MapFields::const_iterator XC::MapFields::buscaField(const std::string &nmb) const
  {
    const_iterator i=begin();
    for(;i!=end();i++)
      if(i->GetNombre()==nmb) break;
    return i;
  }

//! @brief Constructor por defecto.
XC::MapFields::MapFields(Preprocessor *m)
  : EntCmd(m), MovableObject(0), preprocessor(m) {}

//! @bred Defines a new field
XC::FieldInfo &XC::MapFields::newField(const std::string &nmb_field)
  {
    FieldInfo fi(nmb_field);
    fi.set_owner(this);
    push_back(fi);
    return back();
  }

//! @brief Lee un objeto MapFields desde archivo
bool XC::MapFields::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    const std::string msg_proc_cmd= "(MapFields) Procesando comando: " + cmd;
    if(verborrea>2)
      std::clog << msg_proc_cmd << std::endl;
    if(cmd == "def_field" || (cmd == "fields")) //Definición de un conjunto.
      {
	std::string nmb_field= "";
        std::deque<boost::any> fnc_indices= status.Parser().SeparaIndices(this);
        if(fnc_indices.size()>0)
          nmb_field= convert_to_string(fnc_indices[0]); //Nombre del campo.
        FieldInfo fi= newField(nmb_field);
        fi.LeeCmd(status);
        return true;
      }
    else if(cmd == "for_each")
      {
        const std::string nmbBlq= nombre_clase()+":for_each";
        const std::string bloque= status.GetBloque();
        for(iterator i= begin();i!=end();i++)
          (*i).EjecutaBloque(status,bloque,nmbBlq);
        return true;
      }
    iterator field_itr= buscaField(cmd);
    if(field_itr!=end())
      {
        field_itr->LeeCmd(status);
        return true;
      }
    return EntCmd::procesa_comando(status);
  }

//! @brief Borra todo.
void XC::MapFields::clearAll(void)
  { clear(); }

//! @brief Envía los miembros a través del canal que se pasa como parámetro.
int XC::MapFields::sendData(CommParameters &cp)
  {
    std::cerr << "MapFields::sendData no se ha implementado." << std::endl;
    return 0;
  }

//! @brief Recibe los miembros a través del canal que se pasa como parámetro.
int XC::MapFields::recvData(const CommParameters &cp)
  {
    std::cerr << "MapFields::recvData no se ha implementado." << std::endl;
    return 0;
  }

//! @brief Envía el objeto por el canal que se pasa como parámetro.
int XC::MapFields::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(6);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr <<  nombre_clase() << "sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Recibe el objeto a través del canal que se pasa como parámetro.
int XC::MapFields::recvSelf(const CommParameters &cp)
  {
    inicComm(6);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr <<  nombre_clase() << "recvSelf - failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
          std::cerr <<  nombre_clase() << "recvSelf - failed to receive data.\n";
      }
    return res;
  }

//! \brief Devuelve la propiedad del objeto cuyo código (de la propiedad) se pasa
//! como parámetro.
//!
//! Soporta los códigos:
any_const_ptr XC::MapFields::GetProp(const std::string &cod) const
  {
    const_iterator field_itr= buscaField(cod);
    if(field_itr!=end())
      return any_const_ptr(&(*field_itr));
    else
      return EntCmd::GetProp(cod);
  }
