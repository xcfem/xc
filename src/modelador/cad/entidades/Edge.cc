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
//Edge.cc

#include "Edge.h"
#include "Face.h"
#include "Pnt.h"
#include "domain/mesh/node/Node.h"
#include "utility/matrix/util_matrix.h"
#include "xc_basic/src/matrices/m_int.h"
#include "xc_utils/src/geom/d3/BND3d.h"
#include "xc_utils/src/geom/pos_vec/MatrizPos3d.h"
#include "modelador/Modelador.h"
#include "modelador/set_mgmt/Set.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "xc_utils/src/base/utils_any.h"
#include "boost/any.hpp"
#include "xc_utils/src/base/CmdStatus.h"
#include "domain/mesh/element/Element.h"

//! @brief Constructor.
XC::Edge::Edge(Modelador *m,const size_t &nd)
  : EntMdlr(m), ndiv(nd) {}

//! @brief Constructor.
//! @param nombre: Identificador del objeto.
//! @param i: Índice para gráficos.
//! @param m: Apuntador al modelador.
XC::Edge::Edge(const std::string &nombre,Modelador *m,const size_t &nd)
  : EntMdlr(nombre,0,m), ndiv(nd) {}

//! @brief Inserta una superficie en contacto con la línea.
void XC::Edge::inserta_surf(Face *s)
  { sups_linea.insert(s); }

//! @brief Actualiza la topología.
void XC::Edge::actualiza_topologia(void)
  {
    if(P1()) P1()->inserta_linea(this);
    if(P2()) P2()->inserta_linea(this);
  }

//! @brief Devuelve un apuntador al punto inicial.
XC::Pnt *XC::Edge::P1(void)
  {
    std::cerr << "Edge::P1; esta función debe redefinirse en las clases"
              << " derivadas, se devuelve nullptr;" << std::endl;
    return nullptr;
  }

//! @brief Devuelve un apuntador (constante) al punto inicial.
const XC::Pnt *XC::Edge::P1(void) const
  {
    std::cerr << "Edge::P1; esta función debe redefinirse en las clases"
              << " derivadas, se devuelve nullptr;" << std::endl;
    return nullptr;
  }

//! @brief Devuelve un apuntador al punto inicial.
XC::Pnt *XC::Edge::P2(void)
  {
    std::cerr << "Edge::P2; esta función debe redefinirse en las clases"
              << " derivadas, se devuelve nullptr;" << std::endl;
    return nullptr;
  }

//! @brief Devuelve un apuntador (constante) al punto final.
const XC::Pnt *XC::Edge::P2(void) const
  {
    std::cerr << "Edge::P2; esta función debe redefinirse en las clases"
              << " derivadas, se devuelve nullptr;" << std::endl;
    return nullptr;
  }

//! @brief Devuelve verdadero si los puntos que se pasan como parámetro
//! son los extremos de la línea.
bool XC::Edge::ExtremosEn(const Pnt *p1,const Pnt *p2) const
  {
    bool retval= false;
    const Pnt *e1= P1();
    const Pnt *e2= P2();
    if(p1==e1)
      retval= (p2==e2);
    else if(p1==e2)
      retval= (p2==e1);
    return retval;
  }

//! @brief Asigna el número de divisiones.
void XC::Edge::SetNDiv(const size_t &nd)
  {
    if(ndiv!=nd) //Si cambia el número de divisiones
      {
        if(nodos.empty()) //No se ha mallado.
          {
            const size_t ns= sups_linea.size();
            if(ns>1)
              {
// 	        std::clog << "Edge::SetNDiv; el borde: " << GetNombre()
//                           << " lo es de las superficies: "
//                           << NombresSupsTocan()
//                           << ". Se había definido el"
//                           << " número de divisiones en " << ndiv
//                           << " se adopta: " << nd << std::endl;
                ndiv= std::max(ndiv,nd);
              }
            else
              ndiv= nd;
          }
        else
          std::cerr << "Edge::SetNDiv; el borde: " << GetNombre()
                    << " ya está mallado se mantiene su número de divisiones"
                    << " ndiv= " << ndiv << ".\n";
      }
  }

