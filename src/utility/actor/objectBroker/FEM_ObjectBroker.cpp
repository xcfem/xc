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
/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 1999, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */

// $Revision: 1.31 $
// $Date: 2006/01/18 23:02:17 $
// $Source: /usr/local/cvs/OpenSees/SRC/actor/objectBroker/FEM_ObjectBroker.cpp,v $


// Written: fmk
// Revision: A
//
// Purpose: This file contains the class definition for FEM_ObjectBroker.
// FEM_ObjectBroker is is an object broker class for the finite element
// method. All methods are virtual to allow for subclasses; which can be
// used by programmers when introducing new subclasses of the XC::main objects.
//
// What: "@(#) FEM_ObjectBroker.C, revA"


#include "utility/actor/objectBroker/FEM_ObjectBroker.h"
#include "all_includes.h"
#include "utility/utils/misc_utils/colormod.h"
#include "material/nD/nd_material_class_names.h"
#include "material/uniaxial/uniaxial_material_class_names.h"
#include "material/section/section_material_class_names.h"
#include "domain/mesh/node/node_class_names.h"
#include "domain/mesh/element/utils/coordTransformation/coordinate_transformation_class_names.h"
#include "domain/constraints/constraint_class_names.h"
#include "domain/mesh/element/element_class_names.h"
#include "domain/load/load_class_names.h"
#include "domain/load/groundMotion/ground_motion_class_names.h"
#include "domain/load/pattern/time_series/time_series_class_names.h"
#include "domain/load/pattern/load_patterns/load_pattern_class_names.h"

typedef struct uniaxialPackage
  {
    int classTag;
    std::string libName;
    std::string funcName;
    XC::UniaxialMaterial *(*funcPtr)(void);
    struct uniaxialPackage *next;
  } UniaxialPackage;

static UniaxialPackage *theUniaxialPackage= nullptr;



XC::FEM_ObjectBroker::FEM_ObjectBroker(void)
  :lastLinearSolver(nullptr),lastDomainSolver(nullptr)
  {}

//! @brief Creates a new actor with the class tag
//! being passed as parameter.
XC::Actor *XC::FEM_ObjectBroker::getNewActor(int classTag, Channel *theChannel)
  {
  switch(classTag) {

#ifdef _PARALLEL_PROCESSING
  case ACTOR_TAGS_SUBDOMAIN:
    return new ActorSubdomain(*theChannel, *this,nullptr);
#endif

  default:
    std::cerr << Color::red << "FEM_ObjectBroker::" << __FUNCTION__
	      << "; no ActorType type exists for class tag: "
	      << classTag
	      << Color::def << std::endl;
    return nullptr;
  }
  }


//! @brief Creates a new PartitionedModelBuilder de la clase cuyo tag being passed as parameter.
XC::PartitionedModelBuilder *XC::FEM_ObjectBroker::getPtrNewPartitionedModelBuilder(Subdomain &theSubdomain,int classTag)
  {
    switch(classTag) {
        /*
        case PartitionedModelBuilder_TAGS_PartitionedQuick2dFrameModel:
             return new PartitionedQuick2dFrame(theSubdomain);
             */

        default:
             std::cerr << Color::red << "FEM_ObjectBroker::" << __FUNCTION__
		       << "; no PartitionedModelBuilder type exists for class tag "
		       << classTag
		       << Color::def << std::endl;
             return nullptr;

         }
}


//! @brief Broke a GraphNumberer object from its class tag.
XC::GraphNumberer *XC::FEM_ObjectBroker::getPtrNewGraphNumberer(int classTag)
  {
    switch(classTag)
      {
      case GraphNUMBERER_TAG_RCM:
        return new RCM();
      case GraphNUMBERER_TAG_MyRCM:
        return new MyRCM();
      case GraphNUMBERER_TAG_SimpleNumberer:
        return new SimpleNumberer();
      default:
        std::cerr << Color::red << "FEM_ObjectBroker::" << __FUNCTION__
		  << "; no GraphNumberer type exists for class tag "
		  << classTag
		  << Color::def << std::endl;
        return nullptr;
      }
  }

//! @brief Broke a Vertex object from its class tag.
XC::Vertex *XC::FEM_ObjectBroker::getNewVertex(int classTag)
  {
    switch(classTag)
      {
        case GRAPH_TAG_Vertex:
             return new Vertex();
        default:
             std::cerr << Color::red << "FEM_ObjectBroker::" << __FUNCTION__
		       << "; no Vertex type exists for class tag "
		       << classTag
		       << Color::def << std::endl;
             return nullptr;
      }
  }

/*****************************************
 *
 * METHODS TO GET NEW MODELLING CLASSES
 *
 *****************************************/

//! @brief Broke a Element object from its class tag.
XC::Element *XC::FEM_ObjectBroker::getNewElement(int classTag)
  {
    switch(classTag)
      {
        case ELE_TAG_Truss:
	  return new Truss();
        case ELE_TAG_TrussSection:
	  return new TrussSection();
        case ELE_TAG_CorotTruss:
	  return new CorotTruss();
        case ELE_TAG_CorotTrussSection:
	  return new CorotTrussSection();
        case ELE_TAG_ZeroLength:
	  return new ZeroLength();
        case ELE_TAG_ZeroLengthContact2D:
	  return new ZeroLengthContact2D();
        case ELE_TAG_ZeroLengthSection:
             return new ZeroLengthSection();
//         case ELE_TAG_ZeroLengthContact2D:
//              return new ZeroLengthContact2D();
//         case ELE_TAG_ZeroLengthContact3D:
//              return new ZeroLengthContact3D();
             //case ELE_TAG_ZeroLengthND:
             //return new ZeroLengthND();
        case ELE_TAG_FourNodeQuadUP:
	  return new FourNodeQuadUP();
        case ELE_TAG_FourNodeQuad:
	  return new FourNodeQuad();
        case ELE_TAG_ElasticBeam2d:
	  return new ElasticBeam2d();
        case ELE_TAG_ElasticTimoshenkoBeam2d:
	  return new ElasticTimoshenkoBeam2d();
        case ELE_TAG_ElasticBeam3d:
	  return new ElasticBeam3d();
        case ELE_TAG_ElasticTimoshenkoBeam3d:
	  return new ElasticTimoshenkoBeam3d();
        case ELE_TAG_BeamWithHinges2d:
	  return new BeamWithHinges2d();
        case ELE_TAG_BeamWithHinges3d:
	  return new BeamWithHinges3d();
        case ELE_TAG_NLBeamColumn2d:
	  return new NLBeamColumn2d();
        case ELE_TAG_NLBeamColumn3d:
	  return new NLBeamColumn3d();
        case ELE_TAG_ForceBeamColumn2d:
	  return new ForceBeamColumn2d();
        case ELE_TAG_ForceBeamColumn3d:
	  return new ForceBeamColumn3d();
        case ELE_TAG_DispBeamColumn2d:
	  return new DispBeamColumn2d();
        case ELE_TAG_DispBeamColumn3d:
	  return new DispBeamColumn3d();
        case ELE_TAG_BeamContact2D:
	  return new BeamContact2D();
        case ELE_TAG_BeamContact3D:
	  return new BeamContact3D();
        case ELE_TAG_EnhancedQuad:
	  return new EnhancedQuad();
        case ELE_TAG_NineNodeMixedQuad:
	  return new NineNodeMixedQuad();
        case ELE_TAG_ConstantPressureVolumeQuad:
	  return new ConstantPressureVolumeQuad();
        case ELE_TAG_Brick:
	  return new Brick();
        case ELE_TAG_ShellMITC4:
	  return new ShellMITC4();
        case ELE_TAG_ShellMITC9:
	  return new ShellMITC9();
        case ELE_TAG_ShellNLDKGQ:
	  return new ShellNLDKGQ();
        case ELE_TAG_ASDShellQ4:
	  return new ASDShellQ4();
        case ELE_TAG_BbarBrick:
	  return new BbarBrick();
        case ELE_TAG_Joint2D:                                // Arash
	  return new Joint2D();                        // Arash
        case ELE_TAG_Nine_Four_Node_QuadUP:
	  return new NineFourNodeQuadUP();
        case ELE_TAG_BrickUP:
	  return new BrickUP();
        case ELE_TAG_Twenty_Eight_Node_BrickUP:
	  return new TwentyEightNodeBrickUP();
        case ELE_TAG_EightNodeBrick_u_p_U:
	  return new EightNodeBrick_u_p_U();
        case ELE_TAG_TwentyNodeBrick_u_p_U:
	  return new TwentyNodeBrick_u_p_U();
        default:
	  std::cerr << Color::red << "FEM_ObjectBroker::" << __FUNCTION__
		    << "; no Element type exists for class tag "
		    << classTag
		    << Color::def << std::endl;
	  return nullptr;

      }
  }

