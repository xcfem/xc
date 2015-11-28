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
//SetEstruct.cc

#include "SetEstruct.h"
#include "preprocessor/Preprocessor.h"
#include "preprocessor/cad/matrices/TritrizPtrElem.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"
#include "domain/mesh/element/ElementEdges.h"
#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "xc_utils/src/base/utils_any.h"
#include "xc_utils/src/base/Lista.h"
#include "xc_utils/src/nucleo/InterpreteRPN.h"

//! @brief Constructor.
XC::SetEstruct::SetEstruct(const std::string &nmb,Preprocessor *md)
  : SetBase(nmb,md) {}

//! @brief Constructor de copia.
XC::SetEstruct::SetEstruct(const SetEstruct &otro)
  : SetBase(otro) {}

//! @brief Operator asignación.
XC::SetEstruct &XC::SetEstruct::operator=(const SetEstruct &otro)
  {
    SetBase::operator=(otro);
    return *this;
  }

size_t XC::SetEstruct::NumNodos(void) const
  { return GetNumCapasNodos()*GetNumFilasNodos()*GetNumColsNodos(); }
size_t XC::SetEstruct::NumElementos(void) const
  { return GetNumCapasElementos()*GetNumFilasElementos()*GetNumColsElementos(); }

//! @brief Devuelve verdadero si el único índice que varía es el I.
bool XC::SetEstruct::EsFilaI(void) const
  {
    if((GetNumFilasNodos()>1) || (GetNumColsNodos()>1)) return false;
    return true;
  }

//! @brief Devuelve verdadero si el único índice que varía es el J.
bool XC::SetEstruct::EsFilaJ(void) const
  {
    if((GetNumCapasNodos()>1) || (GetNumColsNodos()>1)) return false;
    return true;
  }

//! @brief Devuelve verdadero si el único índice que varía es el K.
bool XC::SetEstruct::EsFilaK(void) const
  {
    if((GetNumCapasNodos()>1) || (GetNumFilasNodos()>1)) return false;
    return true;
  }

//! @brief Devuelve verdadero si los indices que varían son los J y K.
bool XC::SetEstruct::EsCapaICte(void) const
  { return (GetNumCapasNodos()==1); }

//! @brief Devuelve verdadero si los indices que varían son los I y K.
bool XC::SetEstruct::EsCapaJCte(void) const
  { return (GetNumFilasNodos()==1); }

//! @brief Devuelve verdadero si los indices que varían son los I y J.
bool XC::SetEstruct::EsCapaKCte(void) const
  { return (GetNumColsNodos()==1); }

//! @brief Devuelve el tipo del conjunto filaI,filaJ,filaK,capaICte,...
std::string XC::SetEstruct::GetStrTipo(void) const
  {
    if(EsFilaI()) return "filaI";
    if(EsFilaJ()) return "filaJ";
    if(EsFilaK()) return "filaK";
    if(EsCapaICte()) return "capaICte";
    if(EsCapaJCte()) return "capaJCte";
    if(EsCapaKCte()) return "capaKCte";
    return "bloqueIJK";
  }

//! @brief Devuelve la dimensión del rango (0,1,2 ó 3).
size_t XC::SetEstruct::Dimension(void) const
  {
    size_t retval= 0;
    if(GetNumCapasNodos()>1) retval++;
    if(GetNumFilasNodos()>1) retval++;
    if(GetNumColsNodos()> 1) retval++;
    return retval;
  }

//! @brief Devuelve los punteros a los nodos del conjunto.
XC::NodePtrSet XC::SetEstruct::getNodePtrSet(void)
  {
    NodePtrSet retval;
    const size_t ncapas= GetNumCapasNodos();
    const size_t nfilas= GetNumFilasNodos();
    const size_t ncols= GetNumColsNodos();
    for(size_t i= 1;i<=ncapas;i++)
      for(size_t j= 1;j<=nfilas;j++)
        for(size_t k= 1;k<=ncols;k++)
          retval.insert(GetNodo(i,j,k));
    return retval;    
  }

//! @brief Devuelve los bordes de elemento que cuyos dos extremos
//! están entre los nodos del conjunto.
XC::ElementEdges XC::SetEstruct::getElementEdges(void)
  {
    const NodePtrSet nodos= getNodePtrSet();
    return getElementEdgesEntreNodos(nodos);
  }

