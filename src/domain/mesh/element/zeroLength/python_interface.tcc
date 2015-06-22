//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  Este software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita 
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//python_interface.tcc

class_<XC::ZeroLength, bases<XC::Element0D>, boost::noncopyable >("ZeroLength", no_init)
  .def("clearMaterials",&XC::ZeroLength::clearMaterials)
  .def("setMaterial",&XC::ZeroLength::setMaterial)
  .def("getMaterials",make_function(&XC::ZeroLength::getMaterials,return_internal_reference<>()))
  ;

class_<XC::ZeroLengthSection, bases<XC::Element0D>, boost::noncopyable >("ZeroLengthSection", no_init)
  .def("getSection",make_function(&XC::ZeroLengthSection::getSection,return_internal_reference<>()),"Returns element's section.")
  .def("getMaterial",make_function(&XC::ZeroLengthSection::getSection,return_internal_reference<>()),"Returns element's section.")
  ;

class_<XC::ZeroLengthContact, bases<XC::Element0D>, boost::noncopyable >("ZeroLengthContact", no_init);

class_<XC::ZeroLengthContact2D, bases<XC::ZeroLengthContact>, boost::noncopyable >("ZeroLengthContact2D", no_init);

//class_<XC::ZeroLengthContact3D, bases<XC::ZeroLengthContact>, boost::noncopyable >("ZeroLengthContact3D", no_init);


