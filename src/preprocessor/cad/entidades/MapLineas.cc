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
//MapLineas.cc

#include "MapLineas.h"
#include "preprocessor/Preprocessor.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"

#include "preprocessor/cad/entidades/Edge.h"
#include "preprocessor/cad/entidades/Pnt.h"
#include "preprocessor/cad/entidades/Linea.h"
#include "preprocessor/cad/entidades/LineaTramos.h"
#include "preprocessor/cad/entidades/CmbEdge.h"
#include "preprocessor/cad/entidades/ArcoCircunf.h"
#include "preprocessor/set_mgmt/Set.h"
#include "xc_utils/src/base/utils_any.h"
#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/base/any_const_ptr.h"

//! @brief Constructor.
XC::MapLineas::MapLineas(Cad *cad)
  : MapEnt<Edge>(cad) {}

//! @brief Line segment.
XC::Linea *XC::MapLineas::newLine(const size_t &id_p1, const size_t &id_p2)
  {
    Preprocessor *preprocessor= getPreprocessor();
    assert(preprocessor);
    Cad &cad= preprocessor->getCad();
    Pnt *p1= cad.getPuntos().busca(id_p1);
    Pnt *p2= cad.getPuntos().busca(id_p2);
    Linea *retval= dynamic_cast<Linea *>(Line(p1,p2));
    assert(retval);
    return retval;
  }

//! @brief Divided line.
XC::LineaTramos *XC::MapLineas::newDividedLine(const size_t &id_p1, const size_t &id_p2)
  {
    Preprocessor *preprocessor= getPreprocessor();
    assert(preprocessor);
    Cad &cad= preprocessor->getCad();
    Pnt *p1= cad.getPuntos().busca(id_p1);
    Pnt *p2= cad.getPuntos().busca(id_p2);
    LineaTramos *retval= dynamic_cast<LineaTramos *>(DividedLine(p1,p2));
    assert(retval);
    return retval;
  }

//! @brief Circle arc.
XC::ArcoCircunf *XC::MapLineas::newCircleArc(const size_t &id_p1, const size_t &id_p2, const size_t &id_p3)
  {
    Preprocessor *preprocessor= getPreprocessor();
    assert(preprocessor);
    Cad &cad= preprocessor->getCad();
    Pnt *p1= cad.getPuntos().busca(id_p1);
    Pnt *p2= cad.getPuntos().busca(id_p2);
    Pnt *p3= cad.getPuntos().busca(id_p3);
    ArcoCircunf *retval= dynamic_cast<ArcoCircunf *>(Arc(p1,p2,p3));
    assert(retval);
    return retval;
  }


//! @brief Circle arc.
XC::CmbEdge *XC::MapLineas::newLineSequence(void)
  {
    CmbEdge *retval= dynamic_cast<CmbEdge *>(LineSequence());
    assert(retval);
    return retval;
  }


//! @brief Lee un objeto Edge desde el archivo de entrada.
bool XC::MapLineas::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    const std::string str_err= "(MapLineas) Procesando comando: " + cmd;
    if(verborrea>2)
      std::clog << str_err << std::endl;
    if(cmd == "tag_linea")
      {
        setTag(interpretaSize_t(status.GetString())); //Nuevo identificador de línea.
        return true;
      }
    else if(cmd == "linea")
      {
        Nueva<Linea>(status);
        return true;
      }
    else if(cmd == "linea_tramos")
      {
        Nueva<LineaTramos>(status);
        return true;
      }
    else if(cmd == "sec_lineas")
      {
        Nueva<CmbEdge>(status);
        return true;
      }
    else if(cmd == "arco_circunf")
      {
        Nueva<ArcoCircunf>(status);
        return true;
      }
    else
      return MapEnt<Edge>::procesa_comando(status);
  }

//! @brief Inserta la nueva linea en el conjunto total y los conjuntos abiertos.
void XC::MapLineas::UpdateSets(Edge *nueva_linea) const
  {
    Cad *cad= const_cast<Cad *>(dynamic_cast<const Cad *>(Owner()));
    Preprocessor *preprocessor= cad->getPreprocessor();
    preprocessor->get_sets().get_set_total()->GetLineas().push_back(nueva_linea);
    preprocessor->get_sets().inserta_ent_mdlr(nueva_linea);
    MapSet::map_sets &abiertos= preprocessor->get_sets().get_sets_abiertos();
    for(MapSet::map_sets::iterator i= abiertos.begin();i!= abiertos.end();i++)
      {
        Set *ptr_set= dynamic_cast<Set *>((*i).second);
        assert(ptr_set);
        ptr_set->GetLineas().push_back(nueva_linea);
      }
  }

