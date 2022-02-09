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
//python_interface.cxx

GEOM_FT (Pos2d::*distLine2d)(const Line2d &) const= &Pos2d::dist;
GEOM_FT (Pos2d::*distRay2d)(const Ray2d &) const= &Pos2d::dist;
GEOM_FT (Pos2d::*distSegment2d)(const Segment2d &) const= &Pos2d::dist;
GEOM_FT (Pos2d::*dist2Line2d)(const Line2d &) const= &Pos2d::dist2;
GEOM_FT (Pos2d::*dist2Ray2d)(const Ray2d &) const= &Pos2d::dist2;
GEOM_FT (Pos2d::*dist2Segment2d)(const Segment2d &) const= &Pos2d::dist2;
GEOM_FT (Pos2d::*distPos2d)(const Pos2d &) const= &Pos2d::dist;
GEOM_FT (Pos2d::*dist2Pos2d)(const Pos2d &) const= &Pos2d::dist2;
GEOM_FT (Pos2d::*getItemPos2d)(const size_t &) const= &Pos2d::at0;
class_<Pos2d, bases<ProtoGeom> >("Pos2d")
  .def(init<double, double>())
  .def(init<Pos2d>())
  .def("__getitem__",getItemPos2d)
  .add_property("x", &Pos2d::x, &Pos2d::SetX)
  .add_property("y", &Pos2d::y, &Pos2d::SetY)
  .def("getDimension", &Pos2d::dimension,"return the dimension of the object.")
  .def("getPositionVector", &Pos2d::VectorPos,"return position vector of the point.")
  .def("notAPoint", &Pos2d::notAPoint, "return true in one or more of its coordinates is not a number.")
  .def("dist",distPos2d,"distance to a point.")
  .def("dist",distLine2d,"distance to a straight line.")
  .def("dist",distRay2d,"distance to a ray.")
  .def("dist",distSegment2d,"distance to a segment.")
  .def("dist2",dist2Pos2d,"squared distance to a point.")
  .def("dist2",dist2Line2d,"squared distance to a straight line.")
  .def("dist2",dist2Ray2d,"squared distance to a ray.")
  .def("dist2",dist2Segment2d,"squared distance to a segment.")
  .def(self + Vector2d())
  .def(self - Vector2d())
  .def(self - Pos2d())
  .def(self_ns::str(self_ns::self))
  .def(self_ns::repr(self_ns::self))
  ;
typedef TMatrix<Pos2d,std::vector<Pos2d> > mt_pos2d;
class_<mt_pos2d, bases<ProtoMatrix> >("mt_pos2d")
   .def("size", &mt_pos2d::size)
 ;

typedef PosArray<Pos2d> m_pos2d;
class_<m_pos2d, bases<mt_pos2d> >("m_pos2d")
  ;

class_<Pos2dArray, bases<m_pos2d> >("MatrixPos2d")
  .def("getNumQuads",&Pos2dArray::GetNumQuads)
  .def("getNumPoints",&Pos2dArray::size)
  .def("getMax",&Pos2dArray::GetMax)
  .def("getMin",&Pos2dArray::GetMin)
  .def("getIx",&Pos2dArray::getIx)
  .def("getIy",&Pos2dArray::getIy)
  .def("getPxy",&Pos2dArray::GetPxy)
  .def("Ix",&Pos2dArray::Ix)
  .def("Iy",&Pos2dArray::Iy)
  .def("Pxy",&Pos2dArray::Pxy)
  .def("getCenter",&Pos2dArray::getCenter)
  .def("getCenterOfMass",&Pos2dArray::getCenterOfMass)
  .def("getArea",&Pos2dArray::GetArea,"Return the total area.")
  .def("getTriangle1",&Pos2dArray::getTriangle1)
  .def("getTriangle2",&Pos2dArray::getTriangle2)
  .def("getX",&Pos2dArray::GetX)
  .def("getY",&Pos2dArray::GetY)
  .def("getPoint",&Pos2dArray::getPoint)
  .def("getQuad",&Pos2dArray::GetQuad,"Return the area of the (i,j) quad.")
  .def("getAreaQuad",&Pos2dArray::GetAreaQuad)
  .def("getQuadCentroid",&Pos2dArray::getQuadCentroid)
  .def("In",&Pos2dArray::In)
  .def("Transform",&Pos2dArray::Transform)
  .def("Lagrange",&Pos2dArray::Lagrange)
  .def(self_ns::str(self_ns::self))
  .def(self_ns::repr(self_ns::self))
  ;


