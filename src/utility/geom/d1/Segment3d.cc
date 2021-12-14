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
//Segment3d.cc

#include "Segment3d.h"

#include "../pos_vec/Dir3d.h"
#include "../pos_vec/Vector3d.h"
#include "../pos_vec/Pos3d.h"
#include "utility/geom/pos_vec/VectorPos3d.h"
#include <CGAL/AABB_tree.h>
#include <CGAL/AABB_traits.h>
#include <CGAL/AABB_segment_primitive.h>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/graph_utility.hpp>
#include "utility/geom/d1/Polyline3d.h"

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

//! @brief Virtual constructor.
GeomObj *Segment3d::getCopy(void) const
  { return new Segment3d(*this); }

//! @ brief Swaps the segment orientation.
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

//! @brief Return the angle or the line with respect to XY plane.
GEOM_FT Segment3d::getSlope(void) const
  { return getSupportLine().getSlope(); }

//! @brief Return true if the segment goes up.
bool Segment3d::upwards(void) const
  { return (getFromPoint()(3)<getToPoint()(3)); }

//! @brief Return true if the segment goes down.
bool Segment3d::downwards(void) const
  { return (getFromPoint()(3)>getToPoint()(3)); }

const Pos3d Segment3d::Point(const int &i) const
  { return Pos3d(cgseg.point(i)); }

//! @brief Return a point of the line at a distance lambda from its origin.
Pos3d Segment3d::PtoParametricas(const GEOM_FT &lambda) const
  { return Point(0)+lambda*VDir(); }

//! @brief Return the mid point of the segment.
Pos3d Segment3d::getMidPoint(void) const
  { return PtoParametricas(0.5); }

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

//! @brief Set the extremities of the segment.
void Segment3d::Put(const Pos3d &p1,const Pos3d &p2)
  { TwoPoints(p1,p2); }

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

