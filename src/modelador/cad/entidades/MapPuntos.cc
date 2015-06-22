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
//MapPuntos.cc

#include "MapPuntos.h"
#include "modelador/Modelador.h"
#include "xc_utils/src/geom/pos_vec/Vector3d.h"
#include "xc_utils/src/geom/d2/Plano3d.h"
#include "xc_utils/src/geom/d1/Recta3d.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"

#include "modelador/cad/entidades/Pnt.h"
#include "modelador/set_mgmt/Set.h"
#include "xc_utils/src/base/utils_any.h"
#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/base/any_const_ptr.h"

//! @brief Constructor.
XC::MapPuntos::MapPuntos(Cad *cad)
  : MapEnt<Pnt>(cad) {}

//! @brief Lee un objeto Pnt desde el archivo de entrada.
//!
//! Soporta los comandos:
//!
//! - pnt: Lee un nuevo punto.
//! - linea: Lee una nueva línea.
//! - arco: Lee un nuevo arco.
//! - sec_lineas: Lee una nueva línea compuesta.
bool XC::MapPuntos::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    const std::string str_err= "(MapPuntos) Procesando comando: " + cmd;
    if(verborrea>2)
      std::clog << str_err << std::endl;
    if(cmd == "tag_punto")
      {
        setTag(interpretaSize_t(status.GetString())); //Nuevo identificador del punto.
        return true;
      }
    else if(cmd == "pnt") //Crea un nuevo punto (o llama a uno existente).
      {
        Nuevo(status);
        return true;
      }
    else if(cmd == "copia_puntos")
      {
        const std::vector<Indice> tmp= crea_vector_size_t(status.GetString());
        Copia(tmp);
        return true;
      }
    else if(cmd == "for_each")
      {
        const std::string bloque= status.GetBloque();
        EjecutaBloqueForEach(status,bloque);
        return true;
      }
    else
      return MapEnt<Pnt>::procesa_comando(status);
  }

//! @brief Inserta el nuevo punto en el conjunto total y los conjuntos abiertos.
void XC::MapPuntos::UpdateSets(Pnt *nuevo_punto) const
  {
    Cad *cad= const_cast<Cad *>(dynamic_cast<const Cad *>(Owner()));
    Modelador *mdlr= cad->getModelador();
    mdlr->get_sets().get_set_total()->GetPuntos().push_back(nuevo_punto);
    mdlr->get_sets().inserta_ent_mdlr(nuevo_punto);
    MapSet::map_sets &abiertos= mdlr->get_sets().get_sets_abiertos();
    for(MapSet::map_sets::iterator i= abiertos.begin();i!= abiertos.end();i++)
      {
        Set *ptr_set= dynamic_cast<Set *>((*i).second);
        assert(ptr_set);
        ptr_set->GetPuntos().push_back(nuevo_punto);
      }
  }

//! @brief Devuelve el vector definido por los puntos cuyos índices se pasan como parámetro.
Vector3d XC::MapPuntos::getVector(const Indice &i,const Indice &j) const
  {
    Vector3d retval;
    const Pnt *pA= busca(i);
    const Pnt *pB= busca(j);
    if(!pA)
      std::cerr << "MapPuntos::getVector; no se encontró el punto: " << i << std::endl;
    else if(!pB)
      std::cerr << "MapPuntos::getVector; no se encontró el punto: " << j << std::endl;
    else
      retval= Vector3d(pA->GetPos(),pB->GetPos());
    return retval;
  }

//! @brief Devuelve la recta que pasa por los puntos cuyos índices se pasan como parámetro.
Recta3d XC::MapPuntos::getRecta(const Indice &i,const Indice &j) const
  {
    Recta3d retval;
    const Pnt *pA= busca(i);
    const Pnt *pB= busca(j);
    if(!pA)
      std::cerr << "MapPuntos::getRecta; no se encontró el punto: " << i << std::endl;
    else if(!pB)
      std::cerr << "MapPuntos::getRecta; no se encontró el punto: " << j << std::endl;
    else
      retval= Recta3d(pA->GetPos(),pB->GetPos());
    return retval;
  }

//! @brief Devuelve el plano que pasa por los puntos cuyos índices se pasan como parámetro.
Plano3d XC::MapPuntos::getPlano(const Indice &i,const Indice &j,const Indice &k) const
  {
    Plano3d retval;
    const Pnt *pA= busca(i);
    const Pnt *pB= busca(j);
    const Pnt *pC= busca(k);
    if(!pA)
      std::cerr << "MapPuntos::getRecta; no se encontró el punto: " << i << std::endl;
    else if(!pB)
      std::cerr << "MapPuntos::getRecta; no se encontró el punto: " << j << std::endl;
    else if(!pC)
      std::cerr << "MapPuntos::getRecta; no se encontró el punto: " << k << std::endl;
    else
      retval= Plano3d(pA->GetPos(),pB->GetPos(),pC->GetPos());
    return retval;
  }


//! @brief Crea un nuevo punto.
XC::Pnt *XC::MapPuntos::Crea(void)
  {
    Modelador *mdlr= getModelador();
    assert(mdlr);
    Pnt *retval= new Pnt("p"+boost::lexical_cast<std::string>(getTag()),mdlr);
    (*this)[getTag()]= retval;
    UpdateSets(retval);
    tag++;
    return retval;
  }

//! @brief Crea un nuevo punto.
XC::Pnt *XC::MapPuntos::Nuevo(void)
  {
    Pnt *retval= busca(getTag());
    if(!retval) //El punto es nuevo.
      retval= Crea();
    return retval;
  }