//! @brief Creates points along the line.
void XC::Edge::divide(void)
  {
    const MatrizPos3d posiciones= get_posiciones();
    crea_puntos(posiciones);
  }

//! @brief Lee un objeto Edge desde el archivo de entrada.
//!
//! Soporta los comandos:
//!
//! - ndiv: Lee el número de divisiones.
bool XC::Edge::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(Edge) Procesando comando: " << cmd << std::endl;
    if(cmd == "ndiv")
      {
        SetNDiv(interpretaSize_t(status.GetString()));
        return true;
      }
    else if(cmd == "long_elem")
      {
        const double lElem= interpretaDouble(status.GetString());
        const double L= getLongitud();
        if(lElem>0.0)
          {
            const size_t n= std::max(static_cast<size_t>(ceil(L/lElem)),static_cast<size_t>(1));
            SetNDiv(n);
	  }
        return true;
      }
    else if(cmd == "divide") //Crea puntos a lo largo de la línea.
      {
        status.GetString(); //Ignoramos argumentos.
        divide();
        return true;
      }
    else if(cmd == "primer_nodo")
      {
        Node *ptr= GetPrimerNodo();
        if(ptr)
          ptr->LeeCmd(status);
        else
          {
            status.GetString(); //Ignoramos argumentos.
	    std::cerr << "(Edge) Procesando comando: " << cmd
                      << " el puntero al primer nodo es nulo." << std::endl;
          }
        return true;
      }
    else if(cmd == "ultimo_nodo")
      {
        Node *ptr= GetUltimoNodo();
        if(ptr)
          ptr->LeeCmd(status);
        else
          {
            status.GetString(); //Ignoramos argumentos.
	    std::cerr << "(Edge) Procesando comando: " << cmd
                      << " el puntero al primer nodo es nulo." << std::endl;
          }
        return true;
      }
    else
      return EntMdlr::procesa_comando(status);
  }

//! @brief Devuelve un apuntador al nodo cuyos índices se pasan como parámetro.
XC::Node *XC::Edge::GetNodo(const size_t &i,const size_t &j,const size_t &k)
  { return EntMdlr::GetNodo(i,j,k); }

//! @brief Devuelve un apuntador al nodo cuyos índices se pasan como parámetro.
const XC::Node *XC::Edge::GetNodo(const size_t &i,const size_t &j,const size_t &k) const
  { return EntMdlr::GetNodo(i,j,k); }

//! @brief Devuelve un apuntador al nodo cuyo índice se pasa como parámetro.
XC::Node *XC::Edge::GetNodo(const size_t &i)
  {  return const_cast<Node *>(static_cast<const Edge &>(*this).GetNodo(i)); }

//! @brief Devuelve un apuntador al nodo cuyo índice se pasa como parámetro.
const XC::Node *XC::Edge::GetNodo(const size_t &i) const
  { return nodos.getAtI(i); }

//! @brief Devuelve el nodo cuyo índice se pasa como parámetro empezando por el principio.
XC::Node *XC::Edge::GetNodoDir(const size_t &i)
  { return GetNodo(i); }

//! @brief Devuelve el nodo cuyo índice se pasa como parámetro empezando por el principio.
const XC::Node *XC::Edge::GetNodoDir(const size_t &i) const
  { return GetNodo(i); }

//! @brief Devuelve el nodo cuyo índice se pasa como parámetro empezando por el final.
XC::Node *XC::Edge::GetNodoInv(const size_t &i)
  { return GetNodo(NumNodos()-i+1); }

//! @brief Devuelve el nodo cuyo índice se pasa como parámetro empezando por el final.
const XC::Node *XC::Edge::GetNodoInv(const size_t &i) const
  { return GetNodo(NumNodos()-i+1); }

