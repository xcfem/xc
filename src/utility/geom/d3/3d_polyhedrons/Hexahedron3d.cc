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
//Hexahedron3d.cc

#include "Hexahedron3d.h"
#include "Polyhedron3d.h"
#include "utility/geom/pos_vec/Pos3d.h"
#include "utility/geom/d3/HalfSpace3d.h"



#include "utility/matrices/m_double.h"
#include "utility/geom/pos_vec/Pos3dList.h"

const Pos3d cgalV0(1.0,0.0,0.0);
const Pos3d cgalV1(0.0,0.0,1.0);
const Pos3d cgalV2(0.0,0.0,0.0);
const Pos3d cgalV3(0.0,1.0,0.0);
const Pos3d cgalV4(1.0,0.0,1.0);
const Pos3d cgalV5(0.0,1.0,1.0);
const Pos3d cgalV6(1.0,1.0,0.0);
const Pos3d cgalV7(1.0,1.0,1.0);

// see: https://doc.cgal.org/latest/Polyhedron/index.html https://doc.cgal.org/latest/Polyhedron/Polyhedron_2polyhedron_prog_cube_8cpp-example.html
CGPolyhedron_3::Halfedge_handle make_cube_3(CGPolyhedron_3 & P, const Pos3d &p0, const Pos3d &p1,const Pos3d &p2, const Pos3d &p3, const Pos3d &p4, const Pos3d &p5,const Pos3d &p6, const Pos3d &p7)
  {
    // appends a cube of size [0,1]^3 to the polyhedron P.
    CGAL_precondition(P.is_valid());
    typedef typename CGPolyhedron_3::Halfedge_handle Halfedge_handle;
    Halfedge_handle h = P.make_tetrahedron( p0.ToCGAL(),
                                            p1.ToCGAL(),
                                            p2.ToCGAL(),
                                            p3.ToCGAL());
    Halfedge_handle g = h->next()->opposite()->next();             // Fig. (a)
    P.split_edge( h->next());
    P.split_edge( g->next());
    P.split_edge( g);                                              // Fig. (b)
    h->next()->vertex()->point()     = p4.ToCGAL();
    g->next()->vertex()->point()     = p5.ToCGAL();
    g->opposite()->vertex()->point() = p6.ToCGAL();            // Fig. (c)
    Halfedge_handle f = P.split_facet( g->next(),
                                       g->next()->next()->next()); // Fig. (d)
    Halfedge_handle e = P.split_edge( f);
    e->vertex()->point() = p7.ToCGAL();                        // Fig. (e)
    P.split_facet( e, f->next()->next());                          // Fig. (f)
    CGAL_postcondition( P.is_valid());
    return h;
  }

//! @brief Default constructor.
Hexahedron3d::Hexahedron3d(void)
  {
    make_cube_3(cgpolyhedron, cgalV0, cgalV1, cgalV2, cgalV3, cgalV4, cgalV5, cgalV6, cgalV7);
  }

//! @brief Constructor.
Hexahedron3d::Hexahedron3d(const Pos3d &p0, const Pos3d &p1,const Pos3d &p2, const Pos3d &p3, const Pos3d &p4, const Pos3d &p5,const Pos3d &p6, const Pos3d &p7)
  {
    //CGAl->xc indexes: 0->1, 1->4, 2->0, 3->3 
    //                  4->5, 5->7, 6->2, 7->6
    make_cube_3(cgpolyhedron, p1, p4, p0, p3, p5, p7, p2, p6);
  }

//! @brief Return the volume of the body with sign.
GEOM_FT Hexahedron3d::getVolumeWithSign(void) const
  {
    std::cerr << "Hexahedron3d::" << __FUNCTION__
              << "  not implemented yet." << std::endl;
    return 0.0;
  }

//! @brief Return the volume of the object.
GEOM_FT Hexahedron3d::getVolume(void) const
  { return std::abs(getVolumeWithSign()); }

//! @brief Returns true if point inside hexahedron.
bool Hexahedron3d::In(const Pos3d &p,const double &tol) const
  {
    std::cerr << "Hexahedron3d::" << __FUNCTION__
              << "  not implemented yet." << std::endl;
    return false;
  }

//! @brief Prints object.
void Hexahedron3d::Print(std::ostream &os) const
  {
    CGAL::set_ascii_mode(os);
    CGAL::set_pretty_mode(os);
    os << cgpolyhedron << std::endl;
  }


