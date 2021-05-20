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
//BasicAlphaShape2d.cc

#include "BasicAlphaShape2d.h"
#include "utility/geom/d2/2d_polygons/Polygon2d.h"
#include "utility/geom/d2/BND2d.h"
#include "utility/geom/lists/utils_list_pos2d.h"

typedef Alpha_shape_2::Alpha_shape_edges_iterator            Alpha_shape_edges_iterator;
typedef Alpha_shape_2::Alpha_shape_vertices_iterator            Alpha_shape_vertices_iterator;

const double scale= 1e3;

inline double double2intdouble(const double &d)
  { return floor(d*scale); }

inline Point_2 Pos2d_to_Point_2(const Pos2d &p2d)
  { return Point_2(double2intdouble(p2d.x()),double2intdouble(p2d.y())); }

inline CGPoint_2 Point_to_Pos2d(const Point_2 &np)
  { return CGPoint_2(np.x()/scale,np.y()/scale); }

std::list<Segment> alpha_edges( const Alpha_shape_2 &A)
  {
    std::list<Segment> retval;
    Alpha_shape_edges_iterator it = A.alpha_shape_edges_begin(),
                               end = A.alpha_shape_edges_end();
    for( ; it!=end; ++it)
      retval.push_back(A.segment(*it));
    return retval;
  }

std::list<Point_2> alpha_vertices( const Alpha_shape_2 &A)
  {
    std::list<Point_2> retval;
    Alpha_shape_vertices_iterator it = A.alpha_shape_vertices_begin(),
                                 end = A.alpha_shape_vertices_end();
    for( ; it!=end; ++it)
      retval.push_back((*it)->point());
    return retval;
  }

//! @brief Creates alpha shape object.
void BasicAlphaShape2d::alloc(const GeomObj::list_Pos2d &lp, const double &alpha)
  {
    free();
    std::vector<Point_2> input;
    for(GeomObj::list_Pos2d::const_iterator i= lp.begin();i!=lp.end();i++)
      input.push_back(Pos2d_to_Point_2(*i));
    // Remove duplicates.
    std::sort(input.begin(), input.end(),
        [](const Point_2 &e1, const Point_2 &e2)->bool {
        return e2.y() < e1.y();
        });
    std::stable_sort(input.begin(), input.end(),
        [](const Point_2 &e1, const Point_2 &e2)->bool {
        return e2.x() < e1.x();
        });
    input.erase(std::unique(input.begin(), input.end()), input.end());    
    A= new Alpha_shape_2(input.begin(), input.end(), FT(alpha),Alpha_shape_2::GENERAL);//Alpha_shape_2::REGULARIZED); //Alpha_shape_2::GENERAL);
  }

//! @brief Frees memory.
void BasicAlphaShape2d::free(void)
  {
    if(A)
      delete A;
    A= nullptr;
  }

//! @brief Constructor.
BasicAlphaShape2d::BasicAlphaShape2d(const GeomObj::list_Pos2d &lp, const double &alpha)
  : A(nullptr)
  {
    //std::cout << "alpha= " << alpha << std::endl;
    alloc(lp,double2intdouble(alpha));
    std::list<Segment> segments= alpha_edges(*A);
    // std::cout << "Alpha Shape computed" << std::endl;
    // std::cout << segments.size() << " alpha shape edges" << std::endl;
    // std::cout << "Optimal alpha: " << *A->find_optimal_alpha(1)/scale<<std::endl
      ;
  }

//! @brief Destructor.
BasicAlphaShape2d::~BasicAlphaShape2d(void)
  { free(); }

//! 
double BasicAlphaShape2d::getOptimalAlpha(const size_t &nb_components) const
  { return *A->find_optimal_alpha(nb_components); }

//! @brief Return the alpha shape from the point list.
Polygon2d BasicAlphaShape2d::getPolygon2d(void)
  {
    Polygon2d retval;
    std::list<Point_2> result= alpha_vertices(*A);
    for(std::list<Point_2>::const_iterator i= result.begin();i!=result.end();i++)
      retval.push_back(Pos2d(Point_to_Pos2d(*i)));    
    return retval;
 }

Polygon2d get_basic_alpha_shape2d(const GeomObj::list_Pos2d &lp)
  {
    BND2d bnd= getBnd(lp);
    const double alpha= 10000; //bnd.Diagonal().GetModulus()/3.0;
    BasicAlphaShape2d A(lp,alpha);
    return A.getPolygon2d();
  }
