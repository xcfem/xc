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
//Line2d.cc

#include "Line2d.h"
#include "utility/utils/misc_utils/matem.h"
#include <plotter.h>

#include "../pos_vec/Dir2d.h"
#include "../pos_vec/Vector2d.h"
#include "../pos_vec/Pos3d.h"


#include "Segment2d.h"
#include "utility/geom/trf/Trf2d.h"
const double quiet_nan= std::numeric_limits<double>::quiet_NaN();


//! @brief Return the parameter that corresponds to the point over the line.
//!
//! The point is supposed to belong to the line.
GEOM_FT Line2dParametricForm::Parametro(const Pos2d &p) const
  {
    Vector2d v= p-org;
    return dot(v,dir)/Abs(dir);
  }

//! @brief Default constructor.
Line2d::Line2d(void): Linear2d(),cgr(CGPoint_2(0,0),CGPoint_2(1,0))
  {}

//! @brief Constructor.
//! @param p1: 2D point.
//! @param p2: 2D point.
Line2d::Line2d(const Pos2d &p1,const Pos2d &p2)
  : Linear2d(), cgr(p1.ToCGAL(),p2.ToCGAL())
  {
    if(isDegenerated())
      {
        clog << getClassName() << "::" << __FUNCTION__
	     << "; degenerated line, the points: "
             << p1 << " and " << p2 << " are the same." << endl;
      }
    if(p1.dist(p2)<mchne_eps_dbl)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; bad definition for line, the points: "
             << p1 << " and " << p2 << " are too close." << endl;
  }

//! @brief Constructor.
//! @param p1: 2D point.
//! @param dir: 2D direction.
Line2d::Line2d(const Pos2d &p,const Dir2d &dir)
  : Linear2d(), cgr(p.ToCGAL(),dir.ToCGAL()) {}


//! @brief Constructor.
//! @param p1: 2D point.
//! @param vdir: 2D vector.
Line2d::Line2d(const Pos2d &p,const Vector2d &vdir)
  : Linear2d(), cgr(p.ToCGAL(),vdir.ToCGAL()) {}

//! @brief Copy constructor.
Line2d::Line2d(const CGLine_2 &r)
  : Linear2d(), cgr(r) {}

//! @brief Constructs the line from its parametric equation.
Line2d::Line2d(const Line2dParametricForm &param)
  : Linear2d(),cgr(CGPoint_2(0,0),CGPoint_2(1,0))
  { Parametricas(param); }

//! @brief Comparison operator.
bool Line2d::operator==(const Line2d &other) const
  {
    bool retval= false;
    if(this==&other)
      retval= true;
    else
      {
        retval= Linear2d::operator==(other);
        if(retval)
          retval= (cgr==other.cgr); 
       }
    return retval;
  }

const CGLine_2 &Line2d::ToCGAL(void) const
  { return cgr; }

//! @brief Constructor virtual.
GeomObj *Line2d::clon(void) const
  { return new Line2d(*this); }

//! @ brief Swaps the line orientation.
void Line2d::swap(void)
  { cgr= cgr.opposite(); }

void Line2d::TwoPoints(const Pos2d &p1,const Pos2d &p2)
  { (*this)= Line2d(p1,p2); }

//! @brief Return object length.
GEOM_FT Line2d::getLength(void) const
  { return NAN; }

//! @brief Return the center of mass of the line.
Pos2d Line2d::getCenterOfMass(void) const
  { return Pos2d(NAN,NAN); }

Dir2d Line2d::GetDir(void) const
  { return Dir2d(cgr.direction()); }

Vector2d Line2d::VDir(void) const
  { return GetDir().GetVector(); }

//! @brief Return the direction vector of the segment.
Vector2d Line2d::Normal(void) const
  { return VDir().Normal(); }

Vector2d Line2d::VersorDir(void) const
  { return VDir().getNormalized(); }

