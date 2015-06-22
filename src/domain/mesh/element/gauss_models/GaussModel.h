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
//
#ifndef GAUSSMODEL_H
#define GAUSSMODEL_H

#include "GaussPoint.h"
#include "xc_utils/src/geom/pos_vec/Pos2d.h"
#include "xc_utils/src/geom/pos_vec/Pos3d.h"
#include <deque>

namespace XC {

//! @ingroup Elem
//
//! @brief Punto de Gauss en tres dimensiones.
class GaussModel
  {
  protected:
    std::deque<Pos3d> coo_ref_nodes;//!< Coordenadas de los nodos de referencia.
    std::deque<GaussPoint> gauss_points;//!< Puntos de Gauss.
  public:
    GaussModel(void);
    GaussModel(const Pos2d &n1,const Pos2d &n2,const Pos2d &n3,
               const GaussPoint &p);
    GaussModel(const Pos2d &n1,const Pos2d &n2,const Pos2d &n3,
               const GaussPoint &p1,const GaussPoint &p2,const GaussPoint &p3);
    GaussModel(const Pos2d &n1,const Pos2d &n2,const Pos2d &n3,const Pos2d &n4,
               const GaussPoint &p1,const GaussPoint &p2,const GaussPoint &p3,const GaussPoint &p4);
    GaussModel(const Pos2d &n1,const Pos2d &n2,const Pos2d &n3,const Pos2d &n4,
               const Pos2d &n5,const Pos2d &n6,const Pos2d &n7,const Pos2d &n8,const Pos2d &n9,
               const GaussPoint &p1,const GaussPoint &p2,const GaussPoint &p3,const GaussPoint &p4,
               const GaussPoint &p5,const GaussPoint &p6,const GaussPoint &p7,const GaussPoint &p8,const GaussPoint &p9);

    inline size_t getNumNodosReferencia(void) const
      { return coo_ref_nodes.size(); }
    const std::deque<Pos3d> &getPosNodosReferencia(void) const
      { return coo_ref_nodes; }
    inline size_t getNumGaussPoints(void) const
      { return gauss_points.size(); }
    const std::deque<GaussPoint> &getPuntosGauss(void) const
      { return gauss_points; }

  };

const GaussModel gauss_model_empty;
const GaussModel gauss_model_tria1(Pos2d(0,0),Pos2d(1,0),Pos2d(0,1),
                                   GaussPoint(Pos2d(0.333333333333333,0.333333333333333),0.5));

const GaussModel gauss_model_tria3(Pos2d(0,0),Pos2d(1,0),Pos2d(0,1),
                                   GaussPoint(Pos2d(0.2,0.2),0.3334),
                                   GaussPoint(Pos2d(0.8,0.1),0.3334),
                                   GaussPoint(Pos2d(0.1,0.8),0.3334));
const double root3= sqrt(3.0);
const double one_over_root3= 1.0/root3;
const GaussModel gauss_model_quad4(Pos2d(-1,-1),Pos2d(1,-1),Pos2d(1,1),Pos2d(-1,1),
                                    GaussPoint(Pos2d(-one_over_root3,-one_over_root3),1),
                                    GaussPoint(Pos2d(one_over_root3,-one_over_root3),1),
                                    GaussPoint(Pos2d(one_over_root3,one_over_root3),1),
                                    GaussPoint(Pos2d(-one_over_root3,one_over_root3),1));

const double root5= sqrt(5.0);
const double root3_over_root5= root3/root5;

//Modelo de Gauus para el cuadrilátero de nueve nodos.
//
//  6----5----4
//  |         |
//  7    8    3
//  |         |
//  0----1----2
//
const GaussModel gauss_model_quad9(Pos2d(-1,-1),Pos2d(0,-1),Pos2d(1,-1),Pos2d(1,0),
                                    Pos2d(1,1),Pos2d(0,1),Pos2d(-1,1),Pos2d(-1,0),Pos2d(0,0),
                                    GaussPoint(Pos2d(-root3_over_root5,-root3_over_root5),25.0/81.0),
                                    GaussPoint(Pos2d(0,-root3_over_root5),40.0/81.0),
                                    GaussPoint(Pos2d(root3_over_root5,-root3_over_root5),25.0/81.0),
 				    GaussPoint(Pos2d(root3_over_root5,0),40.0 / 81.0),
				    GaussPoint(Pos2d(root3_over_root5,root3_over_root5),25.0 / 81.0),
				    GaussPoint(Pos2d(0,root3_over_root5),40.0 / 81.0),
 				    GaussPoint(Pos2d(-root3_over_root5,root3_over_root5),25.0 / 81.0),
				    GaussPoint(Pos2d(-root3_over_root5,0),40.0 / 81.0),
                                    GaussPoint(Pos2d(0,0),64.0 / 81.0));
} // fin namespace XC

#endif 
//
