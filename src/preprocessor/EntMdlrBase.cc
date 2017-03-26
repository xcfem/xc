//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
//  XC is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//EntMdlrBase.cc

#include "EntMdlrBase.h"
#include "preprocessor/Preprocessor.h"
#include "preprocessor/cad/entidades/Pnt.h"
#include "preprocessor/cad/entidades/Edge.h"

#include "utility/actor/actor/MovableID.h"
#include "utility/actor/actor/MovableVector.h"
#include "utility/actor/actor/MovableString.h"
#include "domain/mesh/element/Element.h"
#include "domain/mesh/node/Node.h"

//! @brief Returns a pointer to the preprocesador.
const XC::Preprocessor *XC::EntMdlrBase::get_preprocessor(void) const
  { return preprocessor; }
//! @brief Returns a pointer to the preprocesador.
XC::Preprocessor *XC::EntMdlrBase::get_preprocessor(void)
  { return preprocessor; }

//! @brief Constructor.
XC::EntMdlrBase::EntMdlrBase(const std::string &nmb,Preprocessor *md)
  : EntConNmb(nmb), MovableObject(0),preprocessor(md) {}

//! @brief Copy constructor.
XC::EntMdlrBase::EntMdlrBase(const EntMdlrBase &otro)
  : EntConNmb(otro), MovableObject(otro), preprocessor(otro.preprocessor)
  {}

//! @brief Assignment operator.
XC::EntMdlrBase &XC::EntMdlrBase::operator=(const EntMdlrBase &otro)
  {
    EntConNmb::operator=(otro);
    MovableObject::operator=(otro);
    preprocessor= otro.preprocessor;
    return *this;
  }

//! @brief Returns the object identifier in the model (tag).
size_t XC::EntMdlrBase::GetTag(void) const
  {
    const std::string tmp= GetNombre().substr(1);
    return boost::lexical_cast<size_t>(tmp);
  }

//! @brief Checks for preprocessor.
bool XC::EntMdlrBase::check_preprocessor(void) const
  {
    if(preprocessor)
      return true;
    else
      {
        std::cerr << nombre_clase() << __FUNCTION__
	          << ": preprocessor not set for object: '"
                  << GetNombre() << "'."
                  << std::endl;
        return false;
      }
  }

//! @brief Returns a pointer to the point identified by
//! the tag being passed as parameter.
XC::Pnt *XC::EntMdlrBase::BuscaPnt(const size_t &id_punto)
  {
    Pnt *retval= nullptr;
    if(check_preprocessor())
      {
        Cad &cad= get_preprocessor()->getCad();
        retval= cad.getPuntos().busca(id_punto);
      }
    return retval;
  }

//! @brief Returns a const pointer to the point identified by
//! the tag being passed as parameter.
const XC::Pnt *XC::EntMdlrBase::BuscaPnt(const size_t &id_punto) const
  {
    const Pnt *retval= nullptr;
    if(check_preprocessor())
      {
        const Cad &cad= get_preprocessor()->getCad();
        retval= cad.getPuntos().busca(id_punto);
      }
    return retval;
  }

//! @brief Returns a pointer to the edge identified by
//! the tag being passed as parameter.
XC::Edge *XC::EntMdlrBase::BuscaEdge(const size_t &id_edge)
  {
    Edge *retval= nullptr;
    if(check_preprocessor())
      {
        Cad &cad= get_preprocessor()->getCad();
        retval= cad.getLineas().busca(id_edge);
      }
    return retval;
  }

//! @brief Returns a pointer to the edge identified by
//! the tag being passed as parameter.
const XC::Edge *XC::EntMdlrBase::BuscaEdge(const size_t &id_edge) const
  {
    const Edge *retval= nullptr;
    if(check_preprocessor())
      {
        const Cad &cad= get_preprocessor()->getCad();
        retval= cad.getLineas().busca(id_edge);
      }
    return retval;
  }

//! @brief Returns a pointer to the face identified by
//! the tag being passed as parameter.
XC::Face *XC::EntMdlrBase::BuscaFace(const size_t &id_face)
  {
    Face *retval= nullptr;
    if(check_preprocessor())
      {
        Cad &cad= get_preprocessor()->getCad();
        retval= cad.getSurfaces().busca(id_face);
      }
    return retval;
  }

//! @brief Returns a pointer to the face identified by
//! the tag being passed as parameter.
const XC::Face *XC::EntMdlrBase::BuscaFace(const size_t &id_face) const
  {
    const Face *retval= nullptr;
    if(check_preprocessor())
      {
        const Cad &cad= get_preprocessor()->getCad();
        retval= cad.getSurfaces().busca(id_face);
      }
    return retval;
  }

//! @brief Send labels through the channel being passed as parameter.
int XC::EntMdlrBase::sendIdsEtiquetas(const int &posSize,const int &posDbTag, CommParameters &cp)
  {
    int res= 0;
    static ID etiqIds;
    const std::set<int> ids= labels.getIdsEtiquetas();
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

//! @brief Receive labels through the channel being passed as parameter.
int XC::EntMdlrBase::recvIdsEtiquetas(const int &posSize,const int &posDbTag,const CommParameters &cp)
  {
    int res= 0;
    static ID etiqIds;
    const size_t sz= getDbTagDataPos(posSize);
    if(sz>0)
      {
        const DiccionarioEtiquetas &dic= labels.getDiccionario();
        res= cp.receiveID(etiqIds,getDbTagData(),CommMetaData(posDbTag));
    
        const size_t sz= etiqIds.Size();
        for(size_t i=0;i<sz;i++)
          labels.addEtiqueta(dic(etiqIds[i]));
      }
    return res;
  }

//! @brief Send members through the channel being passed as parameter.
int XC::EntMdlrBase::sendData(CommParameters &cp)
  {
    int res= cp.sendString(GetNombre(),getDbTagData(),CommMetaData(0));
    res+= sendIdsEtiquetas(1,2,cp);
    return res;
  }

//! @brief Receives members through the channel being passed as parameter.
int XC::EntMdlrBase::recvData(const CommParameters &cp)
  {
    std::string tmp;
    int res= cp.receiveString(tmp,getDbTagData(),CommMetaData(0));
    Nombre()= tmp;
    res+= recvIdsEtiquetas(1,2,cp);
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
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

//! @brief Receives object through the channel being passed as parameter.
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