GEOM_FT (Vector2d::*dotVector2d)(const Vector2d &) const= &Vector2d::GetDot;
class_<Vector2d, bases<ProtoGeom> >("Vector2d")
  .def(init<double, double>())
  .def(init<Vector2d>())
  .add_property("x", &Vector2d::x, &Vector2d::SetX)
  .add_property("y", &Vector2d::y, &Vector2d::SetY)
  .def("normalized", &Vector2d::getNormalized, "Return the normalized vector (vector with norm 1).")
  .def("getAngle", &Vector2d::getAngle, "Return the angle with the vector argumnet (order is important)")
  .def("getXAxisAngle", &Vector2d::XAxisAngle)
  .def("getYAxisAngle", &Vector2d::YAxisAngle)
  .def("dot", dotVector2d,"Returns scalar product.")
  .def("getModulus", &Vector2d::GetModulus, "Return the modulus of the vector.")
  .def("getNormalVector", &Vector2d::Normal, "Return a vector normal to this one.")
  .def(-self) // __neg__ (unary minus)
  .def(self + self)          // __add__
  .def(self - self)           // __sub__
  .def(self * double())
  .def(double() * self)
  .def(self_ns::str(self_ns::self))
  .def(self_ns::repr(self_ns::self))
   ;


class_<Dir3d, boost::noncopyable>("Dir3d", no_init)
  .def(init<Dir3d>())
  .def(init<double, double, double>())
  .def(self_ns::str(self_ns::self))
  .def(self_ns::repr(self_ns::self))
  ;

GEOM_FT (Vector3d::*dotVector3d)(const Vector3d &) const= &Vector3d::GetDot;
class_<Vector3d, bases<ProtoGeom> >("Vector3d")
  .def(init<double, double, double>())
  .def(init<Vector3d>())
  .add_property("x", &Vector3d::x, &Vector3d::SetX)
  .add_property("y", &Vector3d::y, &Vector3d::SetY)
  .add_property("z", &Vector3d::z, &Vector3d::SetZ)
  // .def("normalize", &Vector3d::Normalize)
  .def("normalized", &Vector3d::getNormalized, "Return the normalized vector (vector with norm 1).")
  .def("getAngle", &Vector3d::getAngle, "Return the angle with the vector argument (order is important)")
  .def("getPolarAngle", &Vector3d::getPolarAngle," Return the polar angle (spherical coordinates)")
   .def("getAzimuthalAngle", &Vector3d::getAzimuthalAngle, " Return the azimuthal angle (spherical coordinates)")
  .def("perpendicular", &Vector3d::Perpendicular)
  .def("cross", &Vector3d::getCross)
  .def("dot", dotVector3d,"Returns scalar product.")
  .def("getModulus", &Vector3d::GetModulus)
  .def(-self) // __neg__ (unary minus)
  .def(self + Vector3d())
  .def(self - Vector3d())
  .def(self * double())
  .def(double() * self)
  .def(self_ns::str(self_ns::self))
  .def(self_ns::repr(self_ns::self))
   ;

