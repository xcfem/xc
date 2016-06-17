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
//KDTreeElements.cc

#include "KDTreeElements.h"
#include "Element.h"
#include "xc_utils/src/geom/pos_vec/Pos3d.h"

//! @brief Constructor.
XC::ElemPos::ElemPos(const Element &e)
  :  KDTreePos(e.getPosCdg()), elemPtr(&e){}

//! @brief Constructor.
XC::ElemPos::ElemPos(const Pos3d &p)
  : KDTreePos(p), elemPtr(nullptr) {}
  
//! @brief Constructor.
XC::KDTreeElements::KDTreeElements(void)
  : tree_type(std::ptr_fun(ElemPos::tac)), pend_optimizar(0) {}


void XC::KDTreeElements::insert(const Element &n)
  {
    tree_type::insert(n);
  }

void XC::KDTreeElements::erase(const Element &n)
  {
    tree_type::erase(n);
    pend_optimizar++;
    if(pend_optimizar>=10)
      {
	tree_type::optimise();
        pend_optimizar= 0;
      }
  }

void XC::KDTreeElements::clear(void)
  { tree_type::clear(); }

//! @brief Devuelve el nodo más cercano a la posición being passed as parameter.
const XC::Element *XC::KDTreeElements::getNearestElement(const Pos3d &pos) const
  {
    const Element *retval= nullptr;
    ElemPos target(pos);
    std::pair<tree_type::const_iterator,double> found = find_nearest(target);
    if(found.first != end())
      retval= found.first->getElementPtr();
    return retval;
  }

const XC::Element *XC::KDTreeElements::getNearestElement(const Pos3d &pos, const double &r) const
  {
    const Element *retval= nullptr;
    ElemPos target(pos);
    std::pair<tree_type::const_iterator,double> found = find_nearest(target,r);
    if(found.first != end())
      retval= found.first->getElementPtr();
    return retval;
  }
