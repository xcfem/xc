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
//
#include "GaussModel.h"

//! @brief  Constructor
XC::GaussModel::GaussModel(void)
  {}

//! @brief  Constructor: 3 node triangle 1 Gauss point. 
XC::GaussModel::GaussModel(const Pos2d &n1,const Pos2d &n2,const Pos2d &n3,const GaussPoint &p1)
  {
    coo_ref_nodes.push_back(Pos3d(n1.x(),n1.y(),0));
    coo_ref_nodes.push_back(Pos3d(n2.x(),n2.y(),0));
    coo_ref_nodes.push_back(Pos3d(n3.x(),n3.y(),0));
    gauss_points.push_back(p1);
  }

//! @brief  Constructor: triángulo 3 nodos. 
XC::GaussModel::GaussModel(const Pos2d &n1,const Pos2d &n2,const Pos2d &n3,const GaussPoint &p1,const GaussPoint &p2,const GaussPoint &p3)
  {
    coo_ref_nodes.push_back(Pos3d(n1.x(),n1.y(),0));
    coo_ref_nodes.push_back(Pos3d(n2.x(),n2.y(),0));
    coo_ref_nodes.push_back(Pos3d(n3.x(),n3.y(),0));
    gauss_points.push_back(p1);
    gauss_points.push_back(p2);
    gauss_points.push_back(p3);
  }

//! @brief  Constructor: cuadrilátero 4 nodos. 
XC::GaussModel::GaussModel(const Pos2d &n1,const Pos2d &n2,const Pos2d &n3,const Pos2d &n4,const GaussPoint &p1,const GaussPoint &p2,const GaussPoint &p3,const GaussPoint &p4)
  {
    coo_ref_nodes.push_back(Pos3d(n1.x(),n1.y(),0));
    coo_ref_nodes.push_back(Pos3d(n2.x(),n2.y(),0));
    coo_ref_nodes.push_back(Pos3d(n3.x(),n3.y(),0));
    coo_ref_nodes.push_back(Pos3d(n4.x(),n4.y(),0));
    gauss_points.push_back(p1);
    gauss_points.push_back(p2);
    gauss_points.push_back(p3);
    gauss_points.push_back(p4);
  }

//! @brief  Constructor: cuadrilátero 9 nodos. 
XC::GaussModel::GaussModel(const Pos2d &n1,const Pos2d &n2,const Pos2d &n3,const Pos2d &n4,
               const Pos2d &n5,const Pos2d &n6,const Pos2d &n7,const Pos2d &n8,const Pos2d &n9,
               const GaussPoint &p1,const GaussPoint &p2,const GaussPoint &p3,const GaussPoint &p4,
               const GaussPoint &p5,const GaussPoint &p6,const GaussPoint &p7,const GaussPoint &p8,const GaussPoint &p9)

  {
    coo_ref_nodes.push_back(Pos3d(n1.x(),n1.y(),0));
    coo_ref_nodes.push_back(Pos3d(n2.x(),n2.y(),0));
    coo_ref_nodes.push_back(Pos3d(n3.x(),n3.y(),0));
    coo_ref_nodes.push_back(Pos3d(n4.x(),n4.y(),0));
    coo_ref_nodes.push_back(Pos3d(n5.x(),n5.y(),0));
    coo_ref_nodes.push_back(Pos3d(n6.x(),n6.y(),0));
    coo_ref_nodes.push_back(Pos3d(n7.x(),n7.y(),0));
    coo_ref_nodes.push_back(Pos3d(n8.x(),n8.y(),0));
    coo_ref_nodes.push_back(Pos3d(n9.x(),n9.y(),0));
    gauss_points.push_back(p1);
    gauss_points.push_back(p2);
    gauss_points.push_back(p3);
    gauss_points.push_back(p4);
    gauss_points.push_back(p5);
    gauss_points.push_back(p6);
    gauss_points.push_back(p7);
    gauss_points.push_back(p8);
    gauss_points.push_back(p9);
  }