//! @brief Broke a node from its class tag.
XC::Node *XC::FEM_ObjectBroker::getNewNode(int classTag)
  {
    switch(classTag)
      {
        case NOD_TAG_Node:
             return new Node(classTag);
        default:
             std::cerr << Color::red << "FEM_ObjectBroker::" << __FUNCTION__
		       << "; no Node type exists for class tag "
		       << classTag
		       << Color::def << std::endl;
             return nullptr;
      }
  }

//! @brief Broke a MFreedom_Constraint object from its class tag.
XC::MFreedom_Constraint *XC::FEM_ObjectBroker::getNewMP(int classTag)
  {
    switch(classTag)
      {
      case CNSTRNT_TAG_MFreedom_Constraint:
	return new OneRowMFreedom_Constraint(0, classTag);
      case CNSTRNT_TAG_RigidBeam_Constraint:
	return new RigidBeam(0);
      case CNSTRNT_TAG_RigidRod_Constraint:
	return new RigidRod(0);
      case CNSTRNT_TAG_EqualDOF_Constraint:
	return new EqualDOF(0);
      case CNSTRNT_TAG_Skew_Constraint:
	return new Skew_Constraint(0);
      case CNSTRNT_TAG_SkewPlane_Constraint:
	return new SkewPlane(0);
      case CNSTRNT_TAG_Symmetry_Constraint:
	return new SymmetryConstraint(0);
      case CNSTRNT_TAG_AntiSymmetry_Constraint:
	return new AntiSymmetryConstraint(0);
      case CNSTRNT_TAG_MFreedom_Joint2D:
        return new MFreedom_Joint2D();
      case CNSTRNT_TAG_MFreedom_Joint3D:
        return new MFreedom_Joint3D();
      default:
        std::cerr << Color::red << "FEM_ObjectBroker::" << __FUNCTION__
		  << "; no MFreedom_Constraint type exists for class tag "
		  << classTag
		  << Color::def << std::endl;
        return nullptr;
      }
  }

//! @brief Broke a MRMFreedom_Constraint object from its class tag.
XC::MRMFreedom_Constraint *XC::FEM_ObjectBroker::getNewMRMP(int classTag)
  {
    switch(classTag)
      {
      case CNSTRNT_TAG_GlueNodeToElement:
        return new GlueNodeToElement(0);
      case CNSTRNT_TAG_MRMFreedom_Constraint:
        return new MRMFreedom_Constraint(0,classTag);
      default:
        std::cerr << Color::red << "FEM_ObjectBroker::" << __FUNCTION__
		  << "; MRMFreedom_Constraint type exists for class tag "
		  << classTag
		  << Color::def << std::endl;
        return nullptr;
      }
  }

//! @brief Broke a SFreedom_Constraint object from its class tag.
XC::SFreedom_Constraint *XC::FEM_ObjectBroker::getNewSP(int classTag)
  {
    switch(classTag)
      {
      case CNSTRNT_TAG_SFreedom_Constraint:
        return new SFreedom_Constraint(classTag);
      case CNSTRNT_TAG_ImposedMotionSP:
        return new ImposedMotionSP();

      case CNSTRNT_TAG_ImposedMotionSP1:
	return new ImposedMotionSP1();
      default:
	std::cerr << Color::red << "FEM_ObjectBroker::" << __FUNCTION__
		  << "; no SFreedom_Constraint type exists for class tag "
		  << classTag
		  << Color::def << std::endl;
	return nullptr;
	
      }
  }

//! @brief Broke a nodal load from its class identifier.
XC::NodalLoad *XC::FEM_ObjectBroker::getNewNodalLoad(int classTag)
  {
    switch(classTag)
      {
      case LOAD_TAG_NodalLoad:
        return new NodalLoad(0,classTag);
      default:
        std::cerr << Color::red << "FEM_ObjectBroker::" << __FUNCTION__
		  << "; no NodalLoad type exists for class tag "
		  << classTag
		  << Color::def << std::endl;
        return nullptr;
      }
  }


//! @brief Broke a ElementalLoad object from its class tag.
XC::ElementalLoad *XC::FEM_ObjectBroker::getNewElementalLoad(int classTag)
  {
    switch(classTag)
      {
      case LOAD_TAG_Beam2dUniformLoad:
        return new Beam2dUniformLoad();
      case LOAD_TAG_Beam2dPartialUniformLoad:
        return new Beam2dPartialUniformLoad();
      case LOAD_TAG_Beam2dPointLoad:
        return new Beam2dPointLoad();
      case LOAD_TAG_Beam3dUniformLoad:
        return new Beam3dUniformLoad();
      case LOAD_TAG_Beam3dPointLoad:
        return new Beam3dPointLoad();
      case LOAD_TAG_BeamStrainLoad:
        return new BeamStrainLoad();
      case LOAD_TAG_TrussStrainLoad:
        return new TrussStrainLoad();
      case LOAD_TAG_BidimStrainLoad:
        return new BidimStrainLoad();
      case LOAD_TAG_ThreedimStrainLoad:
        return new ThreedimStrainLoad();
      case LOAD_TAG_QuadRawLoad:
        return new QuadRawLoad();
      case LOAD_TAG_ShellRawLoad:
        return new ShellRawLoad();
      case LOAD_TAG_ShellUniformLoad:
        return new ShellUniformLoad();
      case LOAD_TAG_ShellStrainLoad:
        return new ShellStrainLoad();
      case LOAD_TAG_BrickSelfWeight:
        return new BrickSelfWeight();
      case LOAD_TAG_SurfaceLoad:
	return new SurfaceLoad();
      default:
        std::cerr << Color::red << "FEM_ObjectBroker::" << __FUNCTION__
		  << "; no ElementalLoad type exists for class tag "
		  << classTag
		  << Color::def << std::endl;
        return 0;
      }
    return nullptr;
  }

//! @brief Broke a CrdTransf2d object from its class tag.
XC::CrdTransf2d *XC::FEM_ObjectBroker::getNewCrdTransf2d(int classTag)
  {
    switch(classTag)
      {
      case CRDTR_TAG_LinearCrdTransf2d:
        return new LinearCrdTransf2d();
      case CRDTR_TAG_PDeltaCrdTransf2d:
        return new PDeltaCrdTransf2d();
      case CRDTR_TAG_CorotCrdTransf2d:
        return new CorotCrdTransf2d();
      default:
         std::cerr << Color::red << "FEM_ObjectBroker::" << __FUNCTION__
                   << "; no XC::CrdTransf2d type exists for class tag "
                   << classTag
		   << Color::def << std::endl;
         return nullptr;
      }
  }

//! @brief Broke a CrdTransf3d object from its class tag.
XC::CrdTransf3d *XC::FEM_ObjectBroker::getNewCrdTransf3d(int classTag)
  {
    switch(classTag)
      {
      case CRDTR_TAG_LinearCrdTransf3d:
        return new LinearCrdTransf3d();
      case CRDTR_TAG_PDeltaCrdTransf3d:
        return new PDeltaCrdTransf3d();
      case CRDTR_TAG_CorotCrdTransf3d:
        return new CorotCrdTransf3d();
      default:
        std::cerr << Color::red << "FEM_ObjectBroker::" << __FUNCTION__
		  << "; no XC::CrdTransf3d type exists for class tag "
                  << classTag
		  << Color::def << std::endl;
        return nullptr;
      }
  }

//! @brief Broke a CrdTransf object from its class tag.
XC::CrdTransf *XC::FEM_ObjectBroker::getNewCrdTransf(int classTag)
  {    
    CrdTransf *trf= nullptr;
    if(classTag<10)
      trf= getNewCrdTransf2d(classTag);
    else
      trf= getNewCrdTransf3d(classTag);
    return trf;
  }

