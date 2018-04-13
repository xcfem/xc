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
//LineMap.cc

#include "LineMap.h"
#include "preprocessor/Preprocessor.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"

#include "preprocessor/multi_block_topology/entities/Edge.h"
#include "preprocessor/multi_block_topology/entities/Pnt.h"
#include "preprocessor/multi_block_topology/entities/Line.h"
#include "preprocessor/multi_block_topology/entities/DividedLine.h"
#include "preprocessor/multi_block_topology/entities/CmbEdge.h"
#include "preprocessor/multi_block_topology/entities/CircularArc.h"
#include "preprocessor/set_mgmt/Set.h"



//! @brief Constructor.
XC::LineMap::LineMap(MultiBlockTopology *mbt)
  : EntityMap<Edge>(mbt) {}

//! @brief Line segment.
XC::Line *XC::LineMap::newLine(const size_t &id_p1, const size_t &id_p2)
  {
    Preprocessor *preprocessor= getPreprocessor();
    assert(preprocessor);
    MultiBlockTopology &mbt= preprocessor->getMultiBlockTopology();
    Pnt *p1= mbt.getPoints().busca(id_p1);
    Pnt *p2= mbt.getPoints().busca(id_p2);
    Line *retval= dynamic_cast<Line *>(createLine(p1,p2));
    assert(retval);
    return retval;
  }

//! @brief Divided line.
XC::DividedLine *XC::LineMap::newDividedLine(const size_t &id_p1, const size_t &id_p2)
  {
    Preprocessor *preprocessor= getPreprocessor();
    assert(preprocessor);
    MultiBlockTopology &mbt= preprocessor->getMultiBlockTopology();
    Pnt *p1= mbt.getPoints().busca(id_p1);
    Pnt *p2= mbt.getPoints().busca(id_p2);
    DividedLine *retval= dynamic_cast<DividedLine *>(createDividedLine(p1,p2));
    assert(retval);
    return retval;
  }

//! @brief Circle arc.
XC::CircularArc *XC::LineMap::newCircleArc(const size_t &id_p1, const size_t &id_p2, const size_t &id_p3)
  {
    Preprocessor *preprocessor= getPreprocessor();
    assert(preprocessor);
    MultiBlockTopology &mbt= preprocessor->getMultiBlockTopology();
    Pnt *p1= mbt.getPoints().busca(id_p1);
    Pnt *p2= mbt.getPoints().busca(id_p2);
    Pnt *p3= mbt.getPoints().busca(id_p3);
    CircularArc *retval= dynamic_cast<CircularArc *>(createArc(p1,p2,p3));
    assert(retval);
    return retval;
  }


//! @brief Line sequence.
XC::CmbEdge *XC::LineMap::newLineSequence(void)
  {
    CmbEdge *retval= dynamic_cast<CmbEdge *>(createLineSequence());
    assert(retval);
    return retval;
  }

//! @brief Insert the new line in the total and the opened sets.
void XC::LineMap::UpdateSets(Edge *nueva_linea) const
  {
    MultiBlockTopology *mbt= const_cast<MultiBlockTopology *>(dynamic_cast<const MultiBlockTopology *>(Owner()));
    Preprocessor *preprocessor= mbt->getPreprocessor();
    preprocessor->get_sets().get_set_total()->getLines().push_back(nueva_linea);
    preprocessor->get_sets().insert_ent_mdlr(nueva_linea);
    MapSet::map_sets &abiertos= preprocessor->get_sets().get_sets_abiertos();
    for(MapSet::map_sets::iterator i= abiertos.begin();i!= abiertos.end();i++)
      {
        Set *ptr_set= dynamic_cast<Set *>((*i).second);
        assert(ptr_set);
        ptr_set->getLines().push_back(nueva_linea);
      }
  }