//! @brief Returns a vector in the direction of the local
//! X axis.
Vector2d Line2d::getIVector(void) const
  {
    Vector2d retval= VDir();
    retval.Normalize();
    return retval;
  }

//! @brief Returns a vector in the direction of the local
//! Y axis.
Vector2d Line2d::getJVector(void) const
  {
    Vector2d retval= Normal();
    retval.Normalize();
    return retval;
  }

double Line2d::getLambda(unsigned short int i,const double &d,const Vector2d &i_) const
  { return (d-Point(0)(i))/i_(i);}

//! @brief Return orthogonal projection of p onto the line.
Pos2d Line2d::Projection(const Pos2d &p) const
  { return Pos2d(cgr.projection(p.ToCGAL())); }

//! @brief Return orthogonal projection of v onto the line.
Vector2d Line2d::Projection(const Vector2d &v) const
  {
    const Vector2d d= VDir().getNormalized();
    return dot(v,d)*d;
  }

//! @brief Returns the a parameter of the line equation in general
//! form: a*x + b*y + c= 0
GEOM_RT Line2d::a(void) const
  { return cgr.a(); }

//! @brief Returns the b parameter of the line equation in general
//! form: a*x + b*y + c= 0
GEOM_RT Line2d::b(void) const
  { return cgr.b(); }

//! @brief Returns the c parameter of the line equation in general
//! form: a*x + b*y + c= 0
GEOM_RT Line2d::c(void) const
  { return cgr.c(); }

//! @brief Return the slope of the line.
GEOM_FT Line2d::getSlope(void) const
  { return GetParamA(); }

//! @brief Return the a parameter (y=a*x+b) of the horizontal
//! projection of the line.
GEOM_FT Line2d::GetParamA(void) const
  {
    if(EsVertical())
      {
        clog << getClassName() << "::" << __FUNCTION__
	     << "; line is vertical, infinite slope."
	     << std::endl;
        return NAN;
      }
    else
      return -(cgr.a()/cgr.b());
  }

//! @brief Return the b parameter (y=a*x+b) of the horizontal
//! projection of the line.
GEOM_FT Line2d::GetParamB(void) const
  {
    if(EsVertical())
      {
        clog << getClassName() << "::" << __FUNCTION__
	     << "; vertical line, it intercepts y axis at infinity."
	     << std::endl;
        return NAN;
      }
    else
      return -(cgr.c()/cgr.b());
  }

GEOM_FT Line2d::GetY(const GEOM_FT &x) const
  { 
    if(EsVertical())
      {
        clog << getClassName() << "::" << __LINE__
	     << "; line is vertical (parallel to y axis)." << endl;
        return NAN;
      }
    return cgr.y_at_x(x);
  }
//! @brief Return the parameter of the point on the line.
//!
//! Returns the parameter that corresponds to the point
//! on the line. If the points lies not on the line
//! we compute the parameter of its projection onto it.
GEOM_FT Line2d::Parametro(const Pos2d &p) const
  {
    Pos2d tmp(p);
    if(!In(tmp))
      tmp= Projection(tmp);
    Line2dParametricForm r= GetParametricas();
    return r.Parametro(tmp);
  }

//! @brief Returns the line equation in parametric form a:
//!   v[0]: point in the line.
//!   v[1]: dir vector.
Line2dParametricForm Line2d::GetParametricas(void) const
  { return Line2dParametricForm(Point(0),VDir()); }

//! @brief Return a point of the line at a distance delta
//! from its origin.
Pos2d Line2d::PtoParametricas(const GEOM_FT &lambda) const
  { return Point(0)+lambda*VDir(); }

//! @brief Line redefined from a parametric equation.
void Line2d::Parametricas(const Line2dParametricForm &param)
  { TwoPoints(param.getPoint(0.0),param.getPoint(100.0)); }

//! @brief SOLO SE USA EN Ordena.
bool menor_param(const Pos3d &p1, const Pos3d &p2)
  { return (p1.z()<p2.z()); }