//! @brief Broke a ShellCrdTransf3dBase object from its class tag.
XC::ShellCrdTransf3dBase *XC::FEM_ObjectBroker::getNewShellCrdTransf3d(int classTag)
  {
    switch(classTag)
      {
      case CRDTR_TAG_ShellLinearCrdTransf3d:
        return new ShellLinearCrdTransf3d();
      default:
        std::cerr << Color::red << "FEM_ObjectBroker::" << __FUNCTION__
		  << "; no CrdTransf3d type exists for class tag "
                  << classTag
		  << Color::def << std::endl;
        return nullptr;
      }
  }

//! @brief Return a beam integration scheme from the tag argument.
XC::BeamIntegration *XC::FEM_ObjectBroker::getNewBeamIntegration(int classTag)
  {
    switch(classTag)
      {
      case BEAM_INTEGRATION_TAG_Lobatto:
	return new LobattoBeamIntegration();

      case BEAM_INTEGRATION_TAG_Legendre:
	return new LegendreBeamIntegration();

      case BEAM_INTEGRATION_TAG_Radau:
	  return new RadauBeamIntegration();

      case BEAM_INTEGRATION_TAG_NewtonCotes:
	return new NewtonCotesBeamIntegration();

      case BEAM_INTEGRATION_TAG_Trapezoidal:        
        return new TrapezoidalBeamIntegration();

      case BEAM_INTEGRATION_TAG_UserDefined:        
        return new UserDefinedBeamIntegration();
	
      case BEAM_INTEGRATION_TAG_FixedLocation:        
        return new FixedLocationBeamIntegration();
	
      case BEAM_INTEGRATION_TAG_LowOrder:        
        return new LowOrderBeamIntegration();

      case BEAM_INTEGRATION_TAG_MidDistance:        
        return new MidDistanceBeamIntegration();
	
      case BEAM_INTEGRATION_TAG_CompositeSimpson:        
        return new CompositeSimpsonBeamIntegration();

      case BEAM_INTEGRATION_TAG_HingeMidpoint:
	return new HingeMidpointBeamIntegration();

      case BEAM_INTEGRATION_TAG_HingeRadau:
	return new HingeRadauBeamIntegration();

      case BEAM_INTEGRATION_TAG_HingeRadauTwo:
	return new HingeRadauTwoBeamIntegration();

      case BEAM_INTEGRATION_TAG_HingeEndpoint:
	return new HingeEndpointBeamIntegration();

      case BEAM_INTEGRATION_TAG_UserHinge:
	return new UserDefinedHingeIntegration();

      case BEAM_INTEGRATION_TAG_DistHinge:
	return new DistHingeIntegration();

      // case BEAM_INTEGRATION_TAG_RegularizedHinge:
      // 	return new RegularizedHingeIntegration();

      default:
	std::cerr << Color::red << "FEM_ObjectBroker::" << __FUNCTION__
		  << " - no BeamIntegration type exists for class tag "
		  << classTag
		  << Color::def << std::endl;
	return nullptr;
      }
  }


//! @brief Broke a UniaxialMaterial object from its class tag.
XC::UniaxialMaterial *XC::FEM_ObjectBroker::getNewUniaxialMaterial(int classTag)
  {
    switch(classTag)
      {
      case MAT_TAG_ElasticMaterial:
        return new ElasticMaterial(); // values set in recvSelf
//         case MAT_TAG_Elastic2Material:
//              return new Elastic2Material();
      case MAT_TAG_ElasticPPMaterial:
        return new ElasticPPMaterial(); // values set in recvSelf
      case MAT_TAG_ParallelMaterial:
        return new ParallelMaterial();
      case MAT_TAG_Concrete01:
        return new Concrete01();
      case MAT_TAG_Concrete02:
        return new Concrete02();
      case MAT_TAG_Concrete04:
        return new Concrete04();
      case MAT_TAG_TDConcrete:
        return new TDConcrete();
      case MAT_TAG_TDConcreteMC10:
        return new TDConcreteMC10();
      case MAT_TAG_TDConcreteMC10NL:
        return new TDConcreteMC10NL();
//         case MAT_TAG_Concrete06:
//              return new Concrete06();
      case MAT_TAG_Steel01:
        return new Steel01();
      case MAT_TAG_Steel02:
        return new Steel02();
      case MAT_TAG_Hardening:
        return new HardeningMaterial();

      //PY springs: RWBoulanger and BJeremic
      case MAT_TAG_PySimple1:
        return new PySimple1();
      case MAT_TAG_PyLiq1:
        return new PyLiq1();
      case MAT_TAG_TzSimple1:
        return new TzSimple1();
      case MAT_TAG_TzLiq1:
        return new TzLiq1();
      case MAT_TAG_QzSimple1:
        return new QzSimple1();
      case MAT_TAG_EyBasic:
        return new EyBasic();

//         case MAT_TAG_PySimple2:
//                 return new PySimple2();

//         case MAT_TAG_TzSimple2:
//                 return new TzSimple2();
//         case MAT_TAG_QzSimple2:
//                 return new QzSimple2();


      case MAT_TAG_Hysteretic:
        return new HystereticMaterial();
      case MAT_TAG_EPPGap:
        return new EPPGapMaterial();

      case MAT_TAG_Viscous:
        return new ViscousMaterial();
      case MAT_TAG_PathIndependent:
        return new PathIndependentMaterial();
      case MAT_TAG_SeriesMaterial:
        return new SeriesMaterial();

      case MAT_TAG_CableMaterial:
        return new CableMaterial();
      case MAT_TAG_ENTMaterial:
        return new ENTMaterial();
      case MAT_TAG_ENCMaterial:
        return new ENCMaterial();

      case MAT_TAG_FedeasBond1:
        return new FedeasBond1Material();
      case MAT_TAG_FedeasBond2:
        return new FedeasBond2Material();
      case MAT_TAG_FedeasConcrete1:
        return new FedeasConcr1Material();
      case MAT_TAG_FedeasConcrete2:
        return new FedeasConcr2Material();
      case MAT_TAG_FedeasConcrete3:
        return new FedeasConcr3Material();
      case MAT_TAG_FedeasHardening:
        return new FedeasHardeningMaterial();
      case MAT_TAG_FedeasHysteretic1:
        return new FedeasHyster1Material();
      case MAT_TAG_FedeasHysteretic2:
        return new FedeasHyster2Material();
      case MAT_TAG_FedeasSteel1:
        return new FedeasSteel1Material();
      case MAT_TAG_FedeasSteel2:
        return new FedeasSteel2Material();

//         case MAT_TAG_DrainBilinear:
//                 return new DrainBilinearMaterial();

//         case MAT_TAG_Bilin:
//                 return new Bilin();

      case MAT_TAG_DrainClough1:
        return new DrainClough1Material();
      case MAT_TAG_DrainClough2:
        return new DrainClough2Material();
      case MAT_TAG_DrainPinch1:
        return new DrainPinch1Material();
      case MAT_TAG_MinMax:
        return new MinMaxMaterial();
      default:
//         UniaxialPackage *matCommands = theUniaxialPackage;
//         bool found = false;
//         while(matCommands != nullptr && found == false)
//           {
//             if((matCommands->classTag == classTag) && (matCommands->funcPtr != 0))
//               {
//                 UniaxialMaterial *result = (*(matCommands->funcPtr))();
//                 return result;
//               }
//             matCommands = matCommands->next;
//           }
        std::cerr << Color::red << "FEM_ObjectBroker::" << __FUNCTION__
		  << "; no UniaxialMaterial type exists for class tag "
                  << classTag
		  << Color::def << std::endl;
        return nullptr;
      }
  }