//! @brief Crea un nuevo punto en la posición que se pasa como parámetro.
XC::Pnt *XC::MapPuntos::Nuevo(const Pos3d &pos)
  {
    Pnt *retval= busca(getTag());
    if(retval)
      std::cerr << "MapPuntos::Nuevo; el punto de índice: " 
                << getTag() << " ya existe, no se hacen cambios." << std::endl;
    else //El punto es nuevo.
      {
        retval= Crea();
        retval->Pos()= pos;
      }
    return retval;
  }

//! @brief Lee un nuevo punto desde archivo.
XC::Pnt *XC::MapPuntos::Nuevo(const size_t &tag,const Pos3d &pos)
  {
    bool nuevo= true;
    size_t old_tag= getTag();
    Pnt *retval= nullptr;
    setTag(tag); //Identificador del punto.
    retval= busca(getTag());
    if(retval)
      nuevo= false;
    else
      retval= Crea();
    if(!nuevo)
      setTag(old_tag);
    retval->setPos(pos); //Asigna la posición
    return retval;
  }


//! @brief Lee un nuevo punto desde archivo.
XC::Pnt *XC::MapPuntos::Nuevo(CmdStatus &status)
  {
    std::deque<boost::any> fnc_indices= status.Parser().SeparaIndices(this);
    bool nuevo= true;
    size_t old_tag= getTag();
    Pnt *retval= nullptr;
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

//! @brief Crea un nuevo punto, copia del que se pasa como parámetro con
//! el nombre que le toca según el valor del tag. Las coordenadas del
//! nuevo punto serán las que resulten de sumar a las del primitivo el
//! vector que se pasa como parámetro.
XC::Pnt *XC::MapPuntos::Copia(const Pnt *p,const Vector3d &v= Vector3d())
  {
    Pnt *retval= busca(getTag());
    if(!p)
      {
        std::cerr << "MapPuntos::CopiaPunto; el puntero al punto original es nulo." << std::endl;
        return retval;
      }
    if(retval)
      std::cerr << "MapPuntos::CopiaPunto; el punto con identificador: " 
                << getTag() << " ya existe, no se hacen cambios." << std::endl;
    else //El punto es nuevo.
      {
        retval= new Pnt(*p);
        if(retval)
          {
            if(!v.Nulo())
              retval->Pos()+= v;
            retval->Nombre()= "p"+boost::lexical_cast<std::string>(getTag());
            (*this)[getTag()]= retval;
            UpdateSets(retval);
            tag++;
	  }
        else
	  std::cerr << "MapPuntos::CopiaPunto; memoria agotada." << std::endl; 
      }
    return retval;
  }

//! @brief Crea copias de los puntos cuyos identificadores se pasan como parámetro.
void XC::MapPuntos::Copia(const std::vector<Indice> &indices)
  {
    for(std::vector<Indice>::const_iterator i= indices.begin();i!=indices.end();i++)
      {
        Pnt *original= busca(*i);
        if(original)
          Copia(original);
        else
	  std::cerr << "MapPuntos::CopiaPuntos; no se encontró el punto de índice: " << *i << ".\n";
      }
  }

//! @brief Aplica la transformación a los puntos cuyos identificadores se pasan como parámetro.
void XC::MapPuntos::Transforma(const TrfGeom &trf,const std::vector<Indice> &indices)
  {
    for(std::vector<Indice>::const_iterator i= indices.begin();i!=indices.end();i++)
      {
        Pnt *p= busca(*i);
        if(p)
          p->Transforma(trf);
        else
	  std::cerr << "MapPuntos::TransformaPuntos; no se encontró el punto de índice: " << *i << ".\n";
      }
  }

//! @brief Devuelve la distancia entre los puntos cuyos identificadores se pasan como parámetro.
double XC::MapPuntos::Dist(const Indice &i,const Indice &j) const
  {
    double retval(-1.0);
    const Pnt *pA= busca(i);
    const Pnt *pB= busca(j);
    if(!pA)
      std::cerr << "MapPuntos::DistPuntos; no se encontró el punto: " 
                << i << std::endl;
    else if(!pB)
      std::cerr << "MapPuntos::DistPuntos; no se encontró el punto: " 
                << j << std::endl;
    else
      retval= pA->DistanciaA(pB->GetPos());
    return retval;
  }

//! Devuelve la propiedad del objeto cuyo código se pasa
//! como parámetro.
any_const_ptr XC::MapPuntos::GetProp(const std::string &cod) const
  {
    if(cod == "pnt")
      {
        static const Pnt *punto;
        const size_t iPunto= popSize_t(cod);
        punto= busca(iPunto);
        if(punto)
          return any_const_ptr(punto);
        else
          {
            std::cerr << "MapPuntos::GetProp; no se encontró el punto: '" 
                      << iPunto << "'.\n";
            return any_const_ptr();
          }
      }
    else if(cod=="getTagNearestPnt")
      {
        const Pos3d p= popPos3d(cod);
        const Pnt *tmp= getNearest(p);
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
    else if(cod=="num_puntos")
      {
        tmp_gp_szt= this->size();
        return any_const_ptr(tmp_gp_szt);
      }
    else if(cod=="dist_puntos")
      {
        size_t iPuntoA= 0, iPuntoB= 0;
        if(InterpreteRPN::Pila().size()>1)
          {
            iPuntoA= convert_to_size_t(InterpreteRPN::Pila().Pop());
            iPuntoB= convert_to_size_t(InterpreteRPN::Pila().Pop());
          }
        else
          err_num_argumentos(std::cerr,2,"GetProp",cod);
        tmp_gp_dbl= Dist(iPuntoA,iPuntoB);
        return any_const_ptr(tmp_gp_dbl);
      }
    else
      return MapEnt<Pnt>::GetProp(cod);
  }
