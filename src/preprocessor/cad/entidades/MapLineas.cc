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
#include "preprocessor/cad/entidades/DividedLine.h"
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
    Linea *retval= dynamic_cast<Linea *>(createLine(p1,p2));
    assert(retval);
    return retval;
  }

//! @brief Divided line.
XC::DividedLine *XC::MapLineas::newDividedLine(const size_t &id_p1, const size_t &id_p2)
  {
    Preprocessor *preprocessor= getPreprocessor();
    assert(preprocessor);
    Cad &cad= preprocessor->getCad();
    Pnt *p1= cad.getPuntos().busca(id_p1);
    Pnt *p2= cad.getPuntos().busca(id_p2);
    DividedLine *retval= dynamic_cast<DividedLine *>(createDividedLine(p1,p2));
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
    ArcoCircunf *retval= dynamic_cast<ArcoCircunf *>(createArc(p1,p2,p3));
    assert(retval);
    return retval;
  }


//! @brief Circle arc.
XC::CmbEdge *XC::MapLineas::newLineSequence(void)
  {
    CmbEdge *retval= dynamic_cast<CmbEdge *>(createLineSequence());
    assert(retval);
    return retval;
  }

//! @brief Inserta la nueva linea en the set total and the opened sets.
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

//! @brief Creates a new line entre los puntos being passed as parameters
//! and inserts it on the container
//! @param pA: pointer to back end of the line.
//! @param pB: pointer to front end of the line.
XC::Edge *XC::MapLineas::createLine(Pnt *pA,Pnt *pB)
  {
    Edge *tmp= nullptr;
    if(pA && pB)
      {
        if(pA==pB)
	  std::cerr << nombre_clase() << __FUNCTION__
	            << "; ends of the line: ("
                    << pA->GetNombre() << ","
                    << pB->GetNombre() 
                    << "), are the same." << std::endl;
        tmp= busca_edge_ptr_extremos(*pA,*pB);
        if(!tmp)
          {
            assert(getPreprocessor());
            tmp= Nueva<Linea>();
            assert(tmp);
            tmp->SetVertice(1,pA);
            tmp->SetVertice(2,pB);
          }
        if(!tmp)
	  std::cerr << nombre_clase() << __FUNCTION__
		    << "; can't get a line"
                    << " between points: " << pA->GetNombre()
                    << " and " << pB->GetNombre() << std::endl;
      }
    else
      std::cerr << nombre_clase() << __FUNCTION__
		<< "; error, null pointer to point (A, B or both)."
		<< std::endl;
    return tmp;
  }

//! @brief Creates a new line between the points being passed as parameters
//! and inserts it on the container
//! @param pA: pointer to back end of the line.
//! @param pB: pointer to front end of the line.
XC::Edge *XC::MapLineas::createDividedLine(Pnt *pA,Pnt *pB)
  {
    Edge *tmp= nullptr;
    if(pA && pB)
      {
        if(pA==pB)
	  std::cerr << nombre_clase() << __FUNCTION__
	            << "; ends of the line: ("
                    << pA->GetNombre() << ","
                    << pB->GetNombre() 
                    << "), are the same." << std::endl;
        tmp= busca_edge_ptr_extremos(*pA,*pB);
        if(!tmp)
          {
            assert(getPreprocessor());
            tmp= Nueva<DividedLine>();
            assert(tmp);
            tmp->SetVertice(1,pA);
            tmp->SetVertice(2,pB);
          }
        if(!tmp)
	  std::cerr << nombre_clase() << __FUNCTION__
		    << "; can't get a line"
                    << " between points: " << pA->GetNombre()
                    << " and " << pB->GetNombre() << std::endl;
      }
    else
      std::cerr << nombre_clase() << __FUNCTION__
		<< "; error, null pointer to point (A, B or both)."
		<< std::endl;
    return tmp;
  }

//! @brief Creates a new arc of circle between the points
//! being passed as parameters and inserts it in the edge set.
XC::Edge *XC::MapLineas::createArc(Pnt *pA,Pnt *pB,Pnt *pC)
  {
    Edge *tmp= nullptr;
    if(pA && pB && pC)
      {
        tmp= busca_edge_ptr_extremos(*pA,*pB,*pC);
        if(!tmp)
          {
            assert(getPreprocessor());
            tmp= Nueva<ArcoCircunf>();
            assert(tmp);
            tmp->SetVertice(1,pA);
            tmp->SetVertice(2,pC);
            tmp->SetVertice(3,pB); //Punto intermedio.
          }
        if(!tmp)
	  std::cerr << nombre_clase() << __FUNCTION__
		    << "; can't get an arc"
                    << " between the points: "
		    << pA->GetNombre() << ", " << pB->GetNombre()
                    << " and " << pC->GetNombre() << std::endl;
      }
    else
      std::cerr << nombre_clase() << __FUNCTION__
		<< "; error, null pointer to point (A, B and/or C)." << std::endl;
    return tmp;
  }

//! @brief Creates a line sequence (polyline) with those being
//! passed as parameters and inserts it in the edge set.
XC::Edge *XC::MapLineas::createLineSequence(void)
  {
    Edge *tmp= Nueva<CmbEdge>();
    assert(tmp);
    return tmp;
  }

//! @brief Creates a new line, copia de la being passed as parameter con
//! el nombre being passed as parameter.
XC::Edge *XC::MapLineas::createCopy(const Edge *l)
  {
    Edge *retval= busca(getTag());
    if(retval)
      std::cerr << nombre_clase() << __FUNCTION__
	        << "; line identified by: " 
                << getTag() << " already exist, do nothing." << std::endl;
    else //Line is new.
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
	  std::cerr << nombre_clase() << __FUNCTION__
	            << "; memory exhausted or the objet: '"
                    << l->GetNombre() << "is not a line." << std::endl; 
      }
    return retval;
  }
