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
//python_interface.cxx!

#include "python_interface.h"

std::list<Polygon2d> from_python_list(boost::python::list &l)
  {
    std::list<Polygon2d> retval;
    const size_t sz= len(l);
    for(size_t i= 0;i<sz;++i)
      retval.push_back(boost::python::extract<Polygon2d>(l[i]));
    return retval;
  }

boost::python::list to_python_list(const std::list<Polygon2d> &l)
  {
    boost::python::list retval;
    BOOST_FOREACH(const Polygon2d& p, l) retval.append(p);
    return retval;
  }

boost::python::list python_particiona(boost::python::list &l)
  {
    std::list<Polygon2d> areas= from_python_list(l);
    particiona(areas);
    return to_python_list(areas);
  }

//! @brief Return the result from clipping the polygons on the list 
//! using the Voronoi algorithm.
boost::python::list python_clip(boost::python::list &l, const Polygon2d &plg)
  {
    std::list<Polygon2d> areas= from_python_list(l);
    areas= clip(areas,plg);
    return to_python_list(areas);
  }

void export_d0_entities(void);
void export_d1_entities(void);
void export_d2_entities(void);
void export_d3_entities(void);
void export_sys_trf(void);

BOOST_PYTHON_MODULE(geom)
  {
    using namespace boost::python;
    docstring_options doc_options;

    class_<ProtoGeom, bases<CommandEntity> >("ProtoGeom")
      .add_property("exists",&ProtoGeom::exists, "Return true if not degenerated/invalid.")
      ;

    class_<GeomObj, bases<ProtoGeom>, boost::noncopyable >("GeomObj", no_init)
      .def("getLength", pure_virtual(&GeomObj::getLength))
      .def("getArea", pure_virtual(&GeomObj::getArea), "return the object area.")
      .def("getVolume", pure_virtual(&GeomObj::getVolume))
      .def("getDimension", pure_virtual(&GeomObj::Dimension))
      .def(self_ns::str(self_ns::self))
      .def(self_ns::repr(self_ns::self))
      ;

    class_<GeomObj2d, bases<GeomObj>, boost::noncopyable >("GeomObj2d", no_init)
      .def("getMax",&GeomObj2d::GetMax)
      .def("getMin",&GeomObj2d::GetMin)
      .add_property("getXMax",&GeomObj2d::GetXMax, "Return maximum value for x coordinate.")
      .add_property("getYMax",&GeomObj2d::GetYMax, "Return maximum value for y coordinate.")
      .add_property("getXMin",&GeomObj2d::GetXMin, "Return minimum value for x coordinate.")
      .add_property("getYMin",&GeomObj2d::GetYMin, "Return minimum value for y coordinate.")
      .def("getIx", pure_virtual(&GeomObj2d::Ix))
      .def("getIy", pure_virtual(&GeomObj2d::Iy))
      .def("getIz", &GeomObj2d::Iz)
      .def("getPxy", pure_virtual(&GeomObj2d::Pxy))
      .def("move",&GeomObj2d::Move,"move(vector): move the object.")
      .def("In",&GeomObj2d::In)
      .def<Pos2d (GeomObj2d::*)(const Pos2d &) const>("getProjection", &GeomObj2d::Projection)
      .def<Vector2d (GeomObj2d::*)(const Vector2d &) const>("getProjection", &GeomObj2d::Projection)
      ;

    class_<GeomObj3d, bases<GeomObj>, boost::noncopyable >("GeomObj3d", no_init)
      .def("getMax",&GeomObj3d::GetMax)
      .def("getMin",&GeomObj3d::GetMin)
      .add_property("getXMax",&GeomObj3d::GetXMax, "Return maximum value for x coordinate.")
      .add_property("getYMax",&GeomObj3d::GetYMax, "Return maximum value for y coordinate.")
      .add_property("getZMax",&GeomObj3d::GetZMax, "Return maximum value for z coordinate.")
      .add_property("getXMin",&GeomObj3d::GetXMin, "Return minimum value for x coordinate.")
      .add_property("getYMin",&GeomObj3d::GetYMin, "Return minimum value for y coordinate.")
      .add_property("getZMin",&GeomObj3d::GetZMin, "Return minimum value for z coordinate.")
      .def("getIx", pure_virtual(&GeomObj3d::Ix))
      .def("getIy", pure_virtual(&GeomObj3d::Iy))
      .def("getIz", &GeomObj3d::Iz)
      .def("getPxy", pure_virtual(&GeomObj3d::Pxy)) 
      .def("getPxz", pure_virtual(&GeomObj3d::Pxz)) 
      .def("getPyz", pure_virtual(&GeomObj3d::Pyz))
      .def<Pos3d (GeomObj3d::*)(const Pos3d &) const>("getProjection", &GeomObj3d::Projection)
      .def<Vector3d (GeomObj3d::*)(const Vector3d &) const>("getProjection", &GeomObj3d::Projection)
      .def("In", &GeomObj3d::In, "In(pos3d,tol) return true if the points lies inside the polygon (distance from the point to the polygon less than the tolerance argument).")
     ;

    #include "lists/python_interface.tcc"
    #include "pos_vec/python_interface.tcc"

    export_d0_entities();
    export_d1_entities();
    export_d2_entities();
    export_d3_entities();
    export_sys_trf();
  }

