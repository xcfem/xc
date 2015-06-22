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
#include "ParticlePos3d.h"
#include "xc_utils/src/geom/pos_vec/Pos2d.h"
#include "xc_utils/src/geom/pos_vec/Pos3d.h"

//! @brief  Constructor
XC::ParticlePos3d::ParticlePos3d(const double &r_coord,
                                 const double &s_coord,
				 const double &t_coord)
  {
    set_coordinates(r_coord,s_coord,t_coord);
  }

//! @brief  Constructor
XC::ParticlePos3d::ParticlePos3d(const Pos2d &p)
  { set_coordinates(p); }

//! @brief  Constructor
XC::ParticlePos3d::ParticlePos3d(const Pos3d &p)
  { set_coordinates(p); }

//! @brief Asigna valores a las coordenadas.
void XC::ParticlePos3d::set_coordinates(const double &r_coord,const double &s_coord,const double &t_coord)
  {
    r = r_coord;
    s = s_coord;
    t = t_coord;
  }

//! @brief Asigna valores a las coordenadas.
void XC::ParticlePos3d::set_coordinates(const Pos3d &p)
  { set_coordinates(p.x(),p.y(),p.z());}

//! @brief Asigna valores a las coordenadas.
void XC::ParticlePos3d::set_coordinates(const Pos2d &p)
  { set_coordinates(p.x(),p.y());}

void XC::ParticlePos3d::Print(std::ostream &os) const
  {
    os << "r= " << r
       << " s= " << s
       << " t= " << t;
  }


std::ostream &XC::operator<<(std::ostream &os, const ParticlePos3d &p)
  {
    p.Print(os);
    return os;
  }





