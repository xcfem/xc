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
//NodeLoader.cc

#include "NodeLoader.h"
#include "preprocessor/cad/SisRef.h"
#include "domain/domain/Domain.h"
#include "domain/mesh/node/Node.h"
#include "preprocessor/Preprocessor.h"
#include "xc_utils/src/geom/pos_vec/Pos3d.h"
#include "xc_utils/src/geom/pos_vec/Pos2d.h"
#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/base/utils_any.h"
#include "boost/any.hpp"
#include "xc_utils/src/base/any_const_ptr.h"
#include "domain/mesh/element/Element.h"
#include "utility/tagged/DefaultTag.h"

void XC::NodeLoader::libera(void)
  {
    if(nodo_semilla) delete nodo_semilla;
    nodo_semilla= nullptr; 
  }

XC::NodeLoader::NodeLoader(Preprocessor *preprocessor)
  : Loader(preprocessor), ngdl_def_nodo(2),ncoo_def_nodo(3),nodo_semilla(nullptr) {}

//! @brief Lee un objeto NodeLoader desde archivo
bool XC::NodeLoader::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(NodeLoader) Procesando comando: " << cmd << std::endl;
    if(cmd == "ncoo_def")
      {
        ncoo_def_nodo= interpretaInt(status.GetString());
        if(verborrea>1)
	  std::clog << ncoo_def_nodo << " coordenadas por nodo." << std::endl;
        return true;
      }
    else if(cmd == "tag_nodo")
      {
	setDefaultTag(interpretaInt(status.GetString()));
        return true;
      }
    else if(cmd == "ngdl_def") //Número de grados de libertad por nodo.
      {
        ngdl_def_nodo= interpretaInt(status.GetString());
        if(verborrea>1)
	  std::clog << ngdl_def_nodo << " grados de libertad por nodo." << std::endl;
        return true;
      }
    else if((cmd == "def_nod"))
      {
        Node *ptrNod= lee_nodo(status);
        getDomain()->addNode(ptrNod);
        modelador->UpdateSets(ptrNod);
        return true;
      }
    else if(cmd == "nod")
      {
        Node *ptrNod= lee_nodo(status);
        if(ptrNod)
          {
            if(!getDomain()->existNode(ptrNod->getTag())) //El nodo es nuevo.
              {
                getDomain()->addNode(ptrNod);
                modelador->UpdateSets(ptrNod);
              }
          }
        return true;
      }
    else if(cmd == "duplica_nod")
      {
        const int iNod= interpretaInt(status.GetString());
        duplicateNode(iNod);
        return true;
      }
    else if(cmd == "nod_semilla")
      {
        if(nodo_semilla) delete(nodo_semilla);
        nodo_semilla= lee_nodo(status);
        return true;
      }
    else if(cmd == "for_each")
      {
        const std::string bloque= status.GetBloque();
        getDomain()->EjecutaBloqueForEachNode(status,bloque);
        return true;
      }
    else
      return Loader::procesa_comando(status);
  }

//! @brief Destructor.
XC::NodeLoader::~NodeLoader(void)
  { libera(); }

//! @brief Devuelve el tag por defecto para el siguiente nodo.
int XC::NodeLoader::getDefaultTag(void) const
  { return Node::getDefaultTag().getTag(); }

//! @brief Asigna el tag por defecto para el siguiente nodo.
void XC::NodeLoader::setDefaultTag(const int &tag)
  { Node::getDefaultTag().setTag(tag); }

//! @brief Borra todos los nodos y 
void XC::NodeLoader::clearAll(void)
  {
    libera();
    setDefaultTag(0);
  }

