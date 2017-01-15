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
//Set.cc

#include "Set.h"
#include "domain/domain/Domain.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"
#include "preprocessor/Preprocessor.h"
#include "preprocessor/cad/entidades/Pnt.h"
#include "preprocessor/cad/entidades/Edge.h"
#include "preprocessor/cad/entidades/Face.h"
#include "preprocessor/cad/entidades/Body.h"
#include "preprocessor/cad/entidades/UniformGrid.h"
#include "preprocessor/cad/matrices/TritrizPtrElem.h"
#include "preprocessor/cad/trf/TrfGeom.h"
#include "utility/matrix/ID.h"

#include "xc_utils/src/geom/pos_vec/SVD3d.h"
#include "xc_utils/src/geom/d2/Plano3d.h"
#include "xc_utils/src/geom/d3/SemiEspacio3d.h"


//! @brief Constructor.
XC::Set::Set(const std::string &nmb,Preprocessor *md)
  : SetMeshComp(nmb,md), puntos(this), lineas(this), superficies(this),
    cuerpos(this), uniform_grids(this) {}

//! @brief Constructor de copia.
XC::Set::Set(const Set &otro)
  : SetMeshComp(otro)
  {
    copia_listas(otro);
  }

//! @brief Operator asignación.
XC::Set &XC::Set::operator=(const Set &otro)
  {
    SetMeshComp::operator=(otro);
    copia_listas(otro);
    return *this;
  }

//! @brief Copia las listas de objetos del conjunto s.
//!
//! Copia en ESTE objeto las listas de objetos del conjunto
//! being passed as parameter.
void XC::Set::copia_listas(const Set &otro)
  {
    SetMeshComp::copia_listas(otro);
    puntos= otro.puntos;
    puntos.set_owner(this);
    lineas= otro.lineas;
    lineas.set_owner(this);
    superficies= otro.superficies;
    superficies.set_owner(this);
    cuerpos= otro.cuerpos;
    cuerpos.set_owner(this);
    uniform_grids= otro.uniform_grids;
    uniform_grids.set_owner(this);
  }

//! @brief Agrega a este conjunto los objetos del conjunto
//! being passed as parameter.
void XC::Set::agrega_listas(const Set &otro)
  {
    SetMeshComp::agrega_listas(otro);
    puntos.agrega(otro.puntos);
    lineas.agrega(otro.lineas);
    superficies.agrega(otro.superficies);
    cuerpos.agrega(otro.cuerpos);
    uniform_grids.agrega(otro.uniform_grids);
  }

// //! @brief Agrega a este conjunto los objetos del conjunto
// //! being passed as parameter que cumplen la condición.
// void XC::Set::agrega_listas_cond(const Set &otro,const std::string &cond)
//   {
//     SetMeshComp::agrega_listas_cond(otro,cond);
//     puntos.agrega_cond(otro.puntos,cond);
//     lineas.agrega_cond(otro.lineas,cond);
//     superficies.agrega_cond(otro.superficies,cond);
//     cuerpos.agrega_cond(otro.cuerpos,cond);
//     uniform_grids.agrega_cond(otro.uniform_grids,cond);
//   }

//! @brief Vacía las listas del conjunto.
void XC::Set::clear(void)
  {
    SetMeshComp::clear();
    puntos.clear();
    lineas.clear();
    superficies.clear();
    cuerpos.clear();
    uniform_grids.clear();
  }

//!  \brief Borra todos los objetos del conjunto.
void XC::Set::clearAll(void)
  {
    SetMeshComp::clearAll();
    puntos.clearAll();
    lineas.clearAll();
    superficies.clearAll();
    cuerpos.clearAll();
    uniform_grids.clearAll();
  }

//! @brief Asigna índices a los objetos del conjunto (nodos,elementos,puntos...) poder emplearlos en VTK.
void XC::Set::numera(void)
  {
    SetMeshComp::numera();
    SetMeshComp::numera_lista(puntos);
    SetMeshComp::numera_lista(lineas);
    SetMeshComp::numera_lista(superficies);
//     numera_lista(cuerpos);
  }

//! @brief Desplaza los elementos del conjunto.
void XC::Set::mueve(const Vector3d &desplaz)
  {
    for(lst_ptr_puntos::iterator i= puntos.begin();i!=puntos.end();i++)
      (*i)->Mueve(desplaz);
    SetMeshComp::mueve(desplaz);
  }

//! @brief Aplica la transformación a los elementos del conjunto.
void XC::Set::Transforma(const TrfGeom &trf)
  {
    for(lst_ptr_puntos::iterator i= puntos.begin();i!=puntos.end();i++)
      (*i)->Transforma(trf);
    SetMeshComp::Transforma(trf);
  }

