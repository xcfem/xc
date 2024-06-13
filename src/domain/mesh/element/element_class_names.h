//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
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
#ifndef element_class_names_h
#define element_class_names_h

#include <set>

namespace XC {

#include <set>

const std::set<std::string> element_class_names= {"XC::Element", "XC::ElementBase<>", "XC::EightNodeBrick", "XC::ElemWithMaterial<>", "XC::BeamColumnJoint2d", "XC::BeamColumnJoint3d", "XC::BeamContact2D", "XC::BeamContact3D", "XC::BrickBase", "XC::BbarBrick", "XC::Brick", "XC::BrickUP", "XC::EightNodeBrick_u_p_U", "XC::Joint2D", "XC::Joint3D", "XC::NineFourNodeQuadUP", "XC::NineNodeMixedQuad", "XC::PlaneElement<>", "XC::QuadBase4N<>", "XC::ASDShellQ4", "XC::ConstantPressureVolumeQuad", "XC::FourNodeQuadUP", "XC::Shell4NBase", "XC::ShellMITC4Base", "XC::ShellMITC4", "XC::ShellNLDKGQ", "XC::SolidMech4N", "XC::EnhancedQuad", "XC::FourNodeQuad", "XC::QuadBase9N<>", "XC::ShellMITC9", "XC::TriBase3N<>", "XC::Tri31", "XC::TotalLagrangianFD20NodeBrick", "XC::TotalLagrangianFD8NodeBrick", "XC::TwentyEightNodeBrickUP", "XC::XC::TwentyNodeBrick_u_p_U", "XC::Twenty_Node_Brick", "XC::Element0D", "XC::FrictionElementBase", "XC::FlatSliderSimple2d", "XC::FlatSliderSimple3d", "XC::SingleFPSimple2d", "XC::SingleFPSimple3d", "XC::ZeroLength", "XC::ZeroLengthContact", "XC::ZeroLengthContact2D", "XC::ZeroLengthContact3D", "XC::ZeroLengthSection", "XC::Element1D", "XC::BeamColumnWithSectionFD", "XC::BeamColumnWithSectionFDTrf2d", "XC::BeamWithHinges2d", "XC::DispBeamColumn2dBase", "XC::DispBeamColumn2d", "XC::DispBeamColumnNL2d", "XC::NLForceBeamColumn2dBase", "XC::ForceBeamColumn2d", "XC::NLBeamColumn2d", "XC::BeamColumnWithSectionFDTrf3d", "XC::BeamWithHinges3d", "XC::DispBeamColumn3d", "XC::NLForceBeamColumn3dBase", "XC::ForceBeamColumn3d", "XC::NLBeamColumn3d", "XC::ProtoBeam2d", "XC::ElasticBeam2dBase", "XC::ElasticBeam2d", "XC::ElasticTimoshenkoBeam2d", "XC::beam2d", "XC::beam2d02", "XC::beam2d03", "XC::beam2d04", "XC::ProtoBeam3d", "XC::ElasticBeam3dBase", "XC::ElasticBeam3d", "XC::ElasticTimoshenkoBeam3d", "XC::beam3dBase", "XC::beam3d01", "XC::beam3d02", "XC::ProtoTruss", "XC::CorotTrussBase", "XC::CorotTruss", "XC::CorotTrussSection", "XC::Spring", "XC::TrussBase", "XC::Truss", "XC::TrussSection", "XC::UpdatedLagrangianBeam2D", "XC::Elastic2dGNL", "XC::InelasticYS2DGNL", "XC::Inelastic2DYS01", "XC::Inelastic2DYS02", "XC::Inelastic2DYS03", "XC::SurfaceLoadBase<>", "XC::BrickSurfaceLoad", "XC::QuadSurfaceLoad", "XC::XC::TwentyNodeBrick", "XC::TwentySevenNodeBrick", "XC::NewElement", "XC::Subdomain", "XC::ActorSubdomain", "XC::ShadowSubdomain", "XC::fElement", "XC::fElmt02", "XC::fElmt05"};

//! @brief Return true if the given class name corresponds to an element.
bool is_element(const std::string &className)
  { return (XC::element_class_names.find(className)!= XC::element_class_names.end()); }

 
} // end of XC namespace

#endif