//! @brief Devuelve el primer nodo de la línea.
const XC::Node *XC::Edge::GetPrimerNodo(void) const
  { 
    const Node *retval= GetNodo(1);
    if(!retval)
      {
        const Pnt *p1= P1();
        if(p1)
          retval= p1->GetNodo();
      }
    return retval;
  }

//! @brief Devuelve el primer nodo de la línea.
XC::Node *XC::Edge::GetPrimerNodo(void)
  { 
    Node *retval= GetNodo(1);
    if(!retval)
      {
        Pnt *p1= P1();
        if(p1)
          retval= p1->GetNodo();
      }
    return retval;
  }

//! @brief Devuelve el último nodo de la línea.
const XC::Node *XC::Edge::GetUltimoNodo(void) const
  { 
    const Node *retval= GetNodoInv(1);
    if(!retval)
      {
        const Pnt *p2= P2();
        if(p2)
          retval= p2->GetNodo();
      }
    return retval;
  }

//! @brief Devuelve el último nodo de la línea.
XC::Node *XC::Edge::GetUltimoNodo(void)
  { 
    Node *retval= GetNodoInv(1);
    if(!retval)
      {
        Pnt *p2= P2();
        if(p2)
          retval= p2->GetNodo();
      }
    return retval;
  }

//! @brief Devuelve los identificadores de los nodos en sentido directo.
std::vector<int> XC::Edge::GetTagsNodosDir(void) const
  {
    const size_t nn= NumNodos();
    std::vector<int> retval(nn);
    for(size_t i=1;i<=nn;i++)
      retval[i-1]= GetNodo(i)->getTag();
    return retval;
  }

//! @brief Devuelve los identificadores de los nodos en sentido directo.
std::vector<int> XC::Edge::GetTagsNodosInv(void) const
  {
    const size_t nn= NumNodos();
    std::vector<int> retval(nn);
    for(size_t i=1;i<=nn;i++)
      retval[i-1]= GetNodo(nn-i+1)->getTag();
    return retval;
  }

//! @brief Devuelve las posiciones de los nodos en sentido directo.
MatrizPos3d XC::Edge::GetPosNodosDir(void) const
  {
    const size_t nn= NumNodos();
    MatrizPos3d retval(nn);
    for(size_t i=1;i<=nn;i++)
      retval(i,1)= pos_nodo(*GetNodo(i));
    return retval;
  }

//! @brief Devuelve las posiciones de los nodos en sentido inverso.
MatrizPos3d XC::Edge::GetPosNodosInv(void) const
  {
    const size_t nn= NumNodos();
    MatrizPos3d retval(nn);
    for(size_t i=1;i<=nn;i++)
      retval(i,1)= pos_nodo(*GetNodo(nn-i+1));
    return retval;
  }

//! @brief Devuelve los nombres de las superficies que tocan a a línea.
const std::string &XC::Edge::NombresSupsTocan(void) const
  {
    static std::string retval;
    retval= "";
    if(!sups_linea.empty())
      {
        std::set<const Face *>::const_iterator i= sups_linea.begin();
        retval+= (*i)->GetNombre();
        i++;
        for(;i!=sups_linea.end();i++)
          retval+= "," + (*i)->GetNombre();
      }
    return retval;
  }

//! @brief Devuelve los lados homólogos a éste.
std::set<const XC::Edge *> XC::Edge::GetLadosHomologos(const std::set<const XC::Edge *> &lh) const
  {
    std::set<const Edge *> retval;
    std::set<const Edge *> nuevos_adyacentes;

    if(!sups_linea.empty())
      {
        for(std::set<const Face *>::const_iterator i= sups_linea.begin();i!=sups_linea.end();i++)
          {
            const Edge *h= (*i)->get_lado_homologo(this);
            if(h!=this)
              {
                std::set<const XC::Edge *>::const_iterator k= lh.find(h);
                if(k==lh.end()) //No está
                  nuevos_adyacentes.insert(h);
              }
          }
      }
    for(std::set<const XC::Edge *>::const_iterator i= nuevos_adyacentes.begin();i!=nuevos_adyacentes.end();i++)
      retval.insert(*i);
    if(retval.size()>lh.size()) //Hay lados nuevos.
      {
        for(std::set<const XC::Edge *>::const_iterator i= nuevos_adyacentes.begin();i!=nuevos_adyacentes.end();i++)
          {
            std::set<const XC::Edge *> tmp= (*i)->GetLadosHomologos(retval);
            for(std::set<const XC::Edge *>::const_iterator j= tmp.begin();j!=tmp.end();j++)
              if(this!=*j) retval.insert(*j);
          }
        nuevos_adyacentes.erase(nuevos_adyacentes.begin(),nuevos_adyacentes.end());
      }
    return retval;
  }