//! @brief Return the points ordered by the value of the parameter
//! of its projection onto the line from lowest to highest.
//! Origin is at Line2d::Point(0).
GeomObj::list_Pos2d Line2d::Ordena(const GeomObj::list_Pos2d &ptos) const
  {
    GeomObj::list_Pos2d retval;
    if(ptos.empty()) return retval;

    Line2dParametricForm rp= GetParametricas();
    GeomObj::list_Pos3d tmp;
    for(GeomObj::list_Pos2d::const_iterator i= ptos.begin();i!=ptos.end();i++)
      tmp.push_back(Pos3d((*i).x(),(*i).y(),rp.Parametro(*i)));

    sort(tmp.begin(),tmp.end(),menor_param);

    for(GeomObj::list_Pos3d::const_iterator i= tmp.begin();i!=tmp.end();i++)
      retval.push_back(Pos2d((*i).x(),(*i).y()));
    return retval;
  }

//! @brief Return the projections of the points onto the line.
GeomObj::list_Pos2d Line2d::Project(const GeomObj::list_Pos2d &ptos) const
  {
    GeomObj::list_Pos2d retval;
    for(GeomObj::list_Pos2d::const_iterator i= ptos.begin();i!=ptos.end();i++)
      retval.push_back(Projection(*i));
    return retval;
  }

//! @brief Return the line perpendicular to this one through p.
Line2d Line2d::Perpendicular(const Pos2d &p) const
  { return Line2d(cgr.perpendicular(p.ToCGAL())); }

//! @brief Return the line that result from displacing this
//! one by the vector argument.
//! @param v: displacement vector.
Line2d Line2d::Offset(const Vector2d &v) const
  {
    const Pos2d p= Point()+v;
    return Line2d(p,VDir());
  }

//! @brief Return a parallel line passing through p.
Line2d Line2d::isParallel(const Pos2d &p) const
  {
    const Vector2d v= p-Point();
    return Offset(v);
  }

//! @brief Return a line parallel to this one at the distance
//! being passed as parameter. It the distance is positive the
//! new line is placed on the right.
Line2d Line2d::Offset(const GEOM_FT &d) const
  {
    const Vector2d v= d*VersorDir();
    const Vector2d n(v.y(),-v.x());
    return Offset(n);
  }

//! @brief Set the points that define the line.
void Line2d::Put(const Pos2d &p1,const Pos2d &p2)
  { TwoPoints(p1,p2); }



//! @brief Return true if the line intersects the argument one.
bool Line2d::intersects(const Line2d &r2) const
  { return do_intersect(cgr,r2.cgr); }

//! @brief Return the point intersection of both lines, if doesn't exists
//! it returns an empty list.
GeomObj2d::list_Pos2d Line2d::getIntersection(const Line2d &r2) const
  {
    GeomObj2d::list_Pos2d retval;
    if(isDegenerated())
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; degenerated line: "
                  << *this << std::endl;
        return retval;
      }
    if(r2.isDegenerated())
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
                  << "; degenerated line: " << r2
		  << std::endl;
        return retval;
      }
    if(colineales(*this,r2))
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; the lines are the same, "
	          << "all its points belong to the intersection." 
             << endl;
        return retval;
      }
    if(!parallel(*this,r2))
      {
        CGAL::Object result;
        CGPoint_2 ptoi;
        result = CGAL::intersection(cgr,r2.cgr);
        if(CGAL::assign(ptoi, result))
          retval.push_back(Pos2d(ptoi));
        else
          {
            cerr << getClassName() << "::" << __FUNCTION__
		 << "; unknown error when computing the intersection"
                 << " of the lines:" << endl
                 << "  r: " << *this << endl
                 << "  r2: " << r2 << endl;
          }
      }
    return retval;
  }

