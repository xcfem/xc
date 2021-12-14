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
class_<Trf, boost::noncopyable >("Trf", no_init)
  ;

Pos2d (Trf2d::*TrfPos2d)(const Pos2d &) const= &Trf2d::Transform;
Vector2d (Trf2d::*TrfVector2d)(const Vector2d &) const= &Trf2d::Transform;
class_<Trf2d, bases<Trf>, boost::noncopyable >("Trf2d", no_init)
//.def("getInverse", &Trf2d::Inversa)
  .def("getTrfVector2d", TrfVector2d)
  .def("getTrfPos2d", TrfPos2d)
   ;

class_<Identity2d, bases<Trf2d> >("Identity2d")
  .def(init<>())
  ;

class_<Translation2d, bases<Trf2d> >("Translation2d")
  .def(init<Vector2d>())
  ;

class_<Rotation2d, bases<Trf2d> >("Rotation2d")
  .def(init<double>())
  ;

class_<Scaling2d, bases<Trf2d> >("Scaling2d")
  .def(init<double>())
  ;

class_<Reflection2d, bases<Trf2d> >("Reflection2d")
  .def(init<Line2d>())
  ;

Pos3d (Trf3d::*TrfPos3d)(const Pos3d &) const= &Trf3d::Transform;
Vector3d (Trf3d::*TrfVector3d)(const Vector3d &) const= &Trf3d::Transform;
class_<Trf3d, bases<Trf>, boost::noncopyable >("Trf3d", no_init)
  .def("getTrfVector3d", TrfVector3d)
  .def("getTrfPos3d", TrfPos3d)
  ;

class_<Identity3d, bases<Trf3d> >("Identity3d")
  .def(init<>())
  ;

class_<Translation3d, bases<Trf3d> >("Translation3d")
  .def(init<Vector3d>())
  ;

class_<Rotation3d, bases<Trf3d> >("Rotation3d")
  .def(init<Line3d,double>())
  ;

class_<Scaling3d, bases<Trf3d> >("Scaling3d")
  .def(init<double>())
  ;

class_<Reflection3d, bases<Trf3d> >("Reflection3d")
  .def(init<Plane>())
  ;