//! @brief Calcula el número de divisiones de las líneas
//! para que sea compatible con el mallado de las superficies
//! adyacentes.
size_t XC::calcula_ndiv_lados(const std::set<const XC::Edge *> &lados)
  {
    size_t nd= 0;
    for(std::set<const XC::Edge *>::const_iterator i= lados.begin();i!=lados.end();i++)
      nd= std::max(nd,(*i)->NDiv());
    return nd;
  }

//! @brief Devuelve verdadero si la línea toca a la superficie.
bool XC::Edge::Toca(const Face &s) const
  {
    std::set<const Face *>::const_iterator i= sups_linea.find(&s);
    return (i!=sups_linea.end());
  }

//! @brief Devuelve verdadero si la línea toca al cuerpo.
bool XC::Edge::Toca(const Body &b) const
  {
    for(std::set<const Face *>::const_iterator i= sups_linea.begin(); i!=sups_linea.end();i++)
      { if((*i)->Toca(b)) return true; }
    return false;
  }

//! @brief Devuelve verdadero el punto es un extremo de la línea.
bool XC::Edge::Extremo(const Pnt &p) const
  { return ((&p == P1()) || (&p == P2()));  }

//! @brief Devuelve la lista de líneas que tocan al punto.
std::set<const XC::Edge *> XC::GetLineasTocan(const Pnt &p)
  { return p.EdgesTocan(); }

//! @brief Devuelve posiciones de los nodos lo largo de la línea.
MatrizPos3d XC::Edge::get_pos_nodos(void) const
  { return get_posiciones(); }

//! @brief Crea los nodos en los puntos extremos del objeto.
void XC::Edge::crea_nodos_en_extremos(void)
  {
    if(verborrea>4)
      std::clog << "Creando nodos en extremos linea: '" << GetNombre() << "'...";   
    if(!P1())
      {
	std::cerr << "Edge::crea_nodos_en_extremos; no está definido el punto inicial." << std::endl;
        return;
      }
    else
      P1()->crea_nodos();

    Node *nodo_p1= P1()->GetNodo(1,1,1);
    assert(nodo_p1);
    nodos(1,1,1)= nodo_p1; //Nodo del punto inicial.
      
    if(!P2())
      {
	std::cerr << "Edge::crea_nodos_en_extremos; no está definido el punto final." << std::endl;
        return;
      }
    else
      P2()->crea_nodos();

    Node *nodo_p2= P2()->GetNodo(1,1,1);
    assert(nodo_p2);
    const size_t filas= nodos.getNumFilas();
    const size_t cols= nodos.getNumCols();
    nodos(1,filas,cols)= nodo_p2; //Nodo del punto final.

    if(verborrea>4)
     std::cerr << "Edge::crea_nodos_en_extremos(); creados." << std::endl;
  }

