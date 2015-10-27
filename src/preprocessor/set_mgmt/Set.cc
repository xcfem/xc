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
#include "xc_utils/src/base/CmdStatus.h"
#include "utility/matrix/ID.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "xc_utils/src/geom/pos_vec/SVD3d.h"
#include "xc_utils/src/geom/d2/Plano3d.h"
#include "xc_utils/src/geom/d3/SemiEspacio3d.h"


//! @brief Constructor.
XC::Set::Set(const std::string &nmb,Preprocessor *md)
  : SetMeshComp(nmb,md) {}

//! @brief Constructor de copia.
XC::Set::Set(const Set &otro)
  : SetMeshComp(otro)
  {
    puntos= otro.puntos;
    lineas= otro.lineas;
    superficies= otro.superficies;
    cuerpos= otro.cuerpos;
    uniform_grids= otro.uniform_grids;
  }

//! @brief Operator asignación.
XC::Set &XC::Set::operator=(const Set &otro)
  {
    SetMeshComp::operator=(otro);
    puntos= otro.puntos;
    lineas= otro.lineas;
    superficies= otro.superficies;
    cuerpos= otro.cuerpos;
    uniform_grids= otro.uniform_grids;
    return *this;
  }

//! @brief Copia las listas de objetos del conjunto s.
//!
//! Copia en ESTE objeto las listas de objetos del conjunto
//! que se pasa como parámetro.
void XC::Set::copia_listas(const Set &otro)
  {
    SetMeshComp::copia_listas(otro);
    puntos= otro.puntos;
    lineas= otro.lineas;
    superficies= otro.superficies;
    cuerpos= otro.cuerpos;
    uniform_grids= otro.uniform_grids;
  }

//! @brief Agrega a este conjunto los objetos del conjunto
//! que se pasa como parámetro.
void XC::Set::agrega_listas(const Set &otro)
  {
    SetMeshComp::agrega_listas(otro);
    puntos.agrega(otro.puntos);
    lineas.agrega(otro.lineas);
    superficies.agrega(otro.superficies);
    cuerpos.agrega(otro.cuerpos);
    uniform_grids.agrega(otro.uniform_grids);
  }

//! @brief Agrega a este conjunto los objetos del conjunto
//! que se pasa como parámetro que cumplen la condición.
void XC::Set::agrega_listas_cond(const Set &otro,const std::string &cond)
  {
    SetMeshComp::agrega_listas_cond(otro,cond);
    puntos.agrega_cond(otro.puntos,cond);
    lineas.agrega_cond(otro.lineas,cond);
    superficies.agrega_cond(otro.superficies,cond);
    cuerpos.agrega_cond(otro.cuerpos,cond);
    uniform_grids.agrega_cond(otro.uniform_grids,cond);
  }

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
void XC::Set::mueve(const std::vector<ExprAlgebra> &desplaz)
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
    TrfGeom *trf= get_modelador()->getCad().getTransformacionesGeometricas().busca(indice_trf);
    if(trf)
      Transforma(*trf);
  }

