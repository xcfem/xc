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
//InteractionDiagram.h

#ifndef INTERACTION_DIAGRAM2D_H
#define INTERACTION_DIAGRAM2D_H

#include "xc_utils/src/geom/d2/poligonos2d/Poligono2d.h"

namespace XC {

class Vector;
class FiberSectionBase;
class InteractionDiagramData;

//! \@ingroup MATSCCDiagInt
//
//! @brief Diagrama de interacción (N,My) de una sección.
class InteractionDiagram2d: public Poligono2d
  {
  protected:
    Pos2d get_interseccion(const Pos2d &p) const;
  public:
    InteractionDiagram2d(void);
    InteractionDiagram2d(const Poligono2d &);
    virtual InteractionDiagram2d *clon(void) const;

    void Simplify(void);
    double FactorCapacidad(const Pos2d &esf_d) const;
    Vector FactorCapacidad(const GeomObj::list_Pos2d &lp) const;

    void Print(std::ostream &os) const;
  };

InteractionDiagram2d calcPlaneInteractionDiagram(const FiberSectionBase &scc,const InteractionDiagramData &, const double &);
InteractionDiagram2d calcNMyInteractionDiagram(const FiberSectionBase &scc,const InteractionDiagramData &);
InteractionDiagram2d calcNMzInteractionDiagram(const FiberSectionBase &scc,const InteractionDiagramData &);

} // end of XC namespace

#endif
