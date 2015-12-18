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
#ifndef GAUSSPOINT_H
#define GAUSSPOINT_H

#include "domain/mesh/element/ParticlePos3d.h"

namespace XC {

//! @ingroup Elem
//
//! @brief Punto de Gauss en tres dimensiones.
  class GaussPoint: public ParticlePos3d
  {
  protected:
    double rw;
    double sw;
    double tw;

  public:
    GaussPoint(const double &r_coord = 0,
               const double &s_coord = 0,
               const double &t_coord = 0,
               const double &r_weight = 0,
               const double &s_weight = 0,
               const double &t_weight = 0);
    GaussPoint(const Pos2d &,const double &);
    GaussPoint(const Pos3d &,const double &);
    void set_weights(const double &r_weight = 0, const double &s_weight = 0, const double &t_weight = 0);
    
    inline const double &weight() const
      { return rw; }
    inline const double &r_weight() const
      { return rw; }
    inline const double &s_weight() const
      { return sw; }
    inline const double &t_weight() const    
      { return tw; }

  };
} // end of XC namespace

#endif 
//
