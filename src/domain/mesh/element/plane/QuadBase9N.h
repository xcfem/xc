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
//QuadBase9N.h
                                                                        
#include "ElemPlano.h"

#ifndef QuadBase9N_h
#define QuadBase9N_h

#include "preprocessor/multi_block_topology/matrices/TritrizPtrElem.h"
#include "preprocessor/multi_block_topology/aux_meshing.h"
#include "preprocessor/prep_handlers/LoadHandler.h"
#include "domain/load/plane/BidimStrainLoad.h"
#include "med.h"
#include "vtkCellType.h"

namespace XC {
//! \ingroup Elem
//
//! @defgroup ElemPlanos bi-dimensional elements (2D problems, shells,...).
//
//! \ingroup ElemPlanos
//! @brief Base class for nine node quads.
template <class PhysProp>
class QuadBase9N : public ElemPlano<9,PhysProp>
  {
  protected:
    TritrizPtrElem put_on_mesh(const TritrizPtrNod &,meshing_dir dm) const;

  public:

    QuadBase9N(int classTag);
    QuadBase9N(int tag, int classTag,const PhysProp &);

    Element::NodesEdge getNodesEdge(const size_t &i) const;
    ID getLocalIndexNodesEdge(const size_t &i) const;
    int getEdgeNodes(const Node *,const Node *) const;
    Poligono3d getPolygon(bool initialGeometry= true) const;
    Segmento3d getLado(const size_t &i,bool initialGeometry= true) const;

    int getVtkCellType(void) const;
    int getMEDCellType(void) const;

    void zeroLoad(void);	
    int addLoad(ElementalLoad *theLoad, double loadFactor);
  };

//! @brief Constructor
template <class PhysProp>
XC::QuadBase9N<PhysProp>::QuadBase9N(int classTag)
  : ElemPlano<9,PhysProp>(0,classTag) {}

//! @brief Constructor.
template <class PhysProp>
XC::QuadBase9N<PhysProp>::QuadBase9N(int tag,int classTag,const PhysProp &pp)
  :ElemPlano<9,PhysProp>(tag,classTag,pp) {}


//! @brief Put the element on the mesh being passed as parameter.
template <class PhysProp>
XC::TritrizPtrElem XC::QuadBase9N<PhysProp>::put_on_mesh(const TritrizPtrNod &nodes,meshing_dir dm) const
  { return put_quad9N_on_mesh(*this,nodes,dm); }

//! @brief Returns the element contour as a polygon.
template <class PhysProp>
Poligono3d XC::QuadBase9N<PhysProp>::getPolygon(bool initialGeometry) const
  {
    Poligono3d retval;
    std::cerr << this->getClassName() << "::" << __FUNCTION__
	      << "; not implemented." << std::endl;
    return retval;
  }

//! @brief Returns the element edge.
template <class PhysProp>
Segmento3d XC::QuadBase9N<PhysProp>::getLado(const size_t &i,bool initialGeometry) const
  {
    Segmento3d retval;
    std::cerr << this->getClassName() << "::" << __FUNCTION__
	      << "; not implemented." << std::endl;
    return retval;
  }

//! @brief Returns the nodes of the element edge.
template <class PhysProp>
XC::Element::NodesEdge XC::QuadBase9N<PhysProp>::getNodesEdge(const size_t &i) const
  {
    Element::NodesEdge retval(3,static_cast<Node *>(nullptr));
    std::cerr << this->getClassName() << "::" << __FUNCTION__
	      << "; not implemented." << std::endl;
    return retval;    
  }

//! @brief Returns the edge of the element that ends 
//! in the nodes being passed as parameters.
template <class PhysProp>
int XC::QuadBase9N<PhysProp>::getEdgeNodes(const Node *n1,const Node *n2) const
  {
    int retval= -1;
    std::cerr << this->getClassName() << "::" << __FUNCTION__
              << "; not implemented." << std::endl;
    return retval;
  }

//! @brief Returns the local indexes of the nodes that lie on the i-th edge.
template <class PhysProp>
ID XC::QuadBase9N<PhysProp>::getLocalIndexNodesEdge(const size_t &i) const
  {
    ID retval(2);
    std::cerr << this->getClassName() << "::" << __FUNCTION__
	      << "; not implemented." << std::endl;
    return retval;
  }

//! @brief Zeroes loads on element.
template <class PhysProp>
void XC::QuadBase9N<PhysProp>::zeroLoad(void)
  {
    ElemPlano<9,PhysProp>::zeroLoad();
    this->physicalProperties.getMaterialsVector().zeroInitialGeneralizedStrains(); //Removes initial deformations.
    return;
  }
 
//! @brief Adds to the element the load being passed as parameter.
template <class PhysProp>
int XC::QuadBase9N<PhysProp>::addLoad(ElementalLoad *theLoad, double loadFactor)
  {
    if(this->isDead())
      std::cerr << this->getClassName() 
                << "; load over inactive elements: "
                << this->getTag() << std::endl;
    else
      {
        if(const BidimStrainLoad *strainLoad= dynamic_cast<const BidimStrainLoad *>(theLoad)) //Prescribed strains.
          {
            static std::vector<Vector> initStrains;
            initStrains= strainLoad->getStrains();
            for(std::vector<Vector>::iterator i= initStrains.begin();i!=initStrains.end();i++)
              (*i)*= loadFactor;
            this->physicalProperties.getMaterialsVector().addInitialGeneralizedStrains(initStrains);
          }
        else
          {
            std::cerr << this->getClassName() << "::" << __FUNCTION__
		      << "; load type unknown for element with tag: " <<
            this->getTag() << std::endl;
            return -1;
          }
      }
    return 0;
  }

//! @brief Interfaz con Vtk.
template <class PhysProp>
int XC::QuadBase9N<PhysProp>::getVtkCellType(void) const
  { return VTK_QUADRATIC_QUAD; }

//! @brief Interfaz con el formato MED de Salome.
template <class PhysProp>
int XC::QuadBase9N<PhysProp>::getMEDCellType(void) const
  { return MED_QUAD9; }

} // end of XC namespace
#endif
