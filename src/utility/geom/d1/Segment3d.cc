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
//Segment3d.cc

#include "Segment3d.h"

#include "../pos_vec/Dir3d.h"
#include "../pos_vec/Vector3d.h"
#include "../pos_vec/Pos3d.h"
#include "utility/geom/pos_vec/VectorPos3d.h"
#include <CGAL/AABB_tree.h>
#include <CGAL/AABB_traits.h>
#include <CGAL/AABB_segment_primitive.h>

//! @brief Constructor.
Segment3d::Segment3d(void): Linear3d(),cgseg(CGPoint_3(0,0,0),CGPoint_3(1,0,0)) {}

//! @brief Constructor.
Segment3d::Segment3d(const CGSegment_3 &r)
  : Linear3d(), cgseg(r) {}

//! @brief Constructor.
Segment3d::Segment3d(const Pos3d &p1,const Pos3d &p2)
  : Linear3d(), cgseg(p1.ToCGAL(),p2.ToCGAL())
  {
    if(isDegenerated())
      {
        std::clog << getClassName() << "::" << __FUNCTION__
		  << "; degenerated line, the points: "
             << p1 << " and " << p2 << " are the same." << std::endl;
      }
  }

//! @brief Comparison operator.
bool Segment3d::operator==(const Segment3d &other) const
  {
    bool retval= false;
    if(this==&other)
      retval= true;
    else
      {
        retval= Linear3d::operator==(other);
        if(retval)
          retval= (cgseg==other.cgseg); 
       }
    return retval;
  }

GeomObj *Segment3d::clon(void) const
  { return new Segment3d(*this); }

//! @ brief Swaps the ray orientation.
void Segment3d::swap(void)
  { cgseg= cgseg.opposite(); }

GEOM_FT Segment3d::GetMax(unsigned short int i) const
  { return std::max(getFromPoint()(i),getToPoint()(i)); }
GEOM_FT Segment3d::GetMin(unsigned short int i) const
  { return std::min(getFromPoint()(i),getToPoint()(i)); }
Line3d Segment3d::getSupportLine(void) const
  { return Line3d(cgseg.supporting_line()); }
Pos3d Segment3d::getFromPoint(void) const
  { return Pos3d(cgseg.source()); }
Pos3d Segment3d::getToPoint(void) const
  { return Pos3d(cgseg.target()); }
GEOM_FT Segment3d::getSlope(void) const
  { return getSupportLine().getSlope(); }
const Pos3d Segment3d::Point(const int &i) const
  { return Pos3d(cgseg.point(i)); }

//! @brief Return a point of the line at a distance lambda from its origin.
Pos3d Segment3d::PtoParametricas(const GEOM_FT &lambda) const
  { return Point(0)+lambda*VDir(); }

//! @brief Return the parametric coordinate that corresponds
//! to the natural coordinate being passed as parameter.
double Segment3d::getParamNaturalCoord(const GEOM_FT &chi) const
  { return (chi+1.0)/2.0; }

//! @brief Return the parametric coordinate that corresponds
//! to the natural coordinate being passed as parameter.
Pos3d Segment3d::getPointNaturalCoord(const GEOM_FT &chi) const
  { return PtoParametricas(getParamNaturalCoord(chi)); }

inline bool Segment3d::isDegenerated(void) const
  { return cgseg.is_degenerate(); }
//! @brief Return true if the point is in the segment.
bool Segment3d::In(const Pos3d &p, const double &tol) const
  { return cgseg.has_on(p.ToCGAL()); }

void Segment3d::TwoPoints(const Pos3d &p1,const Pos3d &p2)
  { (*this)= Segment3d(p1,p2); }

Dir3d Segment3d::GetDir(void) const
  { return Dir3d(cgseg.direction()); }

Vector3d Segment3d::VDir(void) const
  { return GetDir().GetVector(); }