//! @brief Carga un nodo desde archivo.
XC::Node *XC::NodeLoader::lee_nodo(CmdStatus &status)
  {
    int ngdl= ngdl_def_nodo; //No. grados de libertad por defecto.
    std::deque<boost::any> fnc_indices= status.Parser().SeparaIndices(this);
    int tag_nodo= getDefaultTag();
    if(fnc_indices.size()>0)
      tag_nodo= convert_to_int(fnc_indices[0]); //Tag del nodo.
    if(fnc_indices.size()>1)
      ngdl= convert_to_int(fnc_indices[1]); //No. de grados de libertad del nodo.
    Node *nod= getDomain()->getNode(tag_nodo);
    if(!nod) //El nodo no existe; lo creamos.
      nod= new_node(tag_nodo,ncoo_def_nodo,ngdl,0.0,0.0,0.0);
    if(nod) nod->LeeCmd(status);
    return nod;
  }


XC::Node *XC::NodeLoader::new_node(const int &tag,const size_t &dim,const int &ngdl,const double &x,const double &y,const double &z)
  {
    Node *retval= nullptr;
    switch(dim)
      {
      case 1:
        retval= new Node(tag,ngdl,x);
        break;
      case 2:
        retval= new Node(tag,ngdl,x,y);
        break;
      default:
        retval= new Node(tag,ngdl,x,y,z);
        break;
      }
    return retval;
  }

XC::Node *XC::NodeLoader::duplicateNode(const int &tagNodoOrg)
  {
    Node *retval= nullptr;
    Node *ptr_nodo_org= getDomain()->getNode(tagNodoOrg);
    if(!ptr_nodo_org)
      std::cerr << "Preprocessor::nuevoNodo; no se encontró el nodo de tag:"
                << tagNodoOrg << std::endl;
    else
      {
        const int ngdl= ptr_nodo_org->getNumberDOF();    
        retval= new Node(getDefaultTag(),ngdl,ptr_nodo_org->getCrds());
        if(retval)
          {
            getDomain()->addNode(retval);
            modelador->UpdateSets(retval);
          }
      }
    return retval;
  }

XC::Node *XC::NodeLoader::nuevoNodo(const double &x,const double &y,const double &z)
  {
    const int tg= getDefaultTag(); //Before seed node creation.
    if(!nodo_semilla)
      nodo_semilla= new_node(0,ncoo_def_nodo,ngdl_def_nodo,0.0,0.0,0.0);

    const size_t dim= nodo_semilla->getDim();
    const int ngdl= nodo_semilla->getNumberDOF();
    Node *retval= new_node(tg,dim,ngdl,x,y,z);
    if(retval)
      {
        getDomain()->addNode(retval);
        modelador->UpdateSets(retval);
      }
    return retval;
  }

XC::Node *XC::NodeLoader::nuevoNodo(const double &x,const double &y)
  {
    const int tg= getDefaultTag(); //Before seed node creation.
    if(!nodo_semilla)
      nodo_semilla= new_node(0,ncoo_def_nodo,ngdl_def_nodo,0.0,0.0);

    const size_t dim= nodo_semilla->getDim();
    const int ngdl= nodo_semilla->getNumberDOF();
    Node *retval= new_node(tg,dim,ngdl,x,y);
    if(retval)
      {
        getDomain()->addNode(retval);
        modelador->UpdateSets(retval);
      }
    return retval;
  }


XC::Node *XC::NodeLoader::nuevoNodo(const double &x)
  {
    const int tg= getDefaultTag(); //Before seed node creation.
    if(!nodo_semilla)
      nodo_semilla= new_node(0,ncoo_def_nodo,ngdl_def_nodo,0.0);

    const size_t dim= nodo_semilla->getDim();
    const int ngdl= nodo_semilla->getNumberDOF();
    Node *retval= new_node(tg,dim,ngdl,x);
    if(retval)
      {
        getDomain()->addNode(retval);
        modelador->UpdateSets(retval);
      }
    return retval;
  }

//! @brief Crea un nodo en la posición que se pasa como parámetro.
XC::Node *XC::NodeLoader::nuevoNodo(const Pos3d &p)
  { return nuevoNodo(p.x(),p.y(),p.z()); }

//! @brief Crea un nodo en la posición que se pasa como parámetro.
XC::Node *XC::NodeLoader::nuevoNodo(const Pos2d &p)
  { return nuevoNodo(p.x(),p.y()); }

