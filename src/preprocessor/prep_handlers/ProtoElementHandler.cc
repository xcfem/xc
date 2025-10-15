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
//ProtoElementHandler.cc

#include "ProtoElementHandler.h"
#include "create_elem.h"

#include "domain/mesh/element/truss_beam_column/truss/CorotTruss.h"
#include "domain/mesh/element/truss_beam_column/truss/CorotTrussSection.h"
#include "domain/mesh/element/truss_beam_column/truss/Truss.h"
#include "domain/mesh/element/truss_beam_column/truss/TrussSection.h"
#include "domain/mesh/element/truss_beam_column/truss/Spring.h"

#include "domain/mesh/element/truss_beam_column/beam2d/beam2d02.h"
#include "domain/mesh/element/truss_beam_column/beam2d/beam2d03.h"
#include "domain/mesh/element/truss_beam_column/beam2d/beam2d04.h"
#include "domain/mesh/element/truss_beam_column/beam3d/beam3d01.h"
#include "domain/mesh/element/truss_beam_column/beam3d/beam3d02.h"
#include "domain/mesh/element/truss_beam_column/beamWithHinges/BeamWithHinges2d.h"
#include "domain/mesh/element/truss_beam_column/beamWithHinges/BeamWithHinges3d.h"
#include "domain/mesh/element/truss_beam_column/elasticBeamColumn/ElasticBeam2d.h"
#include "domain/mesh/element/truss_beam_column/elasticBeamColumn/ElasticTimoshenkoBeam2d.h"
#include "domain/mesh/element/truss_beam_column/elasticBeamColumn/ElasticBeam3d.h"
#include "domain/mesh/element/truss_beam_column/elasticBeamColumn/ElasticTimoshenkoBeam3d.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/ForceBeamColumn2d.h"
#include "domain/mesh/element/truss_beam_column/forceBeamColumn/ForceBeamColumn3d.h"
#include "domain/mesh/element/truss_beam_column/dispBeamColumn/DispBeamColumn2d.h"
#include "domain/mesh/element/truss_beam_column/dispBeamColumn/DispBeamColumn3d.h"
#include "domain/mesh/element/truss_beam_column/nonlinearBeamColumn/element/NLBeamColumn2d.h"
#include "domain/mesh/element/truss_beam_column/nonlinearBeamColumn/element/NLBeamColumn3d.h"
#include "domain/mesh/element/truss_beam_column/updatedLagrangianBeamColumn/UpdatedLagrangianBeam2D.h"
#include "domain/mesh/element/uw_elements/BeamContact2D.h"
#include "material/nD/uw_materials/ContactMaterial2D.h"
#include "domain/mesh/element/uw_elements/BeamContact3D.h"
#include "material/nD/uw_materials/ContactMaterial3D.h"
#include "domain/mesh/element/plane/fourNodeQuad/ConstantPressureVolumeQuad.h"
#include "domain/mesh/element/plane/fourNodeQuad/EnhancedQuad.h"
#include "domain/mesh/element/plane/fourNodeQuad/FourNodeQuad.h"
#include "domain/mesh/element/plane/triangle/Tri31.h"
#include "domain/mesh/element/plane/fourNodeQuad/NineNodeMixedQuad.h"
#include "domain/mesh/element/plane/UP-ucsd/FourNodeQuadUP.h"
#include "domain/mesh/element/plane/UP-ucsd/Nine_Four_Node_QuadUP.h"

#include "domain/mesh/element/volumetric/brick/BbarBrick.h"
#include "domain/mesh/element/volumetric/brick/Brick.h"
#include "domain/mesh/element/volumetric/8nbrick/EightNodeBrick.h"
#include "domain/mesh/element/volumetric/20nbrick/TwentyNodeBrick.h"
#include "domain/mesh/element/volumetric/20nbrick/Twenty_Node_Brick.h"
#include "domain/mesh/element/volumetric/27nbrick/TwentySevenNodeBrick.h"
#include "domain/mesh/element/volumetric/TotalLagrangianFD20NodeBrick/TotalLagrangianFD20NodeBrick.h"
#include "domain/mesh/element/volumetric/TotalLagrangianFD20NodeBrick/TotalLagrangianFD8NodeBrick.h"
#include "domain/mesh/element/volumetric/upU/EightNodeBrick_u_p_U.h"
#include "domain/mesh/element/volumetric/upU/TwentyNodeBrick_u_p_U.h"
#include "domain/mesh/element/volumetric/UP-ucsd/BrickUP.h"
#include "domain/mesh/element/volumetric/UP-ucsd/TwentyEightNodeBrickUP.h"