GEOM_FT (Pos3d::*distLine3d)(const Line3d &) const= &Pos3d::dist;
GEOM_FT (Pos3d::*distRay3d)(const Ray3d &) const= &Pos3d::dist;
GEOM_FT (Pos3d::*distSegment3d)(const Segment3d &) const= &Pos3d::dist;
GEOM_FT (Pos3d::*dist2Line3d)(const Line3d &) const= &Pos3d::dist2;
GEOM_FT (Pos3d::*dist2Ray3d)(const Ray3d &) const= &Pos3d::dist2;
GEOM_FT (Pos3d::*dist2Segment3d)(const Segment3d &) const= &Pos3d::dist2;
GEOM_FT (Pos3d::*getItemPos3d)(const size_t &) const= &Pos3d::at0;
GEOM_FT (Pos3d::*distPos3d)(const Pos3d &) const= &Pos3d::dist;
GEOM_FT (Pos3d::*distPlane)(const Plane &) const= &Pos3d::dist;
GEOM_FT (Pos3d::*distHalfSpace3d)(const HalfSpace3d &) const= &Pos3d::dist;
//GEOM_FT (Pos3d::*distSolid3d)(const Pos3d &) const= &Pos3d::dist;
GEOM_FT (Pos3d::*dist2Pos3d)(const Pos3d &) const= &Pos3d::dist2;
GEOM_FT (Pos3d::*dist2Plane)(const Plane &) const= &Pos3d::dist2;
GEOM_FT (Pos3d::*dist2HalfSpace3d)(const HalfSpace3d &) const= &Pos3d::dist2;
class_<Pos3d, bases<ProtoGeom> >("Pos3d")
  .def(init<double, double, double>())
  .def(init<Pos3d>())
  .def("__getitem__",getItemPos3d)
  .add_property("x", &Pos3d::x, &Pos3d::SetX)
  .add_property("y", &Pos3d::y, &Pos3d::SetY)
  .add_property("z", &Pos3d::z, &Pos3d::SetZ)
  .def("getPositionVector", &Pos3d::VectorPos)
  .def("notAPoint", &Pos3d::notAPoint, "return true in one or more of its coordinates is not a number.")
  .def("dist",distPos3d)
  .def("dist",distLine3d)
  .def("dist",distRay3d)
  .def("dist",distSegment3d)
  .def("dist",distPlane)
  .def("dist",distHalfSpace3d)
  //.def("dist",distSolid3d)
  .def("dist2",dist2Pos3d)
  .def("dist2",dist2Line3d)
  .def("dist2",dist2Ray3d)
  .def("dist2",dist2Segment3d)
  .def("dist2",dist2Plane)
  .def("dist2",dist2HalfSpace3d)
  .def("getDimension", &Pos3d::dimension)
  .def(self + Vector3d())
  .def(self - Vector3d())
  .def(self - Pos3d())
  .def(self_ns::str(self_ns::self))
  .def(self_ns::repr(self_ns::self))
  ;

typedef std::vector<Pos3d> v_pos3d;
class_<v_pos3d>("v_pos3d")
  .def(vector_indexing_suite<v_pos3d>() )
 ;

GEOM_FT (SlidingVector2d::*getMomentPos2d)(const Pos2d &o) const= &SlidingVector2d::getMoment;
// GEOM_FT (SlidingVector2d::*getMomentLine2d)(const Line2d &e) const= &SlidingVector2d::getMoment;

class_<SlidingVector2d, bases<Vector2d> >("SlidingVector2d")
  .def(init<Pos2d,Vector2d>())
  .def(init<Pos2d,Pos2d>())
  .def(init<SlidingVector2d>())
  .def("getOrg", make_function(&SlidingVector2d::getOrg,return_internal_reference<>()), "return the vector application point.")
  .def("getVector2d",&SlidingVector2d::getVector2d,"Return the 2D vector part of the object.")
  .def("getMoment",getMomentPos2d, "Return the moment of the vector with respect to the point argument.")
  .def(self + self)          // __add__
  .def(self - self)           // __sub__
  .def(self * double())
  .def(double() * self)
  .def(self_ns::str(self_ns::self))
  .def(self_ns::repr(self_ns::self))
  ;


GEOM_FT (SlidingVectorsSystem2d::*getMoment2D)(void) const= &SlidingVectorsSystem2d::getMoment;
GEOM_FT (SlidingVectorsSystem2d::*getMomentPos2D)(const Pos2d &) const= &SlidingVectorsSystem2d::getMoment;
Vector2d (SlidingVectorsSystem2d::*getResultant2D)(void) const= &SlidingVectorsSystem2d::getResultant;