//! @brief Return verdadero if exists the intersection with the line argument.
bool Line2d::intersects(const Ray2d &sr) const
  { return do_intersect(ToCGAL(),sr.ToCGAL()); }

//! @brief Return (if exists) the intersection with the ray argument.
Pos2d Line2d::getIntersection(const Ray2d &sr) const
  {
    Pos2d retval(quiet_nan, quiet_nan,quiet_nan);
    GeomObj2d::list_Pos2d tmp= intersection(*this, sr);
    if(!tmp.empty())
      retval= *tmp.begin();
    else
      retval.setExists(false);
    return retval;
  }

//! @brief Return true if the intersection with the
//! segment argument exists.
bool Line2d::intersects(const Segment2d &sg) const
  { return do_intersect(ToCGAL(),sg.ToCGAL()); }

//! @brief Return (if exists) the intersection with the segment argument.
Pos2d Line2d::getIntersection(const Segment2d &sg) const
  {
    Pos2d retval(quiet_nan, quiet_nan, quiet_nan);
    GeomObj2d::list_Pos2d tmp= intersection(*this, sg);
    if(!tmp.empty())
      retval= *tmp.begin();
    else
      retval.setExists(false);
    return retval;
  }

//! @brief Return the intersection of the line with the plane
//! defined by the equation coord_i= d.
GeomObj2d::list_Pos2d Line2d::getIntersection(unsigned short int i, const double &d) const
  {
    GeomObj::list_Pos2d lp;
    unsigned short int j=i;
    j++;
    if(j>2) j=1;
    Pos2d p;
    p.Set(i,d);
    const Vector2d i_= VDir();
    const Pos2d org(Point(0));
    if (fabs(i_(i))<1.0E-12) return lp;
    const double l= getLambda(i,d,i_);
    p.Set(j,org(j)+l*i_(j));
    lp.push_back(p);
    return lp;
  }


//! @brief Return an arbitrary point of the line.
//! Si i==j, se cumple que Point(i) == Point(j).
//! Además, l is directed from point(i)  to point(j), for all i < j.
Pos2d Line2d::Point(const int &i) const
  { return Pos2d(cgr.point(i)); }

//! @brief Return the squared distance between both lines.
GEOM_FT dist2(const Line2d &r1,const Line2d &r2)
  {
    GEOM_FT retval= 0.0;
    if(!intersecan(r1,r2)) //parallel
      if(r1 != r2) //They are different lines.
        {
          const Pos2d p1= r1.Point(0);
          const Pos2d p2= r2.Projection(p1);
          retval= dist2(p1,p2);
	}
    return retval;
  }

//! @brief Return the angle con el vector que se pasa como parámetro.
double Line2d::getAngle(const Vector2d &v) const
  { return angle(VDir(),v); }

//! @brief Return the angle con el Y axis.
double Line2d::Azimuth(void) const
  { return angle(VDir(),Vector2d(0,1)); }

//! @brief Return the angle between the line and the vector.
double angle(const Line2d &r,const Vector2d &v)
  { return r.getAngle(v); }

//! @brief Return the angle between the lines.
double angle(const Line2d &r1,const Line2d &r2)
  { return r1.getAngle(r2.VDir()); }

bool Line2d::isParallel(const Line2d &r) const
  { return (parallel(GetDir(),r.GetDir())); }

//! @brief Return the bisection line of this one
//! an the argument line.
Line2d Line2d::Bisectriz(const Line2d &r) const
  { return Line2d(bisector(this->ToCGAL(),r.ToCGAL())); }

Line2d Line2d::getNormalized(void) const
  { return Line2d(Point(),VersorDir()); }

//! @brief Return true if the line is vertical.
bool Line2d::EsVertical(void) const
  { return cgr.is_vertical(); }

//! @brief Return true if the line is degenerated.
bool Line2d::isDegenerated(void) const
  { return cgr.is_degenerate(); }