#include "domain/mesh/element/special/joint/BeamColumnJoint2d.h"
#include "domain/mesh/element/special/joint/BeamColumnJoint3d.h"
#include "domain/mesh/element/special/joint/Joint2D.h"
#include "domain/mesh/element/special/joint/Joint3D.h"

#include "domain/mesh/element/plane/shell/ShellMITC4.h"
#include "domain/mesh/element/plane/shell/ShellMITC9.h"
#include "domain/mesh/element/plane/shell/ShellNLDKGQ.h"
#include "domain/mesh/element/plane/shell/ASDShellQ4.h"

#include "domain/mesh/element/zero_length/ZeroLength.h"
#include "domain/mesh/element/zero_length/ZeroLengthSection.h"
#include "domain/mesh/element/zero_length/ZeroLengthContact2D.h"
#include "domain/mesh/element/zero_length/ZeroLengthContact3D.h"

#include "domain/mesh/element/plane/surface_pressures/BrickSurfaceLoad.h"
#include "domain/mesh/element/plane/surface_pressures/QuadSurfaceLoad.h"

#include "preprocessor/Preprocessor.h"
#include "utility/utils/misc_utils/colormod.h"


//! @brief Default constructor.
XC::ProtoElementHandler::ProtoElementHandler(Preprocessor *preprocessor)
  : PrepHandler(preprocessor), material_name("nil"), num_sec(3), dim_elem(0), transformation_name("nil"), integrator_name("Lobatto"), dir(0)
 {}

//! @brief Returns a reference to the material handler.
const XC::MaterialHandler &XC::ProtoElementHandler::get_material_handler(void) const
  { return getPreprocessor()->getMaterialHandler(); }

//! @brief Returns an iterator to the material whose name corresponds
//! to material_name contents.
XC::MaterialHandler::const_iterator XC::ProtoElementHandler::get_iter_material(void) const
  { return getPreprocessor()->getMaterialHandler().find(material_name); }

//! @brief Returns a pointer to the material whose name corresponds
//! to material_name contents.
const XC::Material *XC::ProtoElementHandler::get_ptr_material(void) const
  {
    Material *retval= nullptr;
    MaterialHandler::const_iterator imat= get_iter_material();
    if(imat!= get_material_handler().end())
      retval= imat->second;
    else
      {
        std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	          << "; material: '" << material_name
	  	  << "' not found.\n"
	          << "Terminated." 
	          << Color::def << std::endl;
	exit(-1);
      }
    return retval;
  }

//! @brief Returns a reference to de integrator handler.
const XC::BeamIntegratorHandler &XC::ProtoElementHandler::get_beam_integrator_handler(void) const
  { return getPreprocessor()->getBeamIntegratorHandler(); }

//! @brief Returns an iterator to the integrator whose name corresponds to
//! the contents of integrator_name.
XC::BeamIntegratorHandler::const_iterator XC::ProtoElementHandler::get_iter_beam_integrator(void) const
  { return getPreprocessor()->getBeamIntegratorHandler().find(integrator_name); }


//! @brief Returns a pointer to the integrator whose name corresponds to
//! the contents of integrator_name.
const XC::BeamIntegration *XC::ProtoElementHandler::get_ptr_beam_integrator(void) const
  {
    BeamIntegration *retval= nullptr;
    BeamIntegratorHandler::const_iterator iInteg= get_iter_beam_integrator();
    if(iInteg!= get_beam_integrator_handler().end())
      retval= iInteg->second;
    else
      if(verbosity>0)
        std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	          << "; integrator named: '" 
                  << integrator_name << "' not found."
	          << Color::def << std::endl;
    return retval;
  }

//! @brief Returns a reference to the coordinate transformation handler.
const XC::TransfCooHandler &XC::ProtoElementHandler::get_transf_coo_handler(void) const
  { return getPreprocessor()->getTransfCooHandler(); }

