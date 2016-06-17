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
//KDTreeNodes.cc

#include "KDTreeNodes.h"
#include "Node.h"
#include "xc_utils/src/geom/pos_vec/Pos3d.h"

//! @brief Constructor.
XC::NodePos::NodePos(const Node &n)
  : KDTreePos(n.getPosInicial3d()), nodPtr(&n) {}

//! @brief Constructor.
XC::NodePos::NodePos(const Pos3d &p)
  : KDTreePos(p), nodPtr(nullptr) {}
  
//! @brief Constructor.
XC::KDTreeNodes::KDTreeNodes(void)
  : tree_type(std::ptr_fun(NodePos::tac)), pend_optimizar(0) {}


void XC::KDTreeNodes::insert(const Node &n)
  {
    tree_type::insert(n);
  }

void XC::KDTreeNodes::erase(const Node &n)
  {
    tree_type::erase(n);
    pend_optimizar++;
    if(pend_optimizar>=10)
      {
	tree_type::optimise();
        pend_optimizar= 0;
      }
  }

void XC::KDTreeNodes::clear(void)
  { tree_type::clear(); }
  
//! @brief Devuelve el nodo más cercano a la posición being passed as parameter.
const XC::Node *XC::KDTreeNodes::getNearestNode(const Pos3d &pos) const
  {
    const Node *retval= nullptr;
    NodePos target(pos);
    std::pair<tree_type::const_iterator,double> found = find_nearest(target);
    if(found.first != end())
      retval= found.first->getNodePtr();
    return retval;
  }

const XC::Node *XC::KDTreeNodes::getNearestNode(const Pos3d &pos, const double &r) const
  {
    const Node *retval= nullptr;
    NodePos target(pos);
    std::pair<tree_type::const_iterator,double> found = find_nearest(target,r);
    if(found.first != end())
      retval= found.first->getNodePtr();
    return retval;
  }
