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

class_<XC::SectRegion, XC::SectRegion *, bases<XC::DiscretBase>, boost::noncopyable >("SectRegion", no_init)
  .def("getNumCells",&XC::SectRegion::getNumCells)
  .def("getPolygon",&XC::SectRegion::getPolygon)
  .def("getArea",&XC::SectRegion::getArea)
  .def("getIy",&XC::SectRegion::Iy)
  .def("getIz",&XC::SectRegion::Iz)
  .def("getPyz",&XC::SectRegion::Pyz)
  .def("getCenterOfMass",make_function(&XC::SectRegion::getCenterOfMass,return_internal_reference<>()))
  ;


class_<XC::QuadCellRegion, bases<XC::SectRegion>, boost::noncopyable >("QuadCellRegion", no_init)
  .def("setDiscretization",&XC::QuadCellRegion::setDiscretization)
  .add_property("nDivIJ",&XC::QuadCellRegion::getNDivIJ,&XC::QuadCellRegion::setNDivIJ,"Number of divisions of I->J side.")
  .add_property("nDivJK",&XC::QuadCellRegion::getNDivJK,&XC::QuadCellRegion::setNDivJK,"Number of divisions of J->K side.")
  .add_property("nDivCirc",&XC::QuadCellRegion::getNDivIJ,&XC::QuadCellRegion::setNDivIJ,"Number of divisions of I->J side.")
  .add_property("nDivRad",&XC::QuadCellRegion::getNDivJK,&XC::QuadCellRegion::setNDivJK,"Number of divisions of J->K side.")
  .add_property("getNumCells",&XC::QuadCellRegion::getNumCells)
  .def("genGrid",make_function(&XC::QuadCellRegion::getMesh,return_internal_reference<>()))
  .def("getCenterOfMassRejilla",&XC::QuadCellRegion::getCenterOfMassRejilla)
  .def("getVertCoords",&XC::QuadCellRegion::getVertCoords)
  .def("getCellVertCoords",&XC::QuadCellRegion::getCellVertCoords)
  ;

class_<XC::CircularSectRegion, bases<XC::QuadCellRegion>, boost::noncopyable >("CircularSectRegion", no_init)
  .add_property("center",&XC::CircularSectRegion::getCenterPos,&XC::CircularSectRegion::setCenterPos,"Center's position.")
  .add_property("intRad",&XC::CircularSectRegion::getInteriorRadius,&XC::CircularSectRegion::setInteriorRadius,"Interior radiuls.")
  .add_property("extRad",&XC::CircularSectRegion::getExteriorRadius,&XC::CircularSectRegion::setExteriorRadius,"Exterior radius.")
  .add_property("initAngle",&XC::CircularSectRegion::getInitialAngle,&XC::CircularSectRegion::setInitialAngle,"Initial angle.")
  .add_property("finalAngle",&XC::CircularSectRegion::getFinalAngle,&XC::CircularSectRegion::setFinalAngle,"Final angle.")
  ;

class_<XC::QuadSectRegion, bases<XC::QuadCellRegion>, boost::noncopyable >("QuadSectRegion", no_init)
  .add_property("pMin",&XC::QuadSectRegion::getPMin,&XC::QuadSectRegion::setPMin,"Lower left corner.")
  .add_property("pMax",&XC::QuadSectRegion::getPMax,&XC::QuadSectRegion::setPMax,"Upper right corner.")
  .def("setTileSizeIJ",&XC::QuadSectRegion::setTileSizeIJ,"Sets numbers of divisions to get a tile IJ side smaller than size.")
  .def("setTileSizeJK",&XC::QuadSectRegion::setTileSizeJK,"Sets numbers of divisions to get a tile JK side smaller than size.")
  .def("setTileSize",&XC::QuadSectRegion::setTileSize,"Sets numbers of divisions to get tile sizes smaller than sizeIJ and sizeJK.")
  .def("swap",&XC::QuadSectRegion::swap)
  .def("getQuad",&XC::QuadSectRegion::getQuad)
  .def("getPolygon",&XC::QuadSectRegion::getPolygon)
  .def("setQuad",&XC::QuadSectRegion::setQuad)
  ;

class_<XC::PolygonSectRegion, bases<XC::QuadCellRegion>, boost::noncopyable >("PolygonSectRegion", no_init);

// typedef std::list<XC::SectRegion *> lst_reg_ptrs;
// class_<lst_reg_ptrs, boost::noncopyable >("lst_reg_ptrs", no_init)
//   .def("__iter__", boost::python::iterator<lst_reg_ptrs >())
//   ;

class_<XC::RegionContainer, bases<XC::SectionMassProperties>, boost::noncopyable >("RegionContainer", no_init)
  .def("__iter__", boost::python::iterator<XC::RegionContainer >())
  .add_property("getBnd",&XC::RegionContainer::getBnd, "Returns regions boundary.")
  .def("newQuadRegion",make_function(&XC::RegionContainer::newQuadRegion,return_internal_reference<>()))
  .def("newCircularRegion",make_function(&XC::RegionContainer::newCircularRegion,return_internal_reference<>()))
   ;

