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

class_<XC::RegionSecc, XC::RegionSecc *, bases<XC::DiscretBase>, boost::noncopyable >("RegionSecc", no_init)
  .def("getNumCells",&XC::RegionSecc::getNumCells)
  .def("getPolygon",&XC::RegionSecc::getPolygon)
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
  .def("genGrid",make_function(&XC::RgQuadCell::getMesh,return_internal_reference<>()))
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

class_<XC::RgSccQuad, bases<XC::RgQuadCell>, boost::noncopyable >("RgSccQuad", no_init)
  .add_property("pMin",&XC::RgSccQuad::getPMin,&XC::RgSccQuad::setPMin,"Lower left corner.")
  .add_property("pMax",&XC::RgSccQuad::getPMax,&XC::RgSccQuad::setPMax,"Upper right corner.")
  .def("setTileSizeIJ",&XC::RgSccQuad::setTileSizeIJ,"Sets numbers of divisions to get a tile IJ side smaller than size.")
  .def("setTileSizeJK",&XC::RgSccQuad::setTileSizeJK,"Sets numbers of divisions to get a tile JK side smaller than size.")
  .def("setTileSize",&XC::RgSccQuad::setTileSize,"Sets numbers of divisions to get tile sizes smaller than sizeIJ and sizeJK.")
  .def("swap",&XC::RgSccQuad::swap)
  .def("getQuad",&XC::RgSccQuad::getQuad)
  .def("getPolygon",&XC::RgSccQuad::getPolygon)
  .def("setQuad",&XC::RgSccQuad::setQuad)
  ;

class_<XC::RgSccPoligono, bases<XC::RgQuadCell>, boost::noncopyable >("RgSccPoligono", no_init);

// typedef std::list<XC::RegionSecc *> lst_reg_ptrs;
// class_<lst_reg_ptrs, boost::noncopyable >("lst_reg_ptrs", no_init)
//   .def("__iter__", boost::python::iterator<lst_reg_ptrs >())
//   ;

class_<XC::ListRegiones, bases<XC::SectionMassProperties>, boost::noncopyable >("RgSccPoligono", no_init)
  .def("__iter__", boost::python::iterator<XC::ListRegiones >())
  .add_property("getBnd",&XC::ListRegiones::getBnd, "Returns regions boundary.")
  .def("newQuadRegion",make_function(&XC::ListRegiones::newQuadRegion,return_internal_reference<>()))
  .def("newCircularRegion",make_function(&XC::ListRegiones::newCircularRegion,return_internal_reference<>()))
   ;