class_<SlidingVectorsSystem2d, bases<SlidingVector2d> >("SlidingVectorsSystem2d")
  .def(init<Pos2d,Vector2d,GEOM_FT>())
  .def(init<SlidingVector2d>())
  .def(init<SlidingVectorsSystem2d>()) // IMPORTANT: last constructor higher priority.
  .def("getResultant",getResultant2D,"Return the resultant of the SVS.")
  .def("getMoment",getMoment2D, "Return the moment of the SVS.")
  .def("getMoment",getMomentPos2D, "Return the moment of the SVS with respect to the point argument.")
  .def("reduceTo",&SlidingVectorsSystem2d::reduceTo,"Sets the reference point to express the moments with respect to.")
  .def("zeroMomentLine",&SlidingVectorsSystem2d::getZeroMomentLine,"Return zero moment line (if it exists).")
  .def("distribute",&SlidingVectorsSystem2d::distributePy, "distribute(pointList, weights): create an equivalent vector system on the argument points.") 
  .def("distribute",&SlidingVectorsSystem2d::distributePyNoWeights, "distribute(pointList): create an equivalent vector system on the argument points.") 
  .def(SlidingVector2d()+self) //Sobrecarga de operadores
  .def(self+SlidingVector2d())
  .def(SlidingVector2d()-self)
  .def(self-SlidingVector2d())
  .def(self+=SlidingVector2d())
  .def(self-=SlidingVector2d())
  .def(SlidingVectorsSystem2d()+self)
  .def(self+SlidingVectorsSystem2d())
  .def(SlidingVectorsSystem2d()-self)
  .def(self-SlidingVectorsSystem2d())
  .def(self+=SlidingVectorsSystem2d())
  .def(self-=SlidingVectorsSystem2d())
  .def(self * double())
  .def(double() * self)
  .def(self_ns::str(self_ns::self))
  .def(self_ns::repr(self_ns::self))
  ;

SlidingVector3d (SlidingVector3d::*getMomentPos3d)(const Pos3d &o) const= &SlidingVector3d::getMoment;
GEOM_FT (SlidingVector3d::*getMomentLine3d)(const Line3d &e) const= &SlidingVector3d::getMoment;

class_<SlidingVector3d, bases<Vector3d> >("SlidingVector3d")
  .def(init<Pos3d,Vector3d>())
  .def(init<Pos3d,Pos3d>())
  .def(init<SlidingVector3d>())
  .def("getOrg", make_function(&SlidingVector3d::getOrg,return_internal_reference<>()), " return the vector application point.")
  .def("getVector3d",&SlidingVector3d::getVector3d,"Return the 3D vector part of the object.")
  .def("getMoment",getMomentPos3d)
  .def("getMoment",getMomentLine3d)
  .def(self + self)          // __add__
  .def(self - self)           // __sub__
  .def(self * double())
  .def(double() * self)
  .def(self_ns::str(self_ns::self))
  .def(self_ns::repr(self_ns::self))
  ;


const Vector3d &(SlidingVectorsSystem3d::*getMoment3D)(void) const= &SlidingVectorsSystem3d::getMoment;
SlidingVector3d (SlidingVectorsSystem3d::*getMomentPos3D)(const Pos3d &) const= &SlidingVectorsSystem3d::getMoment;
GEOM_FT (SlidingVectorsSystem3d::*getMomentLine3D)(const Line3d &e) const= &SlidingVectorsSystem3d::getMoment;
Vector3d (SlidingVectorsSystem3d::*getResultant3D)(void) const= &SlidingVectorsSystem3d::getResultant;