//!  @brief Lee comandos para un objeto SetEstruct desde el archivo de entrada.
bool XC::SetEstruct::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(SetEstruct) Procesando comando: " << cmd << std::endl;

    if(cmd == "for_each_nod") //Bucle sobre nodos.
      {
        const std::string nmbBlq= nombre_clase()+":for_each_nod";
        const std::string bloque= status.GetBloque();
        const size_t ncapas= GetNumCapasNodos();
        if(ncapas<1) return true;
        const size_t nfilas= GetNumFilasNodos();
        const size_t ncols= GetNumColsNodos();
        for(size_t i= 1;i<=ncapas;i++)
          for(size_t j= 1;j<=nfilas;j++)
            for(size_t k= 1;k<=ncols;k++)
              GetNodo(i,j,k)->EjecutaBloque(status,bloque,nmbBlq);
        return true;
      }
    else if(cmd == "for_each_elem") //Bucle sobre elementos.
      {
        const std::string nmbBlq= nombre_clase()+":for_each_elem";
        const std::string bloque= status.GetBloque();
        const size_t ncapas= GetNumCapasElementos();
        if(ncapas<1) return true;
        const size_t nfilas= GetNumFilasElementos();
        const size_t ncols= GetNumColsElementos();
        for(size_t i= 1;i<=ncapas;i++)
          for(size_t j= 1;j<=nfilas;j++)
            for(size_t k= 1;k<=ncols;k++)
              GetElemento(i,j,k)->EjecutaBloque(status,bloque,nmbBlq);
        return true;
      }
    else if(cmd == "for_each_element_edge") //Bucle sobre bordes de elemento.
      {
        ElementEdges edges= getElementEdges();
        edges.LeeCmd(status);
        return true;
      }
    else
      return SetBase::procesa_comando(status);
  }

//! @brief Agrega al modelo los elementos que se pasan como parámetro.
void XC::SetEstruct::agrega_elementos(const TritrizPtrElem &elementos)
  {
    const size_t capas= elementos.GetCapas();
    if(capas<1) return;
    const size_t filas= elementos(1).getNumFilas();
    const size_t cols= elementos(1).getNumCols();
    for(register size_t i= 1;i<=capas;i++)
      for(register size_t j= 1;j<=filas;j++)
        for(register size_t k= 1;k<=cols;k++)
          get_preprocessor()->getElementLoader().Agrega(elementos(i,j,k));
  }

//! @brief Devuelve un puntero a nodo a partir de los índices que haya en la pila.
const XC::Node *XC::SetEstruct::get_node_ptr_from_stack(const std::string &cod) const
  {
    const Node *retval= nullptr;
    if(InterpreteRPN::Pila().size()>2)
      {
        const size_t c= convert_to_size_t(InterpreteRPN::Pila().Pop());
        const size_t b= convert_to_size_t(InterpreteRPN::Pila().Pop());
        const size_t a= convert_to_size_t(InterpreteRPN::Pila().Pop());
        retval= GetNodo(a,b,c);
      }
    else
      err_num_argumentos(std::cerr,3,"GetProp",cod);
    return retval;
  }

//! @brief Devuelve un puntero a elemento a partir de los índices que haya en la pila.
const XC::Element *XC::SetEstruct::get_element_ptr_from_stack(const std::string &cod) const
  {
    const Element *retval= nullptr;
    if(InterpreteRPN::Pila().size()>2)
      {
        const size_t c= convert_to_size_t(InterpreteRPN::Pila().Pop());
        const size_t b= convert_to_size_t(InterpreteRPN::Pila().Pop());
        const size_t a= convert_to_size_t(InterpreteRPN::Pila().Pop());
        retval= GetElemento(a,b,c);
      }
    else if(InterpreteRPN::Pila().size()>1)
      {
        const size_t b= convert_to_size_t(InterpreteRPN::Pila().Pop());
        const size_t a= convert_to_size_t(InterpreteRPN::Pila().Pop());
        retval= GetElemento(a,b);
      }
    else if(InterpreteRPN::Pila().size()>0)
      {
        const size_t a= convert_to_size_t(InterpreteRPN::Pila().Pop());
        retval= GetElemento(a);
      }
    else
      err_num_argumentos(std::cerr,1,"GetProp",cod);
    return retval;
  }

