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
//DqPtrsNode.h
//deque de punteros a nodo.


#ifndef DQPTRSNODE_H
#define DQPTRSNODE_H

#include "DqPtrs.h"
#include "domain/mesh/node/KDTreeNodes.h"

class Vector3d;
class ExprAlgebra;

namespace XC {
class TrfGeom;

//!  \ingroup Set
//! 
//!  \brief Contenedor de punteros a nodos.
//! 
class DqPtrsNode: public DqPtrs<Node>
  {
    KDTreeNodes kdtreeNodos; //!< KDTree para acelerar la búsqueda de nodos por coordenadas.
  protected:
    void crea_arbol(void);
  public:
    typedef DqPtrs<Node>::const_iterator const_iterator;
    typedef DqPtrs<Node>::iterator iterator;
    typedef DqPtrs<Node>::reference reference;
    typedef DqPtrs<Node>::const_reference const_reference;
    typedef DqPtrs<Node>::size_type size_type;

    DqPtrsNode(EntCmd *owr= nullptr);
    DqPtrsNode(const DqPtrsNode &otro);
    explicit DqPtrsNode(const std::deque<Node *> &ts);
    explicit DqPtrsNode(const std::set<const Node *> &ts);
    DqPtrsNode &operator=(const DqPtrsNode &otro);
    void agrega(const DqPtrsNode &otro);
    //void agrega_cond(const DqPtrsNode &otro,const std::string &cond);
    bool push_back(Node *);
    bool push_front(Node *);
    void clearAll(void);
    inline iterator begin(void)
      { return DqPtrs<Node>::begin(); }
    inline iterator end(void)
      { return DqPtrs<Node>::end(); }
    inline const_iterator begin(void) const
      { return DqPtrs<Node>::begin(); }
    inline const_iterator end(void) const
      { return DqPtrs<Node>::end(); }
    void mueve(const Vector3d &);
    void transforma(const TrfGeom &trf);

    size_t getNumLiveNodes(void) const;
    size_t getNumDeadNodes(void) const;

    bool InNodeTag(const int tag_nodo) const;
    bool InNodeTags(const ID &tag_nodos) const;
    std::set<int> getTags(void) const;

    Node *buscaNodo(const int &tag);
    const Node *buscaNodo(const int &tag) const;
    Node *getNearestNode(const Pos3d &p);
    const Node *getNearestNode(const Pos3d &p) const;

    void numera(void);
  };

} //end of XC namespace
#endif