//! @brief Return true if the point is on the line.
bool Line2d::In(const Pos2d &p, const double &tol) const
  { return cgr.has_on(p.ToCGAL()); }

//! @brief Return true if the point is on the right side of the line.
bool Line2d::negativeSide(const Pos2d &p) const
  { return cgr.has_on_negative_side(p.ToCGAL()); }

//! @brief Return true if the point is on the left side of the line.
bool Line2d::positiveSide(const Pos2d &p) const
  { return cgr.has_on_positive_side(p.ToCGAL()); }

//! @brief Return the squared distance from the point to the line.
GEOM_FT Line2d::dist2(const Pos2d &p) const
  {
    const GEOM_FT A= a(),B= b(), C= c();
    return sqr(A*p.x()+B*p.y()+C)/(A*A+B*B);
  }

//! @brief Return the squared distance from the point to the line.
GEOM_FT Line2d::dist(const Pos2d &p) const
  { return sqrt(dist2(p)); }

//Moment of inertia with respect to the center of mass in local coordinates.
GEOM_FT Line2d::Ix(void) const
  { return 0.0; }

//Moment of inertia with respect to the center of mass in local coordinates.
GEOM_FT Line2d::Iy(void) const
  { return NAN; }

//product of inertia.
GEOM_FT Line2d::Pxy(void) const
  { return NAN; }

//Moment of inertia with respect to the center of mass in local coordinates.
GEOM_FT Line2d::Iz(void) const
  { return NAN; }

//! @brief Return the distance between the lines.
GEOM_FT dist(const Line2d &r1,const Line2d &r2)
  { return sqrt_FT(dist2(r1,r2)); } 

bool colineales(const Line2d &r1,const Line2d &r2)
  {
    const Pos2d p1= r2.Point(0);
    const Pos2d p2= r2.Point(10);
    if( r1.In(p1) && r1.In(p2) )
      return true;
    else
      return false;
  }

//! @brief Returns true if intersection exists.
bool intersecan(const Line2d &r1,const Line2d &r2)
  { return r1.intersects(r2); }

//! @brief Return the intersection.
GeomObj2d::list_Pos2d intersection(const Line2d &r1,const Line2d &r2)
  { return r1.getIntersection(r2); }

//! @brief Return the point de intersection (if exists).
Pos2d intersection_point(const Line2d &r1, const Line2d &r2)
  {
    Pos2d retval(quiet_nan,quiet_nan);
    GeomObj2d::list_Pos2d tmp= intersection(r1,r2);
    if(tmp.empty())
      retval.setExists(false);
    else
      retval= *tmp.begin();
    return retval;
  }

//! @brief Applies the transformation argument to the line.
void Line2d::Transform(const Trf2d &trf2d)
  {
    const Pos2d p1= trf2d.Transform(Point(0));
    const Pos2d p2= trf2d.Transform(Point(100));
    (*this)= Line2d(p1,p2);
  }

//! @brief Return a Python dictionary with the object members values.
boost::python::dict Line2d::getPyDict(void) const
  {
    boost::python::dict retval= Linear2d::getPyDict();
    retval["from"]= Point(0);
    retval["to"]= Point(100);
    return retval;
  }

//! @brief Set the values of the object members from a Python dictionary.
void Line2d::setPyDict(const boost::python::dict &d)
  {
    Linear2d::setPyDict(d);
    const Pos2d from= boost::python::extract<Pos2d>(d["from"]);
    const Pos2d to= boost::python::extract<Pos2d>(d["to"]);
    Put(from,to);
  }

void Line2d::Print(std::ostream &os) const
  { os << PtoParametricas(0.0) << " " << PtoParametricas(100.0); }

void Line2d::Plot(Plotter &psos) const
  {
    //Line is not drawn, it must be clipped first.
  }

Line2d perpendicular_bisector(const Pos2d &p1,const Pos2d &p2)
  { 
    const Segment2d sg(p1,p2);
    return sg.getPerpendicularBisector();
  }
