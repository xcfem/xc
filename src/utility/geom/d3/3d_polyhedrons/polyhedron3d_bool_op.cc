//----------------------------------------------------------------------------
//  xc utils library bilioteca de comandos para el intérprete del lenguaje
//  de entrada de datos.
//
//  Copyright (C)  Luis C. Pérez Tato
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
//polyhedron3d_bool_op.cc

#include "polyhedron3d_bool_op.h"
#include <CGAL/Gmpz.h>
#include <CGAL/Gmpq.h>
#include <CGAL/Homogeneous.h>
#include <CGAL/Cartesian.h>
#include <CGAL/Quotient.h>
#include <CGAL/Nef_polyhedron_3.h>
#include "Polyhedron3d.h"
#include <CGAL/cartesian_homogeneous_conversion.h>
#include <CGAL/Cartesian_converter.h>
#include "PolygonMap.h"
#include "utility/geom/pos_vec/Pos3d.h"

typedef CGAL::Gmpz RT;
typedef CGAL::Homogeneous<RT> KernelHomog;
typedef CGAL::Polyhedron_3<KernelHomog> Polyhedron;
typedef CGAL::Nef_polyhedron_3<KernelHomog> Nef_pol3;
typedef CGAL::Cartesian<RT> KernelCart;
typedef Nef_pol3::Point_3 NPPoint;

inline NPPoint Pos3d_to_Point(const Pos3d &p3d)
  { return NPPoint(p3d.hx(),p3d.hy(),p3d.hz(),p3d.hw()); }

class Pos3d2Point
  {
  public:
   NPPoint operator()(const Pos3d &p) const
     { return Pos3d_to_Point(p); }
  };

struct GmpzConverter
  {
    GEOM_FT operator()(const CGAL::Quotient<RT> &g) const
      {
        return to_double(g);
      }
  };


CGPoint_3 Point_to_Pos3d(const Polyhedron::Vertex::Point &np)
  {
    
//     CGAL::Point_3< CGAL::Cartesian<CGAL::Quotient<RT> > > Q= homogeneous_to_quotient_cartesian(np);
//     CGAL::Cartesian_converter<CGAL::Cartesian<CGAL::Quotient<RT> >,GEOMKernel,GmpzConverter> conv;
    return CGPoint_3(to_double(np.hx()),to_double(np.hy()),to_double(np.hz()),to_double(np.hw()));
  }

class Point2Pos3d
  {
  public:
    CGPoint_3 operator()(const Polyhedron::Vertex::Point &np) const
     { return Point_to_Pos3d(np); }
  };


Nef_pol3 Polyhedron3d_to_Nef_3(const CGPolyhedron_3 &pol3)
  {
    typedef Polyhedron::HalfedgeDS HalfedgeDS;
    Polyhedron tmp;
    if(pol3.is_closed())
      {
        Build_tdest_polyhedron<CGPolyhedron_3,HalfedgeDS,Pos3d2Point> bpoly(pol3);
        tmp.delegate(bpoly);
      }
    else
      std::cerr << __FUNCTION__
		<< "; not a closed polyhedron." << std::endl;
    return Nef_pol3(tmp);
  }

CGPolyhedron_3 Nef_3_to_Polyhedron3d(Nef_pol3 &np)
  {
    typedef CGPolyhedron_3::HalfedgeDS HalfedgeDS;
    CGPolyhedron_3 retval;
    if(np.is_simple())
      {
        Polyhedron tmp;
        np.convert_to_Polyhedron(tmp);
        Build_tdest_polyhedron<Polyhedron,HalfedgeDS,Point2Pos3d> bpoly(tmp);
        retval.delegate(bpoly);
      }
    else
      std::cerr << __FUNCTION__
		<< "; not a simple polyhedron." << std::endl;
    return retval;
  }

Polyhedron3d Union(const Polyhedron3d &a,const Polyhedron3d &b)
  {
    Nef_pol3 nfa= Polyhedron3d_to_Nef_3(a.cgpolyhedron);
    Nef_pol3 nfb= Polyhedron3d_to_Nef_3(b.cgpolyhedron);
    Nef_pol3 nfu= nfa+nfb;
    return Polyhedron3d(Nef_3_to_Polyhedron3d(nfu));
  }
Polyhedron3d intersection(const Polyhedron3d &a,const Polyhedron3d &b)
  {
    Nef_pol3 nfa= Polyhedron3d_to_Nef_3(a.cgpolyhedron);
    Nef_pol3 nfb= Polyhedron3d_to_Nef_3(b.cgpolyhedron);
    Nef_pol3 nfi= nfa*nfb;
    return Polyhedron3d(Nef_3_to_Polyhedron3d(nfi));
  }
Polyhedron3d Diferencia(const Polyhedron3d &a,const Polyhedron3d &b)
  {
    Nef_pol3 nfa= Polyhedron3d_to_Nef_3(a.cgpolyhedron);
    Nef_pol3 nfb= Polyhedron3d_to_Nef_3(b.cgpolyhedron);
    Nef_pol3 nfd= nfa-nfb;
    return Polyhedron3d(Nef_3_to_Polyhedron3d(nfd));
  }
