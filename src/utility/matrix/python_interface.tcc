//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  XC is free software: you can redistribute it and/or modify 
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of 
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//python_interface.tcc

class_<XC::ID, bases<CommandEntity> >("ID")
  .def(vector_indexing_suite<XC::ID>() )  
  .def(init<boost::python::list>())
  .def(init<std::set<int> >())
  .def(init<std::vector<int> >())
  .def(self_ns::str(self_ns::self))
  .def("getReversed",&XC::ID::getReversed,"Returns the reversed sequence.")
  // .def(self + self)
  // .def(self - self)
  // .def(self += self)
  // .def(self -= self)
  // .def(self *= int())
  ;

implicitly_convertible<XC::ID,boost::python::list>();
implicitly_convertible<boost::python::list,XC::ID>();

def("id_to_py_list",XC::xc_id_to_py_list);

double &(XC::Vector::*getItemVector)(const size_t &)= &XC::Vector::at;
class_<XC::Vector, bases<CommandEntity> >("Vector")
  .def(init<boost::python::list>())
  .def(init<Vector2d>())
  .def(init<Vector3d>())
  .def("__getitem__",getItemVector, return_value_policy<return_by_value>())
  .def("__iter__",range(&XC::Vector::begin, &XC::Vector::end))
//  .def( "__getitem__", getItemVector, boost::python::arg( "index" ), boost::python::return_internal_reference<>() )
  .def(self * double())
  .def(double() * self)
  .def(self / double())
  .def(-self) // __neg__ (unary minus)
  .def(self + self)
  .def(self - self)
  .def(self += self)
  .def(self -= self)
  .def(self *= double())
  .def(self /= double())
  .def(self_ns::str(self_ns::self))
  .def("dot",&XC::Vector::dot,"Returns dot product.")
  .def("Norm",&XC::Vector::Norm,"Returns vector norm.")
  .def("Norm2",&XC::Vector::Norm2,"Returns vector norm.")
  .def("size",&XC::Vector::Size)
  .def("__len__",&XC::Vector::Size)
  .def("getComponents",&XC::Vector::getComponents,"Return the specified set of vector components")
  .def("putComponents",&XC::Vector::putComponents,"Assigns the specified values to the specified set of vector components")
  .def("addComponents",&XC::Vector::addComponents,"Sums the specified values to the specified set of vector components")
  .def("Normalized",&XC::Vector::Normalized,"Returns normalizxed vector.")
  ;


implicitly_convertible<XC::Vector,boost::python::list>();
implicitly_convertible<boost::python::list,XC::Vector>();

typedef std::vector<XC::Vector, std::allocator<XC::Vector> > std_vector_xc_vector;
class_<std_vector_xc_vector>("std_vector_xc_vector")
  .def(vector_indexing_suite<std_vector_xc_vector>() )
  ;

double &(XC::Matrix::*at)(int,int)= &XC::Matrix::operator();
class_<XC::Matrix, bases<CommandEntity> >("Matrix")
  .def(init<boost::python::list>())
  .def("__call__",at, return_value_policy<return_by_value>())
  .def(self * double())
  .def(double() * self)
  .def(self * XC::Vector())
  .def(-self) // __neg__ (unary minus)
  .def(self + self)
  .def(self - self)
  .def(self += self)
  .def(self -= self)
  .add_property("noRows",&XC::Matrix::noRows,"Returns number of rows.")
  .add_property("noCols",&XC::Matrix::noCols,"Returns number of columns.")
  .def("at",at, return_value_policy<return_by_value>(),"Returns value at (i,j) position.")
  .def("getRow",&XC::Matrix::getRow,"Returns a row on the matrix.")
  .def("getCol",&XC::Matrix::getCol,"Returns a column on the matrix.")
  .def(self_ns::str(self_ns::self))
  .def("rowSum",&XC::Matrix::rowSum,"Returns the sum of the rows.")
  .def("columnSum",&XC::Matrix::columnSum,"Returns the sum of the columns.")
  .def("rowNorm",&XC::Matrix::rowNorm,"Row norm.")
  .def("columnNorm",&XC::Matrix::columnNorm,"Column norm.")
  .def("Norm2",&XC::Matrix::Norm2,"Return squared value of euclidean norm.")
  .def("Norm",&XC::Matrix::Norm,"Return the value of the euclidean norm.")
  .def("OneNorm",&XC::Matrix::OneNorm,"Return the value of the one norm.")
  .def("RCond",&XC::Matrix::RCond,".Return an estimation of the reciprocal of the condition number using the 1-norm.")
  .def("getInverse",&XC::Matrix::getInverse,"Return the inverse of the matrix-")
   ;


#include "nDarray/python_interface.tcc"