//! @brief Broke a SectionForceDeformation object from its class tag.
XC::SectionForceDeformation *XC::FEM_ObjectBroker::getNewSection(int classTag)
  {
    SectionForceDeformation *retval= nullptr;
    switch(classTag)
      {
      case SEC_TAG_Elastic1d:
	retval= new ElasticSection1d();
	break;
      case SEC_TAG_Elastic2d:
	retval= new ElasticSection2d();
	break;
      case SEC_TAG_Elastic3d:
	retval= new ElasticSection3d();
	break;
      case SEC_TAG_Generic1d:
	retval= new GenericSection1d();
	break;
      //case SEC_TAG_GenericNd:
	//retval= new GenericSectionNd();

      case SEC_TAG_Aggregator:
	retval= new SectionAggregator();
	break;
      //case SEC_TAG_Fiber:
      //retval= new FiberSection();

      case SEC_TAG_FiberSection2d:
	retval= new FiberSection2d();
	break;
      case SEC_TAG_FiberSection3d:
	retval= new FiberSection3d();
	break;
      case SEC_TAG_ElasticPlateSection:
	retval= new ElasticPlateSection();
	break;
      case SEC_TAG_ElasticMembranePlateSection:
	retval= new ElasticMembranePlateSection();
	break;
      case SEC_TAG_MembranePlateFiberSection:
	retval= new MembranePlateFiberSection();
	break;
      case SEC_TAG_LayeredShellFiberSection:
	retval= new LayeredShellFiberSection();
	break;
      case SEC_TAG_Bidirectional:
	retval= new Bidirectional();
	break;
      default:
	   std::cerr << Color::red << "FEM_ObjectBroker::" << __FUNCTION__
		     << " - no section type exists for class tag "
		     << classTag
		     << Color::def << std::endl;
	   retval= nullptr;
      }
    return retval;
  }

//! @brief Broke a NDMaterial object from its class tag.
XC::NDMaterial *XC::FEM_ObjectBroker::getNewNDMaterial(int classTag)
  {
  switch(classTag)
    {
    case ND_TAG_ElasticIsotropicPlaneStrain2d:
      return new ElasticIsotropicPlaneStrain2D();
    case ND_TAG_ElasticIsotropicPlaneStress2d:
      return new ElasticIsotropicPlaneStress2D();
    case ND_TAG_ElasticIsotropicAxiSymm:
      return new ElasticIsotropicAxiSymm();
    case ND_TAG_ElasticIsotropicPlateFiber:
      return new ElasticIsotropicPlateFiber();
    case ND_TAG_ElasticIsotropic3D:
      return new ElasticIsotropic3D();
    case ND_TAG_J2PlaneStrain:
      return new J2PlaneStrain();
    case ND_TAG_J2PlaneStress:
      return new J2PlaneStress();
    case ND_TAG_J2AxiSymm:
      return new J2AxiSymm();
    case ND_TAG_J2PlateFiber:
      return new J2PlateFiber();
    case ND_TAG_J2PlateFibre:
      return new J2PlateFibre();
    case ND_TAG_J2ThreeDimensional:
      return new J2ThreeDimensional();
    case ND_TAG_PlaneStressMaterial:
      return new PlaneStressMaterial();
    case ND_TAG_PlateFiberMaterial:
      return new PlateFiberMaterial();
    case ND_TAG_FluidSolidPorousMaterial:
      return new FluidSolidPorousMaterial();
    case ND_TAG_PressureDependMultiYield:
      return new PressureDependMultiYield();
    case ND_TAG_PressureIndependMultiYield:
      return new PressureIndependMultiYield();
  //   case ND_TAG_FeapMaterial03:
  //     return new FeapMaterial03();
    default:
      std::cerr << Color::red << "FEM_ObjectBroker::" << __FUNCTION__
		<< "; no XC::NDMaterial type exists for class tag "
                << classTag
		<< Color::def << std::endl;
      return nullptr;
    }
  }

//! @brief Broke a Material object from its class tag.
XC::Material *XC::FEM_ObjectBroker::getNewMaterial(int classTag)
  {
    Material *mat= nullptr;
    if(classTag<1000)
      mat= getNewUniaxialMaterial(classTag);
    else if(classTag<2000)
      mat= getNewSection(classTag);
    else if(classTag<3000)
      mat= getNewNDMaterial(classTag);
    return mat;
  }

//! @brief Broke a Damping object from its class tag.
XC::Damping *XC::FEM_ObjectBroker::getNewDamping(int classTag)
  {
    Damping *retval= nullptr;
    switch(classTag)
      {
      case DMP_TAG_UniformDamping:
        retval= new UniformDamping();
	break;
      case DMP_TAG_SecStifDamping:
        retval= new SecStifDamping();
	break;
      case DMP_TAG_URDDamping:
	retval= new URDDamping();
	break;
      case DMP_TAG_URDDampingbeta:
	retval= new URDDampingbeta();
	break;
      default:
        std::cerr << Color::red << "FEM_ObjectBroker::" << __FUNCTION__
		  << "; no Damping type exists for class tag "
                  << classTag
		  << Color::def << std::endl;
        retval= nullptr;
      }

    return retval;
  }

//! @brief Broke a Fiber object from its class tag.
XC::Fiber *XC::FEM_ObjectBroker::getNewFiber(int classTag)
  {
    switch(classTag)
      {
      case FIBER_TAG_Uniaxial2d:
        return new UniaxialFiber2d();
      case FIBER_TAG_Uniaxial3d:
        return new UniaxialFiber3d();
      default:
        std::cerr << Color::red << "FEM_ObjectBroker::" << __FUNCTION__
		  << "; no XC::Fiber type exists for class tag "
                  << classTag
		  << Color::def << std::endl;
        return nullptr;
      }
  }

//! @brief Broke a FrictionModel object from its class tag.
XC::FrictionModel *XC::FEM_ObjectBroker::getNewFrictionModel(int classTag)
  {
    switch(classTag)
      {
      case FRN_TAG_CoulombFriction:
        return new CoulombFriction();
      case FRN_TAG_VDependentFriction:
        return new VDependentFriction();
      case FRN_TAG_VPDependentFriction:
        return new VPDependentFriction();
      default:
        std::cerr << Color::red << "FEM_ObjectBroker::" << __FUNCTION__
		  << "; no FrictionModel type exists for class tag "
                  << classTag
		  << Color::def << std::endl;
        return 0;
      }
  }

//! @brief Broke a Convergence test object from its class tag.
XC::ConvergenceTest *XC::FEM_ObjectBroker::getNewConvergenceTest(CommandEntity *owr,int classTag)
  {
    switch(classTag)
      {
      case CONVERGENCE_TEST_CTestNormUnbalance:
        return new CTestNormUnbalance(owr);
      case CONVERGENCE_TEST_CTestNormDispIncr:
        return new CTestNormDispIncr(owr);
      case CONVERGENCE_TEST_CTestEnergyIncr:
        return new CTestEnergyIncr(owr);
      default:
        std::cerr << Color::red << "FEM_ObjectBroker::" << __FUNCTION__
		  << "; no XC::ConvergenceTest type exists for class tag "
                  << classTag
		  << Color::def << std::endl;
        return nullptr;
      }
  }

//! @brief Broke a NodeLocker object from its class tag.
XC::NodeLocker *XC::FEM_ObjectBroker::getNewNodeLocker(int classTag)
  {
    switch(classTag)
      {
      case PATTERN_TAG_NodeLocker:
        return new NodeLocker();
      default:
        std::cerr << Color::red << "FEM_ObjectBroker::" << __FUNCTION__
         << "; no Load type exists for class tag "
                  << classTag
		  << Color::def << std::endl;
        return nullptr;
      }
  }

//! @brief Broke a LoadPattern object from its class tag.
XC::LoadPattern *XC::FEM_ObjectBroker::getNewLoadPattern(int classTag)
  {
    switch(classTag)
      {
      case PATTERN_TAG_LoadPattern:
        return new LoadPattern();
      case PATTERN_TAG_UniformExcitation:
        return new UniformExcitation();
      case PATTERN_TAG_MultiSupportPattern:
        return new MultiSupportPattern();
      default:
        std::cerr << Color::red << "FEM_ObjectBroker::" << __FUNCTION__
		  << "; no Load type exists for class tag "
                  << classTag
		  << Color::def << std::endl;
        return nullptr;
      }
  }

//! @brief Broke a LoadCombination object from its class tag.
XC::LoadCombination *XC::FEM_ObjectBroker::getNewLoadCombination(int classTag)
  {
    switch(classTag)
      {
      case LOAD_TAG_LoadCombination:
        return new LoadCombination();
      default:
        std::cerr << Color::red << "FEM_ObjectBroker::" << __FUNCTION__
		  << "; no load type exists for class tag "
                  << classTag
		  << Color::def << std::endl;
        return nullptr;
      }
  }

//! @brief Broke a ground motion object from its class tag.
XC::GroundMotion *XC::FEM_ObjectBroker::getNewGroundMotion(int classTag)
  {
    switch(classTag)
      {
        case GROUND_MOTION_TAG_GroundMotionRecord:
          return new GroundMotionRecord();
        case GROUND_MOTION_TAG_InterpolatedGroundMotion:
          return new InterpolatedGroundMotion();
        default:
          std::cerr << Color::red << "FEM_ObjectBroker::" << __FUNCTION__
           << "; no ground motion type exists for class tag "
                    << classTag
		    << Color::def << std::endl;
          return nullptr;

      }
  }

