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
//QuadBase9N.h
                                                                        
#include "ElemPlano.h"

#ifndef QuadBase9N_h
#define QuadBase9N_h

#include "preprocessor/cad/matrices/TritrizPtrElem.h"
#include "preprocessor/cad/aux_mallado.h"
#include "preprocessor/loaders/LoadLoader.h"
#include "domain/load/plane/BidimStrainLoad.h"
#include "med.h"
#include "vtkCellType.h"

namespace XC {
//! \ingroup Elem
//
//! @defgroup ElemPlanos Elementos bidimensionales (problemas planos, láminas,...).
//!
//! \ingroup ElemPlanos
//
//! @brief Clase base para los cuadriláteros de cuatro nodos.
template <class PhysProp>
class QuadBase9N : public ElemPlano<9,PhysProp>
  {
  protected:
    TritrizPtrElem coloca_en_malla(const TritrizPtrNod &,dir_mallado dm) const;

  public:

    QuadBase9N(int classTag);
    QuadBase9N(int tag, int classTag,const PhysProp &);

    Element::NodesEdge getNodesEdge(const size_t &i) const;
    ID getLocalIndexNodesEdge(const size_t &i) const;
    int getEdgeNodes(const Node *,const Node *) const;
    Poligono3d getPoligono(bool initialGeometry= true) const;
    Segmento3d getLado(const size_t &i,bool initialGeometry= true) const;

    int getVtkCellType(void) const;
    int getMEDCellType(void) const;

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


//! @brief Coloca el elemento en la malla being passed as parameter.
template <class PhysProp>
XC::TritrizPtrElem XC::QuadBase9N<PhysProp>::coloca_en_malla(const TritrizPtrNod &nodos,dir_mallado dm) const
  { return coloca_quad9N_en_malla(*this,nodos,dm); }

//! @brief Devuelve el polígono que forma el contorno del elemento.
template <class PhysProp>
Poligono3d XC::QuadBase9N<PhysProp>::getPoligono(bool initialGeometry) const
  {
    Poligono3d retval;
    std::cerr << "QuadBase9N<PhysProp>::getPoligono no implementada." << std::endl;
    return retval;
  }

//! @brief Devuelve un lado del elemento.
template <class PhysProp>
Segmento3d XC::QuadBase9N<PhysProp>::getLado(const size_t &i,bool initialGeometry) const
  {
    Segmento3d retval;
    std::cerr << "QuadBase9N<PhysProp>::getLado no implementada." << std::endl;
    return retval;
  }

//! @brief Devuelve los nodos de un lado del elemento.
template <class PhysProp>
XC::Element::NodesEdge XC::QuadBase9N<PhysProp>::getNodesEdge(const size_t &i) const
  {
    Element::NodesEdge retval(3,static_cast<Node *>(NULL));
    std::cerr << "QuadBase9N<PhysProp>::getNodesEdge no implementada." << std::endl;
    return retval;    
  }

//! @brief Devuelve el borde del elemento
//! que tiene por extremos los nodos que se pasan como parámetros.
template <class PhysProp>
int XC::QuadBase9N<PhysProp>::getEdgeNodes(const Node *n1,const Node *n2) const
  {
    int retval= -1;
    std::cerr << "QuadBase9N<PhysProp>::getEdgeNodes no implementada." << std::endl;
    return retval;
  }

//! @brief Devuelve los índices locales de los nodos del elemento
//! situados sobre el borde (o arista) being passed as parameters.
template <class PhysProp>
ID XC::QuadBase9N<PhysProp>::getLocalIndexNodesEdge(const size_t &i) const
  {
    ID retval(2);
    std::cerr << "QuadBase9N<PhysProp>::getEdgeNodes no implementada." << std::endl;
    return retval;
  }

//! @brief Añade al elemento la carga being passed as parameter.
template <class PhysProp>
int XC::QuadBase9N<PhysProp>::addLoad(ElementalLoad *theLoad, double loadFactor)
  {
    if(this->isDead())
      std::cerr << this->nombre_clase() 
                << "; se intentó cargar el elemento "
                << this->getTag() << " que está desactivado." 
                << std::endl;
    else
      {
        if(BidimStrainLoad *strainLoad= dynamic_cast<BidimStrainLoad *>(theLoad)) //Deformaciones impuestas.
          {
            static std::vector<Vector> initStrains;
            initStrains= strainLoad->Deformaciones();
            for(std::vector<Vector>::iterator i= initStrains.begin();i!=initStrains.end();i++)
              (*i)*= loadFactor;
            this->physicalProperties.getMaterialsVector().addInitialGeneralizedStrains(initStrains);
          }
        else
          {
            std::cerr << "QuadBase9N::addLoad -- load type unknown for element with tag: " <<
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
