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
//MeshEdges.h
                                                                        
#ifndef MeshEdges_h
#define MeshEdges_h

#include "xc_utils/src/nucleo/EntCmd.h"
#include "domain/mesh/MeshEdge.h"

class Polilinea3d;


namespace XC {
class Element;
class Domain;
class ID;
 

//! @ingroup Elem
//
//! @brief Element edge container.
class MeshEdges: public EntCmd, public std::deque<MeshEdge>
  {
  public:
    MeshEdges(void);

    std::deque<const MeshEdge *> getLoop(const MeshEdge *) const;
    MeshEdges getEdgesNotInLoop(const std::deque<const MeshEdge *> &) const;
    std::deque<Polilinea3d> getContours(bool undeformedGeometry) const;
    void print(std::ostream &) const;
  };

std::deque<Polilinea3d> getContours(MeshEdges edges,bool undeformedGeometry);

inline std::ostream &operator<<(std::ostream &os, const MeshEdges &me)
  {
    me.print(os);
    return os;
  }
} // end of XC namespace

#endif

