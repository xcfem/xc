//----------------------------------------------------------------------------
//  xc utils library bilioteca de comandos para el intérprete del lenguaje
//  de entrada de datos.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  XC utils is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//CircularSector3d.cc

#include "CircularSector3d.h"
#include "../cgal_types.h"
#include "utility/geom/d2/Plane.h"
#include "utility/utils/misc_utils/matem.h"
#include <plotter.h>

#include "utility/geom/pos_vec/Pos2dArray.h"
#include "utility/geom/pos_vec/Pos3dArray.h"
#include "utility/geom/pos_vec/Vector3d.h"

//! @brief Constructor virtual.
D2to3d *CircularSector3d::getCopy(void) const
  { return new CircularSector3d(*this); }

//! @brief Constructor.
CircularSector3d::CircularSector3d(const Pos3d centro,const GEOM_FT &rad,const double &th1,const double &th2)
  : D2to3d(centro), sect_circ()
  { sect_circ= CircularSector2d(Circle2d(to_2d(centro),rad),th1,th2); }

//! @brief Constructor.
CircularSector3d::CircularSector3d(const GEOM_FT &rad2,const Pos3d &centro,const double &th1,const double &th2)
  : D2to3d(centro), sect_circ()
  { sect_circ= CircularSector2d(Circle2d(rad2,to_2d(centro)),th1,th2); }

CircularSector3d::CircularSector3d(const Pos3d &c,const double &r,const Vector3d &n,const Vector3d &Xaxis,const double &th1,const double &th2)
  : D2to3d(c,n,Xaxis), sect_circ()
  { sect_circ= CircularSector2d(Circle2d(to_2d(c),r),th1,th2); }

 
//! @brief Constructor; circular sector defined by three points p1->p2->p3.
CircularSector3d::CircularSector3d(const Pos3d &p1,const Pos3d &p2,const Pos3d &p3)
  : D2to3d(p1,p2,p3), sect_circ()
  {
    const Pos2d pA= to_2d(p1);
    const Pos2d pB= to_2d(p2);
    const Pos2d pC= to_2d(p3);
    const Circle2d C(pA,pB,pC);
    const Pos2d po= C.Centro();
    //Adjust the center and the coordinate system axis.
    ThreePoints(to_3d(po),p1,p3);
    const Pos2d pa= to_2d(p1);
    const Pos2d pb= to_2d(p2);
    const Pos2d pc= to_2d(p3);
    sect_circ= CircularSector2dThreepoints(pa,pb,pc);
  }

//! @brief Return el centro del círculo.
Pos3d CircularSector3d::Centro(void) const
  { return to_3d(sect_circ.Centro()); }

//! @brief Return the point inicial del arco.
Pos3d CircularSector3d::PInic(void) const
  { return to_3d(sect_circ.PInic()); }

//! @brief Return the point final del arco.
Pos3d CircularSector3d::PFin(void) const
  { return to_3d(sect_circ.PFin()); }

//! @brief Return the point medio del arco.
Pos3d CircularSector3d::PMed(void) const
  { return to_3d(sect_circ.PMed()); }

Pos3d CircularSector3d::getCenterOfMass(void) const
  { return Centro(); }

//! @brief Return the position of the arc centroid.
Pos3d CircularSector3d::ArcCentroid(void) const
  { return to_3d(sect_circ.ArcCentroid()); }

GEOM_FT CircularSector3d::getSquaredRadius(void) const
  { return sect_circ.getSquaredRadius(); }
GEOM_FT CircularSector3d::getRadius(void) const
  { return sect_circ.getRadius(); }
GEOM_FT CircularSector3d::getDiameter(void) const
  { return sect_circ.getDiameter(); }

double CircularSector3d::getAngle(const Pos3d &p) const
  { return sect_circ.getAngle(to_2d(p)); }

//! @brief Returns the parameter of the point in the arc (distance to the arc's first point measured over the arc)
double CircularSector3d::getLambda(const Pos3d &p) const
  { return sect_circ.getLambda(to_2d(p)); }

GEOM_FT CircularSector3d::Ix(void) const
  { return sect_circ.Ix(); }
GEOM_FT CircularSector3d::Iy(void) const
  { return sect_circ.Iy(); }
GEOM_FT CircularSector3d::Pxy(void) const
  { return sect_circ.Pxy(); }
GEOM_FT CircularSector3d::Iz(void) const
  { return sect_circ.Iz(); }

GEOM_FT CircularSector3d::GetMax(unsigned short int i) const
  {
    std::cerr << "CircularSector3d::GetMax(i), not implemented." << std::endl;
    return 1;
  }
GEOM_FT CircularSector3d::GetMin(unsigned short int i) const
  {
    std::cerr << "CircularSector3d::GetMin(i), not implemented." << std::endl;
    return -1;
  }

//! @brief Return true if the point is in the plane.
bool CircularSector3d::In(const Pos3d &p, const double &tol) const
  {
    if(getPlane().In(p,tol))
      return sect_circ.In(to_2d(p),tol);
    else
      return false;
  }

//! @brief Return n points equally spaced over the circle arc.
Pos3dArray CircularSector3d::getArcPoints(const size_t &n) const
  {
    Pos3dArray retval= to_3d(sect_circ.getArcPoints(n));
    return retval;
  }

void CircularSector3d::Print(std::ostream &os) const
  { os << sect_circ; }

bool operator ==(const CircularSector3d &a,const CircularSector3d &b)
  {
    if((const D2to3d &) a ==(const D2to3d &) b)  
      return ( a.sect_circ == b.sect_circ );
    else
      return false;
  }
