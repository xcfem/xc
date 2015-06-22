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
#include "GaussPoint.h"
#include "xc_utils/src/geom/pos_vec/Pos2d.h"
#include "xc_utils/src/geom/pos_vec/Pos3d.h"

//! @brief  Constructor
XC::GaussPoint::GaussPoint(const double &r_coord,
               const double &s_coord,
               const double &t_coord,
               const double &r_weight,
               const double &s_weight,
               const double &t_weight)
  : ParticlePos3d(r_coord,s_coord,t_coord)
  { set_weights(r_weight,s_weight,t_weight); }

//! @brief  Constructor
XC::GaussPoint::GaussPoint(const Pos2d &p,const double &w)
  : ParticlePos3d(p)
  { set_weights(w); }

//! @brief  Constructor
XC::GaussPoint::GaussPoint(const Pos3d &p,const double &w)
  : ParticlePos3d(p)
  { set_weights(w); }

//! @brief Asigna valores a los pesos.
void XC::GaussPoint::set_weights(const double &r_weight, const double &s_weight, const double &t_weight)
  {
    rw = r_weight;
    sw = s_weight;
    tw = t_weight;
  }






