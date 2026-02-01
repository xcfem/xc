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
#include <set>
#include <string>
#include "uniaxial_material_class_names.h"

const std::set<std::string> uniaxial_material_class_names= {"XC::UniaxialMaterial", "XC::BarSlipMaterial", "XC::Bilinear", "XC::BoucWenMaterial", "XC::Clough", "XC::CloughDamage", "XC::ConnectedMaterial", "XC::ParallelMaterial", "XC::SeriesMaterial", "XC::DrainMaterial", "XC::DrainBilinearMaterial", "XC::DrainClough1Material", "XC::DrainClough2Material", "XC::DrainHardeningMaterial", "XC::DrainPinch1Material", "XC::ElasticBaseMaterial", "XC::CableMaterial", "XC::ENTMaterial", "XC::ENCMaterial", "XC::EPPBaseMaterial", "XC::EPPGapMaterial", "XC::ElasticPPMaterialBase", "XC::ElasticPPMaterial", "XC::EyBasic", "XC::ElasticMaterial", "XC::EncapsulatedUniaxialMaterial", "XC::HalfDiagramMaterial", "XC::CompressionOnlyMaterial", "XC::TensionOnlyMaterial", "XC::InitStrainBaseMaterial", "XC::InitStrainMaterial", "XC::InitStressMaterial", "XC::InvertMaterial", "XC::MinMaxMaterial", "XC::PathIndependentMaterial", "XC::FatigueMaterial", "XC::FedeasMaterial", "XC::FedeasBondMaterial", "XC::FedeasBond1Material", "XC::FedeasBond2Material", "XC::FedeasConcrMaterial", "XC::FedeasConcr1Material", "XC::FedeasConcr2Material", "XC::FedeasConcr3Material", "XC::FedeasHardeningMaterial", "XC::FedeasHyster1Material", "XC::FedeasHyster2Material", "XC::FedeasSteel1Material", "XC::FedeasSteel2Material", "XC::HardeningMaterial", "XC::HystereticMaterial", "XC::MultiLinear", "XC::NewUniaxialMaterial", "XC::PYBase", "XC::PQyzBase", "XC::PySimple1", "XC::PyLiq1", "XC::QzSimple1", "XC::TzSimple1", "XC::TzLiq1", "XC::Pinching", "XC::Pinching4Material", "XC::PinchingDamage", "XC::RawConcrete", "XC::Concrete02", "XC::Concrete02IS", "XC::ConcreteBase", "XC::Concrete01", "XC::Concrete04", "XC::TDConcreteBase", "XC::TDConcrete", "XC::TDConcreteMC10Base", "XC::TDConcreteMC10", "XC::TDConcreteMC10NL", "XC::ReinforcingSteel", "XC::SteelBase", "XC::Steel02", "XC::SteelBase0103", "XC::Steel01", "XC::Steel03", "XC::ViscousMaterial"};
  
//! @brief Return true if the given class name corresponds to a uniaxial material.
bool XC::is_uniaxial_material(const std::string &className)
  { return (uniaxial_material_class_names.find(className)!= uniaxial_material_class_names.end()); }
  
