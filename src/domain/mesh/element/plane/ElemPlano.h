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
//ElemPlano.h

#ifndef ElemPlano_h
#define ElemPlano_h
                                                                        
#include <domain/mesh/element/ElemWithMaterial.h>
#include "xc_utils/src/geom/d2/Poligono3d.h"
#include "xc_utils/src/geom/d1/Segmento3d.h"
#include "xc_utils/src/geom/pos_vec/Pos3d.h"
#include "preprocessor/Preprocessor.h"

#include "domain/mesh/node/Node.h"

namespace XC {
//! \ingroup Elem
//
//! @defgroup ElemPlanos Bidimensional elements (plane problems, shells,...).
//!
//! \ingroup ElemPlanos
//
//! @brief Base class for plane elements.
template <int NNODOS,class PhysProp>
class ElemPlano : public ElemWithMaterial<NNODOS, PhysProp>
  {
  protected:
    mutable std::vector<double> areasTributarias;
  public:
    ElemPlano(int tag, int classTag,const PhysProp &);
    void checkElem(void);
    void setDomain(Domain *theDomain);

    virtual Poligono3d getPoligono(bool initialGeometry= true) const;
    virtual Segmento3d getLado(const size_t &i,bool initialGeometry= true) const;
    Pos3d getPosCdg(bool initialGeometry= true) const;
    double getPerimetro(bool initialGeometry= true) const;
    double getArea(bool initialGeometry= true) const;
    virtual void calculaAreasTributarias(bool initialGeometry= true) const;
    double getAreaTributaria(const Node *) const;

    double getDist2(const Pos2d &p,bool initialGeometry= true) const;
    double getDist(const Pos2d &p,bool initialGeometry= true) const;
    double getDist2(const Pos3d &p,bool initialGeometry= true) const;
    double getDist(const Pos3d &p,bool initialGeometry= true) const;

    size_t getDimension(void) const;

  };

//! @brief Constructor.
template <int NNODOS,class PhysProp>
XC::ElemPlano<NNODOS, PhysProp>::ElemPlano(int tag,int classTag,const PhysProp &physProp)
  :ElemWithMaterial<NNODOS, PhysProp>(tag,classTag,physProp), areasTributarias(NNODOS,0.0)
  {}

//! @brief Asigna los nodos y chequea el elemento.
template <int NNODOS,class PhysProp>
void XC::ElemPlano<NNODOS, PhysProp>::checkElem(void)
  {
    if(this->getNodePtrs().hasNull())
      std::cerr << "El elemento: " << this->getTag()
                << " tiene pointers to node, nulos." << std::endl;
    else
      {
        const double area= this->getArea();
        if(area<1e-3)
          {
            std::cerr << "El elemento: " << this->getTag() << " de nodos: [";
            const std::vector<int> inodos= this->getNodePtrs().getTags();
            std::vector<int>::const_iterator i= inodos.begin();
            std::cerr << *i;
            i++;
            for(;i!=inodos.end();i++)
	      std::cerr << "," << *i;
            std::cerr << "] tiene un área muy pequeña (" << area << ").\n";
          }
      }
  }

//! @brief Asigna el domain al elemento.
template <int NNODOS,class PhysProp>
void XC::ElemPlano<NNODOS, PhysProp>::setDomain(Domain *theDomain)
  {
    ElemWithMaterial<NNODOS, PhysProp>::setDomain(theDomain);
    if(theDomain)
      checkElem();
    else
      std::cerr << "ElemPlano::setDomain -- Domain is null\n";
  }

//! @brief Return the position of the element centroid.
template <int NNODOS,class PhysProp>
Pos3d XC::ElemPlano<NNODOS, PhysProp>::getPosCdg(bool initialGeometry) const
  { return getPoligono(initialGeometry).Cdg(); }

//! @brief Return the element dimension (0, 1, 2 o3 3).
template <int NNODOS,class PhysProp>
size_t XC::ElemPlano<NNODOS, PhysProp>::getDimension(void) const
  { return 2; }

//! @brief Returns the perímetro del elemento.
template <int NNODOS,class PhysProp>
double XC::ElemPlano<NNODOS, PhysProp>::getPerimetro(bool initialGeometry) const
  { return getPoligono(initialGeometry).Perimetro(); }

//! @brief Returns the área del elemento.
template <int NNODOS,class PhysProp>
double XC::ElemPlano<NNODOS, PhysProp>::getArea(bool initialGeometry) const
  { return getPoligono(initialGeometry).Area(); }

//! @brief Calcula las áreas tributarias correspondientes a cada nodo.
template <int NNODOS,class PhysProp>
void XC::ElemPlano<NNODOS, PhysProp>::calculaAreasTributarias(bool initialGeometry) const
  {
    areasTributarias= getPoligono(initialGeometry).getAreasTributarias();
    this->vuelcaTributarias(areasTributarias);
  }

//! @brief Returns the área del elemento.
template <int NNODOS,class PhysProp>
double XC::ElemPlano<NNODOS, PhysProp>::getAreaTributaria(const Node *nod) const
  {
    double retval= 0.0;
    const int i= this->theNodes.find(nod);
    if(i>=0) //El nodo es de este elemento.
      retval= areasTributarias[i];
    return retval;
  }

//! @brief Returns the element contour as a polygon.
template <int NNODOS,class PhysProp>
Poligono3d XC::ElemPlano<NNODOS, PhysProp>::getPoligono(bool initialGeometry) const
  {
    const std::list<Pos3d> posiciones= this->getPosNodos(initialGeometry);
    return Poligono3d(posiciones.begin(),posiciones.end());
  }

//! @brief Returns a lado del elemento. 
// Redefine for elements with more than two nodes by face.
template <int NNODOS,class PhysProp>
Segmento3d XC::ElemPlano<NNODOS, PhysProp>::getLado(const size_t &i,bool initialGeometry) const
  {
    Segmento3d retval;
    const NodePtrsWithIDs &nodos= this->getNodePtrs();
    const size_t sz= nodos.size();
    if(i<sz)
      {
        const Pos3d p1= nodos.getPosNodo(i,initialGeometry);
        if(i<(sz-1))
          retval= Segmento3d(p1,nodos.getPosNodo(i+1,initialGeometry));
        else
          retval= Segmento3d(p1,nodos.getPosNodo(0,initialGeometry));
      }
    return retval;
  }

//! @brief Returns the cuadrado de la distancia desde el elemento al punto que
//! being passed as parameter.
template <int NNODOS,class PhysProp>
double XC::ElemPlano<NNODOS, PhysProp>::getDist2(const Pos2d &p,bool initialGeometry) const
  { return getDist2(To3dXY2d(p),initialGeometry); }

//! @brief Return the distancia desde el elemento al punto que
//! being passed as parameter.
template <int NNODOS,class PhysProp>
double XC::ElemPlano<NNODOS, PhysProp>::getDist(const Pos2d &p,bool initialGeometry) const
  { return getDist(To3dXY2d(p),initialGeometry); }

//! @brief Returns the cuadrado de la distancia desde el elemento al punto que
//! being passed as parameter.
template <int NNODOS,class PhysProp>
double XC::ElemPlano<NNODOS, PhysProp>::getDist2(const Pos3d &p,bool initialGeometry) const
  { return getPoligono(initialGeometry).dist2(p); }

//! @brief Return the distancia desde el elemento al punto que
//! being passed as parameter.
template <int NNODOS,class PhysProp>
double XC::ElemPlano<NNODOS, PhysProp>::getDist(const Pos3d &p,bool initialGeometry) const
  { return getPoligono(initialGeometry).dist(p); }

} // end of XC namespace
#endif