//! @brief Returns an iterator to the coordinate transformation with the name being passed as parameter.
XC::TransfCooHandler::const_iterator XC::ProtoElementHandler::get_iter_transf_coo(void) const
  { return getPreprocessor()->getTransfCooHandler().find(transformation_name); }

//! @brief Returns a pointer to the coordinate transformation with the name being passed as parameter (nullptr if not found).
const XC::CrdTransf *XC::ProtoElementHandler::get_ptr_transf_coo(void) const
  {
    CrdTransf *retval= nullptr;
    TransfCooHandler::const_iterator itrf= get_iter_transf_coo();
    if(itrf!= this->get_transf_coo_handler().end())
      retval= itrf->second;
    else
      if(verbosity>0)
        std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	          << "; coordinate transformation named: '" 
                  << transformation_name << "' not found."
	          << Color::def << std::endl;
    return retval;
  }

//! @brief Warning messages about deprecated element type names.
void deprecatedElementNameMsg(const std::string &errHeader, const std::string &oldCmd,const std::string &newCmd)
  {
     std::clog << Color::red << errHeader
               << "; type name: '" << oldCmd
	       << "' is deprecated use '"
               << newCmd << "' instead."
	       << Color::def << std::endl;
  }

//! @brief Warning messages about materials that are not valid
//! for an element type.
void materialNotSuitableMsg(const std::string &errHeader, const std::string &matName, const std::string &elemType)
  {
    std::cerr << Color::red <<"Error in "
              << errHeader << "; material: '"
              << matName << "' is not suitable for "
	      << elemType << " elements."
	      << Color::def << std::endl;
  }

