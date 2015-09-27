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

class_<XC::RegionSecc, XC::RegionSecc *, bases<XC::DiscretBase>, boost::noncopyable >("RegionSecc", no_init)
  .def("getNumCells",&XC::RegionSecc::getNumCells)
  .def("getPoligono",&XC::RegionSecc::getPoligono)
  .def("getArea",&XC::RegionSecc::Area)
  .def("getIy",&XC::RegionSecc::Iy)
  .def("getIz",&XC::RegionSecc::Iz)
  .def("getPyz",&XC::RegionSecc::Pyz)
  .def("getCdg",make_function(&XC::RegionSecc::Cdg,return_internal_reference<>()))
  ;


class_<XC::RgQuadCell, bases<XC::RegionSecc>, boost::noncopyable >("RgQuadCell", no_init)
  .def("setDiscretization",&XC::RgQuadCell::setDiscretization)
  .add_property("nDivIJ",&XC::RgQuadCell::getNDivIJ,&XC::RgQuadCell::setNDivIJ,"Number of divisions of I->J side.")
  .add_property("nDivJK",&XC::RgQuadCell::getNDivJK,&XC::RgQuadCell::setNDivJK,"Number of divisions of J->K side.")
  .add_property("nDivCirc",&XC::RgQuadCell::getNDivIJ,&XC::RgQuadCell::setNDivIJ,"Number of divisions of I->J side.")
  .add_property("nDivRad",&XC::RgQuadCell::getNDivJK,&XC::RgQuadCell::setNDivJK,"Number of divisions of J->K side.")
  .add_property("getNumCells",&XC::RgQuadCell::getNumCells)
  .def("genGrid",make_function(&XC::RgQuadCell::Malla,return_internal_reference<>()))
  .def("getCdgRejilla",&XC::RgQuadCell::getCdgRejilla)
  .def("getVertCoords",&XC::RgQuadCell::getVertCoords)
  .def("getCellVertCoords",&XC::RgQuadCell::getCellVertCoords)
  ;

class_<XC::RgSccCirc, bases<XC::RgQuadCell>, boost::noncopyable >("RgSccCirc", no_init)
  .add_property("center",&XC::RgSccCirc::getCenterPos,&XC::RgSccCirc::setCenterPos,"Center's position.")
  .add_property("intRad",&XC::RgSccCirc::getInteriorRadius,&XC::RgSccCirc::setInteriorRadius,"Interior radiuls.")
  .add_property("extRad",&XC::RgSccCirc::getExteriorRadius,&XC::RgSccCirc::setExteriorRadius,"Exterior radius.")
  .add_property("initAngle",&XC::RgSccCirc::getInitialAngle,&XC::RgSccCirc::setInitialAngle,"Initial angle.")
  .add_property("finalAngle",&XC::RgSccCirc::getFinalAngle,&XC::RgSccCirc::setFinalAngle,"Final angle.")
  ;

class_<XC::RgSccCuadrilatero, bases<XC::RgQuadCell>, boost::noncopyable >("RgSccCuadrilatero", no_init)
  .add_property("pMin",&XC::RgSccCuadrilatero::getPMin,&XC::RgSccCuadrilatero::setPMin,"Lower left corner.")
  .add_property("pMax",&XC::RgSccCuadrilatero::getPMax,&XC::RgSccCuadrilatero::setPMax,"Upper right corner.")
  .def("swap",&XC::RgSccCuadrilatero::swap)
  .def("getQuad",&XC::RgSccCuadrilatero::getCuadrilatero)
  .def("getPolygon",&XC::RgSccCuadrilatero::getPoligono)
  .def("setQuad",&XC::RgSccCuadrilatero::setQuad)
  ;

class_<XC::RgSccPoligono, bases<XC::RgQuadCell>, boost::noncopyable >("RgSccPoligono", no_init);

// typedef std::list<XC::RegionSecc *> lst_reg_ptrs;
// class_<lst_reg_ptrs, boost::noncopyable >("lst_reg_ptrs", no_init)
//   .def("__iter__", boost::python::iterator<lst_reg_ptrs >())
//   ;

class_<XC::ListRegiones, bases<XC::SeccionInerte>, boost::noncopyable >("RgSccPoligono", no_init)
  .def("__iter__", boost::python::iterator<XC::ListRegiones >())
  .add_property("getBnd",&XC::ListRegiones::getBnd, "Returns regions boundary.")
  .def("newQuadRegion",make_function(&XC::ListRegiones::newQuadRegion,return_internal_reference<>()))
  .def("newCircularRegion",make_function(&XC::ListRegiones::newCircularRegion,return_internal_reference<>()))
   ;