//! @brief Broke a time series object from its class tag.
XC::TimeSeries *XC::FEM_ObjectBroker::getNewTimeSeries(int classTag)
  {
    switch(classTag)
      {
        case TSERIES_TAG_LinearSeries:
          return new LinearSeries;
        case TSERIES_TAG_RectangularSeries:
          return new RectangularSeries;
        case TSERIES_TAG_PathTimeSeries:
          return new PathTimeSeries;
        case TSERIES_TAG_PathSeries:
          return new PathSeries;
        case TSERIES_TAG_ConstantSeries:
          return new ConstantSeries;
        case TSERIES_TAG_TrigSeries:
          return new TrigSeries;
        default:
             std::cerr << Color::red << "FEM_ObjectBroker::" << __FUNCTION__
		       << "; no time series type exists for class tag "
                       << classTag
		       << Color::def << std::endl;
             return nullptr;
      }
  }

//! @brief Broke a time series object from its class tag.
XC::TimeSeriesIntegrator *XC::FEM_ObjectBroker::getNewTimeSeriesIntegrator(int classTag)
  {
    switch(classTag)
      {
      case TIMESERIES_INTEGRATOR_TAG_Trapezoidal:
          return new TrapezoidalTimeSeriesIntegrator();
      default:
             std::cerr << Color::red << "FEM_ObjectBroker::" << __FUNCTION__
		       << "; no time series integrator exists for class tag "
                       << classTag
		       << Color::def << std::endl;
             return nullptr;

         }
  }


//! @brief Broke a matrix object pointer from its class tag and its size.
XC::Matrix *XC::FEM_ObjectBroker::getPtrNewMatrix(int classTag, int noRows, int noCols)
  {
    switch(classTag)
      {
        case MATRIX_TAG_Matrix:
             return new Matrix(noRows,noCols);
        default:
             std::cerr << Color::red << "FEM_ObjectBroker::" << __FUNCTION__
		       << "; no matrix type exists for class tag "
                       << classTag
		       << Color::def << std::endl;
             return nullptr;

         }
  }


//! @brief Broke a vector object pointer from its class tag and its size.
XC::Vector *XC::FEM_ObjectBroker::getPtrNewVector(int classTag, int size)
  {
    switch(classTag) {
        case VECTOR_TAG_Vector:
             return new Vector(size);


        default:
             std::cerr << Color::red << "FEM_ObjectBroker::" << __FUNCTION__
		       << "; no XC::Vector type exists for class tag "
                       << classTag
		       << Color::def << std::endl;
             return nullptr;

         }
 }


//! @brief Broke an ID object pointer from its class tag and its size.
XC::ID *XC::FEM_ObjectBroker::getPtrNewID(int classTag, int size)
  {
    switch(classTag)
      {
        case ID_TAG_ID:
             return new ID(size);


        default:
             std::cerr << Color::red << "FEM_ObjectBroker::" << __FUNCTION__
		       << "; no XC::ID type exists for class tag "
                       << classTag
		       << Color::def << std::endl;
             return nullptr;

         }
  }

/*****************************************
 *
 * METHODS TO GET NEW OUTPUT CLASS OBJECTS
 *
 *****************************************/

//! @brief Broke a DataOutputHandler object from its class tag.
XC::DataOutputHandler *XC::FEM_ObjectBroker::getPtrNewDataOutputHandler(int classTag)
  {
    switch(classTag) {
        case DATAHANDLER_TAGS_DataOutputStreamHandler:
             return new DataOutputStreamHandler();

        case DATAHANDLER_TAGS_DataOutputFileHandler:
             return new DataOutputFileHandler();

        case DATAHANDLER_TAGS_DataOutputDatabaseHandler:
             return new DataOutputDatabaseHandler();

        default:
             std::cerr << Color::red << "FEM_ObjectBroker::" << __FUNCTION__
		       << "; no XC::DataOutputHandler type exists for class tag "
                       << classTag
		       << Color::def << std::endl;
             return nullptr;

         }
  }

//! @brief Broke a recorde object from its class tag.
XC::Recorder *XC::FEM_ObjectBroker::getPtrNewRecorder(int classTag)
  {
    switch(classTag)
     {
        case RECORDER_TAGS_ElementRecorder:
             return new ElementRecorder();

        case RECORDER_TAGS_NodeRecorder:
             return new NodeRecorder();

        case RECORDER_TAGS_EnvelopeNodeRecorder:
             return new EnvelopeNodeRecorder();

        case RECORDER_TAGS_EnvelopeElementRecorder:
             return new EnvelopeElementRecorder();
        case RECORDER_TAGS_TclFeViewer:
          return 0;
  //           return new TclFeViewer();
        default:
             std::cerr << Color::red << "FEM_ObjectBroker::" << __FUNCTION__
		       << "; no Recorder type exists for class tag "
                       << classTag
		       << Color::def << std::endl;
             return nullptr;

     }
  }



/*****************************************
 *
 * METHODS TO GET NEW ANALYSIS CLASSES
 *
 *****************************************/

//! @brief Broke a ConstraintHandler object from its class tag.
XC::ConstraintHandler *XC::FEM_ObjectBroker::getNewConstraintHandler(int classTag)
  {
    switch(classTag)
     {
        case HANDLER_TAG_PlainHandler:
             return new PlainHandler(nullptr);

        case HANDLER_TAG_PenaltyConstraintHandler:
	  return new PenaltyConstraintHandler(nullptr,1.0e12, 1.0e12);

        case HANDLER_TAG_LagrangeConstraintHandler:
	  return new LagrangeConstraintHandler(nullptr,1.0, 1.0);

        case HANDLER_TAG_TransformationConstraintHandler:
             return new TransformationConstraintHandler(nullptr);

        default:
             std::cerr << Color::red << "FEM_ObjectBroker::" << __FUNCTION__
		       << "; no ConstraintHandler type exists for class tag "
                       << classTag
		       << Color::def << std::endl;
             return nullptr;

     }
  }


//! @brief Broke a DOF_Numberer object from its class tag.
XC::DOF_Numberer *XC::FEM_ObjectBroker::getNewNumberer(int classTag)
  {
    switch(classTag) {
        case NUMBERER_TAG_DOF_Numberer:
             return new DOF_Numberer(nullptr);


        case NUMBERER_TAG_PlainNumberer:
             return new PlainNumberer(nullptr);


#ifdef _PARALLEL_PROCESSING
        case NUMBERER_TAG_ParallelNumberer:
             return new ParallelNumberer(nullptr);
#endif

        default:
             std::cerr << Color::red << "FEM_ObjectBroker::" << __FUNCTION__
		       << "; no XC::ConstraintHandler type exists for class tag "
                       << classTag
		       << Color::def << std::endl;
             return nullptr;

         }
  }


//! @brief Broke a AnalysisModel object from its class tag.
XC::AnalysisModel *XC::FEM_ObjectBroker::getNewAnalysisModel(int classTag)
  {
    switch(classTag)
      {
      case AnaMODEL_TAGS_AnalysisModel:
        return new AnalysisModel(nullptr);

      default:
        std::cerr << Color::red << "FEM_ObjectBroker::" << __FUNCTION__
		  << "; no AnalysisModel type exists for class tag "
                  << classTag
		  << Color::def << std::endl;
        return nullptr;
      }
  }


//! @brief Broke a EquiSolnAlgo object from its class tag.
XC::EquiSolnAlgo *XC::FEM_ObjectBroker::getNewEquiSolnAlgo(int classTag)
  {
    switch(classTag) {
        case EquiALGORITHM_TAGS_Linear:
             return new Linear(nullptr);

        case EquiALGORITHM_TAGS_NewtonRaphson:
             return new NewtonRaphson(nullptr);

        case EquiALGORITHM_TAGS_NewtonLineSearch:
             return new NewtonLineSearch(nullptr);

        case EquiALGORITHM_TAGS_KrylovNewton:
             return new KrylovNewton(nullptr);

//         case EquiALGORITHM_TAGS_AcceleratedNewton:
//              return new AcceleratedNewton();

        case EquiALGORITHM_TAGS_ModifiedNewton:
             return new ModifiedNewton(nullptr);

        case EquiALGORITHM_TAGS_Broyden:
             return new Broyden(nullptr);

        default:
             std::cerr << Color::red << "FEM_ObjectBroker::" << __FUNCTION__
		       << "; no XC::EquiSolnAlgo type exists for class tag "
                       << classTag
		       << Color::def << std::endl;
             return nullptr;

         }
  }