//! @brief Return true if the segment is connected to the argument point
//! (i. e.) the distance to one of its ends is less or equal than the tolerance.
//! @param p: point
//! @param tol: tolerance
bool Segment3d::connected(const Pos3d &p, const GEOM_FT &tol) const
  {
    bool retval= false;
    const Pos3d p0= getFromPoint();
    const Pos3d p1= getToPoint();
    const GEOM_FT tol2= tol*tol;
    if(p.dist2(p0)<tol2)
      retval= true;
    else if(p.dist2(p1)<tol2)
      retval= true;
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

//! @brief Returns the distance from the point to the segment.
GEOM_FT dist(const Pos3d &p,const Segment3d &r)
  { return sqrt_FT(dist2(r,p)); }

//! @brief Returns the angle with the vector argument.
GEOM_FT Segment3d::getAngle(const Vector3d &v) const
  { return angle(VDir(),v); }

//! @brief Returns the angle with the line argument.
GEOM_FT Segment3d::getAngle(const Line3d &r) const
  { return getAngle(r.VDir()); }

//! @brief Returns the angle with the ray argument.
GEOM_FT Segment3d::getAngle(const Ray3d &sr) const
  { return getAngle(sr.VDir()); }

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

//! @brief Return a Python dictionary with the object members values.
boost::python::dict Segment3d::getPyDict(void) const
  {
    boost::python::dict retval= Linear3d::getPyDict();
    retval["from"]= getFromPoint();
    retval["to"]= getToPoint();
    return retval;
  }

//! @brief Set the values of the object members from a Python dictionary.
void Segment3d::setPyDict(const boost::python::dict &d)
  {
    Linear3d::setPyDict(d);
    const Pos3d from= boost::python::extract<Pos3d>(d["from"]);
    const Pos3d to= boost::python::extract<Pos3d>(d["to"]);
    Put(from,to);
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

std::list<Polyline3d> get_polylines(const std::list<Segment3d> &segments, const GEOM_FT &tol)
  {
    std::list<Polyline3d> retval;
    GeomObj::list_Pos3d positions;
    if(!segments.empty())
      {
	// Inserts first point.
	const Segment3d &fs= *segments.begin();
        const Pos3d fp= fs.getFromPoint();
	positions.push_back(fp);
	// Insert following points
	const GEOM_FT tol2= tol*tol;
	typedef GeomObj::list_Pos3d::const_iterator pos_iterator;
	typedef std::pair<pos_iterator, pos_iterator> edge_pair;
	typedef std::list<edge_pair> edge_pair_list;
	edge_pair_list edge_pairs;
	for(std::list<Segment3d>::const_iterator i= segments.begin();i!=segments.end();i++)
	  {
	    const Segment3d &s= *i;
	    const Pos3d p0= s.getFromPoint();
	    pos_iterator npIterA= positions.getNearestPoint(p0);
	    GEOM_FT d2= p0.dist2(*npIterA);
	    if(d2>tol2)
	      {
		positions.push_back(p0);
	        npIterA= (++positions.rbegin()).base();
	      }
	    const Pos3d p1= s.getToPoint();
	    pos_iterator npIterB= positions.getNearestPoint(p1);
	    d2= p1.dist2(*npIterB);
	    if(d2>tol2)
	      {
		positions.push_back(p1);
	        npIterB= (++positions.rbegin()).base();
	      }
	    edge_pairs.push_back(edge_pair(npIterA, npIterB));
	  }
	// Create Graph
        struct VertexProps { int idx; char name; };
        typedef boost::adjacency_list < boost::listS, boost::listS, boost::undirectedS, VertexProps > graph_t;
	typedef boost::graph_traits < graph_t >::vertex_descriptor vertex_t;
        typedef boost::graph_traits< graph_t >::out_edge_iterator edge_iterator;
	graph_t G;
	// Create and populate vertex container.
	const int nv= positions.size();
	std::vector<vertex_t> verts(nv);
	for(int i= 0;i<nv;i++)
	  {
	    VertexProps p;
	    p.idx= i;
	    p.name= 'a' + i;
	    verts[i] = add_vertex(p, G);
	  }
	// Populate edges.
	const pos_iterator p0= positions.begin();
	for(edge_pair_list::const_iterator i= edge_pairs.begin();i!=edge_pairs.end();i++)
	  {
	    const pos_iterator pA= (*i).first;
	    const int iA= pA-p0;
	    const pos_iterator pB= (*i).second;
	    const int iB= pB-p0;
	    add_edge(verts[iA], verts[iB], G);
	  }
	// Extract polylines.
	graph_t::vertex_iterator v, vend;
	std::set<int> visited;
	for(boost::tie(v, vend) = vertices(G); v != vend; ++v)
	  {
	    int degree= out_degree(*v,G);
	    if(degree==1) // polyline starts.
	      {
	        Polyline3d tmp;
		bool stop= false;
		vertex_t nv= *v;
                int nextVertexIdx= G[nv].idx;
		do
		  {
		    if(visited.find(nextVertexIdx)==visited.end()) // not already visited.
		      {
			const Pos3d pos= positions[nextVertexIdx];
			tmp.push_back(pos);
			visited.insert(nextVertexIdx);
			// Get the next vertex id.
 		        edge_iterator ei, ei_end;
			boost::tie(ei, ei_end) = out_edges(nv, G);
			for(; ei != ei_end; ++ei)
			  {
			    nv= target(*ei, G);
			    nextVertexIdx= G[nv].idx;
			    degree= out_degree(nv,G);
			    if(visited.find(nextVertexIdx)==visited.end()) // not already visited.
			      break;
			  }
		      }
		    else // already visited.
		      stop= true;
		  }
		while(!stop);
		if(!tmp.empty())
                    retval.push_back(tmp);
	      }
          }
      }
    return retval;
  }

boost::python::list py_get_3d_polylines(const boost::python::list &l, const GEOM_FT &tol)
  {
    std::list<Segment3d> segments;
    const int sz= len(l);
    // copy the components
    for(int i=0; i<sz; i++)
      {
	Segment3d s= boost::python::extract<Segment3d>(l[i]);
        segments.push_back(s);
      }
    const std::list<Polyline3d> polylines= get_polylines(segments, tol);
    boost::python::list retval;
    for(std::list<Polyline3d>::const_iterator i= polylines.begin();i!= polylines.end();i++)
      retval.append(*i);
    return retval;    
  }
