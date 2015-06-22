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
//EntMdlrBase.cc

#include "EntMdlrBase.h"
#include "modelador/Modelador.h"
#include "modelador/cad/entidades/Pnt.h"
#include "modelador/cad/entidades/Edge.h"
#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "utility/actor/actor/MovableID.h"
#include "utility/actor/actor/MovableVector.h"
#include "utility/actor/actor/MovableString.h"
#include "domain/mesh/element/Element.h"
#include "domain/mesh/node/Node.h"

//! @brief Devuelve un puntero al modelador.
const XC::Modelador *XC::EntMdlrBase::get_modelador(void) const
  { return mdlr; }
//! @brief Devuelve un puntero al modelador.
XC::Modelador *XC::EntMdlrBase::get_modelador(void)
  { return mdlr; }

//! @brief Constructor.
XC::EntMdlrBase::EntMdlrBase(const std::string &nmb,Modelador *md)
  : EntConNmb(nmb), MovableObject(0),mdlr(md) {}

//! @brief Constructor de copia.
XC::EntMdlrBase::EntMdlrBase(const EntMdlrBase &otro)
  : EntConNmb(otro), MovableObject(otro), mdlr(otro.mdlr)
  {}

//! @brief Operator asignación.
XC::EntMdlrBase &XC::EntMdlrBase::operator=(const EntMdlrBase &otro)
  {
    EntConNmb::operator=(otro);
    MovableObject::operator=(otro);
    mdlr= otro.mdlr;
    return *this;
  }

//!  @brief Lee un objeto EntMdlrBase desde el archivo de entrada.
//! 
//!  Soporta los comandos:
//! 
bool XC::EntMdlrBase::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(EntMdlrBase) Procesando comando: " << cmd << std::endl;

    if(!mdlr)
      std::cerr << "¡Ojo!, el objeto: " << GetNombre() << " no tiene asignado un modelador." << std::endl;
    if(cmd=="mdlr")
      {
        if(mdlr)
          mdlr->LeeCmd(status);
        else
	  status.GetBloque(); //Ignoramos entrada.
        return true;
      }
    else if(cmd == "add_labels") //Agrega una etiqueta al objeto.
      {
        const std::vector<boost::any> &labels= interpretaVectorAny(status.GetBloque());
        for(std::vector<boost::any>::const_iterator i= labels.begin();i!=labels.end();i++)
          etiquetas.addEtiqueta(convert_to_string(*i));
        return true;
      }
    else if(cmd == "remove_labels") //Elimina una etiqueta del objeto.
      {
        const std::vector<boost::any> &labels= interpretaVectorAny(status.GetBloque());
        for(std::vector<boost::any>::const_iterator i= labels.begin();i!=labels.end();i++)
          etiquetas.removeEtiqueta(convert_to_string(*i));
        return true;
      }
    else
      return EntConNmb::procesa_comando(status);
  }

//! @brief Devuelve el código del objeto.
size_t XC::EntMdlrBase::GetTag(void) const
  {
    const std::string tmp= GetNombre().substr(1);
    return boost::lexical_cast<size_t>(tmp);
  }

//! @brief Devuelve un puntero al punto cuyo índice se pasa como parámetro.
XC::Pnt *XC::EntMdlrBase::BuscaPnt(const size_t &id_punto)
  {
    Pnt *retval= nullptr;
    if(mdlr)
      {
        Cad &cad= get_modelador()->getCad();
        retval= cad.getPuntos().busca(id_punto);
      }
    else
      std::cerr << "EntMdlrBase::BuscaPnt: el objeto: '"
                << GetNombre() << "' no tiene asignado modelador."
                << std::endl;
    return retval;
  }

//! @brief Devuelve un puntero al punto cuyo índice se pasa como parámetro
const XC::Pnt *XC::EntMdlrBase::BuscaPnt(const size_t &id_punto) const
  {
    const Pnt *retval= nullptr;
    if(mdlr)
      {
        const Cad &cad= get_modelador()->getCad();
        retval= cad.getPuntos().busca(id_punto);
      }
    else
      std::cerr << "XC::EntMdlrBase::BuscaPnt: el objeto: '" 
                << GetNombre() << "' no tiene asignado modelador."
                << std::endl;
    return retval;
  }

//! @brief Devuelve un puntero al edge cuyo índice se pasa como parámetro.
XC::Edge *XC::EntMdlrBase::BuscaEdge(const size_t &id_edge)
  {
    Edge *retval= nullptr;
    if(mdlr)
      {
        Cad &cad= get_modelador()->getCad();
        retval= cad.getLineas().busca(id_edge);
      }
    else
      std::cerr << "XC::EntMdlrBase::BuscaEdge: el objeto: '"
                << GetNombre() << "' no tiene asignado modelador."
                << std::endl;
    return retval;
  }

