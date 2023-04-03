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

class_<XC::NineFourNodeQuadUP, bases<ElemWithMaterial9N_Mech2D>, boost::noncopyable >("NineFourNodeQuadUP", no_init)
//.def("detJ", &XC::NineFourNodeQuadUP::detJ)
  .add_property("bulk", &XC::NineFourNodeQuadUP::getCombinedBulkModulus, &XC::NineFourNodeQuadUP::setCombinedBulkModulus,"combined bulk modulus.")
  .add_property("horizontalPermeability", &XC::NineFourNodeQuadUP::getHorizontalPermeability, &XC::NineFourNodeQuadUP::setHorizontalPermeability,"horizontal permeability.")
  .add_property("verticalPermeability", &XC::NineFourNodeQuadUP::getVerticalPermeability, &XC::NineFourNodeQuadUP::setVerticalPermeability,"vertical permeability.")
  .add_property("fluidRho", &XC::NineFourNodeQuadUP::getFluidRho, &XC::NineFourNodeQuadUP::setFluidRho,"fluid mass per unit volume.")
  .add_property("bodyForces", make_function(&XC::NineFourNodeQuadUP::getBodyForces,return_internal_reference<>() ), &XC::NineFourNodeQuadUP::setBodyForces,"body forces.")
   ;