//! @brief Devuelve los tags de los nodos.
std::set<int> XC::SetEstruct::getNodeTags(void) const
  {
    std::set<int> retval;
    const size_t ncapas= GetNumCapasNodos();
    if(ncapas>0)
      {
        const size_t nfilas= GetNumFilasNodos();
        const size_t ncols= GetNumColsNodos();
        for(size_t i= 1;i<=ncapas;i++)
          for(size_t j= 1;j<=nfilas;j++)
            for(size_t k= 1;k<=ncols;k++)
              retval.insert(GetNodo(i,j,k)->getTag());
      }
    return retval;
  }

boost::python::list XC::SetEstruct::getNodes(void)
  {
    boost::python::list retval;
    const size_t ncapas= GetNumCapasNodos();
    if(ncapas>0)
      {
        const size_t nfilas= GetNumFilasNodos();
        const size_t ncols= GetNumColsNodos();
        for(size_t i= 1;i<=ncapas;i++)
          for(size_t j= 1;j<=nfilas;j++)
            for(size_t k= 1;k<=ncols;k++)
              {
                Node *tmp= GetNodo(i,j,k);
                boost::python::object pyObj(boost::ref(*tmp));
                retval.append(pyObj);
              }
      }
    return retval;
  }


//! @brief Devuelve los tags de los elementos.
std::set<int> XC::SetEstruct::getElementTags(void) const
  {
    std::set<int> retval;
    const size_t ncapas= GetNumCapasElementos();
    if(ncapas>0)
      {
        const size_t nfilas= GetNumFilasElementos();
        const size_t ncols= GetNumColsElementos();
        for(size_t i= 1;i<=ncapas;i++)
          for(size_t j= 1;j<=nfilas;j++)
            for(size_t k= 1;k<=ncols;k++)
              retval.insert(GetElemento(i,j,k)->getTag());
      }
    return retval;
  }

boost::python::list XC::SetEstruct::getElements(void)
  {
    boost::python::list retval;
    const size_t ncapas= GetNumCapasElementos();
    if(ncapas>0)
      {
        const size_t nfilas= GetNumFilasElementos();
        const size_t ncols= GetNumColsElementos();
        for(size_t i= 1;i<=ncapas;i++)
          for(size_t j= 1;j<=nfilas;j++)
            for(size_t k= 1;k<=ncols;k++)
              {
                Element *tmp= GetElemento(i,j,k);
                boost::python::object pyObj(boost::ref(*tmp));
                retval.append(pyObj);
              }
      }
    return retval;
  }

//! @brief Devuelve verdadero si el nodo está en el conjunto.
bool XC::SetEstruct::In(const Node *n) const
  {
    bool retval= false;
    const size_t ncapas= GetNumCapasNodos();
    if(ncapas>0)
      {
        const size_t nfilas= GetNumFilasNodos();
        const size_t ncols= GetNumColsNodos();
        for(size_t i= 1;i<=ncapas;i++)
          for(size_t j= 1;j<=nfilas;j++)
            for(size_t k= 1;k<=ncols;k++)
              if(GetNodo(i,j,k)==n)
                {
                  retval= true;
                  break;
                }
      }
    return retval;
  }

//! @brief Devuelve verdadero si el elemento está en el conjunto.
bool XC::SetEstruct::In(const Element *e) const
  {
    bool retval= false;
    const size_t ncapas= GetNumCapasElementos();
    if(ncapas>0)
      {
        const size_t nfilas= GetNumFilasElementos();
        const size_t ncols= GetNumColsElementos();
        for(size_t i= 1;i<=ncapas;i++)
          for(size_t j= 1;j<=nfilas;j++)
            for(size_t k= 1;k<=ncols;k++)
              if(GetElemento(i,j,k)==e)
                {
                  retval= true;
                  break;
                }
      }
    return retval;
  }


XC::Node *XC::SetEstruct::getNodeI(const size_t &i)
  {
    if(EsFilaI())
      return GetNodo(i,1,1);
    else if(EsFilaJ())
      return GetNodo(1,i,1);
    else if(EsFilaK())
      return GetNodo(1,1,i);
    else
      {
	std::cerr << "SetEstruct::getNodoI; el conjunto de nodos no es unidimensional." << std::endl;
        return nullptr;
      }
  }