//! @brief Broke a LineSearch object from its class tag.
XC::LineSearch *XC::FEM_ObjectBroker::getLineSearch(int classTag)
  {
    switch(classTag)
      {
    case LINESEARCH_TAGS_BisectionLineSearch:
      return new BisectionLineSearch();

    case LINESEARCH_TAGS_InitialInterpolatedLineSearch:
      return new InitialInterpolatedLineSearch();

    case  LINESEARCH_TAGS_RegulaFalsiLineSearch:
      return new RegulaFalsiLineSearch();

    case  LINESEARCH_TAGS_SecantLineSearch:
      return new SecantLineSearch();
    default:
      std::cerr << Color::red << "FEM_ObjectBroker::" << __FUNCTION__
		<< "; no EquiSolnAlgo type exists for class tag "
                << classTag
		<< Color::def << std::endl;
      return 0;
    }
  }

//! @brief Broke a DomainDecompAlgo object from its class tag.
XC::DomainDecompAlgo *XC::FEM_ObjectBroker::getNewDomainDecompAlgo(int classTag)
  {
    switch(classTag) {
        case DomDecompALGORITHM_TAGS_DomainDecompAlgo:
             return new DomainDecompAlgo(nullptr);

        default:
             std::cerr << Color::red << "FEM_ObjectBroker::" << __FUNCTION__
		       << "; no XC::DomainDecompAlgo type exists for class tag "
                       << classTag
		       << std::endl;
             return nullptr;

         }
  }


//! @brief Broke a StaticIntegrator object from its class tag.
XC::StaticIntegrator *XC::FEM_ObjectBroker::getNewStaticIntegrator(int classTag)
  {
    switch(classTag)
      {
        case INTEGRATOR_TAGS_LoadControl:
	  return new LoadControl(nullptr,1.0,1,1.0,.10); // must recvSelf

#ifdef _PARALLEL_PROCESSING
        case INTEGRATOR_TAGS_DistributedDisplacementControl:
             return new DistributedDisplacementControl(nullptr); // must recvSelf
#endif

        case INTEGRATOR_TAGS_ArcLength:
             return new ArcLength(nullptr,1.0);      // must recvSelf


        default:
	  std::cerr << Color::red << "FEM_ObjectBroker::" << __FUNCTION__
	            << "; no StaticIntegrator type exists for class tag "
		    << classTag
		    << Color::def << std::endl;
             return nullptr;

         }
  }


//! @brief Broke a TransientIntegrator object from its class tag.
XC::TransientIntegrator *XC::FEM_ObjectBroker::getNewTransientIntegrator(int classTag)
  {
    switch(classTag)
      {
        case INTEGRATOR_TAGS_Newmark:
          return new Newmark(nullptr);

//         case INTEGRATOR_TAGS_TRBDF2:
//              return new TRBDF2();

        case INTEGRATOR_TAGS_HHT:
          return new HHT(nullptr);

        case INTEGRATOR_TAGS_CentralDifference:
          return new CentralDifference(nullptr);      // must recvSelf

        case INTEGRATOR_TAGS_CentralDifferenceNoDamping:
          return new CentralDifferenceNoDamping(nullptr);      // must recvSelf

        case INTEGRATOR_TAGS_CentralDifferenceAlternative:
          return new CentralDifferenceAlternative(nullptr);      // must recvSelf

        default:
          std::cerr << Color::red << "FEM_ObjectBroker::" << __FUNCTION__
		    << "; no TransientIntegrator type exists for class tag "
                    << classTag
		    << Color::def << std::endl;
          return 0;
      }
  }


//! @brief Broke a IncrementalIntegrator object from its class tag.
XC::IncrementalIntegrator *XC::FEM_ObjectBroker::getNewIncrementalIntegrator(int classTag)
  {
    switch(classTag)
      {
      case INTEGRATOR_TAGS_LoadControl:
        return new LoadControl(nullptr,1.0,1,1.0,1.0); // must recvSelf
      case INTEGRATOR_TAGS_ArcLength:
        return new ArcLength(nullptr,1.0);      // must recvSelf
      case INTEGRATOR_TAGS_Newmark:
        return new Newmark(nullptr);

#ifdef _PARALLEL_PROCESSING
      case INTEGRATOR_TAGS_DistributedDisplacementControl:
        return new DistributedDisplacementControl(nullptr); // must recvSelf
#endif
      default:
        std::cerr << Color::red << "FEM_ObjectBroker::" << __FUNCTION__
		  << "; no IncrementalIntegrator type exists for class tag "
                  << classTag
		  << Color::def << std::endl;
        return nullptr;
      }
  }


XC::LinearSOESolver *XC::FEM_ObjectBroker::getNewLinearSolver(void)
  { return lastLinearSolver; }

