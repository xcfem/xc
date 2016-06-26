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
//KDTreeElements.h
#ifndef KDTreeElements_h
#define KDTreeElements_h

#include "xc_utils/src/geom/pos_vec/KDTreePos.h"
#include "xc_basic/src/kdtree++/kdtree.hpp"

class Pos3d;

namespace XC {
class Element;

//! \ingroup Elem
//
//! @brief Posición de un elemento para el KDTree.
class ElemPos: public KDTreePos
  {
  private:
    const Element *elemPtr;
  public:
    ElemPos(const Element &);
    explicit ElemPos(const Pos3d &p);
    inline const Element *getElementPtr(void) const
      { return elemPtr; }

    static inline double tac( ElemPos p, size_t k ) { return p[k]; }
  };

inline bool operator==(const ElemPos &A,const ElemPos &B)
  { return ((A.getElementPtr()== B.getElementPtr()) && (A[0] == B[0]) && (A[1] == B[1]) && (A[2] == B[2])); }


class KDTreeElements: protected kd_tree::KDTree<3, ElemPos, std::pointer_to_binary_function<ElemPos,size_t,double> >
  {
    size_t pend_optimizar;
  public:
    typedef kd_tree::KDTree<3, ElemPos, std::pointer_to_binary_function<ElemPos,size_t,double> > tree_type;
    KDTreeElements(void);

    void insert(const Element &);
    void erase(const Element &);
    void clear(void);

    const Element *getNearestElement(const Pos3d &pos) const;
    const Element *getNearestElement(const Pos3d &pos, const double &r) const;
  };

} // end of XC namespace 


#endif
