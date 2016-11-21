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
//! @defgroup ElemPlanos Elementos bidimensionales (problemas planos, láminas,...).
//!
//! \ingroup ElemPlanos
//
//! @brief Clase base para los cuadriláteros de cuatro nodos.
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
                << " tiene punteros a nodo, nulos." << std::endl;
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

//! @brief Asigna el dominio al elemento.
template <int NNODOS,class PhysProp>
void XC::ElemPlano<NNODOS, PhysProp>::setDomain(Domain *theDomain)
  {
    ElemWithMaterial<NNODOS, PhysProp>::setDomain(theDomain);
    if(theDomain)
      checkElem();
    else
      std::cerr << "ElemPlano::setDomain -- Domain is null\n";
  }

//! @brief Devuelve la posición del centro de gravedad del elemento.
template <int NNODOS,class PhysProp>
Pos3d XC::ElemPlano<NNODOS, PhysProp>::getPosCdg(bool initialGeometry) const
  { return getPoligono(initialGeometry).Cdg(); }

//! @brief Devuelve la dimensión del elemento.
template <int NNODOS,class PhysProp>
size_t XC::ElemPlano<NNODOS, PhysProp>::getDimension(void) const
  { return 2; }

//! @brief Devuelve el perímetro del elemento.
template <int NNODOS,class PhysProp>
double XC::ElemPlano<NNODOS, PhysProp>::getPerimetro(bool initialGeometry) const
  { return getPoligono(initialGeometry).Perimetro(); }

//! @brief Devuelve el área del elemento.
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

//! @brief Devuelve el área del elemento.
template <int NNODOS,class PhysProp>
double XC::ElemPlano<NNODOS, PhysProp>::getAreaTributaria(const Node *nod) const
  {
    double retval= 0.0;
    const int i= this->theNodes.find(nod);
    if(i>=0) //El nodo es de este elemento.
      retval= areasTributarias[i];
    return retval;
  }

//! @brief Devuelve el polígono que forma el contorno del elemento.
template <int NNODOS,class PhysProp>
Poligono3d XC::ElemPlano<NNODOS, PhysProp>::getPoligono(bool initialGeometry) const
  {
    const std::list<Pos3d> posiciones= this->getPosNodos(initialGeometry);
    return Poligono3d(posiciones.begin(),posiciones.end());
  }

//! @brief Devuelve un lado del elemento. 
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

//! @brief Devuelve el cuadrado de la distancia desde el elemento al punto que
//! se pasa como parámetro.
template <int NNODOS,class PhysProp>
double XC::ElemPlano<NNODOS, PhysProp>::getDist2(const Pos2d &p,bool initialGeometry) const
  { return getDist2(To3dXY2d(p),initialGeometry); }

//! @brief Devuelve la distancia desde el elemento al punto que
//! se pasa como parámetro.
template <int NNODOS,class PhysProp>
double XC::ElemPlano<NNODOS, PhysProp>::getDist(const Pos2d &p,bool initialGeometry) const
  { return getDist(To3dXY2d(p),initialGeometry); }

//! @brief Devuelve el cuadrado de la distancia desde el elemento al punto que
//! se pasa como parámetro.
template <int NNODOS,class PhysProp>
double XC::ElemPlano<NNODOS, PhysProp>::getDist2(const Pos3d &p,bool initialGeometry) const
  { return getPoligono(initialGeometry).dist2(p); }

//! @brief Devuelve la distancia desde el elemento al punto que
//! se pasa como parámetro.
template <int NNODOS,class PhysProp>
double XC::ElemPlano<NNODOS, PhysProp>::getDist(const Pos3d &p,bool initialGeometry) const
  { return getPoligono(initialGeometry).dist(p); }

} // end of XC namespace
#endif
