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
//TriBase3N.h
                                                                        
#include "ElemPlano.h"

#ifndef TriBase3N_h
#define TriBase3N_h

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
template <class PhysProp> //3 Gauss point by default.
class TriBase3N: public ElemPlano<3,PhysProp>
  {
  protected:
    TritrizPtrElem coloca_en_malla(const TritrizPtrNod &,dir_mallado dm) const;
  public:

    TriBase3N(int classTag,const PhysProp &);
    TriBase3N(int tag, int classTag,const PhysProp &);
    TriBase3N(int tag, int classTag, int node1, int node2, int node3,const PhysProp &pp);

    Element::NodesEdge getNodesEdge(const size_t &i) const;
    ID getLocalIndexNodesEdge(const size_t &i) const;
    int getEdgeNodes(const Node *,const Node *) const;

    int getVtkCellType(void) const;
    int getMEDCellType(void) const;

    int addLoad(ElementalLoad *theLoad, double loadFactor);

  };

//! @brief Constructor
template <class PhysProp>
XC::TriBase3N<PhysProp>::TriBase3N(int classTag,const PhysProp &pp)
  : ElemPlano<3,PhysProp>(0,classTag,pp) {}

//! @brief Constructor.
template <class PhysProp>
XC::TriBase3N<PhysProp>::TriBase3N(int tag,int classTag,const PhysProp &physProp)
  :ElemPlano<3,PhysProp>(tag,classTag,physProp) {}

//! @brief Constructor.
template <class PhysProp>
XC::TriBase3N<PhysProp>::TriBase3N(int tag, int classTag, int node1, int node2, int node3,const PhysProp &pp)
  : ElemPlano<3,PhysProp>(tag,classTag,pp)
  {
    this->theNodes.set_id_nodos(node1,node2,node3);
  }

//! @brief Coloca el elemento en la malla being passed as parameter.
template <class PhysProp>
XC::TritrizPtrElem XC::TriBase3N<PhysProp>::coloca_en_malla(const XC::TritrizPtrNod &nodos,dir_mallado dm) const
  {
    std::cerr << "TritrizPtrElem XC::TriBase3N<PhysProp>::coloca_en_malla not implemented" << std::endl; 
    TritrizPtrElem retval;
    return retval;
  }

//! @brief Devuelve los nodos de un lado del elemento.
template <class PhysProp>
XC::Element::NodesEdge XC::TriBase3N<PhysProp>::getNodesEdge(const size_t &i) const
  {
    Element::NodesEdge retval(2,static_cast<Node *>(nullptr));
    const NodePtrsWithIDs &nodos= ElemPlano<3,PhysProp>::getNodePtrs();
    const size_t sz= nodos.size();
    if(i<sz)
      {
        retval[0]= nodos(i);
        if(i<(sz-1))
          retval[1]= nodos(i+1);
        else
          retval[1]= nodos(0);
      }
    return retval;    
  }

//! @brief Devuelve el borde del elemento
//! que tiene por extremos los nodos que se pasan como parámetros.
template <class PhysProp>
int XC::TriBase3N<PhysProp>::getEdgeNodes(const Node *n1,const Node *n2) const
  {
    int retval= -1;
    const NodePtrsWithIDs &nodos= ElemPlano<3,PhysProp>::getNodePtrs();
    const int i1= nodos.find(n1);
    const int i2= nodos.find(n2);
    if((i1>=0) && (i2>=0))
      {
        const int dif= i2-i1;
        if(dif==1)
          retval= i1;
        else if(dif==-1)
          retval= i2;
        else if((i1==3) && (i2==0))
          retval= 3;
        else if((i1==0) && (i2==3))
          retval= 3;
      }
    return retval;
  }

//! @brief Devuelve los índices locales de los nodos del elemento
//! situados sobre el borde (o arista) being passed as parameters.
template <class PhysProp>
ID XC::TriBase3N<PhysProp>::getLocalIndexNodesEdge(const size_t &i) const
  {
    ID retval(2);
    const NodePtrsWithIDs &nodos= ElemPlano<3,PhysProp>::getNodePtrs();
    const size_t sz= nodos.size();
    if(i<sz)
      {
        retval[0]= i;
        if(i<(sz-1))
          retval[1]= i+1;
        else
          retval[1]= 0;
      }
    return retval;
  }

//! @brief Añade al elemento la carga being passed as parameter.
template <class PhysProp>
int XC::TriBase3N<PhysProp>::addLoad(ElementalLoad *theLoad, double loadFactor)
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
            std::cerr << "TriBase3N::addLoad -- load type unknown for element with tag: " <<
            this->getTag() << std::endl;
            return -1;
          }
      }
    return 0;
  }

//! @brief Interfaz con VTK.
template <class PhysProp>
int XC::TriBase3N<PhysProp>::getVtkCellType(void) const
  { return VTK_TRIANGLE; }

//! @brief Interfaz con el formato MED de Salome.
template <class PhysProp>
int XC::TriBase3N<PhysProp>::getMEDCellType(void) const
  { return MED_TRIA3; }

} // end of XC namespace
#endif