//! @brief Crea los nodos del objeto.
void XC::Edge::crea_nodos(void)
  {
    if(verborrea>4)
      std::clog << "Creando nodos linea: '" << GetNombre() << "'...";

 
    if(nodos.Null())
      {
        if(!get_modelador())
          std::cerr << "Edge::crea_nodos; no está definido el modelador." << std::endl;
        else
          {
            const MatrizPos3d posiciones= get_pos_nodos();
            const size_t filas= posiciones.getNumFilas();
            const size_t cols= posiciones.getNumCols();
            nodos= TritrizPtrNod(1,filas,cols);

            crea_nodos_en_extremos();


            if((filas*cols)>2) //Si tiene nodos intermedios...
              {
                const size_t fila_fin= std::max(filas-1,size_t(1));
                const size_t fila_ini= (fila_fin == 1 ? 1 : 2);
                const size_t col_fin= std::max(cols-1,size_t(1));
                const size_t col_ini= (col_fin == 1 ? 1 : 2);
                for(register size_t j= fila_ini;j<=fila_fin;j++)
                  for(register size_t k= col_ini;k<=col_fin;k++)
                    crea_nodo(posiciones(j,k),1,j,k);
              }
          }
        if(verborrea>4)
	  std::clog << "Edge::crea_nodos(); creados " << nodos.NumPtrs() << " nodo(s)." << std::endl;
      }
    else
      {
        if(verborrea>2)
          std::clog << "Edge::crea_nodos; los nodos de la línea: '" << GetNombre() << "' ya existen." << std::endl;
      }
    if(verborrea>4)
      std::clog << "creados." << std::endl;
  }

//! @brief Crea la malla.
void XC::Edge::Malla(dir_mallado dm)
  {
    if(verborrea>3)
      std::clog << "Mallando edge...(" << GetNombre() << ")...";   
    crea_nodos();
    crea_elementos(dm);
    if(verborrea>3)
      std::clog << "hecho." << std::endl;
  }


//! @brief Devuelve un puntero a nodo a partir de los índices que haya en la pila.
const XC::Node *XC::Edge::get_node_ptr_from_stack(const std::string &cod) const
  {
    const Node *retval= nullptr;
    const size_t a= popSize_t(cod);
    retval= GetNodo(a);
    return retval;
  }

//! @brief Devuelve un puntero a elemento a partir de los índices que haya en la pila.
const XC::Element *XC::Edge::get_element_ptr_from_stack(const std::string &cod) const
  {
    const Element *retval= nullptr;
    const size_t a= popSize_t(cod);
    retval= GetElemento(a);
    return retval;
  }

//! @brief Devuelve un vector tangente a la línea en el punto s
const XC::Vector &XC::Edge::getTang(const double &s) const
  {
    static XC::Vector retval(1);
    std::cerr << "Edge::getTang; no implementada." << std::endl;
    return retval;
  }

//! @brief Devuelve los conjuntos a los que pertenece este «edge».
std::set<XC::SetBase *> XC::Edge::get_sets(void) const
  {
    std::set<SetBase *> retval;
    const Modelador *mdlr= GetModelador();
    if(mdlr)
      {
        MapSet &sets= const_cast<MapSet &>(mdlr->get_sets());
        retval= sets.get_sets(this);
      }
    else
      std::cerr << "Edge::get_sets; no se ha definido el modelador." << std::endl;
    return retval;
  }

//! @brief Agrega el «edge» a los conjuntos que se pasan como parámetro.
void XC::Edge::add_to_sets(std::set<SetBase *> &sets)
  {
    for(std::set<SetBase *>::iterator i= sets.begin();i!= sets.end();i++)
      {
        Set *s= dynamic_cast<Set *>(*i);
        if(s) s->GetLineas().push_back(this);
      }
  }

//! Return indices of the vertices.
std::vector<int> XC::Edge::getIndicesVertices(void) const
  {
    const size_t nv= NumVertices();
    std::vector<int> retval(nv);
    if(nv>=1)
      {
        for(size_t i=0;i<nv;i++)
          retval[i]= GetVertice(i+1)->getIdx();
      }
    return retval;
  }

//! @brief Returns k-points.
XC::ID XC::Edge::getKPoints(void) const
  {
    ID retval(2);
    retval[0]= P1()->GetTag();
    retval[1]= P2()->GetTag();
    return retval;
  }