//! @brief Aplica al conjunto la transformación cuyo índice se pasa como parámetro.
void XC::Set::Transforma(const size_t &indice_trf)
  {
    TrfGeom *trf= get_preprocessor()->getCad().getTransformacionesGeometricas().busca(indice_trf);
    if(trf)
      Transforma(*trf);
  }

//! @brief Crea una copia de los elementos del conjunto y los mete en otro
//! cuyo nombre se pasa como parámetro.Las coordenadas de los
//! nuevos puntos serán las que resulten de sumar a las del primitivo el
//! vector being passed as parameter.
void XC::Set::crea_copia(const std::string &nombre,const Vector3d &v= Vector3d())
  {
    Preprocessor *preprocessor= get_preprocessor();
    if(!preprocessor)
      {
        std::cerr << "Set::crea_copia; preprocessor not assigned." << std::endl;
	return;
      }
    Set *nuevo_set= get_preprocessor()->get_sets().crea_set(nombre);
    std::clog << "Set::crea_copia; ¡ojo! no se ha implementado la copia"
              << " de nodos ni de elementos." << std::endl;
    //Copiamos los puntos.
    std::map<std::string,std::string> nombres_nuevos_puntos;
    for(lst_ptr_puntos::iterator i= puntos.begin();i!=puntos.end();i++)
      {
        const std::string nombre_viejo= (*i)->GetNombre();
        const std::string nombre_nuevo= nombre+nombre_viejo;
        Pnt *nuevo_punto= get_preprocessor()->getCad().getPuntos().Copia(*i,v);
        nuevo_punto->BorraPtrNodElem();
        nuevo_set->puntos.push_back(nuevo_punto);
        nombres_nuevos_puntos[nombre_viejo]= nombre_nuevo;
      }
    //Copiamos las líneas.
    std::map<std::string,std::string> nombres_nuevos_lineas;
    for(lst_ptr_lineas::iterator i= lineas.begin();i!=lineas.end();i++)
      {
        const std::string nombre_viejo= (*i)->GetNombre();
        const std::string nombre_nuevo= nombre+nombre_viejo;
        Edge *nuevo_edge= get_preprocessor()->getCad().getLineas().Copia(*i);
        nuevo_edge->BorraPtrNodElem();
        nuevo_set->lineas.push_back(nuevo_edge);
        nombres_nuevos_lineas[nombre_viejo]= nombre_nuevo;
        const size_t nv= nuevo_edge->NumVertices();
        for(size_t i= 0;i<nv;i++)
          {
            const Pnt *vertice_viejo= nuevo_edge->GetVertice(i);
            const std::string nombre_viejo= vertice_viejo->GetNombre();
            const std::string nombre_nuevo= nombres_nuevos_lineas[nombre_viejo];
            Pnt *nuevo_punto= nuevo_set->puntos.BuscaNmb(nombre_nuevo);
            nuevo_edge->SetVertice(i,nuevo_punto);
	  }
      }
  }

//! @brief Crea nodos y, en su caso, elementos en los puntos del conjunto.
void XC::Set::malla_puntos(dir_mallado dm)
  {
    if(verborrea>2)
      std::clog << "Mallando puntos...";
    for(lst_ptr_puntos::iterator i= puntos.begin();i!=puntos.end();i++)
      (*i)->Malla(dm);
    if(verborrea>2)
      std::clog << "hecho." << std::endl;
  }

//! @brief Crea nodos y, en su caso, elementos en las líneas del conjunto.
void XC::Set::malla_lineas(dir_mallado dm)
  {
    if(verborrea>2)
      std::clog << "Mallando lineas...";
    for(lst_ptr_lineas::iterator i= lineas.begin();i!=lineas.end();i++)
      (*i)->Malla(dm);
    if(verborrea>2)
      std::clog << "hecho." << std::endl;
  }

//! @brief Crea nodos y, en su caso, elementos en las superficies del conjunto.
void XC::Set::malla_superficies(dir_mallado dm)
  {
    if(verborrea>2)
      std::clog << "Mallando superficies...";
    for(lst_ptr_superficies::iterator i= superficies.begin();i!=superficies.end();i++)
      (*i)->Malla(dm);
    if(verborrea>2)
      std::clog << "hecho." << std::endl;
  }

//! @brief Crea nodos y, en su caso, elementos en los cuerpos del conjunto.
void XC::Set::malla_cuerpos(dir_mallado dm)
  {
    if(verborrea>2)
      std::clog << "Mallando cuerpos...";
    for(lst_ptr_cuerpos::iterator i= cuerpos.begin();i!=cuerpos.end();i++)
      (*i)->Malla(dm);
    if(verborrea>2)
      std::clog << "hecho." << std::endl;
  }