//! @brief Broke a LinearSOE object from its class tag.
XC::LinearSOE *XC::FEM_ObjectBroker::getNewLinearSOE(int classTagSOE, int classTagSolver)
  {
    LinearSOE *theSOE= nullptr;
//    SlowLinearSOESolver *theSlowSolver= nullptr;
    FullGenLinSolver *theGenSolver= nullptr;
    BandGenLinSolver *theGenBandSolver= nullptr;
    BandSPDLinSolver *theBandSPDSolver= nullptr;
    ProfileSPDLinSolver *theProfileSPDSolver= nullptr;
    SuperLU *theSparseGenLinSolver= nullptr;

#ifdef _PETSC
    PetscSolver *thePetscSolver = 0;
#endif

#ifdef _PARALLEL_PROCESSING
    DistributedSuperLU *theDistributedSparseGenLinSolver= nullptr;
    DistributedDiagonalSolver *theDistributedDiagonalSolver= nullptr;
#endif

    /*
      case LinSOE_TAGS_SlowLinearSOE:
        if (classTagSolver == SOLVER_TAGS_SlowLinearSOESolver) {
            theSlowSolver = new SlowLinearSOESolver();
            theSOE = new SlowLinearSOE(*theSlowSolver);
            lastLinearSolver = theSlowSolver;
            return theSOE;
        } else {
            std::cerr << Color::red << "FEM_ObjectBroker::" << __FUNCTION__
             << "; no SlowLinearSOESolver type exists for class tag "
                      << classTagSolver 
		      << Color::def << std::endl;
            return nullptr;
        }

        */


    switch(classTagSOE) {
      case LinSOE_TAGS_FullGenLinSOE:

        if(classTagSolver == SOLVER_TAGS_FullGenLinLapackSolver)
          {
            theGenSolver = new FullGenLinLapackSolver();
            theSOE = new FullGenLinSOE(nullptr);
            theSOE->setSolver(theGenSolver);
            lastLinearSolver = theGenSolver;
            return theSOE;
          }
        else
          {
            std::cerr << Color::red << "FEM_ObjectBroker::" << __FUNCTION__
		      << "; no FullGenLinSOESolver type exists for class tag "
                      << classTagSolver
		      << Color::def << std::endl;
            return nullptr;
          }
      case LinSOE_TAGS_BandGenLinSOE:
          if(classTagSolver == SOLVER_TAGS_BandGenLinLapackSolver)
            {
              theGenBandSolver = new BandGenLinLapackSolver();
              theSOE = new BandGenLinSOE(nullptr);
              theSOE->setSolver(theGenBandSolver);
              lastLinearSolver = theGenBandSolver;
              return theSOE;
            }
          else
            {
              std::cerr << Color::red << "FEM_ObjectBroker::" << __FUNCTION__
			<< "; no XC::BandGenLinSolver type exists for class tag "
                        << classTagSolver
			<< Color::def << std::endl;
              return nullptr;
            }

        case LinSOE_TAGS_BandSPDLinSOE:

          if(classTagSolver == SOLVER_TAGS_BandSPDLinLapackSolver)
            {
              theBandSPDSolver = new BandSPDLinLapackSolver();
              theSOE= new BandSPDLinSOE(nullptr);
              theSOE->setSolver(theBandSPDSolver);
              lastLinearSolver = theBandSPDSolver;
              return theSOE;
            }
          else
            {
              std::cerr << Color::red << "FEM_ObjectBroker::" << __FUNCTION__
			<< "; no BandSPDLinSOESolver type exists for class tag "
                        << classTagSolver
			<< Color::def << std::endl;
              return nullptr;
            }

        case LinSOE_TAGS_ProfileSPDLinSOE:

          if(classTagSolver == SOLVER_TAGS_ProfileSPDLinDirectSolver)
            {
              theProfileSPDSolver = new ProfileSPDLinDirectSolver();
              theSOE= new ProfileSPDLinSOE(nullptr);
              theSOE->setSolver(theProfileSPDSolver);
              lastLinearSolver = theProfileSPDSolver;
              return theSOE;
            }
          else if(classTagSolver == SOLVER_TAGS_ProfileSPDLinSubstrSolver)
            {
              theProfileSPDSolver = new ProfileSPDLinSubstrSolver();
              theSOE = new ProfileSPDLinSOE(nullptr);
              theSOE->setSolver(theProfileSPDSolver);
              lastLinearSolver = theProfileSPDSolver;
              return nullptr;
            }
          else
	    {
	      std::cerr << Color::red << "FEM_ObjectBroker::" << __FUNCTION__
			<< "; no ProfileSPD_LinSolver type exists for class tag "
                        << classTagSolver
			<< Color::def << std::endl;
              return nullptr;
	    }


#ifdef _PETSC
      case LinSOE_TAGS_PetscSOE:

          if(classTagSolver == SOLVER_TAGS_PetscSolver)
            {
              thePetscSolver= new PetscSolver();
              theSOE= new PetscSOE(nullptr);
              theSOE->setSolver(thePetscSolver);
              return theSOE;
            }
          else
            {
              std::cerr << Color::red << "FEM_ObjectBroker::" << __FUNCTION__
			<< "; no PetscSolver type exists for class tag "
                        << classTagSolver
			<< Color::def << std::endl;
              return nullptr;
            }
#endif

#ifdef _PARALLEL_PROCESSING
      case LinSOE_TAGS_DistributedBandGenLinSOE:

          if(classTagSolver == SOLVER_TAGS_BandGenLinLapackSolver)
            {
              theGenBandSolver= new BandGenLinLapackSolver();
              theSOE= new DistributedBandGenLinSOE(nullptr);
              theSOE->setSolver(theGenBandSolver);
              lastLinearSolver = theGenBandSolver;
              return theSOE;
            }
          else
            {
              std::cerr << Color::red << "FEM_ObjectBroker::" << __FUNCTION__
			<< "; no DistributedBandGenLinSolver type exists for class tag "
                        << classTagSolver
			<< Color::def << std::endl;
              return nullptr;
            }

        case LinSOE_TAGS_DistributedBandSPDLinSOE:

          if(classTagSolver == SOLVER_TAGS_BandSPDLinLapackSolver)
            {
              theBandSPDSolver = new BandSPDLinLapackSolver();
              theSOE = new DistributedBandSPDLinSOE(nullptr);
              theSOE->setSolver(theBandSPDSolver);
              lastLinearSolver = theBandSPDSolver;
              return theSOE;
            }
          else
            {
              std::cerr << Color::red << "FEM_ObjectBroker::" << __FUNCTION__
			<< "; no DistributedBandSPDLinSolver type exists for class tag "
                        << classTagSolver
			<< Color::def << std::endl;
              return nullptr;
            }


        case LinSOE_TAGS_DistributedProfileSPDLinSOE:

          if(classTagSolver == SOLVER_TAGS_ProfileSPDLinDirectSolver)
            {
              theProfileSPDSolver = new ProfileSPDLinDirectSolver();
              theSOE = new DistributedProfileSPDLinSOE(nullptr);
              theSOE->setSolver(theProfileSPDSolver);
              lastLinearSolver = theProfileSPDSolver;
              return theSOE;
            }
          else if(classTagSolver == SOLVER_TAGS_ProfileSPDLinSubstrSolver)
            {
              theProfileSPDSolver = new ProfileSPDLinSubstrSolver();
              theSOE = new DistributedProfileSPDLinSOE(nullptr);
              theSOE->setSolver(theProfileSPDSolver);
              lastLinearSolver = theProfileSPDSolver;
              return nullptr;
            }
          else
            {
              std::cerr << Color::red << "FEM_ObjectBroker::" << __FUNCTION__
			<< "; no ProfileSPD_LinSolver type exists for class tag "
                        << classTagSolver
			<< Color::def << std::endl;
              return nullptr;
            }

        case LinSOE_TAGS_DistributedDiagonalSOE:

          if(classTagSolver == SOLVER_TAGS_DistributedDiagonalSolver)
            {
              theDistributedDiagonalSolver= new DistributedDiagonalSolver();
              theSOE= new DistributedDiagonalSOE(nullptr);
              theSOE->setSolver(theDistributedDiagonalSolver);
              lastLinearSolver = theDistributedDiagonalSolver;
              return theSOE;
            }
          else
            {
              std::cerr << Color::red << "FEM_ObjectBroker::" << __FUNCTION__
			<< "; no DistributedSparseGenLinSolverSolver type exists for class tag "
                        << classTagSolver
			<< Color::def << std::endl;
              return nullptr;
            }

        case LinSOE_TAGS_DistributedSparseGenColLinSOE:

          if(classTagSolver == SOLVER_TAGS_SuperLU)
            {
              theSparseGenLinSolver = new SuperLU();
              theSOE= new DistributedSparseGenColLinSOE(nullptr);
              theSOE->setSolver(theSparseGenLinSolver);
              lastLinearSolver = theSparseGenLinSolver;
              return theSOE;
            }
          else if(classTagSolver == SOLVER_TAGS_DistributedSuperLU)
            {
              theDistributedSparseGenLinSolver = new DistributedSuperLU();
              theSOE= new DistributedSparseGenColLinSOE(nullptr,theDistributedSparseGenLinSolver);
              theSOE->setSolver(theDistributedSparseGenLinSolver);
              lastLinearSolver = theSparseGenLinSolver;
              return theSOE;
            }
          else
            {
              std::cerr << Color::red << "FEM_ObjectBroker::" << __FUNCTION__
			<< "; no DistributedSparseGenLinSolverSolver type exists for class tag "
                        << classTagSolver
			<< Color::def << std::endl;
              return nullptr;
            }
#else
        case LinSOE_TAGS_SparseGenColLinSOE:

          if(classTagSolver == SOLVER_TAGS_SuperLU)
            {
              theSparseGenLinSolver = new SuperLU();
              theSOE = new SparseGenColLinSOE(nullptr);
              theSOE->setSolver(theSparseGenLinSolver);
              lastLinearSolver = theSparseGenLinSolver;
              return theSOE;
            }
          else
            {
              std::cerr << Color::red << "FEM_ObjectBroker::" << __FUNCTION__
			<< "; no SparseGenLinSolverSolver type exists for class tag "
                        << classTagSolver
			<< Color::def << std::endl;
              return nullptr;
            }

#endif

        default:
          std::cerr << Color::red << "FEM_ObjectBroker::" << __FUNCTION__
		    << "; no XC::LinearSOE type exists for class tag "
                    << classTagSOE << Color::def << std::endl;
          return nullptr;
    }
  }

//! @brief Broke an EigenSOE object from its class tag.
XC::EigenSOE *XC::FEM_ObjectBroker::getNewEigenSOE(int classTagSOE)
  {
    EigenSOE *theSOE= nullptr;
    switch(classTagSOE)
      {

        case EigenSOE_TAGS_BandArpackSOE:
          theSOE = new BandArpackSOE(nullptr);
          break;
        case EigenSOE_TAGS_SymArpackSOE:
          theSOE = new SymArpackSOE(nullptr);
          break;
        case EigenSOE_TAGS_SymBandEigenSOE:
          theSOE = new SymBandEigenSOE(nullptr);
          break;
        case EigenSOE_TAGS_BandArpackppSOE:
          theSOE = new BandArpackppSOE(nullptr);
          break;
#ifdef USE_SPECTRA
        case EigenSOE_TAGS_SpectraSOE:
          theSOE = new SpectraSOE(nullptr);
          break;
#endif
        case EigenSOE_TAGS_FullGenEigenSOE:
          theSOE = new BandArpackppSOE(nullptr);
          break;
        default:
          std::cerr << Color::red << "FEM_ObjectBroker::" << __FUNCTION__
		    << "; no EigenSOE type exists for class tag "
                    << classTagSOE
		    << Color::def << std::endl;
      }
    return theSOE;
  }