//! @brief Process the comands used to define
//! the elements of the finite element model. It interprets
//! the following commands (if the identifier (tag) is not
//! specified a default value will be assigned):
//!
//! - Truss: Defines a truss element (Truss). 
//! - TrussSection: Defines a truss element with section type material.
//! - CorotTruss: Defines a truss element with corotational formulation. 
//! - CorotTrussSection: Defines a truss element with corotational formulation and section type material.
//! - Beam2d_02: Defines a beam-column element (beam2d02) for plane problems. 
//! - Beam2d_03: Defines a beam-column element (beam2d03) for plane problems. 
//! - Beam2d_04: Defines a beam-column element (beam2d04) for plane problems.
//! - Beam3d_01: Defines a beam-column element (beam3d01) for 3D problems.
//! - Beam3d_02: Defines a beam-column element (beam2d02) for 3D problems.
//! - ElasticBeam2d: Defines a beam-column element (ElasticBeam2d) for plane problems.
//! - ElasticTimoshenkoBeam2d: Defines a timoshenko beam-column element (ElasticTimoshenkoBeam2d) for plane problems.
//! - ElasticBeam3d: Defines a beam-column element (ElasticBeam3d) for 3D problems.
//! - ElasticTimoshenkoBeam3d: Defines a timoshenko beam-column element (ElasticTimoshenkoBeam3d) for plane problems.
//! - ForceBeamColumn_2d: Defines a beam-column element (ForceBeamColumn2d) no lineal,
//!   for plane problems.
//! - ForceBeamColumn_3d: Defines a beam-column element (ForceBeamColumn3d) no lineal,
//!   for 3D problems.
//! - ShellMITC4: Defines a shell element (ShellMITC4),
//! - ShellMITC9: Defines a shell element (ShellMITC9),
//! - ShellNLDKGQ: Defines a geometric non-linear shell element (ShellNLDKGQ),
//!   for shell problems.
//! - ASDShellQ4: Defines a 4-node general shell element (ASDShellQ4),
//! - FourNodeQuad: Defines a four node quad (FourNodeQuad),
//! - EnhancedQuad: Defines an enhanced quad (EnhancedQuad),
//! - Tri31: Defines a three node triangle with just a Gauss point (Tri31),
//!   for plane problems.
//! - Brick: Defines an eight node hexahedron (Brick),
//!   para solid analysis.
//! - ZeroLength: Defines a zero length element (ZeroLength).
//! - ZeroLengthSection: Defines a zero length element with section type material (ZeroLengthSection).
//! - BeamContact2D: Defines a two-dimensional beam-to-node contact element which defines a frictional contact interface between a beam element and a separate body.
//! - BeamContact3D: Defines a three-dimensional beam-to-node contact element which defines a frictional contact interface between a beam element and a separate body.
XC::Element *XC::ProtoElementHandler::create_element(const std::string &elementType,int tag_elem)
  {
    Element *retval= nullptr;
    const std::string errHeader= getClassName() + "::" + __FUNCTION__;
    if((elementType == "truss")||(elementType=="Truss"))
      {
	if(elementType=="truss")
	  deprecatedElementNameMsg(errHeader,elementType,"Truss");
        retval= new_element_dim_gen_mat<Truss>(tag_elem, dim_elem, get_ptr_material());
      }
    else if((elementType == "truss_section")||(elementType == "TrussSection"))
      {
	if(elementType=="truss_section")
	  deprecatedElementNameMsg(errHeader,elementType,"TrussSection");
        retval=  new_element_dim_gen_mat<TrussSection>(tag_elem, dim_elem, get_ptr_material());
      }
    else if((elementType == "corot_truss")||(elementType == "CorotTruss"))
      {
	if(elementType=="corot_truss")
	  deprecatedElementNameMsg(errHeader,elementType,"CorotTruss");
        retval=  new_element_dim_gen_mat<CorotTruss>(tag_elem, dim_elem, get_ptr_material());
      }
    else if((elementType == "corot_truss_section")||(elementType == "CorotTrussSection"))
      {
	if(elementType=="corot_truss_section")
	  deprecatedElementNameMsg(errHeader,elementType,"CorotTrussSection");
        retval=  new_element_dim_gen_mat<CorotTrussSection>(tag_elem, dim_elem, get_ptr_material());
      }
    else if((elementType == "spring")||(elementType == "Spring"))
      {
	if(elementType=="spring")
	  deprecatedElementNameMsg(errHeader,elementType,"Spring");
        retval=  new_element_dim_gen_mat<Spring>(tag_elem, dim_elem, get_ptr_material());
      }
    else if((elementType == "beam2d_02")||(elementType == "beam2d02"))
      {
	if(elementType=="beam2d_02")
	  deprecatedElementNameMsg(errHeader,elementType,"beam2d02");
        retval=  new_element<beam2d02>(tag_elem);
      }
    else if((elementType == "beam2d_03")||(elementType == "beam2d03"))
      {
	if(elementType=="beam2d_03")
	  deprecatedElementNameMsg(errHeader,elementType,"beam2d03");
        retval=  new_element<beam2d03>(tag_elem);
      }
    else if((elementType == "beam2d_04")||(elementType == "beam2d04"))
      {
	if(elementType=="beam2d_04")
	  deprecatedElementNameMsg(errHeader,elementType,"beam2d04");
        retval=  new_element<beam2d04>(tag_elem);
      }
    else if((elementType == "beam3d_01")||(elementType == "beam3d01"))
      {
	if(elementType=="beam3d_01")
	  deprecatedElementNameMsg(errHeader,elementType,"beam3d01");
        retval=  new_element<beam3d01>(tag_elem);
      }
    else if((elementType == "beam3d_02")||(elementType == "beam3d02"))
      {
	if(elementType=="beam3d_02")
	  deprecatedElementNameMsg(errHeader,elementType,"beam3d02");
        retval=  new_element<beam3d02>(tag_elem);
      }
    else if((elementType == "elastic_beam_2d")||(elementType == "ElasticBeam2d"))
      {
	if(elementType=="elastic_beam_2d")
	  deprecatedElementNameMsg(errHeader,elementType,"ElasticBeam2d");
        retval=  new_element_gen_mat_crd<ElasticBeam2d>(tag_elem, get_ptr_material(), get_ptr_transf_coo());
      }
    else if(elementType == "ElasticTimoshenkoBeam2d")
      {
        retval=  new_element_gen_mat_crd<ElasticTimoshenkoBeam2d>(tag_elem, get_ptr_material(), get_ptr_transf_coo());
      }
    else if((elementType == "elastic_beam_3d")||(elementType == "ElasticBeam3d"))
      {
	if(elementType=="elastic_beam_3d")
	  deprecatedElementNameMsg(errHeader,elementType,"ElasticBeam3d");
        retval=  new_element_gen_mat_crd<ElasticBeam3d>(tag_elem, get_ptr_material(), get_ptr_transf_coo());
      }
    else if(elementType == "ElasticTimoshenkoBeam3d")
      {
        retval=  new_element_gen_mat_crd<ElasticTimoshenkoBeam3d>(tag_elem, get_ptr_material(), get_ptr_transf_coo());
      }
    else if((elementType == "beam_with_hinges_2d")||(elementType == "BeamWithHinges2d"))
      {
	if(elementType=="beam_with_hinges_2d")
	  deprecatedElementNameMsg(errHeader,elementType,"BeamWithHinges2d");
        retval=  new_element_gen_mat_crd<BeamWithHinges2d>(tag_elem, get_ptr_material(), get_ptr_transf_coo());
      }
    else if((elementType == "beam_with_hinges_3d")||(elementType == "BeamWithHinges3d"))
      {
	if(elementType=="beam_with_hinges_3d")
	  deprecatedElementNameMsg(errHeader,elementType,"BeamWithHinges3d");
        retval=  new_element_gen_mat_crd<BeamWithHinges3d>(tag_elem, get_ptr_material(), get_ptr_transf_coo());
      }
    else if((elementType == "disp_beam_column_2d")||(elementType == "DispBeamColumn2d"))
      {
	if(elementType=="disp_beam_column_2d")
	  deprecatedElementNameMsg(errHeader,elementType,"DispBeamColumn2d");
        retval=  new_element_ns_gen_mat_crd_integ<DispBeamColumn2d>(tag_elem, get_ptr_material(), num_sec, get_ptr_transf_coo(), get_ptr_beam_integrator());
      }
    else if((elementType == "disp_beam_column_3d")||(elementType == "DispBeamColumn3d"))
      {
	if(elementType=="disp_beam_column_3d")
	  deprecatedElementNameMsg(errHeader,elementType,"DispBeamColumn3d");
        retval=  new_element_ns_gen_mat_crd_integ<DispBeamColumn3d>(tag_elem, get_ptr_material(), num_sec, get_ptr_transf_coo(), get_ptr_beam_integrator());
      }
    else if((elementType == "nl_beam_column_2d")||(elementType == "NLBeamColumn2d"))
      {
	if(elementType=="nl_beam_column_2d")
	  deprecatedElementNameMsg(errHeader,elementType,"NLBeamColumn2d");
        retval=  new_element_ns_gen_mat_crd<NLBeamColumn2d>(tag_elem, get_ptr_material(), num_sec, get_ptr_transf_coo());
      }
    else if((elementType == "nl_beam_column_3d")||(elementType == "NLBeamColumn3d"))
      {
	if(elementType=="nl_beam_column_3d")
	  deprecatedElementNameMsg(errHeader,elementType,"NLBeamColumn3d");
        retval=  new_element_ns_gen_mat_crd<NLBeamColumn3d>(tag_elem, get_ptr_material(), num_sec, get_ptr_transf_coo());
      }
    else if((elementType == "force_beam_column_2d")||(elementType == "ForceBeamColumn2d"))
      {
	if(elementType=="force_beam_column_2d")
	  deprecatedElementNameMsg(errHeader,elementType,"ForceBeamColumn2d");
        retval=  new_element_ns_gen_mat_crd_integ<ForceBeamColumn2d>(tag_elem, get_ptr_material(), num_sec, get_ptr_transf_coo(),get_ptr_beam_integrator());
      }
    else if((elementType == "force_beam_column_3d")||(elementType == "ForceBeamColumn3d"))
      {
	if(elementType=="force_beam_column_3d")
	  deprecatedElementNameMsg(errHeader,elementType,"ForceBeamColumn3d");
        retval=  new_element_ns_gen_mat_crd_integ<ForceBeamColumn3d>(tag_elem, get_ptr_material(), num_sec, get_ptr_transf_coo(),get_ptr_beam_integrator());
      }
    else if((elementType == "BeamContact2d") || (elementType == "BeamContact2D"))
      {
        retval=  new_element_mat<BeamContact2D, ContactMaterial2D>(tag_elem, get_ptr_material());
      }
    else if((elementType == "BeamContact3d")  || (elementType == "BeamContact3D"))
      {
        retval=  new_element_mat_crd<BeamContact3D, ContactMaterial3D>(tag_elem, get_ptr_material(), get_ptr_transf_coo());
      }
    else if((elementType == "zero_length")||(elementType == "ZeroLength"))
      {
	if(elementType=="zero_length")
	  deprecatedElementNameMsg(errHeader,elementType,"ZeroLength");
        retval=  new_element_dim_gen_mat_dir<ZeroLength>(tag_elem, dim_elem, get_ptr_material(),dir);
      }
    else if((elementType == "zero_length_contact_2d")||(elementType == "ZeroLengthContact2D"))
      {
	if(elementType=="zero_length_contact_2d")
	  deprecatedElementNameMsg(errHeader,elementType,"ZeroLengthContact2D");
        retval=  new_element<ZeroLengthContact2D>(tag_elem);
      }
    else if((elementType == "zero_length_contact_3d")||(elementType == "ZeroLengthContact3D"))
      {
	if(elementType=="zero_length_contact_3d")
	  deprecatedElementNameMsg(errHeader,elementType,"ZeroLengthContact3D");
        retval=  new_element<ZeroLengthContact3D>(tag_elem);
      }
    else if((elementType == "zero_length_section")||(elementType == "ZeroLengthSection"))
      {
	if(elementType=="zero_length_section")
	  deprecatedElementNameMsg(errHeader,elementType,"ZeroLengthSection");
        retval=  new_element_dim_gen_mat<ZeroLengthSection>(tag_elem, dim_elem, get_ptr_material());
      }
    else if((elementType == "shell_mitc4")||(elementType == "ShellMITC4"))
      {
	if(elementType=="shell_mitc4")
	  deprecatedElementNameMsg(errHeader,elementType,"ShellMITC4");
        retval= new_element_mat<ShellMITC4,SectionForceDeformation>(tag_elem, get_ptr_material());
        if(!retval)
	  materialNotSuitableMsg(errHeader,material_name,elementType);
      }
    else if((elementType == "shell_nl")||(elementType == "ShellMITC9"))
      {
	if(elementType=="shell_nl")
	  deprecatedElementNameMsg(errHeader,elementType,"ShellMITC9");
        retval= new_element_mat<ShellMITC9,SectionForceDeformation>(tag_elem, get_ptr_material());
        if(!retval)
	  materialNotSuitableMsg(errHeader,material_name,elementType);
      }
    else if(elementType == "ShellNLDKGQ")
      {
        retval= new_element_mat<ShellNLDKGQ,SectionForceDeformation>(tag_elem, get_ptr_material());
        if(!retval)
	  materialNotSuitableMsg(errHeader,material_name,elementType);
      }
    else if(elementType == "ASDShellQ4")
      {
        retval= new_element_mat<ASDShellQ4,SectionForceDeformation>(tag_elem, get_ptr_material());
        if(!retval)
	  materialNotSuitableMsg(errHeader,material_name,elementType);
      }
    else if((elementType == "quad4n")||(elementType == "FourNodeQuad"))
      {
	if(elementType=="quad4n")
	  deprecatedElementNameMsg(errHeader,elementType,"FourNodeQuad");
        retval= new_element_mat<FourNodeQuad,NDMaterial>(tag_elem, get_ptr_material());
        if(!retval)
	  materialNotSuitableMsg(errHeader,material_name,elementType);
      }
    else if(elementType == "NineFourNodeQuadUP")
      {
        retval= new_element_mat<NineFourNodeQuadUP, NDMaterial>(tag_elem, get_ptr_material());
        if(!retval)
	  materialNotSuitableMsg(errHeader,material_name,elementType);
      }
    else if(elementType == "EnhancedQuad")
      {
        retval= new_element_mat<EnhancedQuad,NDMaterial>(tag_elem, get_ptr_material());
        if(!retval)
	  materialNotSuitableMsg(errHeader,material_name,elementType);
      }
    else if((elementType == "tri31")||(elementType == "Tri31"))
      {
	if(elementType=="tri31")
	  deprecatedElementNameMsg(errHeader,elementType,"Tri31");
        retval= new_element_mat<Tri31,NDMaterial>(tag_elem, get_ptr_material());
        if(!retval)
	  materialNotSuitableMsg(errHeader,material_name,elementType);
      }
    else if((elementType == "brick")||(elementType == "Brick"))
      {
	if(elementType=="brick")
	  deprecatedElementNameMsg(errHeader,elementType,"Brick");
        retval= new_element_mat<Brick,NDMaterial>(tag_elem, get_ptr_material());
        if(!retval)
	  materialNotSuitableMsg(errHeader,material_name,elementType);
      }
    else if((elementType == "quad_surface_load")||(elementType == "QuadSurfaceLoad"))
      {
        retval= new_element<QuadSurfaceLoad>(tag_elem);
        if(!retval)
	  materialNotSuitableMsg(errHeader,material_name,elementType);
      }
    else if((elementType == "brick_surface_load")||(elementType == "BrickSurfaceLoad"))
      {
        retval= new_element<BrickSurfaceLoad>(tag_elem);
        if(!retval)
	  materialNotSuitableMsg(errHeader,material_name,elementType);
      }
    // else if(elementType == "TotalLagrangianFD8NodeBrick")
    //   {
    //     retval= new_element_mat<TotalLagrangianFD8NodeBrick,NDMaterial>(tag_elem, get_ptr_material());
    //     if(!retval)
    // 	  materialNotSuitableMsg(errHeader,material_name,elementType);
    //   }
    else
      std::cerr << Color::red << errHeader
		<< "; element type: '" << elementType << "' unknown."
		<< Color::def << std::endl;
    return retval;
  }