//! @brief Crea una nueva línea entre los puntos que se pasan como parámetro
//! y la inserta en el conjunto de lados.
XC::Edge *XC::MapLineas::Line(Pnt *pA,Pnt *pB)
  {
    Edge *tmp= nullptr;
    if(pA && pB)
      {
        if(pA==pB)
	  std::cerr << "MapLineas::Line; los extremos: ("
                    << pA->GetNombre() << ","
                    << pB->GetNombre() 
                    << ") de la línea, coinciden." << std::endl;
        tmp= busca_edge_ptr_extremos(*pA,*pB);
        if(!tmp)
          {
            assert(get_modelador());
            tmp= Nueva<Linea>();
            assert(tmp);
            tmp->SetVertice(1,pA);
            tmp->SetVertice(2,pB);
          }
        if(!tmp)
	  std::cerr << "MapLineas::Line; no se pudo obtener una linea"
                    << " entre los puntos: " << pA->GetNombre()
                    << " y " << pB->GetNombre() << std::endl;
      }
    else
      std::cerr << "MapLineas::Line; error, se pasó un puntero nulo." << std::endl;
    return tmp;
  }

//! @brief Crea una nueva línea entre los puntos que se pasan como parámetro
//! y la inserta en el conjunto de lados.
XC::Edge *XC::MapLineas::DividedLine(Pnt *pA,Pnt *pB)
  {
    Edge *tmp= nullptr;
    if(pA && pB)
      {
        if(pA==pB)
	  std::cerr << "MapLineas::DividedLine; los extremos: ("
                    << pA->GetNombre() << ","
                    << pB->GetNombre() 
                    << ") de la línea, coinciden." << std::endl;
        tmp= busca_edge_ptr_extremos(*pA,*pB);
        if(!tmp)
          {
            assert(get_modelador());
            tmp= Nueva<LineaTramos>();
            assert(tmp);
            tmp->SetVertice(1,pA);
            tmp->SetVertice(2,pB);
          }
        if(!tmp)
	  std::cerr << "MapLineas::DividedLine; no se pudo obtener una linea"
                    << " entre los puntos: " << pA->GetNombre()
                    << " y " << pB->GetNombre() << std::endl;
      }
    else
      std::cerr << "MapLineas::DividedLine; error, se pasó un puntero nulo." << std::endl;
    return tmp;
  }

//! @brief Crea un nuevo arco de circunferencia entre los puntos que se pasan como parámetro
//! y la inserta en el conjunto de lados.
XC::Edge *XC::MapLineas::Arc(Pnt *pA,Pnt *pB,Pnt *pC)
  {
    Edge *tmp= nullptr;
    if(pA && pB && pC)
      {
        tmp= busca_edge_ptr_extremos(*pA,*pB,*pC);
        if(!tmp)
          {
            assert(get_modelador());
            tmp= Nueva<ArcoCircunf>();
            assert(tmp);
            tmp->SetVertice(1,pA);
            tmp->SetVertice(2,pC);
            tmp->SetVertice(3,pB); //Punto intermedio.
          }
        if(!tmp)
	  std::cerr << "MapLineas::Arc; no se pudo obtener un arco de circunferencia"
                    << " entre los puntos: " << pA->GetNombre() << ", " << pB->GetNombre()
                    << " y " << pC->GetNombre() << std::endl;
      }
    else
      std::cerr << "MapLineas::Arc; error, se pasó un puntero nulo." << std::endl;
    return tmp;
  }

//! @brief Crea una secuencia de líneas (polyline) con las que se pasan como parámetro
//! y la inserta en el conjunto de lados.
XC::Edge *XC::MapLineas::LineSequence(void)
  {
    Edge *tmp= Nueva<CmbEdge>();
    assert(tmp);
    return tmp;
  }

//! @brief Crea una nueva línea, copia de la que se pasa como parámetro con
//! el nombre que se pasa como parámetro.
XC::Edge *XC::MapLineas::Copia(const Edge *l)
  {
    Edge *retval= busca(getTag());
    if(retval)
      std::cerr << "MapLineas::Copia; la línea con identificador: " 
                << getTag() << " ya existe, no se hacen cambios." << std::endl;
    else //La línea es nueva.
      {
        retval= dynamic_cast<Edge *>(l->getCopy());
        if(retval)
          {
            retval->Nombre()= "l"+boost::lexical_cast<std::string>(getTag());
            (*this)[getTag()]= retval;
            UpdateSets(retval);
            tag++;
	  }
        else
	  std::cerr << "MapLineas::Copia; memoria agotada o el objeto: '"
                    << l->GetNombre() << " no es una línea." << std::endl; 
      }
    return retval;
  }

//! Devuelve la propiedad del objeto cuyo código se pasa
//! como parámetro.
any_const_ptr XC::MapLineas::GetProp(const std::string &cod) const
  {
    if(cod == "linea")
      {
        static const Edge *linea= nullptr;
        const size_t iLinea= popSize_t(cod);
        linea= busca(iLinea);
        if(linea)
          return any_const_ptr(linea);
        else
          {
            std::cerr << "MapLineas::GetProp; no se encontró la línea: '" 
                      << iLinea << "'.\n";
            return any_const_ptr();
          }
      }
    else if(cod=="getTagNearestEdge")
      {
        const Pos3d p= popPos3d(cod);
        const Edge *tmp= getNearest(p);
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
    else if(cod=="num_lineas")
      {
        tmp_gp_szt= this->size();
        return any_const_ptr(tmp_gp_szt);
      }
    else
      return MapEnt<Edge>::GetProp(cod);
  }
