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
//Line3d.cc

#include "Line3d.h"
#include "Line2d.h"
#include "../pos_vec/Dir3d.h"
#include "../d2/Plane.h"
#include "CGAL/linear_least_squares_fitting_3.h"


const Pos3d Line3d::defaultOrg= Pos3d(0,0,0);
const Pos3d Line3d::defaultDest= Pos3d(1,0,0);
const Vector3d defaultVDir= Vector3d(1,0,0);

Line3d::Line3d(void)
  : Linear3d(), cgr(defaultOrg.ToCGAL(),defaultDest.ToCGAL()){}
Line3d::Line3d(const CGLine_3 &r)
  : Linear3d(),cgr(r) {}

Line3d::Line3d(const Pos3d &p1,const Pos3d &p2)
  : Linear3d(), cgr(p1.ToCGAL(),p2.ToCGAL())
  {
    if(cgr.is_degenerate())
      {
        std::clog << getClassName() << "::" << __FUNCTION__
	          << ": degenerated line, the points: "
             << p1 << " and " << p2 << " are the same." << std::endl;
      }
    const double d= p1.dist(p2);
    if(d<mchne_eps_dbl)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; ill defined line, the points: "
		<< p1 << " and " << p2 << " are too close d(p1,p2)= "
                << d << ".\n";
  }

Line3d::Line3d(const Pos3d &p,const Dir3d &dir)
  : Linear3d(), cgr(p.ToCGAL(),dir.ToCGAL()) {}

Line3d::Line3d(const Plane &p1,const Plane &p2)
  : Linear3d(), cgr()
  {
    const Line3d tmp= intersection_line(p1,p2);
    if(tmp.exists())
      cgr= tmp.ToCGAL();
    else
      setExists(false);
  }

//! @brief Constructs the line from its parametric equation.
Line3d::Line3d(const Line3dParametricForm &param)
  : Linear3d(), cgr(defaultOrg.ToCGAL(),defaultDest.ToCGAL())
  { Parametricas(param); }

//! @ brief Swaps the line orientation.
void Line3d::swap(void)
  { cgr= cgr.opposite(); }

void Line3d::TwoPoints(const Pos3d &p1,const Pos3d &p2)
  { (*this)= Line3d(p1,p2); }

Dir3d Line3d::GetDir(void) const
  { return Dir3d(cgr.direction()); }

//! @brief Return the direction vector.
Vector3d Line3d::VDir(void) const
  { return Vector3d(cgr.to_vector()); }

//! @brief Returns a vector in the direction of the local
//! X axis.
Vector3d Line3d::getIVector(void) const
  {
    Vector3d retval= VDir();
    retval.Normalize();
    return retval;
  }

//! @brief Returns a vector in the direction of the local
//! Y axis.
Vector3d Line3d::getJVector(void) const
  {
    const Vector3d i= getIVector();
    const Vector3d K(0.0,0.0,1.0); //Global Z.
    Vector3d retval= K.getCross(i);
    const double d= retval.GetModulus2();
    if(d<1e-12) // parallel to z axis.
      {
	const Vector3d J(0.0,1.0,0.0); //Global Y.
        retval= J.getCross(i);
      }
    retval.Normalize();
    return retval;
  }

//! @brief Returns a vector in the direction of the local
//! Z axis
Vector3d Line3d::getKVector(void) const
  {
    const Vector3d vI= getIVector();
    const Vector3d vJ= getJVector();
    return vI.getCross(vJ);
  }

double Line3d::getLambda(unsigned short int i,const double &d,const Vector3d &i_) const
      { return (d-Point(0)(i))/i_(i);}

//! @brief Return the orthogonal projection onto the line.
//! @param p: point to project.
Pos3d Line3d::Projection(const Pos3d &p) const
  { return Pos3d(cgr.projection(p.ToCGAL())); }

//! @brief Return the projection onto the line.
//! @param v: vector to project.
Vector3d Line3d::Projection(const Vector3d &v) const
  {
    const Vector3d d= VDir().getNormalized();
    return dot(v,d)*d;
  }

//! @brief Return the projection onto the XY plane.
Line3d Line3d::XY3DProjection(void) const
  { return XYPlane3d.Projection(*this); }

//! @brief Return the projection onto the XZ plane.
Line3d Line3d::XZ3DProjection(void) const
  { return XZPlane3d.Projection(*this); }

//! @brief Return the projection onto the YZ plane.
Line3d Line3d::YZ3DProjection(void) const
  { return YZPlane3d.Projection(*this); }