//! @brief Crea nodos y, en su caso, elementos en los puntos del conjunto.
void XC::Set::malla_uniform_grids(dir_mallado dm)
  {
    if(verborrea>2)
      std::clog << "Mallando uniform grids...";
    for(lst_ptr_uniform_grids::iterator i= uniform_grids.begin();i!=uniform_grids.end();i++)
      (*i)->Malla(dm);
    if(verborrea>2)
      std::clog << "hecho." << std::endl;
  }

//!  \brief Genera una malla a partir de los objetos del conjunto.
//!
//! @param nodo_semilla: Nodo a copiar en las posiciones de la malla.
//! @param elemento_semilla: Elemento a copiar en las posiciones de la malla.
void XC::Set::Malla(dir_mallado dm)
  {
    Preprocessor *mdl= get_preprocessor();
    assert(mdl);
    mdl->get_sets().abre_set(GetNombre()); //Para que nodos y elementos entren en ESTE conjunto.

    if(verborrea>1)
      std::clog << "Iniciando mallado conjunto (" << GetNombre() << ")...";

    //malla_cuerpos(dm);
    malla_superficies(dm);
    malla_lineas(dm);
    malla_puntos(dm);

    malla_uniform_grids(dm);

    mdl->get_sets().cierra_set(GetNombre()); //Cerramos.

    if(verborrea>1)
      std::clog << "hecho." << std::endl;
  }

//! @brief Devuelve verdadero si el punto pertenece al conjunto.
bool XC::Set::In(const Pnt *p) const
  { return puntos.in(p); }

//! @brief Devuelve verdadero si el «edge» pertenece al conjunto.
bool XC::Set::In(const Edge *e) const
  { return lineas.in(e); }

//! @brief Devuelve verdadero si la superficie pertenece al conjunto.
bool XC::Set::In(const Face *f) const
  { return superficies.in(f); }

//! @brief Devuelve verdadero si el cuerpo pertenece al conjunto.
bool XC::Set::In(const Body *b) const
  { return cuerpos.in(b); }

//! @brief Devuelve verdadero si la «uniform grid» pertenece al conjunto.
bool XC::Set::In(const UniformGrid *ug) const
  { return uniform_grids.in(ug); }

//! @brief Agrega al conjunto being passed as parameter
//! los elementos que intervienen en la
//! definición de los que ya están en el conjunto.
void XC::Set::CompletaHaciaAbajo(void)
  {
//     for(lst_cuerpos::iterator i=cuerpos.begin();i!=cuerpos.end();i++)
//       {
//         lst_superficies ss= (*i)->GetSuperficies();
//         superficies.insert(superficies.end(),ss.begin(),ss.end());
//       }
    for(sup_iterator i=superficies.begin();i!=superficies.end();i++)
      {
        //Lineas.
        lst_ptr_lineas ll((*i)->GetEdges());
        lineas.insert(lineas.end(),ll.begin(),ll.end());

        //Elementos.
        TritrizPtrElem &ttz_elementos= (*i)->GetTtzElementos();
        const size_t ncapas= ttz_elementos.GetCapas();
        const size_t nfilas= ttz_elementos.getNumFilas();
        const size_t ncols= ttz_elementos.getNumCols();
        for(size_t i=1;i<=ncapas;i++)
          for(size_t j=1;j<=nfilas;j++)
            for(size_t k=1;k<=ncols;k++)
              agregaElemento(ttz_elementos(i,j,k));

      }
    for(lin_iterator i=lineas.begin();i!=lineas.end();i++)
      {
        //Puntos.
        const size_t nv= (*i)->NumVertices();
        for(register size_t j=1;j<=nv;j++)
          puntos.push_back(const_cast<Pnt *>((*i)->GetVertice(j)));

        //Elementos.
        TritrizPtrElem &ttz_elementos= (*i)->GetTtzElementos();
        const size_t ncapas= ttz_elementos.GetCapas();
        const size_t nfilas= ttz_elementos.getNumFilas();
        const size_t ncols= ttz_elementos.getNumCols();
        for(size_t i=1;i<=ncapas;i++)
          for(size_t j=1;j<=nfilas;j++)
            for(size_t k=1;k<=ncols;k++)
              agregaElemento(ttz_elementos(i,j,k));
      }
    SetMeshComp::CompletaHaciaAbajo();
  }

//! @brief  Agrega a este conjunto los objetos que hacen referencia
//! a uno o más de los que ya están en el conjunto.
void XC::Set::CompletaHaciaArriba(void)
  {
    SetMeshComp::CompletaHaciaArriba();
    std::cerr << "Set::CompletaHaciaArriba() está sin terminar." << std::endl;
    for(pnt_iterator i=puntos.begin();i!=puntos.end();i++)
      {
        std::set<const Edge *> ll= GetLineasTocan(**i);
        for(std::set<const Edge *>::const_iterator j= ll.begin();j!=ll.end();j++)
          lineas.push_back(const_cast<Edge *>(*j));
      }
    for(lin_iterator i=lineas.begin();i!=lineas.end();i++)
      {
        lst_ptr_superficies ss(GetSupsTocan(**i));
        superficies.insert(superficies.end(),ss.begin(),ss.end());
      }
//     for(lst_superficies::iterator i=superficies.begin();i!=superficies.end();i++)
//       {
//         lst_cuerpos bb= GetCuerposTocan(**i);
//         cuerpos.insert(cuerpos.end(),bb.begin(),bb.end());
//       }
  }

