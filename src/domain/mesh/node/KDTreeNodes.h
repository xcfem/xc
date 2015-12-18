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
//KDTreeNodes.h
#ifndef KDTreeNodes_h
#define KDTreeNodes_h

#include "../KDTreePos.h"
#include "xc_basic/src/kdtree++/kdtree.hpp"

class Pos3d;

namespace XC {
class Node;

//! \ingroup Nod
//
//! @brief Posición de un nodo para el KDTree.
class NodePos: public KDTreePos
  {
  private:
    const Node *nodPtr;
  public:
    NodePos(const Node &);
    explicit NodePos(const Pos3d &p);
    inline const Node *getNodePtr(void) const
      { return nodPtr; }
    static inline double tac( NodePos p, size_t k ) { return p[k]; }
  };

inline bool operator==(const NodePos &A,const NodePos &B)
  { return ((A.getNodePtr()== B.getNodePtr()) && (A[0] == B[0]) && (A[1] == B[1]) && (A[2] == B[2])); }


class KDTreeNodes: protected kd_tree::KDTree<3, NodePos, std::pointer_to_binary_function<NodePos,size_t,double> >
  {
    size_t pend_optimizar;
  public:
    typedef kd_tree::KDTree<3, NodePos, std::pointer_to_binary_function<NodePos,size_t,double> > tree_type;
    KDTreeNodes(void);

    void insert(const Node &);
    void erase(const Node &);
    void clear(void);

    const Node *getNearestNode(const Pos3d &pos) const;
    const Node *getNearestNode(const Pos3d &pos, const double &r) const;
  };

} // end of XC namespace 


#endif