//! @brief Crea una copia de los elementos del conjunto y los mete en otro
//! cuyo nombre se pasa como parámetro.Las coordenadas de los
//! nuevos puntos serán las que resulten de sumar a las del primitivo el
//! vector que se pasa como parámetro.
void XC::Set::crea_copia(const std::string &nombre,const Vector3d &v= Vector3d())
  {
    Preprocessor *preprocessor= get_modelador();
    if(!preprocessor)
      {
        std::cerr << "Set::crea_copia; no está asignado el modelador." << std::endl;
	return;
      }
    Set *nuevo_set= get_modelador()->get_sets().crea_set(nombre);
    std::clog << "Set::crea_copia; ¡ojo! no se ha implementado la copia"
              << " de nodos ni de elementos." << std::endl;
    //Copiamos los puntos.
    std::map<std::string,std::string> nombres_nuevos_puntos;
    for(lst_ptr_puntos::iterator i= puntos.begin();i!=puntos.end();i++)
      {
        const std::string nombre_viejo= (*i)->GetNombre();
        const std::string nombre_nuevo= nombre+nombre_viejo;
        Pnt *nuevo_punto= get_modelador()->getCad().getPuntos().Copia(*i,v);
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
        Edge *nuevo_edge= get_modelador()->getCad().getLineas().Copia(*i);
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

//!  @brief Lee un objeto Set desde el archivo de entrada.
//! 
//!  Soporta los comandos:
//! 
//!  - sel_pnt: Inserta en el conjunto los puntos cuyos identificadores se pasan como parámetro.
//!  - sel_edge: Inserta en el conjunto los edges cuyos identificadores se pasan como parámetro.
bool XC::Set::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(Set) Procesando comando: " << cmd << std::endl;
    if(!get_modelador())
      std::cerr << "¡Ojo!, el conjunto: " << GetNombre() << " no tiene asignado un modelador." << std::endl;
    if(cmd == "sel_pnt") //Lista literal de identificadores de punto.
      {
	std::vector<std::string> nmbs_pntos= crea_vector_string(status.GetString());
        const size_t sz= nmbs_pntos.size();
        if(get_modelador())
          {
            Cad &cad= get_modelador()->getCad();
            for(size_t i= 0;i<sz;i++)
              {
	        Pnt *ipt= cad.getPuntos().busca(interpretaSize_t(nmbs_pntos[i])); 
                if(ipt)
                  puntos.push_back(ipt);
                else
		  std::cerr << "No se encontró el punto: " << nmbs_pntos[i] << std::endl;
              }
          }
        return true;
      }
    else if(cmd == "sel_puntos_cond") //Condición que han de cumplir para añadirlos al conjunto.
      {
	std::string cond= status.GetBloque(); //Condición a cumplir.
        Preprocessor *preprocessor= get_modelador();
        if(preprocessor)
          {
            Set *set_base= nullptr; //Conjunto del que se seleccionan las superficies.
            std::deque<boost::any> fnc_indices= status.Parser().SeparaIndices(this);
            if(fnc_indices.size()>0)
              {
                const std::string nmbSetBase= convert_to_string(fnc_indices[0]); //Nombre del conjunto del que se toman las lineas.
                set_base= dynamic_cast<Set *>(preprocessor->get_sets().busca_set(nmbSetBase));
                if(!set_base)
                  {
                    const std::string posLectura= status.GetEntradaComandos().getPosicionLecturaActual();
	            std::cerr << "(Set) Procesando comando: " << cmd
                              << " no se encotró el conjunto:'" << nmbSetBase
                              << " en el modelador." << posLectura << std::endl;
                  }
	      }
            else
              set_base= preprocessor->get_sets().get_set_total();
            if(set_base)
              puntos.agrega_cond(set_base->puntos,cond);
          }
        return true;
      }
    else if(cmd == "sel_puntos_lista") //Lista de tags de puntos.
      {
	const Lista tmp= interpretaLista(status.GetString());
        const size_t sz= tmp.size();
        Preprocessor *preprocessor= get_modelador();
        int tag= 0;
        if(preprocessor)
          {
            Cad &cad= get_modelador()->getCad();
            for(size_t i= 0;i<sz;i++)
              {
                tag= convert_to_size_t(tmp[i]);
	        Pnt *ipt= cad.getPuntos().busca(tag); 
                if(ipt)
                  puntos.push_back(ipt);
                else
		  std::cerr << "No se encontró el punto: " << tag << std::endl;
              }
          }
        return true;
      }
    else if(cmd == "sel_edge") //Lista de identificadores de edge.
      {
	std::vector<std::string> nmbs_edges= crea_vector_string(status.GetString());
        const size_t sz= nmbs_edges.size();
        if(get_modelador())
          {
            Cad &cad= get_modelador()->getCad();
            for(size_t i= 0;i<sz;i++)
              {
	        Edge *iedge= cad.getLineas().busca(interpretaSize_t(nmbs_edges[i])); 
                if(iedge)
                  lineas.push_back(iedge);
                else
		  std::cerr << "No se encontró la línea: " << nmbs_edges[i] << std::endl;
              }
          }
        return true;
      }
    else if(cmd == "sel_edge_cond") //Condición que han de cumplir para añadirlas al conjunto.
      {
	std::string cond= status.GetBloque(); //Condición a cumplir.
        Preprocessor *preprocessor= get_modelador();
        if(preprocessor)
          {
            Set *set_base= nullptr; //Conjunto del que se seleccionan las superficies.
            std::deque<boost::any> fnc_indices= status.Parser().SeparaIndices(this);
            if(fnc_indices.size()>0)
              {
                const std::string nmbSetBase= convert_to_string(fnc_indices[0]); //Nombre del conjunto del que se toman las lineas.
                set_base= dynamic_cast<Set *>(preprocessor->get_sets().busca_set(nmbSetBase));
                if(!set_base)
                  {
                    const std::string posLectura= status.GetEntradaComandos().getPosicionLecturaActual();
	            std::cerr << "(Set) Procesando comando: " << cmd
                              << " no se encotró el conjunto:'" << nmbSetBase
                              << " en el modelador." << posLectura << std::endl;
                  }
	      }
            else
              set_base= preprocessor->get_sets().get_set_total();
            if(set_base)
              lineas.agrega_cond(set_base->lineas,cond);
          }
        return true;
      }
    else if(cmd == "sel_edges_lista") //Lista de tags de líneas.
      {
	const Lista tmp= interpretaLista(status.GetString());
        const size_t sz= tmp.size();
        Preprocessor *preprocessor= get_modelador();
        int tag= 0;
        if(preprocessor)
          {
            Cad &cad= get_modelador()->getCad();
            for(size_t i= 0;i<sz;i++)
              {
                tag= convert_to_size_t(tmp[i]);
	        Edge *iedge= cad.getLineas().busca(tag); 
                if(iedge)
                  lineas.push_back(iedge);
                else
		  std::cerr << "No se encontró el punto: " << tag << std::endl;
              }
          }
        return true;
      }
    else if(cmd == "sel_face") //Lista literal de identificadores de superficie.
      {
	std::vector<std::string> nmbs_faces= crea_vector_string(status.GetString());
        const size_t sz= nmbs_faces.size();
        if(get_modelador())
          {
            Cad &cad= get_modelador()->getCad();
            for(size_t i= 0;i<sz;i++)
              {
	        Face *iface= cad.getSuperficies().busca(interpretaSize_t(nmbs_faces[i])); 
                if(iface)
                  superficies.push_back(iface);
                else
		  std::cerr << "No se encontró la supeficie: " << nmbs_faces[i] << std::endl;
              }
          }
        return true;
      }
    else if(cmd == "sel_face_cond") //Condición que han de cumplir para añadirlas al conjunto.
      {
	std::string cond= status.GetBloque(); //Condición a cumplir.
        Preprocessor *preprocessor= get_modelador();
        if(preprocessor)
          {
            Set *set_base= nullptr; //Conjunto del que se seleccionan las superficies.
            std::deque<boost::any> fnc_indices= status.Parser().SeparaIndices(this);
            if(fnc_indices.size()>0)
              {
                const std::string nmbSetBase= convert_to_string(fnc_indices[0]); //Nombre del conjunto del que se toman las superficies.
                set_base= dynamic_cast<Set *>(preprocessor->get_sets().busca_set(nmbSetBase));
                if(!set_base)
                  {
                    const std::string posLectura= status.GetEntradaComandos().getPosicionLecturaActual();
	            std::cerr << "(Set) Procesando comando: " << cmd
                              << " no se encotró el conjunto:'" << nmbSetBase
                              << " en el modelador." << posLectura << std::endl;
                  }
	      }
            else
              set_base= preprocessor->get_sets().get_set_total();
            if(set_base)
              superficies.agrega_cond(set_base->superficies,cond);
          }
        return true;
      }
    else if(cmd == "sel_faces_lista") //Lista de identificadores de caras.
      {
	const Lista tmp= interpretaLista(status.GetString());
        const size_t sz= tmp.size();
        Preprocessor *preprocessor= get_modelador();
        int tag= 0;
        if(preprocessor)
          {
            Cad &cad= get_modelador()->getCad();
            for(size_t i= 0;i<sz;i++)
              {
                tag= convert_to_size_t(tmp[i]);
	        Face *iface= cad.getSuperficies().busca(tag); 
                if(iface)
                  superficies.push_back(iface);
                else
		  std::cerr << "No se encontró el punto: " << tag << std::endl;
              }
          }
        return true;
      }
    else if(cmd == "sel_set")
      {
        const std::string nmb_set= interpretaString(status.GetString());
        Preprocessor *preprocessor= get_modelador();
        if(preprocessor)
          {
            const Set *set= dynamic_cast<Set *>(preprocessor->get_sets().busca_set(nmb_set));
            if(set)
              agrega_listas(*set);
            else
              {
                const std::string posLectura= get_ptr_status()->GetEntradaComandos().getPosicionLecturaActual();
                std::cerr << "No se pudo agregar el conjunto: '"
                          << nmb_set << "'." << posLectura << std::endl;
              }
          }
        return true;
      }
    else if(cmd == "sel_set_cond")
      {
        const std::vector<std::string> args= crea_vector_string(status.GetString());
        if(args.size()>=2)
          {
            const std::string nmbSet= args[0];
            const std::string cond= args[1];
            Preprocessor *preprocessor= get_modelador();
            if(preprocessor)
              {
                const Set *set= dynamic_cast<Set *>(preprocessor->get_sets().busca_set(nmbSet));
                if(set)
                  agrega_listas_cond(*set,cond);
                else
                  {
                    const std::string posLectura= get_ptr_status()->GetEntradaComandos().getPosicionLecturaActual();
	            std::cerr << "No se pudo agregar el conjunto: '"
                              << nmbSet << "'." << posLectura << std::endl;
                  }
              }
          }
        else
          {
            const std::string posLectura= get_ptr_status()->GetEntradaComandos().getPosicionLecturaActual();
            std::cerr << "sel_set_cond se esperaban dos argumentos." << posLectura << std::endl;
          }
        return true;
      }
    else if(cmd == "pnts")
      {
        puntos.LeeCmd(status);
        return true;
      }
    else if(cmd == "lineas")
      {
        lineas.LeeCmd(status);
        return true;
      }
    else if(cmd == "superficies")
      {
        superficies.LeeCmd(status);
        return true;
      }
    else if(cmd == "cuerpos")
      {
        cuerpos.LeeCmd(status);
        return true;
      }
    else if(cmd == "uniform_grids")
      {
        uniform_grids.LeeCmd(status);
        return true;
      }
    else if(cmd == "calc_resisting_force")
      {
        status.GetBloque(); //Ignoramos entrada.
        calc_resisting_force();
        return true;
      }
    else if(cmd == "numera")
      {
        status.GetBloque(); //Ignoramos entrada.
        numera();
        return true;
      }
    else if(cmd == "mueve")
      {
        //Leemos el vector desplazamiento.
	std::vector<ExprAlgebra> desplaz= crea_vector_expr(status.GetString());
        mueve(desplaz);
        return true;
      }
    else if(cmd == "transforma")
      {
        const std::vector<MapTrfGeom::Indice> tmp= crea_vector_size_t(status.GetString());
        if(tmp.size()>0)
          Transforma(tmp[0]); 
        else
	  std::cerr << "(Set) Procesando comando: " << cmd
                    << " se necesitan el índice de la transformación." << std::endl;
        return true;
      }
    else if(cmd == "completa_hacia_arriba")
      {
        status.GetBloque(); //Ignoramos entrada.
        CompletaHaciaArriba();
        return true;
      }
    else if(cmd == "completa_hacia_abajo")
      {
        status.GetBloque(); //Ignoramos entrada.
        CompletaHaciaAbajo();
        return true;
      }
    else if(cmd == "clearAll")
      {
        status.GetBloque(); //Ignoramos entrada.
        clearAll();
        return true;
      }
    else
      return SetMeshComp::procesa_comando(status);
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
    Preprocessor *mdl= get_modelador();
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

//! Devuelve la propiedad del objeto cuyo código se pasa
//! como parámetro.
any_const_ptr XC::Set::GetProp(const std::string &cod) const
  {
    if(cod=="getNumPuntos")
      {
        tmp_gp_szt= puntos.size();
        return any_const_ptr(tmp_gp_szt);
      }
    else if(cod=="getNumLineas")
      {
        tmp_gp_szt= lineas.size();
        return any_const_ptr(tmp_gp_szt);
      }
    else if(cod=="getNumCaras")
      {
        tmp_gp_szt= superficies.size();
        return any_const_ptr(tmp_gp_szt);
      }
    else if(cod=="lineas")
      return any_const_ptr(lineas);
    else if(cod=="getTagNearestPnt")
      {
        const Pos3d p= popPos3d(cod);
        const Pnt *tmp= puntos.getNearest(p);
        if(!tmp)
          {
            const std::string posLectura= get_ptr_status()->GetEntradaComandos().getPosicionLecturaActual();
            std::cerr << "No se encontró un punto cercano a la posición: "
                      << p << ". " << posLectura << std::endl;
          }
        else
          tmp_gp_int= tmp->GetTag();
        return any_const_ptr(tmp_gp_int);
      }
    else if(cod=="getTagNearestLine")
      {
        const Pos3d p= popPos3d(cod);
        const Edge *tmp= lineas.getNearest(p);
        if(!tmp)
          {
            const std::string posLectura= get_ptr_status()->GetEntradaComandos().getPosicionLecturaActual();
            std::cerr << "No se encontró una línea cercana a la posición: "
                      << p << ". " << posLectura << std::endl;
          }
        else
          tmp_gp_int= tmp->GetTag();
        return any_const_ptr(tmp_gp_int);
      }
    else if(cod=="getTagNearestFace")
      {
        const Pos3d p= popPos3d(cod);
        const Face *tmp= superficies.getNearest(p);
        if(!tmp)
          {
            const std::string posLectura= get_ptr_status()->GetEntradaComandos().getPosicionLecturaActual();
            std::cerr << "No se encontró una superficie cercana a la posición: "
                      << p << ". " << posLectura << std::endl;
          }
        else
          tmp_gp_int= tmp->GetTag();
        return any_const_ptr(tmp_gp_int);
      }
    else
      return SetMeshComp::GetProp(cod);
  }

//! @brief Agrega al conjunto que se pasa como parámetro
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
        Preprocessor *preprocessor= get_modelador();
        if(preprocessor)
          {
            Cad &cad= get_modelador()->getCad();
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
          std::cerr << "Set::sel_puntos_lista; necesito un modelador." << std::endl;
      }
  }

//! @brief Selecciona las lineas cuyos tags se pasan como parámetro.
void XC::Set::sel_lineas_lista(const ID &tags)
  {
    const size_t sz= tags.Size();
    if(sz>0)
      {
        Preprocessor *preprocessor= get_modelador();
        if(preprocessor)
          {
            Cad &cad= get_modelador()->getCad();
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
          std::cerr << "Set::sel_lineas_lista; necesito un modelador." << std::endl;
      }
  }

//! @brief Selecciona las superficies cuyos tags se pasan como parámetro.
void XC::Set::sel_superficies_lista(const ID &tags)
  {
    const size_t sz= tags.Size();
    if(sz>0)
      {
        Preprocessor *preprocessor= get_modelador();
        if(preprocessor)
          {
            Cad &cad= get_modelador()->getCad();
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
          std::cerr << "Set::sel_elementos_lista; necesito un modelador." << std::endl;
      }
  }

//! @brief Devuelve un vector para almacenar los dbTags
//! de los miembros de la clase.
XC::DbTagData &XC::Set::getDbTagData(void) const
  {
    static DbTagData retval(19);
    return retval;
  }

//! @brief Envía los miembros a través del canal que se pasa como parámetro.
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

//! @brief Recibe los miembros a través del canal que se pasa como parámetro.
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

//! @brief Envía el objeto por el canal que se pasa como parámetro.
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

//! @brief Recibe el objeto a través del canal que se pasa como parámetro.
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