XC::Node *XC::SetEstruct::getNodeIJ(const size_t &i,const size_t &j)
  {
    //XXX Falta obtener la capa (no ncesariemente será la 1),
    if(EsCapaICte())
      return GetNodo(1,i,j);
    else if(EsCapaJCte()) 
      return GetNodo(i,1,j);
    else if(EsCapaKCte()) 
      return GetNodo(i,j,1);
    else
      {
	std::cerr << "SetEstruct::getNodoI; el conjunto de nodos no es bidimensional." << std::endl;
        return nullptr;
      } 
  }

//! @brief Devuelve la propiedad del objeto cuyo código se pasa
//! como parámetro.
any_const_ptr XC::SetEstruct::GetProp(const std::string &cod) const
  {
    if(verborrea>4)
      std::clog << "SetEstruct::GetProp (" << nombre_clase() << "::GetProp) Buscando propiedad: " << cod << std::endl;

    if(cod=="nCapasNod") //Devuelve el número de capas de nodos.
      {
        tmp_gp_szt= GetNumCapasNodos();
        return any_const_ptr(tmp_gp_szt);
      }
    else if(cod=="nFilasNod") //Devuelve el número de filas de nodos.
      {
        tmp_gp_szt= GetNumFilasNodos();
        return any_const_ptr(tmp_gp_szt);
      }
    else if(cod=="nColsNod") //Devuelve el número de columnas de nodos.
      {
        tmp_gp_szt= GetNumColsNodos();
        return any_const_ptr(tmp_gp_szt);
      }
    else if(cod=="nCapasElem") //Devuelve el número de capas de elementos.
      {
        tmp_gp_szt= GetNumCapasElementos();
        return any_const_ptr(tmp_gp_szt);
      }
    else if(cod=="nFilasElem") //Devuelve el número de filas de elementos.
      {
        tmp_gp_szt= GetNumFilasElementos();
        return any_const_ptr(tmp_gp_szt);
      }
    else if(cod=="nColsElem") //Devuelve el número de columnas de elementos.
      {
        tmp_gp_szt= GetNumColsElementos();
        return any_const_ptr(tmp_gp_szt);
      }
    else if(cod=="dim") //Devuelve la dimensión.
      {
        tmp_gp_szt= Dimension();
        return any_const_ptr(tmp_gp_szt);
      }
    else if(cod=="tipo") //Devuelve el tipo.
      {
        tmp_gp_str= GetStrTipo();
        return any_const_ptr(tmp_gp_str);
      }
    else if(cod=="esFilaI")
      {
        tmp_gp_bool= EsFilaI();
        return any_const_ptr(tmp_gp_str);
      }
    else if(cod=="esFilaJ")
      {
        tmp_gp_bool= EsFilaJ();
        return any_const_ptr(tmp_gp_str);
      }
    else if(cod=="esFilaK")
      {
        tmp_gp_bool= EsFilaK();
        return any_const_ptr(tmp_gp_str);
      }
    else if(cod=="esCapaICte")
      {
        tmp_gp_bool= EsCapaICte();
        return any_const_ptr(tmp_gp_str);
      }
    else if(cod=="esCapaJCte")
      {
        tmp_gp_bool= EsCapaJCte();
        return any_const_ptr(tmp_gp_str);
      }
    else if(cod=="esCapaKCte")
      {
        tmp_gp_bool= EsCapaKCte();
        return any_const_ptr(tmp_gp_str);
      }
    else if(cod=="nod")
      {
        const Node *ptr_nod= get_node_ptr_from_stack(cod);
        if(ptr_nod)
          return any_const_ptr(ptr_nod);
        else
          return any_const_ptr();
      }
    else if(cod=="tagNod")
      {
        const Node *ptr_nod= get_node_ptr_from_stack(cod);
        if(ptr_nod)
          tmp_gp_szt= ptr_nod->getTag();
        return any_const_ptr(tmp_gp_szt);
      }
    else if(cod=="elem")
      {
        const Element *ptr_elem= get_element_ptr_from_stack(cod);
        if(ptr_elem)
          return any_const_ptr(ptr_elem);
        else
          return any_const_ptr();
      }
    else if(cod=="tagElem")
      {
        const Element *ptr_elem= get_element_ptr_from_stack(cod);
        if(ptr_elem)
          tmp_gp_szt= ptr_elem->getTag();
        return any_const_ptr(tmp_gp_szt);
      }
    else
      return SetBase::GetProp(cod);
  }
