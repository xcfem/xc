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
#ifndef ParticlePos3d_H
#define ParticlePos3d_H

#include <iostream>

class Pos2d;
class Pos3d;

namespace XC {

//! @ingroup Elem
//
//! @brief Natural coordinates of an element's particle
class ParticlePos3d
  {
  protected:
    double r;
    double s;
    double t;

  public:
    ParticlePos3d(const double &r_coord = 0,
                  const double &s_coord = 0,
		  const double &t_coord = 0);
    ParticlePos3d(const Pos2d &);
    ParticlePos3d(const Pos3d &);
    void set_coordinates(const double &r_coord = 0,const double &s_coord = 0,const double &t_coord = 0);
    void set_coordinates(const Pos3d &);
    void set_coordinates(const Pos2d &);
    
    inline const double &r_coordinate(void) const
      { return r; }
    inline void set_r_coordinate(const double &r_coord)
      { r= r_coord; }
    inline const double &s_coordinate(void) const
      { return s; }
    inline void set_s_coordinate(const double &s_coord)
      { s= s_coord; }
    inline const double &t_coordinate(void) const
      { return t; }
    inline void set_t_coordinate(const double &t_coord)
      { r= t_coord; }
    void Print(std::ostream &) const;
  };

std::ostream &operator<<(std::ostream &, const ParticlePos3d &);

} // end of XC namespace

#endif 
//