//! @brief Crea un nodo en la posición que se pasa como parámetro.
XC::Node *XC::NodeLoader::nuevoNodo(const Vector &coo)
  {
    int sz= coo.Size();
    Node *retval= nullptr;
    if(sz==1)
      retval= nuevoNodo(coo[0]);
    else if(sz==2)
      retval= nuevoNodo(coo[0],coo[1]);
    else if(sz>=3)
      retval= nuevoNodo(coo[0],coo[1],coo[2]);
    else
      std::cerr << "NodeLoader::newNodeIDV; vector empty."
                << std::endl;
    return retval;
  }

//! @brief Defines the seed node.
XC::Node *XC::NodeLoader::newSeedNode(void)
  {
    libera();
    nodo_semilla= new_node(0,ncoo_def_nodo,ngdl_def_nodo,0.0,0.0,0.0);
    return nodo_semilla;
  }

XC::Node *XC::NodeLoader::newNodeIDV(const int &tag,const Vector &coo)
  { 
    setDefaultTag(tag);
    return nuevoNodo(coo);
  } 

XC::Node *XC::NodeLoader::newNodeIDXYZ(const int &tag,const double &x,const double &y,const double &z)
  { 
    setDefaultTag(tag);
    Node *retval= nuevoNodo(x,y,z);
    return retval;
  } 

XC::Node *XC::NodeLoader::newNodeIDXY(const int &tag,const double &x,const double &y)
  {
    setDefaultTag(tag);
    return nuevoNodo(x,y);
  } 


//! @brief Crea el nodo cuyo identificador se pasa como parámetro.
XC::Node *XC::NodeLoader::getNode(const int &tag)
  { return getDomain()->getNode(tag); }

//! @brief Calcula las reacciones en los nodos.
void XC::NodeLoader::calculateNodalReactions(bool inclInertia)
  {
    getDomain()->calculateNodalReactions(inclInertia,1e-4);
  }

//! \brief Devuelve la propiedad del objeto cuyo código (de la propiedad) se pasa
//! como parámetro.
any_const_ptr XC::NodeLoader::GetProp(const std::string &cod) const
  {
    if(cod=="tag_nodo")
      return any_const_ptr(getDefaultTag());
    else if(cod=="last_tag_nodo")
      {
        tmp_gp_szt= getDefaultTag()-1;
        return any_const_ptr(tmp_gp_szt);
      }
    else if(cod=="dist_inicial")
      {
        size_t iNodoA= 0, iNodoB= 0;
        if(InterpreteRPN::Pila().size())
          {
            iNodoA= convert_to_size_t(InterpreteRPN::Pila().Pop());
            iNodoB= convert_to_size_t(InterpreteRPN::Pila().Pop());
          }
        else
          err_num_argumentos(std::cerr,2,"GetProp",cod);
        const Node *ptrNodA= getDomain()->getNode(iNodoA);
        const Node *ptrNodB= getDomain()->getNode(iNodoB);
        tmp_gp_dbl= dist(ptrNodA->getPosInicial3d(),ptrNodB->getPosInicial3d());
        return any_const_ptr(tmp_gp_dbl);
      }
    else if(cod=="dist_final")
      {
        size_t iNodoA= 0, iNodoB= 0;
        if(InterpreteRPN::Pila().size())
          {
            iNodoA= convert_to_size_t(InterpreteRPN::Pila().Pop());
            iNodoB= convert_to_size_t(InterpreteRPN::Pila().Pop());
          }
        else
          err_num_argumentos(std::cerr,2,"GetProp",cod);
        const Node *ptrNodA= getDomain()->getNode(iNodoA);
        const Node *ptrNodB= getDomain()->getNode(iNodoB);
        tmp_gp_dbl= dist(ptrNodA->getPosFinal3d(),ptrNodB->getPosFinal3d()); 
        return any_const_ptr(tmp_gp_dbl);
      }
    else
      return Loader::GetProp(cod);
  }
