//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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

class_<XC::ForceBeamColumn2d, bases<XC::NLForceBeamColumn2dBase>, boost::noncopyable >("ForceBeamColumn2d", no_init)
  .add_property("maxSubdivisions", &XC::ForceBeamColumn2d::getMaxSubdivisions,&XC::ForceBeamColumn2d::setMaxSubdivisions, "Get/set the maximum number of subdivisons of dv for local iterations.")
  .add_property("subdivideFactor", &XC::ForceBeamColumn2d::getSubdivideFactor,&XC::ForceBeamColumn2d::setSubdivideFactor, "Get/set the factor to reduce newton scheme step size.")
  .def("getSectionLocations",&XC::ForceBeamColumn2d::getSectionLocationsPy,"Returns the positions of the sections along the element.")
  .def("getSectionWeights",&XC::ForceBeamColumn2d::getSectionWeightsPy,"Returns the weights corresponding to each of the element sections.")
   ;

class_<XC::ForceBeamColumn3d, bases<XC::NLForceBeamColumn3dBase>, boost::noncopyable >("ForceBeamColumn3d", no_init)
  .add_property("maxSubdivisions", &XC::ForceBeamColumn3d::getMaxSubdivisions,&XC::ForceBeamColumn3d::setMaxSubdivisions, "Get/set the maximum number of subdivisons of dv for local iterations.")
  .add_property("subdivideFactor", &XC::ForceBeamColumn3d::getSubdivideFactor,&XC::ForceBeamColumn3d::setSubdivideFactor, "Get/set the factor to reduce newton scheme step size.")
  .def("getSectionLocations",&XC::ForceBeamColumn3d::getSectionLocationsPy,"Returns the positions of the sections along the element.")
  .def("getSectionWeights",&XC::ForceBeamColumn3d::getSectionWeightsPy,"Returns the weights corresponding to each of the element sections.")
   ;

#include "beam_integration/python_interface.tcc"
