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
//SetMeshComp.cc

#include "SetMeshComp.h"
#include "Set.h"
#include "domain/domain/Domain.h"
#include "modelador/Modelador.h"
#include "modelador/cad/matrices/TritrizPtrElem.h"
#include "modelador/cad/trf/TrfGeom.h"
#include "xc_utils/src/base/CmdStatus.h"
#include "domain/constraints/SP_Constraint.h"
#include "domain/constraints/MP_Constraint.h"
#include "domain/mesh/element/Element.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/NodePtrsWithIDs.h"
#include "solution/graph/graph/Graph.h"
#include "solution/graph/graph/Vertex.h"
#include "utility/matrix/ID.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "xc_utils/src/geom/pos_vec/SVD3d.h"
#include "xc_utils/src/geom/d2/Plano3d.h"
#include "xc_utils/src/geom/d3/SemiEspacio3d.h"
#include "xc_utils/src/nucleo/InterpreteRPN.h"


//! @brief Constructor.
XC::SetMeshComp::SetMeshComp(const std::string &nmb,Modelador *md)
  : SetBase(nmb,md) {}

//! @brief Constructor de copia.
XC::SetMeshComp::SetMeshComp(const SetMeshComp &otro)
  : SetBase(otro)
  { copia_listas(otro); }

//! @brief Operator asignación.
XC::SetMeshComp &XC::SetMeshComp::operator=(const SetMeshComp &otro)
  {
    SetBase::operator=(otro);
    copia_listas(otro);
    return *this;
  }

//! @brief Copia las listas de objetos del conjunto s.
//!
//! Copia en ESTE objeto las listas de objetos del conjunto
//! que se pasa como parámetro.
void XC::SetMeshComp::copia_listas(const SetMeshComp &otro)
  {
    nodos= otro.nodos;
    elementos= otro.elementos;
    constraints= otro.constraints;
  }

//! @brief Agrega a este conjunto los objetos del conjunto
//! que se pasa como parámetro.
void XC::SetMeshComp::agrega_listas(const SetMeshComp &otro)
  {
    nodos.agrega(otro.nodos);
    elementos.agrega(otro.elementos);
    constraints.agrega(otro.constraints);
  }

//! @brief Agrega a este conjunto los objetos del conjunto
//! que se pasa como parámetro que cumplen la condición.
void XC::SetMeshComp::agrega_listas_cond(const SetMeshComp &otro,const std::string &cond)
  {
    nodos.agrega_cond(otro.nodos,cond);
    elementos.agrega_cond(otro.elementos,cond);
    constraints.agrega_cond(otro.constraints,cond);
  }

//!  \brief Borra todos los objetos de las listas.
void XC::SetMeshComp::clear(void)
  {
    //SetBase::clear();
    nodos.clear();
    constraints.clear();
    elementos.clear();
  }

//!  \brief Borra todos los objetos del conjunto.
void XC::SetMeshComp::clearAll(void)
  {
    SetBase::clearProps();
    nodos.clearAll();
    constraints.clearAll();
    elementos.clearAll();
  }

//! @brief Agrega el puntero a nodo que se pasa como parámetro.
void XC::SetMeshComp::agregaNodo(Node *nPtr)
  { nodos.push_back(nPtr); }

//! @brief Agrega el elemento que se pasa como parámetro.
void XC::SetMeshComp::agregaElemento(Element *ePtr)
  { elementos.push_back(ePtr); }

//! @brief Devuelve verdadero si el nodo pertenece al conjunto.
bool XC::SetMeshComp::In(const Node *n) const
  { return nodos.in(n); }

//! @brief Devuelve verdadero si el «edge» pertenece al conjunto.
bool XC::SetMeshComp::In(const Element *e) const
  { return elementos.in(e); }

//! @brief Asigna índices a los objetos del conjunto (nodos,elementos,puntos...) poder emplearlos en VTK.
void XC::SetMeshComp::numera(void)
  {
    nodos.numera();
    elementos.numera();
    numera_lista(constraints);
  }

//! @brief Desplaza los nodos del conjunto.
void XC::SetMeshComp::mueve(const std::vector<ExprAlgebra> &desplaz)
  { nodos.mueve(desplaz); }

//! @brief Aplica la transformación a los elementos del conjunto.
void XC::SetMeshComp::Transforma(const TrfGeom &trf)
  { nodos.transforma(trf); }