class_<SlidingVectorsSystem3d, bases<SlidingVector3d> >("SlidingVectorsSystem3d")
  .def(init<SlidingVector3d>())
  .def(init<Pos3d,Vector3d,Vector3d>())
  .def(init<SlidingVectorsSystem3d>()) // IMPORTANT: last constructor higher priority.
  .def("getResultant",getResultant3D," return the resultant of the SVS.")
  .def("getMoment",make_function(getMoment3D,return_internal_reference<>()), "return the SVS moment.")
  .def("getMoment",getMomentPos3D, "return the SVS moment.")
  .def("getMoment",getMomentLine3D, "return the SVS moment.")
  .def("zeroMomentLine",&SlidingVectorsSystem3d::getZeroMomentLine,"Return zero moment line (if it exists).")
  .def("reduceTo",&SlidingVectorsSystem3d::reduceTo,"Sets the reference point to express the moments with respect to.")
  .def("distribute",&SlidingVectorsSystem3d::distributePy, "distribute(pointList, weights): create an equivalent vector system on the argument points.") 
  .def("distribute",&SlidingVectorsSystem3d::distributePyNoWeights, "distribute(pointList): create an equivalent vector system on the argument points.") 
  .def(SlidingVector3d()+self) //Sobrecarga de operadores
  .def(self+SlidingVector3d())
  .def(SlidingVector3d()-self)
  .def(self-SlidingVector3d())
  .def(self+=SlidingVector3d())
  .def(self-=SlidingVector3d())
  .def(SlidingVectorsSystem3d()+self)
  .def(self+SlidingVectorsSystem3d())
  .def(SlidingVectorsSystem3d()-self)
  .def(self-SlidingVectorsSystem3d())
  .def(self+=SlidingVectorsSystem3d())
  .def(self-=SlidingVectorsSystem3d())
  .def(self * double())
  .def(double() * self)
  .def(self_ns::str(self_ns::self))
  .def(self_ns::repr(self_ns::self))
  ;


typedef TMatrix<Pos3d,v_pos3d > mt_pos3d;
class_<mt_pos3d, bases<ProtoMatrix, v_pos3d> >("mt_pos3d")
  ;

typedef PosArray<Pos3d> m_pos3d;
class_<m_pos3d, bases<mt_pos3d> >("m_pos3d")
  ;

class_<Pos3dArray, bases<m_pos3d> >("Pos3dArray")
  ;

typedef std::vector<m_pos3d > ttz_pos3d;
class_<ttz_pos3d >("ttz_pos3d")
  .def(vector_indexing_suite<ttz_pos3d>() )
  ;

typedef PosArray3d<Pos3d> t_pos3d;
class_<t_pos3d, bases<ttz_pos3d> >("t_pos3d")
  ;

class_<Pos3dArray3d, bases<t_pos3d> >("Pos3dArray3d")
  ;

class_<Pos2dList, bases<GeomObj2d> >("ListPos2d")
  .def(init<>())
  .def(init<Pos2dList>())
  .def(init<boost::python::list>())
  .def("getArea", &Pos2dList::getArea,"returns enclosed area.")
  .def("getCenterOfMass", &Pos2dList::getCenterOfMass,"returns center of gravity.")
  .def("appendPoint", make_function(&Pos2dList::appendPoint,return_internal_reference<>()),"appends a point to the end of the list.")
  .def("getNumberOfVertices", &Pos2dList::getNumberOfPoints,"returns the number of vertices.")
  .def(self_ns::str(self_ns::self))
  .def(self_ns::repr(self_ns::self))
  ;

class_<Pos3dList, bases<GeomObj3d> >("ListPos3d")
  .def(init<>())
  .def(init<Pos3dList>())
  .def(init<boost::python::list>())
  .def("orientation", &Pos3dList::orientation, "orientation(vPoint) return the orientation of the point list with respect to the point of view argument.")
  .def("clockwise", &Pos3dList::clockwise, "clockwise(vPoint) return true if the point list is clockwise oriented with respect to the point of view argument.")
  .def("counterclockwise", &Pos3dList::counterclockwise, "counterclockwise(vPoint) return true if the point list is counterclockwise oriented with respect to the point of view argument.")
  .def("getCenterOfMass", &Pos3dList::getCenterOfMass, "Return the centroid of the point sequence.")
  .def("getAvgKVector", &Pos3dList::getAvgKVector, "Return the average K vector of the point sequence.")
  .def("appendPoint", make_function(&Pos3dList::appendPoint,return_internal_reference<>()),"appends a point to the end of the list.")
  .def(self_ns::str(self_ns::self))
  .def(self_ns::repr(self_ns::self))
  ;
