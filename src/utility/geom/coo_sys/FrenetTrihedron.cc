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
//FrenetTrihedron.cc

#include "FrenetTrihedron.h"
#include "utility/geom/d2/Circle3d.h" //Circle center.

//! @brief Default constructor.
FrenetTrihedron::FrenetTrihedron(void)
  : ProtoGeom(), path() 
  {}

//! @brief Constructor.
FrenetTrihedron::FrenetTrihedron(const Polyline3d &pth)
  : ProtoGeom(), path(pth) 
  {
    interval_map= compute_interval_map();
    tangent_vectors= compute_tangent_vectors();
    normal_vectors= compute_normal_vectors();
  }

//! @brief Compute a map that relates a lenght with its
//! corresponding segment (v_i, v_(i+1)) in the path.
FrenetTrihedron::IntervalMap FrenetTrihedron::compute_interval_map(void)
  {
    IntervalMap retval;
    std::vector<double> lengths= path.getLengths();
    Polyline3d::const_iterator vi= path.begin(); // vertex iterator.
    for(std::vector<double>::const_iterator li= lengths.begin(); li!= lengths.end(); li++, vi++)
      {
	retval[*li]= vi;
      }
    return retval;
  }

//! @brief Return the end of the interval corresponding to the given arc length.
//! @param s: arc length.
FrenetTrihedron::IntervalMap::const_iterator FrenetTrihedron::get_interval_end(const double &s) const
  {
    const double length= path.getLength();
    FrenetTrihedron::IntervalMap::const_iterator retval= interval_map.end();
    if(!interval_map.empty())
      {
	if(s<length)
	  retval= interval_map.upper_bound(s);
	else
	  retval= std::prev(interval_map.end()); // last item.
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "The interval map is empty."
		<< std::endl;
    return retval;
  }

//! @brief Compute the tangent vectors at each of the
//! of the path vertices.
std::vector<Vector3d> FrenetTrihedron::compute_tangent_vectors(void)
  {
    const std::vector<Segment3d> segments= path.getSegments();
    const size_t sz= segments.size();
    std::vector<Vector3d> retval(sz+1);
    if(sz>0)
      {
        Vector3d t0= segments[0].getIVector();
	retval[0]= t0; // first tangent.
	Vector3d t1;
	for(size_t i=1; i<sz; i++)
	  {
	    t1= segments[i].getIVector();
	    retval[i]= ((t1+t0)*0.5).getNormalized();
	    t0= t1; // update previous vector.
	  }
	retval[sz]= t1; // last tangent.
      }
    return retval;
  }

//! @brief Compute the normal vectors at each of the
//! of the path vertices.
std::vector<Vector3d> FrenetTrihedron::compute_normal_vectors(void)
  {
    const size_t sz= path.getNumVertices();
    std::vector<Vector3d> retval(sz);
    if(sz<2) // No segments.
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << ";ERROR: no segments, so no curvature vectors."
		  << std::endl;
      }
    else if(sz==2) // One segment only.
      {
	Segment3d sg= path.getSegment(1);
	retval[0]= sg.getJVector();
	retval[1]= retval[0];
      }
    else // 3 vertex at least.
      {
	std::vector<Vector3d> vertex_normals(sz);
	size_t count= 0; // vertex iterator.
	for(Polyline3d::const_iterator vi= path.begin(); vi!=path.end(); vi++, count++)
	  {
	    Polyline3d::const_iterator previousVertexIter= vi-1;
	    Polyline3d::const_iterator thisVertexIter= vi;
	    Polyline3d::const_iterator nextVertexIter= vi+1;
	    if(thisVertexIter == path.begin()) // No previous vertex.
	      {
		previousVertexIter= vi;
		thisVertexIter= vi+1;
		nextVertexIter= vi+2;
	      }
	    else if(thisVertexIter == path.end()) // No following vertex.
	      {
		previousVertexIter= vi-2;
		thisVertexIter= vi-1;
		nextVertexIter= vi;
	      }
	    const Pos3d &A= *thisVertexIter;
	    const Pos3d &B= *previousVertexIter;
	    const Pos3d &C= *nextVertexIter;
	    const Pos3d center= circle_center(A, B, C);
	    retval[count]= (center-*vi).getNormalized();
	  }
      }
    return retval;
  }

//! @brief Return the tangent vector corresponding to the given arc length.
//! @param s: arc length.
Vector3d FrenetTrihedron::getTangent(const double &s) const
  {
    Vector3d retval;
    const IntervalMap::const_iterator i1= get_interval_end(s);
    const IntervalMap::const_iterator i0= std::prev(i1);
    const double s0= (*i0).first;
    const Pos3d v0= (*(*i0).second);
    const size_t j0= std::distance(path.begin(),(*i0).second);
    if(j0<tangent_vectors.size())
      {
	const Vector3d t0= tangent_vectors[j0];
	const double s1= (*i1).first;
	const Pos3d v1= (*(*i1).second);
	const size_t j1= j0+1; //std::distance(path.begin(),(*i1).second);
	const Vector3d t1= tangent_vectors[j1];
	retval= (t1-t0)/(s1-s0)*(s-s0)+t0;
      }
    else
      std::cerr << "Index out of range." << std::endl;
    return retval;
  }

//! @brief Return the normal vector corresponding to the given arc length.
//! @param s: arc length.
Vector3d FrenetTrihedron::getNormal(const double &s) const
  {
    Vector3d retval;
    const IntervalMap::const_iterator i1= get_interval_end(s);
    const IntervalMap::const_iterator i0= std::prev(i1);
    const double s0= (*i0).first;
    const Pos3d v0= (*(*i0).second);
    const size_t j0= std::distance(path.begin(),(*i0).second);
    if(j0<normal_vectors.size())
      {
	const Vector3d n0= normal_vectors[j0];
	const double s1= (*i1).first;
	const Pos3d v1= (*(*i1).second);
	const size_t j1= j0+1; //std::distance(path.begin(),(*i1).second);
	const Vector3d n1= normal_vectors[j1];
	retval= (n1-n0)/(s1-s0)*(s-s0)+n0;
      }
    else
      std::cerr << "Index out of range." << std::endl;
    return retval;
  }

//! @brief Prints the matrix.
void FrenetTrihedron::Print(std::ostream &os) const
  { os << "path= " << this->path; }

//! @brief Return a Python dictionary with the object members values.
boost::python::dict FrenetTrihedron::getPyDict(void) const
  {
    boost::python::dict retval= ProtoGeom::getPyDict();
    retval["path"]= path.getPyDict();
    return retval;
  }

//! @brief Set the values of the object members from a Python dictionary.
void FrenetTrihedron::setPyDict(const boost::python::dict &d)
  {
    ProtoGeom::setPyDict(d);
    this->path= boost::python::extract<Polyline3d>(d["path"]);
  }

std::ostream &operator<<(std::ostream &os,const FrenetTrihedron &sc)
  {
    sc.Print(os);
    return os;
  }