//! @brief Aplica al conjunto la transformación cuyo índice se pasa como parámetro.
void XC::SetMeshComp::Transforma(const size_t &indice_trf)
  {
    TrfGeom *trf= get_modelador()->getCad().getTransformacionesGeometricas().busca(indice_trf);
    if(trf)
      Transforma(*trf);
  }

//!  @brief Lee un objeto SetMeshComp desde el archivo de entrada.
//! 
//!  Soporta los comandos:
//! 
//!  - sel_nod: Inserta en el conjunto los nodos cuyos tags se pasan como parámetro.
//!  - sel_elem: Inserta en el conjunto los elementos cuyos tags se pasan como parámetro.
bool XC::SetMeshComp::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(SetMeshComp) Procesando comando: " << cmd << std::endl;
    if(!get_modelador())
      std::cerr << "¡Ojo!, el conjunto: " << GetNombre() << " no tiene asignado un modelador." << std::endl;
    if(cmd == "sel_nod") //Lista de tags de nodos.
      {
	std::vector<int> tags_nodos= crea_vector_int(status.GetString());
        const size_t sz= tags_nodos.size();
        Modelador *mdlr= get_modelador();
        if(mdlr)
          for(size_t i= 0;i<sz;i++)
            {
              nodos.push_back(mdlr->GetDominio()->getNode(tags_nodos[i]));
            }
        return true;
      }
    else if(cmd == "sel_nodos_lista") //Lista de tags de nodos.
      {
	const Lista tmp= interpretaLista(status.GetString());
        const size_t sz= tmp.size();
        Modelador *mdlr= get_modelador();
        int tag= 0;
        if(mdlr)
          for(size_t i= 0;i<sz;i++)
            {
              tag= convert_to_int(tmp[i]);
              nodos.push_back(mdlr->GetDominio()->getNode(tag));
            }
        return true;
      }
    else if(cmd == "sel_nod_cond") //Condición que han de cumplir para añadirlos al conjunto.
      {
	std::string cond= status.GetBloque(); //Condición a cumplir.
        Modelador *mdlr= get_modelador();
        if(mdlr)
          {
            SetMeshComp *set_base= nullptr; //Conjunto del que se seleccionan los nodos.
            std::deque<boost::any> fnc_indices= status.Parser().SeparaIndices(this);
            if(fnc_indices.size()>0)
              {
                const std::string nmbSetBase= convert_to_string(fnc_indices[0]); //Nombre del conjunto del que se toman los nodos.
                set_base= dynamic_cast<SetMeshComp *>(mdlr->get_sets().busca_set(nmbSetBase));
                if(!set_base)
                  {
                    const std::string posLectura= status.GetEntradaComandos().getPosicionLecturaActual();
	            std::cerr << "(SetMeshComp) Procesando comando: " << cmd
                              << " no se encotró el conjunto:'" << nmbSetBase
                              << " en el modelador." << posLectura << std::endl;
                  }
	      }
            else
              set_base= mdlr->get_sets().get_set_total();
            if(set_base)
              nodos.agrega_cond(set_base->nodos,cond);
          }
        return true;
      }
    else if(cmd=="nodo_con_tag")
      {
        std::deque<boost::any> fnc_args= status.Parser().SeparaArgs(this);
        if(fnc_args.size()<1)
          std::cerr << "puntos - uso: puntos(numPuntos) " << std::endl;
        const int tag= convert_to_int(fnc_args[0]); //Tag del nodo.
        Node *tmp= buscaNodo(tag);
        if(tmp)
          tmp->LeeCmd(status);
        else
          {
	    std::cerr << "(SetMeshComp) Procesando comando: " << cmd
                      << " no se encotró el nodo: " << tag
                      << " en este conjunto. Se ignora la entrada." << std::endl;
            status.GetBloque();
          }
        return true;
      }
    else if(cmd == "sel_elem") //Lista literal de tags de elementos.
      {
	std::vector<int> tags_elementos= crea_vector_int(status.GetString());
        const size_t sz= tags_elementos.size();
        Modelador *mdlr= get_modelador();
        if(mdlr)
          for(size_t i= 0;i<sz;i++)
            {
              elementos.push_back(mdlr->GetDominio()->getElement(tags_elementos[i]));
            }
        return true;
      }
    else if(cmd == "sel_elementos_lista") //Lista de tags de elementos.
      {
	const Lista tmp= interpretaLista(status.GetString());
        const size_t sz= tmp.size();
        Modelador *mdlr= get_modelador();
        int tag= 0;
        if(mdlr)
          for(size_t i= 0;i<sz;i++)
            {
              tag= convert_to_int(tmp[i]);
              elementos.push_back(mdlr->GetDominio()->getElement(tag));
            }
        return true;
      }
    else if(cmd == "sel_elem_cond") //Condición que han de cumplir para añadirlos al conjunto.
      {
	const std::string cond= status.GetBloque(); //Condición a cumplir.
        Modelador *mdlr= get_modelador();
        if(mdlr)
          {
            SetMeshComp *set_base= nullptr; //Conjunto del que se seleccionan los elementos.
            std::deque<boost::any> fnc_indices= status.Parser().SeparaIndices(this);
            if(fnc_indices.size()>0)
              {
                const std::string nmbSetBase= convert_to_string(fnc_indices[0]); //Tag del nodo.
                set_base= dynamic_cast<SetMeshComp *>(mdlr->get_sets().busca_set(nmbSetBase));
                if(!set_base)
                  {
                    const std::string posLectura= status.GetEntradaComandos().getPosicionLecturaActual();
	            std::cerr << "(SetMeshComp) Procesando comando: " << cmd
                              << " no se encotró el conjunto:'" << nmbSetBase
                              << " en el modelador." << posLectura << std::endl;
                  }
	      }
            else
              set_base= mdlr->get_sets().get_set_total();
            elementos.agrega_cond(set_base->elementos,cond);
          }
        return true;
      }
    else if(cmd == "sel_sp_constraint") //Lista de tags de constraints.
      {
	std::vector<int> tags_constraints= crea_vector_int(status.GetString());
        const size_t sz= tags_constraints.size();
        Modelador *mdlr= get_modelador();
        if(mdlr)
          for(size_t i= 0;i<sz;i++)
            {
              constraints.push_back(mdlr->GetDominio()->getCondsContorno().getSP_Constraint(tags_constraints[i]));
            }
        return true;
      }
    else if(cmd == "sel_mp_constraint") //Lista literal de tags de constraints.
      {
	std::vector<int> tags_constraints= crea_vector_int(status.GetString());
        const size_t sz= tags_constraints.size();
        Modelador *mdlr= get_modelador();
        if(mdlr)
          for(size_t i= 0;i<sz;i++)
            {
              constraints.push_back(mdlr->GetDominio()->getCondsContorno().getMP_Constraint(tags_constraints[i]));
            }
        return true;
      }
    else if(cmd == "sel_sp_constraints_lista") //Lista de tags de constraints.
      {
	const Lista tmp= interpretaLista(status.GetString());
        const size_t sz= tmp.size();
        Modelador *mdlr= get_modelador();
        int tag= 0;
        if(mdlr)
          for(size_t i= 0;i<sz;i++)
            {
              tag= convert_to_int(tmp[i]);
              constraints.push_back(mdlr->GetDominio()->getCondsContorno().getSP_Constraint(tag));
            }
        return true;
      }
    else if(cmd == "sel_mp_constraints_lista") //Lista de tags de constraints.
      {
	const Lista tmp= interpretaLista(status.GetString());
        const size_t sz= tmp.size();
        Modelador *mdlr= get_modelador();
        int tag= 0;
        if(mdlr)
          for(size_t i= 0;i<sz;i++)
            {
              tag= convert_to_int(tmp[i]);
              constraints.push_back(mdlr->GetDominio()->getCondsContorno().getMP_Constraint(tag));
            }
        return true;
      }
    else if(cmd == "sel_constraints_cond") //Condición que han de cumplir para añadirlos al conjunto.
      {
	const std::string cond= status.GetBloque(); //Condición a cumplir.
        Modelador *mdlr= get_modelador();
        if(mdlr)
          {
            SetMeshComp *set_base= nullptr; //Conjunto del que se seleccionan los nodos.
            std::deque<boost::any> fnc_indices= status.Parser().SeparaIndices(this);
            if(fnc_indices.size()>0)
              {
                const std::string nmbSetBase= convert_to_string(fnc_indices[0]); //Tag del nodo.
                set_base= dynamic_cast<SetMeshComp *>(mdlr->get_sets().busca_set(nmbSetBase));
                if(!set_base)
                  {
                    const std::string posLectura= status.GetEntradaComandos().getPosicionLecturaActual();
	            std::cerr << "(SetMeshComp) Procesando comando: " << cmd
                              << " no se encotró el conjunto:'" << nmbSetBase
                              << " en el modelador." << posLectura << std::endl;
                  }
	      }
            else
              set_base= mdlr->get_sets().get_set_total();
            constraints.agrega_cond(set_base->constraints,cond);
          }
        return true;
      }
    else if(cmd=="elemento_con_tag")
      {
        std::deque<boost::any> fnc_args= status.Parser().SeparaArgs(this);
        if(fnc_args.size()<1)
          std::cerr << "puntos - uso: elemento_con_tag[tagElem]{...} " << std::endl;
        const int tag= convert_to_int(fnc_args[0]); //Tag del nodo.
        Element *tmp= buscaElemento(tag);
        if(tmp)
          tmp->LeeCmd(status);
        else
          {
	    std::cerr << "(SetMeshComp) Procesando comando: " << cmd
                      << " no se encotró el elemento: " << tag
                      << " en este conjunto. Se ignora la entrada." << std::endl;
            status.GetBloque();
          }
        return true;
      }
    else if(cmd=="constraint_con_tag")
      {
        std::deque<boost::any> fnc_args= status.Parser().SeparaArgs(this);
        if(fnc_args.size()<1)
          std::cerr << "puntos - uso: constraint_con_tag[tagElem]{...} " << std::endl;
        const int tag= convert_to_int(fnc_args[0]); //Tag del nodo.
        Constraint *tmp= buscaConstraint(tag);
        if(tmp)
          tmp->LeeCmd(status);
        else
          {
	    std::cerr << "(SetMeshComp) Procesando comando: " << cmd
                      << " no se encotró el constraint: " << tag
                      << " en este conjunto. Se ignora la entrada." << std::endl;
            status.GetBloque();
          }
        return true;
      }
    else if(cmd == "sel_set")
      {
        const std::string nmb_set= interpretaString(status.GetString());
        Modelador *mdlr= get_modelador();
        if(mdlr)
          {
            const SetMeshComp *set= dynamic_cast<SetMeshComp *>(mdlr->get_sets().busca_set(nmb_set));
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
            Modelador *mdlr= get_modelador();
            if(mdlr)
              {
                const SetMeshComp *set= dynamic_cast<SetMeshComp *>(mdlr->get_sets().busca_set(nmbSet));
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
    else if(cmd == "nodos")
      {
        nodos.LeeCmd(status);
        return true;
      }
    else if(cmd == "elementos")
      {
        elementos.LeeCmd(status);
        return true;
      }
    else if(cmd == "kill_elements")
      {
        status.GetBloque(); //Ignoramos entrada.
        kill_elements();
        return true;
      }
    else if(cmd == "alive_elements")
      {
        status.GetBloque(); //Ignoramos entrada.
        alive_elements();
        return true;
      }
    else if(cmd == "calc_resisting_force")
      {
        status.GetBloque(); //Ignoramos entrada.
        calc_resisting_force();
        return true;
      }
    else if(cmd == "constraints")
      {
        constraints.LeeCmd(status);
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
	  std::cerr << "(SetMeshComp) Procesando comando: " << cmd
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
      return SetBase::procesa_comando(status);
  }

//! @brief Impone desplazamiento nulo en los nodos de este conjunto.
void XC::SetMeshComp::fix(const SP_Constraint &spc)
  {
    ConstraintLoader &cl= GetModelador()->getConstraintLoader();
    for(nod_const_iterator i= nodos_begin();i!=nodos_end();i++)
      {
        const int tag_nod= (*i)->getTag();
        cl.addSP_Constraint(tag_nod,spc);
      }
  }

// //! @brief Devuelve un iterador apuntando al principio de la lista de nodos.
// XC::SetMeshComp::nod_iterator XC::SetMeshComp::nodos_begin(void)
//   { return nodos.begin(); }
//! @brief Devuelve un iterador apuntando al principio de la lista de nodos.
XC::SetMeshComp::nod_const_iterator XC::SetMeshComp::nodos_begin(void) const
  { return nodos.begin(); }
// //! @brief Devuelve un iterador apuntando tras el final de la lista de nodos.
// XC::SetMeshComp::nod_iterator XC::SetMeshComp::nodos_end(void)
//   { return nodos.end(); }
//! @brief Devuelve un iterador apuntando tras el final de la lista de nodos.
XC::SetMeshComp::nod_const_iterator XC::SetMeshComp::nodos_end(void) const
  { return nodos.end(); }

//! @brief Devuelve, si lo encuentra, un puntero al nodo
//! cuyo tag se pasa como parámetro.
XC::Node *XC::SetMeshComp::buscaNodo(const int &tag)
  { return nodos.buscaNodo(tag); }

//! @brief Devuelve, si lo encuentra, un puntero al nodo
//! cuyo tag se pasa como parámetro.
const XC::Node *XC::SetMeshComp::buscaNodo(const int &tag) const
  { return nodos.buscaNodo(tag); }

// //! @brief Devuelve un iterador apuntando al principio de la lista de elementos.
// XC::SetMeshComp::elem_iterator XC::SetMeshComp::elem_begin(void)
//   { return elementos.begin(); }
//! @brief Devuelve un iterador apuntando al principio de la lista de elementos.
XC::SetMeshComp::elem_const_iterator XC::SetMeshComp::elem_begin(void) const
  { return elementos.begin(); }
// //! @brief Devuelve un iterador apuntando tras el final de la lista de elementos.
// XC::SetMeshComp::elem_iterator XC::SetMeshComp::elem_end(void)
//   { return elementos.end(); }
//! @brief Devuelve un iterador apuntando tras el final de la lista de elementos.
XC::SetMeshComp::elem_const_iterator XC::SetMeshComp::elem_end(void) const
  { return elementos.end(); }

//! @brief Devuelve un iterador apuntando al principio de la lista de constraints.
XC::SetMeshComp::constraint_iterator XC::SetMeshComp::constraints_begin(void)
  { return constraints.begin(); }
//! @brief Devuelve un iterador apuntando al principio de la lista de constraints.
XC::SetMeshComp::constraint_const_iterator XC::SetMeshComp::constraints_begin(void) const
  { return constraints.begin(); }
//! @brief Devuelve un iterador apuntando tras el final de la lista de constraints.
XC::SetMeshComp::constraint_iterator XC::SetMeshComp::constraints_end(void)
  { return constraints.end(); }
//! @brief Devuelve un iterador apuntando tras el final de la lista de constraints.
XC::SetMeshComp::constraint_const_iterator XC::SetMeshComp::constraints_end(void) const
  { return constraints.end(); }

//! @brief Devuelve, si lo encuentra, un puntero al nodo
//! cuyo tag se pasa como parámetro.
XC::Element *XC::SetMeshComp::buscaElemento(const int &tag)
  { return elementos.buscaElemento(tag); }

//! @brief Devuelve, si lo encuentra, un puntero al nodo
//! cuyo tag se pasa como parámetro.
const XC::Element *XC::SetMeshComp::buscaElemento(const int &tag) const
  { return elementos.buscaElemento(tag); }

//! @brief Devuelve, si lo encuentra, un puntero a la condicion
//! de contorno cuyo tag se pasa como parámetro.
XC::Constraint *XC::SetMeshComp::buscaConstraint(const int &tag)
  {
    Constraint *retval= nullptr;
    Constraint *tmp= nullptr;
    for(lst_ptr_constraints::iterator i= constraints.begin();i!=constraints.end();i++)
      {
        tmp= *i;
        if(tmp)
          {
            if(tag == tmp->getTag())
              {
                retval= tmp;
                break;
              }
          }
      }
    return retval;
  }

//! @brief Devuelve, si lo encuentra, un puntero a la condicion
//! de contorno cuyo tag se pasa como parámetro.
const XC::Constraint *XC::SetMeshComp::buscaConstraint(const int &tag) const
  {
    const Constraint *retval= nullptr;
    const Constraint *tmp= nullptr;
    for(lst_ptr_constraints::const_iterator i= constraints.begin();i!=constraints.end();i++)
      {
        tmp= *i;
        if(tmp)
          {
            if(tag == tmp->getTag())
              {
                retval= tmp;
                break;
              }
          }
      }
    return retval;
  }

//! @brief Devuelve el número de elementos del conjunto que están activos.
size_t XC::SetMeshComp::getNumLiveElements(void) const
  { return elementos.getNumLiveElements(); }

//! @brief Devuelve el número de elementos del conjunto que están inactivos.
size_t XC::SetMeshComp::getNumDeadElements(void) const
  { return elementos.getNumDeadElements(); }

//! @brief Devuelve el número de nods del conjunto que están activos.
size_t XC::SetMeshComp::getNumLiveNodes(void) const
  { return nodos.getNumLiveNodes(); }


//! @brief Devuelve el número de nodos del conjunto que están inactivos.
size_t XC::SetMeshComp::getNumDeadNodes(void) const
  { return nodos.getNumDeadNodes(); }

//! @brief Desactiva los elementos del conjunto.
void XC::SetMeshComp::kill_elements(void)
  { elementos.kill_elements(); }

//! @brief Reactiva los elementos del conjunto.
void XC::SetMeshComp::alive_elements(void)
  { elementos.alive_elements(); }

//! @brief Calcula los esfuerzos en cada uno de los elementos.
void XC::SetMeshComp::calc_resisting_force(void)
  { elementos.calc_resisting_force(); }

//! @brief Devuelve la resultante de las fuerzas que ejercen sobre
//! los nodos próximos al plano, los elementos situados tras el plano.
SVD3d XC::SetMeshComp::getResistingSVD3d(const Plano3d &plano,const Pos3d &centro,const double &tol,const bool &inc_inertia) const
  {
    //XX Se puede mejorar calculando la resultante en la intersección de
    //los lados del elemento con el plano e interpolando el valor en ese
    //punto a partir de los valores en los nodos extremos del lado.
    const double tol2= tol*tol;
    const Node *ptrNod= nullptr;
    const SemiEspacio3d se(plano);
    std::set<const Node *> nodos_plano;
    std::set<const Element *> conectados_nodo;
    std::set<const Element *> elem_conectados;
    for(DqPtrsNode::const_iterator i= nodos.begin();i!=nodos.end();i++)
      {
        ptrNod= *i;
        if(ptrNod)
          if(ptrNod->isAlive())
            {
              if(plano.dist2(ptrNod->getPosInicial3d())<tol2)
                {
                  conectados_nodo= ptrNod->getElementosConectados();
                  if(!conectados_nodo.empty())
                    {
                      nodos_plano.insert(ptrNod); //Nodo sobre el plano.
                      for(std::set<const Element *>::const_iterator j= conectados_nodo.begin();
                          j!=conectados_nodo.end();j++)
                        {
                          if((*j)->getNodePtrs().In(se)) //Elemento tras el plano.
                            elem_conectados.insert(*j);
                          else if((*j)->getNodePtrs().Corta(plano)) //Elemento sobre el plano.
                            elem_conectados.insert(*j);
                        }
                    }
	        }
            }
      }
    SVD3d retval(centro);
    for(std::set<const Node *>::const_iterator i= nodos_plano.begin();i!=nodos_plano.end();i++)
      retval+= (*i)->getResistingSVD3d(elem_conectados,inc_inertia);
    return retval;    
  }

//! Devuelve la propiedad del objeto cuyo código se pasa
//! como parámetro.
any_const_ptr XC::SetMeshComp::GetProp(const std::string &cod) const
  {
    if(cod=="getNumNodos")
      {
        tmp_gp_szt= nodos.size();
        return any_const_ptr(tmp_gp_szt);
      }
    else if(cod=="getNumNodosActivos")
      {
        tmp_gp_szt= getNumLiveNodes();
        return any_const_ptr(tmp_gp_szt);
      }
    else if(cod=="getNumNodosInactivos")
      {
        tmp_gp_szt= getNumDeadNodes();
        return any_const_ptr(tmp_gp_szt);
      }
    else if(cod=="getNumElementos")
      {
        tmp_gp_szt= elementos.size();
        return any_const_ptr(tmp_gp_szt);
      }
    else if(cod=="getNumElementosActivos")
      {
        tmp_gp_szt= getNumLiveElements();
        return any_const_ptr(tmp_gp_szt);
      }
    else if(cod=="getNumElementosInactivos")
      {
        tmp_gp_szt= getNumDeadElements();
        return any_const_ptr(tmp_gp_szt);
      }
    else if(cod=="getNumConstraints")
      {
        tmp_gp_szt= constraints.size();
        return any_const_ptr(tmp_gp_szt);
      }
    else if(cod=="getTagNearestNode")
      {
        const Pos3d p= popPos3d(cod);
        const Node *tmp= getNearestNode(p);
        if(!tmp)
          {
            const std::string posLectura= get_ptr_status()->GetEntradaComandos().getPosicionLecturaActual();
            std::cerr << "No se encontró un nodo cercano a la posición: "
                      << p << " ¿se ha mallado el conjunto? ." << posLectura << std::endl;
          }
        else
          tmp_gp_int= tmp->getTag();
        return any_const_ptr(tmp_gp_int);
      }
    else if(cod=="getTagNearestNodeToPos")
      {
        const Pos3d p= popPos3d(cod);
        const Node *tmp= getNearestNode(p);
        if(!tmp)
          {
            const std::string posLectura= get_ptr_status()->GetEntradaComandos().getPosicionLecturaActual();
            std::cerr << "No se encontró un nodo cercano a la posición: "
                      << p << " ¿se ha mallado el conjunto? ." << posLectura << std::endl;
          }
        else
          tmp_gp_int= tmp->getTag();
        return any_const_ptr(tmp_gp_int);
      }
    else if(cod=="getTagNearestElement")
      {
        const Pos3d p= popPos3d(cod);
        const Element *tmp= getNearestElement(p);
        tmp_gp_int= tmp->getTag();
        return any_const_ptr(tmp_gp_int);
      }
    else if(cod=="getResistingSVD3d")
      {
        Plano3d plano;
        Pos3d centro;
        double tol= 0.01;
        bool inc_inertia(true);
        if(InterpreteRPN::Pila().size()>3)
          {
            inc_inertia= convert_to_bool(InterpreteRPN::Pila().Pop());
            tol= convert_to_double(InterpreteRPN::Pila().Pop());
            centro= convert_to_pos3d(InterpreteRPN::Pila().Pop());
            plano= convert_to_plano3d(InterpreteRPN::Pila().Pop());
          }
        else
          err_num_argumentos(std::cerr,4,"GetProp",cod);
        tmp_gp_svd3d= getResistingSVD3d(plano,centro,tol,inc_inertia);
        return any_const_ptr(tmp_gp_svd3d);
      }
    else
      return SetBase::GetProp(cod);
  }

//! @brief Devuelve verdadero si el nodo, cuyo tag se
//! pasa como parámetro, pertenece al conjunto.
bool XC::SetMeshComp::InNodeTag(const int tag_nodo) const
  { return nodos.InNodeTag(tag_nodo); }

//! @brief Devuelve verdadero si todos los nodos, cuyos tags se
//! pasan como parámetro, pertenecen al conjunto.
bool XC::SetMeshComp::InNodeTags(const ID &tag_nodos) const
  { return nodos.InNodeTags(tag_nodos); }

//! @brief Devuelve los tags de los nodos.
std::set<int> XC::SetMeshComp::getNodeTags(void) const
  { return nodos.getTags(); }

//! @brief Devuelve el nodo más próximo al punto que se pasa como parámetro.
XC::Node *XC::SetMeshComp::getNearestNode(const Pos3d &p)
  { return nodos.getNearestNode(p); }

//! @brief Devuelve el nodo más próximo al punto que se pasa como parámetro.
const XC::Node *XC::SetMeshComp::getNearestNode(const Pos3d &p) const
  {
    SetMeshComp *this_no_const= const_cast<SetMeshComp *>(this);
    return this_no_const->getNearestNode(p);
  }


//! @brief Devuelve los tags de los elementos.
std::set<int> XC::SetMeshComp::getElementTags(void) const
  { return elementos.getTags(); }

//! @brief Devuelve el elemento más próximo al punto que se pasa como parámetro.
XC::Element *XC::SetMeshComp::getNearestElement(const Pos3d &p)
  { return elementos.getNearestElement(p); }

//! @brief Devuelve el elemento más próximo al punto que se pasa como parámetro.
const XC::Element *XC::SetMeshComp::getNearestElement(const Pos3d &p) const
  {
    SetMeshComp *this_no_const= const_cast<SetMeshComp *>(this);
    return this_no_const->getNearestElement(p);
  }

//! @brief Devuelve los tags de los constraints.
std::set<int> XC::SetMeshComp::getConstraintTags(void) const
  {
    std::set<int> retval;
    for(constraint_const_iterator i= constraints.begin();i!=constraints.end();i++)
      retval.insert((*i)->getTag());
    return retval;
  }


//! @brief Agrega al conjunto que se pasa como parámetro
//! los elementos que intervienen en la
//! definición de los que ya están en el conjunto.
void XC::SetMeshComp::CompletaHaciaAbajo(void)
  {
    const DqPtrsElem &elems= elementos;
    for(elem_const_iterator i= elems.begin();i!=elems.end();i++)
      {
        Element *elem= (*i);
        assert(elem);
        NodePtrs &theNodes= elem->getNodePtrs();
        for(NodePtrs::iterator j= theNodes.begin();j!=theNodes.end();j++)
          nodos.push_back(*j);
      }
  }

//! @brief  Agrega a este conjunto los objetos que hacen referencia
//! a uno o más de los que ya están en el conjunto.
void XC::SetMeshComp::CompletaHaciaArriba(void)
  {
    std::cerr << "SetMeshComp::CompletaHaciaArriba() está sin terminar." << std::endl;
  }

//! @brief Selecciona los nodos cuyos tags se pasan como parámetro.
void XC::SetMeshComp::sel_nodos_lista(const ID &tags)
  {
    const size_t sz= tags.Size();
    if(sz>0)
      {
        Modelador *mdlr= get_modelador();
        if(mdlr)
          for(size_t i= 0;i<sz;i++)
            nodos.push_back(mdlr->GetDominio()->getNode(tags(i)));
        else
          std::cerr << "SetMeshComp::sel_nodos_lista; necesito un modelador." << std::endl;
      }
  }

//! @brief Selecciona los elementos cuyos tags se pasan como parámetro.
void XC::SetMeshComp::sel_elementos_lista(const ID &tags)
  {
    const size_t sz= tags.Size();
    if(sz>0)
      {
        Modelador *mdlr= get_modelador();
        if(mdlr)
          for(size_t i= 0;i<sz;i++)
            elementos.push_back(mdlr->GetDominio()->getElement(tags(i)));
        else
          std::cerr << "SetMeshComp::sel_elementos_lista; necesito un modelador." << std::endl;
      }
  }

//! @brief Selecciona las coacciones cuyos tags se pasan como parámetro.
void XC::SetMeshComp::sel_constraints_lista(const ID &tags)
  {
    const size_t sz= tags.Size();
    if(sz>0)
      {
        Modelador *mdlr= get_modelador();
        Constraint *tmp= nullptr;
        if(mdlr)
          for(size_t i= 0;i<sz;i++)
            {
              if((tmp= mdlr->GetDominio()->getCondsContorno().getSP_Constraint(tags(i))))
                constraints.push_back(tmp);
              else if((tmp=mdlr->GetDominio()->getCondsContorno().getMP_Constraint(tags(i))))
                constraints.push_back(tmp);
              else
		std::cerr << "SetMeshComp::sel_constraints_lista;no se encontró la coacción de tag: "
                          << tags(i) << std::endl;
            }

        else
          std::cerr << "SetMeshComp::sel_constraints_lista; necesito un modelador." << std::endl;
      }
  }

//! @brief Devuelve un vector para almacenar los dbTags
//! de los miembros de la clase.
XC::DbTagData &XC::SetMeshComp::getDbTagData(void) const
  {
    static DbTagData retval(19);
    return retval;
  }

//! @brief Envía los miembros a través del canal que se pasa como parámetro.
int XC::SetMeshComp::sendData(CommParameters &cp)
  {
    int res= SetBase::sendData(cp);
    res+= nodos.sendTags(3,4,getDbTagData(),cp);
    res+= elementos.sendTags(5,6,getDbTagData(),cp);
    res+= constraints.sendTags(7,8,getDbTagData(),cp);
    return res;
  }

//! @brief Recibe los miembros a través del canal que se pasa como parámetro.
int XC::SetMeshComp::recvData(const CommParameters &cp)
  {
    ID tmp;
    int res= SetBase::recvData(cp);
    tmp= nodos.receiveTags(3,4,getDbTagData(),cp);
    sel_nodos_lista(tmp);
    tmp= elementos.receiveTags(5,6,getDbTagData(),cp);
    sel_elementos_lista(tmp);
    tmp= constraints.receiveTags(7,8,getDbTagData(),cp);
    sel_constraints_lista(tmp);
    return res;
  }

//!  \brief Destructor.
XC::SetMeshComp::~SetMeshComp(void)
  { clearAll(); }

//! @brief Envía el objeto por el canal que se pasa como parámetro.
int XC::SetMeshComp::sendSelf(CommParameters &cp)
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
int XC::SetMeshComp::recvSelf(const CommParameters &cp)
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