//! @brief Returns a vector in the direction of the local
//! X axis.
Vector3d Segment3d::getIVector(void) const
  { return getSupportLine().getIVector(); }

//! @brief Returns a vector in the direction of the local
//! Y axis.
Vector3d Segment3d::getJVector(void) const
  { return getSupportLine().getJVector(); }

//! @brief Returns a vector in the direction of the local
//! Z axis
Vector3d Segment3d::getKVector(void) const
  { return getSupportLine().getKVector(); }

//! @brief Return the lambda parameter: p= getFromPoint()+lambda*VDir()
GEOM_FT Segment3d::getLambda(const Pos3d &p) const
  {
    const Vector3d v(getFromPoint(),p);
    const Vector3d dir(normalize(VDir()));
    return dot(v,dir);
  }

//! @brief Return the squared distance from the point to the segment.
GEOM_FT Segment3d::dist2(const Pos3d &p) const
  {
    const Line3d r= getSupportLine();
    const Pos3d proj= r.Projection(p);
    GEOM_FT retval= p.dist2(proj); //Ok if projected point inside segment.
    const Pos3d A= getFromPoint();
    const Pos3d B= getToPoint();
    const GEOM_FT denom= (B.x()-A.x())*(B.x()-A.x())+(B.y()-A.y())*(B.y()-A.y())+(B.z()-A.z())*(B.z()-A.z());
    if(denom!=0)
      {
        const GEOM_FT u= ( (p.x()-A.x())*(B.x()-A.x()) + (p.y()-A.y())*(B.y()-A.y()) + (p.z()-A.z())*(B.z()-A.z()) )/denom;
        if(u<0.0) //Distance to the origin.
	  retval= p.dist2(A);
	else if(u>1.0) //Distance to the destination.
          retval= p.dist2(B);
      }
    return retval;
  }

//! @brief Return the distance from the point to the segment.
GEOM_FT Segment3d::dist(const Pos3d &p) const
  { return sqrt(dist2(p)); }

bool Segment3d::isParallel(const Line3d &r) const
  { return parallel(getSupportLine(),r); }
bool Segment3d::isParallel(const Ray3d &sr) const
  { return parallel(getSupportLine(),sr); }
bool Segment3d::isParallel(const Segment3d &r) const
  { return parallel(getSupportLine(),r.getSupportLine()); }

//! @brief Return the length of the segment.
GEOM_FT Segment3d::getLength(void) const
  { return getFromPoint().dist(getToPoint()); }

Pos3d Segment3d::getCenterOfMass(void) const
  {
    Pos3d retval= getFromPoint();
    const Vector3d v= (getToPoint()-retval)/2;
    retval= retval+v;
    return retval;
  }

//! @brief Return the intersection of the segment and the line,
//! if it doesn't exists return an empty list.
GeomObj3d::list_Pos3d Segment3d::getIntersection(const Line3d &r) const
  {
    GeomObj3d::list_Pos3d retval;
    auto result= intersection(cgseg, r.ToCGAL());
    if(result)
      {
	const CGSegment_3 *s= boost::get<CGSegment_3>(&*result);
        if(s)
	  {
	    std::cerr << getClassName() << "::" << __FUNCTION__
	              << "; segments is inside the line."
	              << " All its points belong to their intersection set."
	              << std::endl;
            retval.push_back(getCenterOfMass()); //Return the segment center.
	  }
	else
	  {
            const CGPoint_3* p= boost::get<CGPoint_3 >(&*result);
	    retval.push_back(Pos3d(*p));
          }
      }
    return retval;
  }