//! @brief Devuelve un puntero al edge cuyo índice se pasa como parámetro
const XC::Edge *XC::EntMdlrBase::BuscaEdge(const size_t &id_edge) const
  {
    const Edge *retval= nullptr;
    if(mdlr)
      {
        const Cad &cad= get_modelador()->getCad();
        retval= cad.getLineas().busca(id_edge);
      }
    else
      std::cerr << "EntMdlrBase::BuscaEdge: el objeto: '" 
                << GetNombre() << "' no tiene asignado modelador."
                << std::endl;
    return retval;
  }

//! @brief Devuelve un puntero al edge cuyo índice se pasa como parámetro.
XC::Face *XC::EntMdlrBase::BuscaFace(const size_t &id_face)
  {
    Face *retval= nullptr;
    if(mdlr)
      {
        Cad &cad= get_modelador()->getCad();
        retval= cad.getSuperficies().busca(id_face);
      }
    else
      std::cerr << "XC::EntMdlrBase::BuscaFace: el objeto: '"
                << GetNombre() << "' no tiene asignado modelador."
                << std::endl;
    return retval;
  }

//! @brief Devuelve un puntero al edge cuyo índice se pasa como parámetro
const XC::Face *XC::EntMdlrBase::BuscaFace(const size_t &id_face) const
  {
    const Face *retval= nullptr;
    if(mdlr)
      {
        const Cad &cad= get_modelador()->getCad();
        retval= cad.getSuperficies().busca(id_face);
      }
    else
      std::cerr << "EntMdlrBase::BuscaFace: el objeto: '" 
                << GetNombre() << "' no tiene asignado modelador."
                << std::endl;
    return retval;
  }

//! @brief Envía las etiquetas por el canal que se pasa como parámetro.
int XC::EntMdlrBase::sendIdsEtiquetas(const int &posSize,const int &posDbTag, CommParameters &cp)
  {
    int res= 0;
    static ID etiqIds;
    const std::set<int> ids= etiquetas.getIdsEtiquetas();
    const size_t sz= ids.size();
    setDbTagDataPos(posSize,sz);
    if(sz>0)
      {
        etiqIds.resize(sz);
        int conta= 0;
        for(std::set<int>::const_iterator i= ids.begin();i!=ids.end();i++,conta++)
          etiqIds[conta]= *i;
        res+= cp.sendID(etiqIds,getDbTagData(),CommMetaData(posDbTag));
      }
    else
      cp.sendInt(0,getDbTagData(),CommMetaData(posDbTag));
    return res;
  }

//! @brief Recibe las etiquetas por el canal que se pasa como parámetro.
int XC::EntMdlrBase::recvIdsEtiquetas(const int &posSize,const int &posDbTag,const CommParameters &cp)
  {
    int res= 0;
    static ID etiqIds;
    const size_t sz= getDbTagDataPos(posSize);
    if(sz>0)
      {
        const DiccionarioEtiquetas &dic= etiquetas.getDiccionario();
        res= cp.receiveID(etiqIds,getDbTagData(),CommMetaData(posDbTag));
    
        const size_t sz= etiqIds.Size();
        for(size_t i=0;i<sz;i++)
          etiquetas.addEtiqueta(dic(etiqIds[i]));
      }
    return res;
  }

//! @brief Envía los miembros a través del canal que se pasa como parámetro.
int XC::EntMdlrBase::sendData(CommParameters &cp)
  {
    int res= cp.sendString(GetNombre(),getDbTagData(),CommMetaData(0));
    res+= sendIdsEtiquetas(1,2,cp);
    return res;
  }

//! @brief Recibe los miembros a través del canal que se pasa como parámetro.
int XC::EntMdlrBase::recvData(const CommParameters &cp)
  {
    std::string tmp;
    int res= cp.receiveString(tmp,getDbTagData(),CommMetaData(0));
    Nombre()= tmp;
    res+= recvIdsEtiquetas(1,2,cp);
    return res;
  }

//! @brief Envía el objeto por el canal que se pasa como parámetro.
int XC::EntMdlrBase::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(3);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << nombre_clase() << "::sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Recibe el objeto a través del canal que se pasa como parámetro.
int XC::EntMdlrBase::recvSelf(const CommParameters &cp)
  {
    inicComm(3);
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

//! Devuelve la propiedad del objeto cuyo código se pasa
//! como parámetro.
any_const_ptr XC::EntMdlrBase::GetProp(const std::string &cod) const
  {
    if(verborrea>4)
      std::clog << "EntMdlrBase::GetProp (" << nombre_clase() << "::GetProp) Buscando propiedad: " << cod << std::endl;
    if(cod=="tag")
      {
        tmp_gp_szt= GetTag();
        return any_const_ptr(tmp_gp_szt);
      }
    else if(cod=="hasLabel")
      {
        const std::string label= popString(cod);
        tmp_gp_bool= etiquetas.hasEtiqueta(label);
        return any_const_ptr(tmp_gp_bool);
      }
    else if(cod=="getNumEtiquetas")
      {
        tmp_gp_szt= etiquetas.getNumEtiquetas();
        return any_const_ptr(tmp_gp_szt);
      }
    else
      return EntConNmb::GetProp(cod);
  }