//! @brief Creates a new line between the points being passed as parameters
//! and inserts it on the container
//! @param pA: pointer to back end of the line.
//! @param pB: pointer to front end of the line.
XC::Edge *XC::LineMap::createLine(Pnt *pA,Pnt *pB)
  {
    Edge *tmp= nullptr;
    if(pA && pB)
      {
        if(pA==pB)
	  std::cerr << getClassName() << __FUNCTION__
	            << "; ends of the line: ("
                    << pA->getName() << ","
                    << pB->getName() 
                    << "), are the same." << std::endl;
        tmp= busca_edge_ptr_extremos(*pA,*pB);
        if(!tmp)
          {
            assert(getPreprocessor());
            tmp= New<Line>();
            assert(tmp);
            tmp->SetVertice(1,pA);
            tmp->SetVertice(2,pB);
          }
        if(!tmp)
	  std::cerr << getClassName() << __FUNCTION__
		    << "; can't get a line"
                    << " between points: " << pA->getName()
                    << " and " << pB->getName() << std::endl;
      }
    else
      std::cerr << getClassName() << __FUNCTION__
		<< "; error, null pointer to point (A, B or both)."
		<< std::endl;
    return tmp;
  }

//! @brief Creates a new line between the points being passed as parameters
//! and inserts it on the container
//! @param pA: pointer to back end of the line.
//! @param pB: pointer to front end of the line.
XC::Edge *XC::LineMap::createDividedLine(Pnt *pA,Pnt *pB)
  {
    Edge *tmp= nullptr;
    if(pA && pB)
      {
        if(pA==pB)
	  std::cerr << getClassName() << __FUNCTION__
	            << "; ends of the line: ("
                    << pA->getName() << ","
                    << pB->getName() 
                    << "), are the same." << std::endl;
        tmp= busca_edge_ptr_extremos(*pA,*pB);
        if(!tmp)
          {
            assert(getPreprocessor());
            tmp= New<DividedLine>();
            assert(tmp);
            tmp->SetVertice(1,pA);
            tmp->SetVertice(2,pB);
          }
        if(!tmp)
	  std::cerr << getClassName() << __FUNCTION__
		    << "; can't get a line"
                    << " between points: " << pA->getName()
                    << " and " << pB->getName() << std::endl;
      }
    else
      std::cerr << getClassName() << __FUNCTION__
		<< "; error, null pointer to point (A, B or both)."
		<< std::endl;
    return tmp;
  }

//! @brief Creates a new arc of circle between the points
//! being passed as parameters and inserts it in the edge set.
XC::Edge *XC::LineMap::createArc(Pnt *pA,Pnt *pB,Pnt *pC)
  {
    Edge *tmp= nullptr;
    if(pA && pB && pC)
      {
        tmp= busca_edge_ptr_extremos(*pA,*pB,*pC);
        if(!tmp)
          {
            assert(getPreprocessor());
            tmp= New<CircularArc>();
            assert(tmp);
            tmp->SetVertice(1,pA);
            tmp->SetVertice(2,pC);
            tmp->SetVertice(3,pB); //intermediate point.
          }
        if(!tmp)
	  std::cerr << getClassName() << __FUNCTION__
		    << "; can't get an arc"
                    << " between the points: "
		    << pA->getName() << ", " << pB->getName()
                    << " and " << pC->getName() << std::endl;
      }
    else
      std::cerr << getClassName() << __FUNCTION__
		<< "; error, null pointer to point (A, B and/or C)."
		<< std::endl;
    return tmp;
  }

//! @brief Creates a line sequence (polyline) with those being
//! passed as parameters and inserts it in the edge set.
XC::Edge *XC::LineMap::createLineSequence(void)
  {
    Edge *tmp= New<CmbEdge>();
    assert(tmp);
    return tmp;
  }

//! @brief Creates a new line, copia de la being passed as parameter con
//! el nombre being passed as parameter.
XC::Edge *XC::LineMap::createCopy(const Edge *l)
  {
    Edge *retval= busca(getTag());
    if(retval)
      std::cerr << getClassName() << __FUNCTION__
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
	  std::cerr << getClassName() << __FUNCTION__
	            << "; memory exhausted or the objet: '"
                    << l->getName() << "is not a line." << std::endl; 
      }
    return retval;
  }