//! @brief Return the intersection of the ray and the segment, if
//! the intersection doesn't exists returns an empty list.
GeomObj3d::list_Pos3d Segment3d::getIntersection(const Ray3d &sr) const
  {
    GeomObj3d::list_Pos3d retval;
    auto result= intersection(cgseg, sr.ToCGAL());
    if(result)
      {
	const CGSegment_3 *s= boost::get<CGSegment_3>(&*result);
        if(s)
	  {
	    std::clog << getClassName() << "::" << __FUNCTION__
	              << "; segments and ray overlap."
	              << " Returning the center of the intersection segment."
	              << std::endl;
	    const Segment3d tmp(*s);
            retval.push_back(tmp.getCenterOfMass()); //Return the segment center.
	  }
	else
	  {
            const CGPoint_3* p= boost::get<CGPoint_3 >(&*result);
	    retval.push_back(Pos3d(*p));
          }
      }
    return retval;
  }

//! @brief Return the intersection of the line with the plane defined
//! by the equation coord_i=cte.
GeomObj3d::list_Pos3d Segment3d::getIntersection(unsigned short int i, const double &d) const
  {
    GeomObj3d::list_Pos3d lp;
    lp= getSupportLine().getIntersection(i,d);
    if(!lp.empty())
      {
        const Vector3d i_= VDir();
        const double l= getSupportLine().getLambda(i,d,i_);
        if( (l<0.0) || (l>getLength()) )
          lp.erase(lp.begin(),lp.end());
      }
    return lp;
  }


//! @brief Return the intersection of the segments, if
//! the intersection doesn't exists returns an empty list.
GeomObj3d::list_Pos3d Segment3d::getIntersection(const Segment3d &sg2) const
  {
    GeomObj3d::list_Pos3d retval;
    auto result= intersection(cgseg, sg2.cgseg);
    if(!result)
      result= intersection(sg2.cgseg, cgseg); // Sometimes this works.
    if(result)
      {
	const CGSegment_3 *s= boost::get<CGSegment_3>(&*result);
        if(s)
	  {
	    std::clog << getClassName() << "::" << __FUNCTION__
	              << "; both: " << (*this) << " and " << sg2
	              << " segments overlap."
	              << " Returning the center of the intersection segment."
	              << std::endl;
	    const Segment3d tmp(*s);
            retval.push_back(tmp.getCenterOfMass()); //Return the segment center.
	  }
	else
	  {
            const CGPoint_3* p= boost::get<CGPoint_3 >(&*result);
	    retval.push_back(Pos3d(*p));
          }
      }
    return retval;
  }

//! @brief Return the orthogonal projection onto the line.
//! @param p: point to project.
Pos3d Segment3d::Projection(const Pos3d &p) const
  {
    const Line3d l= getSupportLine();
    return l.Projection(p);
  }

//! @brief Return the projection onto the line.
//! @param v: vector to project.
Vector3d Segment3d::Projection(const Vector3d &v) const
  {
    const Line3d l= getSupportLine();
    return l.Projection(v);
  }

GEOM_FT dist(const Pos3d &p,const Segment3d &r)
  { return sqrt_FT(dist2(r,p)); }

//! @brief Returns angle with vector.
GEOM_FT Segment3d::getAngle(const Vector3d &v) const
  { return angle(VDir(),v); }
//! @brief Returns angle with another line segment.
GEOM_FT Segment3d::getAngle(const Segment3d &s) const
  { return getAngle(s.VDir()); }
GEOM_FT angle(const Segment3d &r,const Vector3d &v)
  { return r.getAngle(v); }

//! @brief Moment of inertia with respect to the center of mass in local coordinates.
GEOM_FT Segment3d::Ix(void) const
  { return 0.0; }

//! @brief Moment of inertia with respect to the center of mass in local coordinates.
GEOM_FT Segment3d::Iy(void) const
  { return NAN; }

//! @brief Moment of inertia with respect to the center of mass in local coordinates.
GEOM_FT Segment3d::Iz(void) const
  { return NAN; }

//! @brief Return the points that results from the segment division.
//!
//! @param num_partes: number of segments.
VectorPos3d Segment3d::Divide(int num_partes) const
  { return VectorPos3d(getFromPoint(),getToPoint(),num_partes); }