//! @brief Create a new element.
//! @param type: type of element. Available types:'Truss','TrussSection','CorotTruss','CorotTrussSection','Spring', 'Beam2d02', 'Beam2d03',  'Beam2d04', 'Beam3d01', 'Beam3d02', 'ElasticBeam2d', 'ElasticTimoshenkoBeam2d', 'ElasticBeam3d', 'ElasticTimoshenkoBeam3d', 'BeamWithHinges2d', 'BeamWithHinges3d', 'NlBeamColumn2d', 'NlBeamColumn3d','ForceBeamColumn2d', 'ForceBeamColumn3d', 'ShellMitc4', ' shellNl', 'Quad4n', 'Tri31', 'Brick', 'ZeroLength', 'ZeroLengthContact2d', 'ZeroLengthContact3d', 'ZeroLengthSection', 'BeamContact2D', 'BeamContact3D'.
//! @param iNodes: nodes ID, e.g. xc.ID([1,2]) to create a linear element from node 1 to node 2.
XC::Element *XC::ProtoElementHandler::newElement(const std::string &type,const ID &iNodes)
  {
    const int tag_elem= getDefaultTag();
    Element *retval= getPreprocessor()->getDomain()->getElement(tag_elem);
    if(!retval) //It doesn't already exists.
      {
        retval= create_element(type,tag_elem);
        if(retval)
          {
	    if(iNodes.Size()>0)
                retval->setIdNodes(iNodes);
            add(retval);
          }
      }
    else
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << "; ERROR the element: "
                << tag_elem << " already exists."
	        << Color::def << std::endl;
    return retval;
  }

