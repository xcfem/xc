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
            assert(get_preprocessor());
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
            assert(get_preprocessor());
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
            assert(get_preprocessor());
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

//! @brief Crea una nueva línea, copia de la being passed as parameter con
//! el nombre being passed as parameter.
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