//! @brief Return the projection onto the XY plane.
Line2d Line3d::XY2DProjection(void) const
  {
    Line2d retval;
    Line3d r3d= XY3DProjection();
    if(r3d.exists())
      {
        const Pos2d p1= r3d.Point(0).XY2DProjection();
        const Pos2d p2= r3d.Point(100).XY2DProjection();
        const double d= p1.dist(p2);
        if(d>mchne_eps_dbl)
          retval= Line2d(p1,p2);
        else
	  retval.setExists(false);
      }
    return retval;
  }

//! @brief Return the projection onto the XZ plane.
Line2d Line3d::XZ2DProjection(void) const
  {
    Line2d retval;
    Line3d r3d= XZ3DProjection();
    if(r3d.exists())
      {
        const Pos2d p1= r3d.Point(0).XZ2DProjection();
        const Pos2d p2= r3d.Point(100).XZ2DProjection();
        const double d= p1.dist(p2);
        if(d>mchne_eps_dbl)
          retval= Line2d(p1,p2);
        else
          retval.setExists(false);
      }
    return retval;
  }

//! @brief Return the projection onto the YZ plane.
Line2d Line3d::YZ2DProjection(void) const
  {
    Line2d retval;
    Line3d r3d= YZ3DProjection();
    if(r3d.exists())
      {
        const Pos2d p1= r3d.Point(0).YZ2DProjection();
        const Pos2d p2= r3d.Point(100).YZ2DProjection();
        const double d= p1.dist(p2);
        if(d>mchne_eps_dbl)
          retval= Line2d(p1,p2);
        else
          retval.setExists(false);
      }
    return retval;
  }

//! @brief Return the angle with respecto to XY plane.
GEOM_FT Line3d::getSlope(void) const
  { return angle(*this,XYPlane3d); }

//! @brief Compute the line that best suits the point cloud.
GEOM_FT Line3d::linearLeastSquaresFitting(const GeomObj3d::list_Pos3d &lp)
  {
    std::list<CGPoint_3> points;
    for(GeomObj3d::list_Pos3d::const_iterator i=lp.begin(); i!=lp.end();i++)
      points.push_back((*i).ToCGAL()); 
    GEOM_FT quality= linear_least_squares_fitting_3(points.begin(),points.end(),cgr,CGAL::Dimension_tag<0>());
    return quality;
  }

//! @brief Return true if the lines are parallel.
bool Line3d::isParallel(const Line3d &r) const
  { return parallel(GetDir(),r.GetDir()); }

//! @brief Return the plane normal to r that passes through p.
Plane Line3d::Perpendicular(const Pos3d &p) const
  { return Plane(cgr.perpendicular_plane(p.ToCGAL())); }

GEOM_FT coo_intersection(const GeomObj2d::list_Pos2d &int_a, const GeomObj2d::list_Pos2d &int_b,const size_t &coo,const double &tol)
  {
    Pos3d pint_a,pint_b;
    if(!int_a.empty()) pint_a= To3d(*int_a.begin(),coo+1);
    if(!int_b.empty()) pint_b= To3d(*int_b.begin(),coo+2);

    GEOM_FT retval=0.0;
    if((!int_a.empty()) && (!int_b.empty())) //Both projections intercepts.
      {
        if(fabs(pint_a(coo)-pint_b(coo))<tol)
          retval= (pint_a(coo)+pint_b(coo))/2;
        else
          {
            std::cerr << __FUNCTION__
		      << "(int_a,int_b): error when averaging coordinate "
		      << coo 
                      << " of the intersection. Obtained values: "
		      << pint_a(coo) << " and " << pint_b(coo)
                      << " are too different." << std::endl;
          }
      }
    else
      if(!int_a.empty()) //First projection intercepts.
        retval= pint_a(coo);
      else
        if(!int_b.empty()) //Second projection intercepts.
          retval= pint_b(coo);
        else //None of the projections intercepts.
          {
            std::cerr << __FUNCTION__
	              << "; error when computing" << coo
		      << " coordinate of the intersection."
                      << std::endl;
          }
    return retval;
  }

//! @brief Return true if the lines intersect.
bool Line3d::intersects(const Line3d &r2) const
  {
    if(!coplanarias(*this,r2)) return false; //Not coplanar.
    if(colineales(*this,r2)) return true; //Collinear.
    if(parallel(*this,r2))
      return false; //Different and parallel.
    else
      return true;
  }