//! Devuelve la propiedad del objeto cuyo código se pasa
//! como parámetro.
any_const_ptr XC::Edge::GetProp(const std::string &cod) const
  {
    static const Pnt *ptr= nullptr;;
    if(cod=="ndiv")
      {
        return any_const_ptr(ndiv);
      }
    else if(cod=="long") //Devuelve la longitud.
      {
        tmp_gp_dbl= getLongitud();
        return any_const_ptr(tmp_gp_dbl);
      }
    else if(cod=="p1")
      {
        ptr= P1();
        return any_const_ptr(ptr);
      }
    else if(cod=="p2")
      {
        ptr= P2();
        return any_const_ptr(ptr);
      }
    else if(cod=="getTagP1")
      {
        tmp_gp_szt= P1()->GetTag();
        return any_const_ptr(tmp_gp_szt);
      }
    else if(cod=="getTagP2")
      {
        tmp_gp_szt= P2()->GetTag();
        return any_const_ptr(tmp_gp_szt);
      }
    else if(cod=="getKPts")
      {
        static ID retval(2);
        retval= getKPoints();
        return any_const_ptr(retval);
      }
    else if(cod=="tang")
      {
        const double s= popDouble(cod);
        return vector_to_prop_vector(getTang(s));
      }
    if(cod == "vertice")
      {
        static const Pnt *vertice;
        const size_t ivertice= popSize_t(cod);
        vertice= GetVertice(ivertice);
        if(vertice)
          return any_const_ptr(vertice);
        else
          {
            std::cerr << "Edge::GetProp; no se encontró el vértice: '" 
                      << ivertice << "'.\n";
            return any_const_ptr();
          }
      }
    else if(cod=="getVertices")
      {
        const size_t nv= NumVertices();
        const std::vector<int> idxVertices= getIndicesVertices();
	static m_int retval;
        retval= m_int(nv,1,0);
        if(nv<1) return any_const_ptr(retval);
        for(size_t i=0;i<nv;i++)
          retval[i]= idxVertices[i];
        return any_const_ptr(retval);
      }
    else if(cod=="getNombresSupsTocan")
      {
        tmp_gp_str= NombresSupsTocan();
        return any_const_ptr(tmp_gp_str);
      }
    else if(cod=="nsup")
      {
        tmp_gp_szt= SupsTocan().size();
        return any_const_ptr(tmp_gp_szt);
      }
    else if(cod=="nodo")
      {
        static const Node *nodo;
        const size_t inodo= popSize_t(cod);
        nodo= GetNodo(inodo);
        if(nodo)
          return any_const_ptr(nodo);
        else
          {
            std::cerr << "Edge::GetProp; no se encontró el nodo de índice: " 
                      << inodo << ".\n";
            return any_const_ptr();
          }
      }
    else if(cod=="primerNodo")
      {
        static const Node *nodo;
        nodo= GetPrimerNodo();
        if(nodo)
          return any_const_ptr(nodo);
        else
          {
            std::cerr << "Edge::GetProp; no se encontró el primer nodo." << std::endl; 
            return any_const_ptr();
          }
      }
    else if(cod=="ultimoNodo")
      {
        static const Node *nodo;
        nodo= GetUltimoNodo();
        if(nodo)
          return any_const_ptr(nodo);
        else
          {
            std::cerr << "Edge::GetProp; no se encontró el primer nodo." << std::endl; 
            return any_const_ptr();
          }
      }
    else if(cod=="getTagPrimerNodo")
      {
        const Node *nodo= GetPrimerNodo();
        if(nodo)
          {
            tmp_gp_int= nodo->getTag();
            return any_const_ptr(tmp_gp_int);
          }
        else
          {
            std::cerr << "Edge::GetProp; no se encontró el primer nodo." << std::endl; 
            return any_const_ptr();
          }
      }
    else if(cod=="getTagUltimoNodo")
      {
        const Node *nodo= GetUltimoNodo();
        if(nodo)
          {
            tmp_gp_int= nodo->getTag();
            return any_const_ptr(tmp_gp_int);
          }
        else
          {
            std::cerr << "Edge::GetProp; no se encontró el último nodo." << std::endl; 
            return any_const_ptr();
          }
      }
    else
      return EntMdlr::GetProp(cod);
  }