//! @brief Return a Python list containing the points that results
//! from the segment division.
//!
//! @param num_partes: number of segments.
boost::python::list Segment3d::DividePy(int num_partes) const
  {
    VectorPos3d tmp= Divide(num_partes);
    boost::python::list retval;
    for(VectorPos3d::const_iterator i= tmp.begin();i!=tmp.end(); i++)
      retval.append(*i);
    return retval;
  }

void Segment3d::Print(std::ostream &os) const
  { os << getFromPoint() << " " << getToPoint(); }

typedef std::vector<CGSegment_3> SegmentVector;
typedef SegmentVector::const_iterator ConstIterator;
typedef std::pair<ConstIterator,ConstIterator> IdPair;
typedef std::list<IdPair> PairList;
typedef SegmentVector::iterator Iterator;
typedef CGAL::AABB_segment_primitive<GEOMKernel, Iterator> Primitive;
typedef CGAL::AABB_traits<GEOMKernel, Primitive> Traits;
typedef CGAL::AABB_tree<Traits> Tree;
typedef Tree::Primitive_id PrimitiveId;
typedef std::list<PrimitiveId> IdList;
typedef PairList::iterator PairIterator;

bool connected(const CGSegment_3 &a, const CGSegment_3 &b, const double &tol= 1e-6)
  {
    bool retval= false;
    if(&a!=&b)
      {
	const Pos3d &pa0= Pos3d(a.source());
	const Pos3d &pa1= Pos3d(a.target());
	const Pos3d &pb0= Pos3d(b.source());
	const Pos3d &pb1= Pos3d(b.target());
	const double d1= dist2(pa0,pb0);
	if(d1<tol)
	  { retval= true; }
	else
	  {
	    const double d2= dist2(pa0,pb1);
	    if(d2<tol)
	      { retval= true; }
	    else
	      {
		const double d3= dist2(pa1,pb0);
		if(d3<tol)
		  { retval= true; }
		else
		  {
		    const double d4= dist2(pa1,pb1);
		    if(d4<tol)
		      { retval= true; }
		  }
	      }
	  }
      }
    return retval;
  }

PairList getIntersections(SegmentVector &segments)
  {
    
    // constructs the AABB tree and the internal search tree for 
    // efficient distance computations.
    Tree tree(segments.begin(),segments.end());
    tree.accelerate_distance_queries();

    PairList pairList;
    std::set<ConstIterator> visited;    

    for(ConstIterator i= segments.begin();i!= segments.end();i++)
      {
    	const CGSegment_3 &s= *i;
        IdList intersectedPrimitives;
    	tree.all_intersected_primitives(s,std::back_inserter(intersectedPrimitives));
    	visited.insert(i);
        for(IdList::iterator j= intersectedPrimitives.begin();j!= intersectedPrimitives.end();j++)
          {
    	    const Iterator k= *j;
    	    if(visited.find(k)!=visited.end())
    	      {
    	        const auto &is= *k;
    	        const bool itself= (&is==&s);
    	        const bool conn= connected(is,s);
    	        if(!itself && !conn)
    	          pairList.push_back(IdPair(i,k));
    	      }
    	  }
      }
    return pairList;
  }

//! @brief Return the intersections between the segments in
//! the container.
int_pair_deque getIntersections(const std::deque<Segment3d> &segments)
  {
    // We copy the segments to CGAL (not a very elegant solution indeed).
    const size_t sz= segments.size();
    SegmentVector cgalSegments(sz);
    for(size_t i= 0;i<sz;i++)
      cgalSegments[i]= segments[i].ToCGAL();
    PairList pairList= getIntersections(cgalSegments);
    ConstIterator begin= cgalSegments.begin();
    int_pair_deque retval;
    for(PairList::const_iterator i= pairList.begin(); i!=pairList.end(); i++)
      {
	IdPair idPair= *i;
	const int first= idPair.first-begin;
	const int second= idPair.second-begin;
	retval.push_back(int_pair(first,second));
      }
    return retval;
  }
