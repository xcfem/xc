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

#ifndef MeshEdge_h
#define MeshEdge_h

#include "domain/mesh/element/Element.h"

namespace XC {

//! \ingroup Mesh
//
//! @brief Mesh edge.
class MeshEdge: public EntCmd
  {
  public:
    typedef Element::NodesEdge NodesEdge; 
  private:
    NodesEdge nodes; //!< Nodes of a mesh edge.
    typedef std::set<const Element *> ElementConstPtrSet;
  public:
    MeshEdge(void);
    MeshEdge(const NodesEdge &);
    NodesEdge getNodes(void) const;
    inline size_t size(void) const
      { return nodes.size(); }
    const Node *getFirstNode(void) const;
    const Node *getLastNode(void) const;
    bool isConnected(const Node &) const;
    bool isConnected(const MeshEdge &) const;
    ElementConstPtrSet getConnectedElements(void) const;
    bool operator==(const MeshEdge &) const;
    void print(std::ostream &) const;
  };

inline std::ostream &operator<<(std::ostream &os, const MeshEdge &me)
  {
    me.print(os);
    return os;
  }

} // end of XC namespace

#endif

