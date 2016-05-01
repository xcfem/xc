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
//KDTreePos.cc

#include "KDTreePos.h"
#include "xc_basic/src/util/matem.h"
#include "xc_utils/src/geom/pos_vec/Pos3d.h"

//! @brief Constructor.
XC::KDTreePos::KDTreePos(const Pos3d &p)
  { d[0] = p.x(); d[1] = p.y(); d[2] = p.z(); }
  
//! @brief Distancia entre posiciones.
double XC::KDTreePos::distance_to(const KDTreePos &otra) const
  {
    double dist = sqr(d[0]-otra.d[0]);
    dist+= sqr(d[1]-otra.d[1]);
    dist+= sqr(d[2]-otra.d[2]);
    return sqrt(dist);
  }
