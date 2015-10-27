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
//BrickBase.cc

#include "BrickBase.h"
#include "preprocessor/cad/matrices/TritrizPtrNod.h"
#include "preprocessor/cad/matrices/TritrizPtrElem.h"
#include "domain/mesh/node/Node.h"

//! @brief Constructor
XC::BrickBase::BrickBase(int classTag)
  : ElemWithMaterial<8,NDMaterialPhysicalProperties>(0,classTag,NDMaterialPhysicalProperties(8,nullptr))
  {}


//! @brief Constructor.
XC::BrickBase::BrickBase(int tag,int classTag,const NDMaterialPhysicalProperties &pp)
  :ElemWithMaterial<8,NDMaterialPhysicalProperties>(tag,classTag,pp)
  {}

//! @brief Constructor.
XC::BrickBase::BrickBase(int tag, int classTag, int node1, int node2, int node3, int node4,int node5,int node6,int node7,int node8, const NDMaterialPhysicalProperties &pp) 
  : ElemWithMaterial<8,NDMaterialPhysicalProperties>(tag,classTag,pp)
  {
    theNodes.set_id_nodos(node1,node2,node3,node4,node5,node6,node7,node8);
  }

//! @brief Devuelve la dimensión del elemento.
size_t XC::BrickBase::getDimension(void) const
  { return 3; }

//Coloca el elemento en la malla que se pasa como parámetro.
XC::TritrizPtrElem XC::BrickBase::coloca_en_malla(const XC::TritrizPtrNod &nodos,dir_mallado dm) const
  {
    const size_t ncapas= nodos.GetCapas();
    const size_t nfilas= nodos.getNumFilas();
    const size_t ncols= nodos.getNumCols();
    const size_t dim_malla= nodos.GetDim();
    TritrizPtrElem retval(ncapas-1,nfilas-1,ncols-1);
    if(dim_malla<3)
      std::cerr << "BrickBase::coloca_en_malla; se necesita una malla tridimensional, no se pudieron crear elementos." << std::endl;
    else
      {
        for(size_t i=1;i<ncapas;i++)
          for(size_t j=1;j<nfilas;j++)
            for(size_t k=1;k<ncols;k++)
              {
	        XC::Element *tmp= this->getCopy();
                const int &Nd1= nodos(i,j,k)->getTag();
                const int &Nd2= nodos(i,j,k+1)->getTag();
                const int &Nd3= nodos(i,j+1,k+1)->getTag();
                const int &Nd4= nodos(i,j+1,k)->getTag();
                const int &Nd5= nodos(i+1,j,k)->getTag();
                const int &Nd6= nodos(i+1,j,k+1)->getTag();
                const int &Nd7= nodos(i+1,j+1,k+1)->getTag();
                const int &Nd8= nodos(i+1,j+1,k)->getTag();
                tmp->getNodePtrs().set_id_nodos(Nd1,Nd2,Nd3,Nd4,Nd5,Nd6,Nd7,Nd8);
                retval(i,j,k)= tmp;
              }
      }
    return retval;
  }