//! @brief Selecciona los puntos cuyos tags se pasan como parámetro.
void XC::Set::sel_puntos_lista(const ID &tags)
  {
    const size_t sz= tags.Size();
    if(sz>0)
      {
        Preprocessor *preprocessor= get_preprocessor();
        if(preprocessor)
          {
            Cad &cad= get_preprocessor()->getCad();
            for(size_t i= 0;i<sz;i++)
              {
	        Pnt *ipt= cad.getPuntos().busca(tags(i)); 
                if(ipt)
                  puntos.push_back(ipt);
                else
		  std::cerr << "No se encontró el punto de tag: " << tags(i) << std::endl;
              }
          }
        else
          std::cerr << "Set::sel_puntos_lista; necesito un preprocesador." << std::endl;
      }
  }

//! @brief Selecciona las lineas cuyos tags se pasan como parámetro.
void XC::Set::sel_lineas_lista(const ID &tags)
  {
    const size_t sz= tags.Size();
    if(sz>0)
      {
        Preprocessor *preprocessor= get_preprocessor();
        if(preprocessor)
          {
            Cad &cad= get_preprocessor()->getCad();
            for(size_t i= 0;i<sz;i++)
              {
	        Edge *iedge= cad.getLineas().busca(tags(i)); 
                if(iedge)
                  lineas.push_back(iedge);
                else
		  std::cerr << "No se encontró la linea de tag: " << tags(i) << std::endl;
              }
          }
        else
          std::cerr << "Set::sel_lineas_lista; necesito un preprocesador." << std::endl;
      }
  }

//! @brief Selecciona las superficies cuyos tags se pasan como parámetro.
void XC::Set::sel_superficies_lista(const ID &tags)
  {
    const size_t sz= tags.Size();
    if(sz>0)
      {
        Preprocessor *preprocessor= get_preprocessor();
        if(preprocessor)
          {
            Cad &cad= get_preprocessor()->getCad();
            for(size_t i= 0;i<sz;i++)
              {
	        Face *iface= cad.getSuperficies().busca(tags(i)); 
                if(iface)
                  superficies.push_back(iface);
                else
		  std::cerr << "No se encontró la superficie de tag: " << tags(i) << std::endl;
              }
          }
        else
          std::cerr << "Set::sel_elementos_lista; necesito un preprocesador." << std::endl;
      }
  }

//! @brief Devuelve un vector para almacenar los dbTags
//! de los miembros de la clase.
XC::DbTagData &XC::Set::getDbTagData(void) const
  {
    static DbTagData retval(19);
    return retval;
  }

//! @brief Send members through the channel being passed as parameter.
int XC::Set::sendData(CommParameters &cp)
  {
    int res= SetMeshComp::sendData(cp);
//     res+= puntos.sendTags(9,10,getDbTagData(),cp);
//     res+= lineas.sendTags(11,12,getDbTagData(),cp);
//     res+= superficies.sendTags(13,14,getDbTagData(),cp);
//     res+= cuerpos.sendTags(15,16,getDbTagData(),cp);
//     res+= uniform_grids.sendTags(17,18,getDbTagData(),cp);
    return res;
  }

//! @brief Receives members through the channel being passed as parameter.
int XC::Set::recvData(const CommParameters &cp)
  {
    ID tmp;
    int res= SetMeshComp::recvData(cp);
//     tmp= puntos.receiveTags(9,10,getDbTagData(),cp);
//     sel_puntos_lista(tmp);
//     tmp= lineas.receiveTags(11,12,getDbTagData(),cp);
//     sel_lineas_lista(tmp);
//     tmp= superficies.receiveTags(13,14,getDbTagData(),cp);
//     sel_superficies_lista(tmp);
//     tmp= cuerpos.receiveTags(15,16,getDbTagData(),cp);
//     sel_cuerpos_lista(tmp);
//     tmp= uniform_grids.receiveTags(17,18,getDbTagData(),cp);
    return res;
  }

//!  \brief Destructor.
XC::Set::~Set(void)
  { clearAll(); }

//! @brief Sends object through the channel being passed as parameter.
int XC::Set::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(19);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << nombre_clase() << "::sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::Set::recvSelf(const CommParameters &cp)
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