//! @brief Sets the default material name for new elements.
void XC::ProtoElementHandler::setDefaultMaterial(const std::string &nmb)
  { material_name= nmb; }

//! @brief Returns the default material name for new elements.
const std::string &XC::ProtoElementHandler::getDefaultMaterial(void) const
  { return material_name; }

//! @brief Default number of sections for new elements.
void XC::ProtoElementHandler::setNumSections(const int &ns)
  { num_sec= ns; }

//! @brief Returns the default number of sections for new elements.
int XC::ProtoElementHandler::getNumSections(void) const
  { return num_sec; }

//! @brief Sets the default dimension (0D,1D,2D or 3D) for new elements.
void XC::ProtoElementHandler::setDimElem(const int &dim)
  { dim_elem= dim; }

//! @brief Returns the default dimension (0D,1D,2D or 3D) for new elements.
int XC::ProtoElementHandler::getDimElem(void) const
  { return dim_elem; }

//! @brief Sets the name of the default coordinate transformation for new elements.
void XC::ProtoElementHandler::setDefaultTransf(const std::string &nmb)
  { transformation_name= nmb; }

//! @brief Returns the name of the default coordinate transformation for new elements.
const std::string &XC::ProtoElementHandler::getDefaultTransf(void) const
  { return transformation_name; }

//! @brief Sets the name of the default integrator for new elements.
void XC::ProtoElementHandler::setDefaultIntegrator(const std::string &nmb)
  { integrator_name= nmb; }

//! @brief Returns the name of the default integrator for new elements.
const std::string &XC::ProtoElementHandler::getDefaultIntegrator(void) const
  { return integrator_name; }