//! @brief Return the intersection of the line with the plane
//! defined by the equation coord_i= d.
GeomObj3d::list_Pos3d Line3d::getIntersection(unsigned short int i, const double &d) const
  {
    GeomObj::list_Pos3d lp;
    unsigned short int j=i;
    j++;
    if(j>3) j=1;
    unsigned short int k=j;k++;
    if(k>3) k=1;
    Pos3d p;
    p.Set(i,d);
    const Vector3d i_= VDir();
    const Pos3d org(Point(0));
    if (fabs(i_(i))<1.0E-12) return lp;
    const double l= getLambda(i,d,i_);
    p.Set(j,org(j)+l*i_(j));
    p.Set(k,org(k)+l*i_(k));
    lp.push_back(p);
    return lp;
  }

//! @brief Return the point intersection with the line, if it doesn't exists
//! it returns an empty list.
GeomObj3d::list_Pos3d Line3d::getIntersection(const Line3d &r2,const double &tol) const
  {
    GeomObj3d::list_Pos3d retval;
    if(!intersecan(*this,r2)) return retval;
    if(colineales(*this,r2))
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << ": lines are the same, all its points belong"
	          << " to the intersection." << std::endl;
        return retval;
      }

    const double tol2= tol*tol;
    bool exists= true;

    const Line2d r1_xy= (*this).XY2DProjection();
    const Line2d r1_xz= (*this).XZ2DProjection();
    const Line2d r1_yz= (*this).YZ2DProjection();

    const Line2d r2_xy= r2.XY2DProjection();
    const Line2d r2_xz= r2.XZ2DProjection();
    const Line2d r2_yz= r2.YZ2DProjection();
    exists= (r1_xy.exists() && r1_xz.exists() && r1_yz.exists() && r2_xy.exists() && r2_xz.exists() && r2_yz.exists());
    if(!exists)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; error when computing projections." 
                << std::endl;

    const GeomObj2d::list_Pos2d int_xy= intersection(r1_xy,r2_xy);
    const GeomObj2d::list_Pos2d int_xz= intersection(r1_xz,r2_xz);
    const GeomObj2d::list_Pos2d int_yz= intersection(r1_yz,r2_yz);

    const GEOM_FT x= coo_intersection(int_xy,int_xz,1,tol);
    const GEOM_FT y= coo_intersection(int_xy,int_yz,2,tol);
    const GEOM_FT z= coo_intersection(int_xz,int_yz,3,tol);

    Pos3d pint(x,y,z);
    if((this->dist2(pint)>tol2) || (r2.dist2(pint))>tol2)
      {
        std::cerr << __FUNCTION__
		  << "(Line3d,Line3d): error when computing intersection."
                  << std::endl;
      }
    retval.push_back(pint);
    return retval;
  }

//! @brief Return the squared distance from the point to the line.
GEOM_FT Line3d::dist(const Pos3d &p) const
  { return sqrt_FT(dist2(p)); }

//! @brief Return a Python dictionary with the object members values.
boost::python::dict Line3d::getPyDict(void) const
  {
    boost::python::dict retval= Linear3d::getPyDict();
    retval["from"]= Point(0);
    retval["to"]= Point(100);
    return retval;
  }

//! @brief Set the values of the object members from a Python dictionary.
void Line3d::setPyDict(const boost::python::dict &d)
  {
    Linear3d::setPyDict(d);
    const Pos3d from= boost::python::extract<Pos3d>(d["from"]);
    const Pos3d to= boost::python::extract<Pos3d>(d["to"]);
    Put(from,to);
  }

//! @brier Print stuff.
void Line3d::Print(std::ostream &os) const
  { os << PtoParametricas(0.0) << " " << PtoParametricas(100.0); }

//! @brief Return the distance from the point to the line.
GEOM_FT dist(const Pos3d &p,const Line3d &r)
  { return sqrt_FT(dist2(r,p)); }

bool colineales(const Line3d &r1,const Line3d &r2)
  {
    const Pos3d p1= r2.Point(0);
    const Pos3d p2= r2.Point(100);
    if( r1.In(p1) && r1.In(p2) )
      return true;
    else
      return false;
  }

bool coplanarias(const Line3d &r1,const Line3d &r2)
  {
    const Pos3d p1= r2.Point(0);
    const Pos3d p2= r2.Point(100);
    const Pos3d p3= r1.Point(0);
    const Pos3d p4= r1.Point(100);
    return coplanarios(p1,p2,p3,p4);
  }