//! @brief Broke a DomainSolver object from its class tag.
XC::DomainSolver *XC::FEM_ObjectBroker::getNewDomainSolver(void)
  { return lastDomainSolver; }

//! @brief Broke a LinearSOE object from its class tag.
XC::LinearSOE *XC::FEM_ObjectBroker::getPtrNewDDLinearSOE(int classTagSOE, int classTagDDSolver)
  {
    ProfileSPDLinSubstrSolver *theProfileSPDSolver= nullptr;

    switch(classTagSOE) {
      case LinSOE_TAGS_ProfileSPDLinSOE:

        if (classTagDDSolver == SOLVER_TAGS_ProfileSPDLinSubstrSolver) {
            theProfileSPDSolver = new ProfileSPDLinSubstrSolver();
            LinearSOE *theSOE = new ProfileSPDLinSOE(nullptr);
            theSOE->setSolver(theProfileSPDSolver);
            lastDomainSolver = theProfileSPDSolver;
            return theSOE;
        }
        else {
            std::cerr << Color::red << "FEM_ObjectBroker::" << __FUNCTION__
		      << "; no ProfileSPD XC::Domain XC::Solver type exists for class tag "
                      << classTagDDSolver
		      << Color::def << std::endl;
            return nullptr;
        }


      default:
        std::cerr << Color::red << "FEM_ObjectBroker::" << __FUNCTION__
                  << "; no LinearSOE type exists for class tag "
                  << classTagSOE
		  << Color::def << std::endl;
        return nullptr;

    }
  }

//! @brief Broke a DomainDecompositionAnalysis object from its class tag.
XC::DomainDecompositionAnalysis *XC::FEM_ObjectBroker::getNewDomainDecompAnalysis(int classTag, Subdomain &theSubdomain)
  {
    switch(classTag) {
      case DomDecompANALYSIS_TAGS_DomainDecompositionAnalysis:
        return new DomainDecompositionAnalysis(theSubdomain);

#ifdef _PARALLEL_PROCESSING
      case ANALYSIS_TAGS_StaticDomainDecompositionAnalysis:
        return new StaticDomainDecompositionAnalysis(theSubdomain);

      case ANALYSIS_TAGS_TransientDomainDecompositionAnalysis:
        return new TransientDomainDecompositionAnalysis(theSubdomain);
#endif

      default:
        std::cerr << Color::red << "FEM_ObjectBroker::getNewDomainDecompAnalysis "
                  << "; no DomainDecompAnalysis type exists for class tag " 
                  << classTag
		  << Color::def << std::endl;
        return nullptr;

    }
  }

//! @brief Broke a Subdomain object from its class tag.
XC::Subdomain *XC::FEM_ObjectBroker::getSubdomainPtr(int classTag)
  {
    std::cerr << Color::red << "FEM_ObjectBroker::" << __FUNCTION__
	      << "; not implemented yet."
	      << Color::def << std::endl;
    return nullptr;
  }

//! @brief Broke a Parameter object from its class tag.
XC::Parameter *XC::FEM_ObjectBroker::getParameter(int classTag)
  {
    Parameter *retval= nullptr;

    switch(classTag)
      {
      case  PARAMETER_TAG_Parameter:
	retval = new Parameter;
	break;
    //case PARAMETER_TAG_MaterialStageParameter:
    //  retval = new MaterialStageParameter();
    //  break;
      case PARAMETER_TAG_MatParameter:
	retval = new MatParameter();
	break;
      default:
	;
      }

    return retval;
  }

int XC::FEM_ObjectBroker::addUniaxialMaterial(int classTag, const std::string &lib, const std::string &funcName, UniaxialMaterial *(*funcPtr)(void))
  {
    // check to see if it's already added

    UniaxialPackage *matCommands = theUniaxialPackage;
    bool found = false;
    while(matCommands != nullptr && found == false)
      {
        if((lib == matCommands->libName) && (funcName == matCommands->funcName))
         { return 0; }
      }

    //
    // if funPtr == 0; go get the handle
    //

    void *libHandle;
    if(funcPtr == 0)
      {
        if(getLibraryFunction(lib, funcName, &libHandle, (void **)&funcPtr) != 0)
          {
            std::cerr << Color::red << "FEM_ObjectBroker::" << __FUNCTION__
	              << "; could not find function: " << funcName
	              << Color::def << std::endl;
            return -1;
          }
      }

    //
    // add the new funcPtr
    //

    std::string libNameCopy= lib;
    std::string funcNameCopy= funcName;
    UniaxialPackage *theMat= new UniaxialPackage;
    if(theMat == nullptr)
      {
        std::cerr << Color::red << "FEM_ObjectBroker::" << __FUNCTION__
		  << "; could not add lib, out of memory."
		  << Color::def << std::endl;
        return -1;
      }
    theMat->classTag= classTag;
    theMat->funcName= funcNameCopy;
    theMat->libName= libNameCopy;
    theMat->funcPtr= funcPtr;
    theMat->next= theUniaxialPackage;
    theUniaxialPackage = theMat;
    return 0;
  }


//! @brief Return a Python dictionary with the object members values.
boost::python::dict XC::FEM_ObjectBroker::getPyDict(void) const
  {
    boost::python::dict retval;
    // Nothing to keep.
    return retval;
  }

//! @brief Set the values of the object members from a Python dictionary.
void XC::FEM_ObjectBroker::setPyDict(const boost::python::dict &d)
  {
    // Nothing to read.
  }


const std::vector<std::string> crd_transf_class_identifiers{"XC::LinearCrdTransf2d"};

//! @brief Creates a new tagged object whose class is determined by the given class name and class tag.
//! @param className: the class name is used to determine the adequate broker method.
//! @param classTag: the class tag defines the specific type of the object (see classTags.h).
XC::TaggedObject *XC::get_new_tagged_object(const std::string &className, const int &classTag)
  {
    static FEM_ObjectBroker broker;
    TaggedObject *retval= nullptr;
    if(XC::is_nd_material(className))
      retval= broker.getNewNDMaterial(classTag);
    else if(XC::is_uniaxial_material(className))
      retval= broker.getNewUniaxialMaterial(classTag);
    else if(XC::is_section_material(className))
      retval= broker.getNewSection(classTag);
    else if(XC::is_node(className))
      retval= broker.getNewNode(classTag);
    else if(XC::is_coordinate_transformation(className))
      retval= broker.getNewCrdTransf(classTag);
    else if(XC::is_element(className))
      retval= broker.getNewElement(classTag);
    else if(XC::is_sp_constraint(className))
      retval= broker.getNewSP(classTag);
    else if(XC::is_mp_constraint(className))
      retval= broker.getNewMP(classTag);
    else if(XC::is_mrmp_constraint(className))
      retval= broker.getNewMRMP(classTag);
    else if(XC::is_load_pattern(className))
      retval= broker.getNewLoadPattern(classTag);
    else if(XC::is_nodal_load(className))
      retval= broker.getNewNodalLoad(classTag);
    else if(XC::is_elemental_load(className))
      retval= broker.getNewElementalLoad(classTag);
    if(!retval)
      std::cerr << Color::red << "FEM_ObjectBroker::" << __FUNCTION__
	        << "; no " << className
	        << " type exists for class tag: " << classTag
	        << Color::def << std::endl;
    return retval;
  }
//! @brief Creates a new movable object whose class is determined by the given class name and class tag.
//! @param className: the class name is used to determine the adequate broker method.
//! @param classTag: the class tag defines the specific type of the object (see classTags.h).
XC::MovableObject *XC::get_new_movable_object(const std::string &className, const int &classTag)
  {
    static FEM_ObjectBroker broker;
    MovableObject *retval= nullptr;
    if(XC::is_ground_motion(className))
      retval= broker.getNewGroundMotion(classTag);
    else if(XC::is_time_series(className))
      retval= broker.getNewTimeSeries(classTag);
    if(!retval)
      std::cerr << Color::red << "FEM_ObjectBroker::" << __FUNCTION__
	        << "; no " << className
	        << " type exists for class tag: " << classTag
	        << Color::def << std::endl;
    return retval;
  }

