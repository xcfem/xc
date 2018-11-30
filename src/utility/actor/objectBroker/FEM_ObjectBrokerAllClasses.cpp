//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
//  software es libre: usted puede redistribuirlo y/o modificarlo
//  bajo los términos de la Licencia Pública General GNU publicada
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO.
//  GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// junto a este programa.
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

// $Revision: 1.18 $
// $Date: 2010/06/10 18:53:09 $
// $Source: /usr/local/cvs/OpenSees/SRC/actor/objectBroker/FEM_ObjectBrokerAllClasses.cpp,v $

// Written: fmk
// Revision: A
//
// Purpose: This file contains the class definition for FEM_ObjectBrokerAllClasses.
// FEM_ObjectBrokerAllClasses is is an object broker class for the finite element
// method. All methods are virtual to allow for subclasses; which can be
// used by programmers when introducing new subclasses of the main objects.
//
// What: "@(#) FEM_ObjectBrokerAllClasses.C, revA"

#include "FEM_ObjectBrokerAllClasses.h"

//#include <mpi.h>
#include "all_includes.h"

//#include <FileStream.h>
//#include <StandardStream.h>
//#include <XmlFileStream.h>
//#include <DataFileStream.h>
//#include <BinaryFileStream.h>
//#include <DatabaseStream.h>
//#include <DummyStream.h>

// #include <packages.h>

// typedef struct uniaxialPackage {
//   int classTag;
//   char *libName;
//   char *funcName;
//   UniaxialMaterial *(*funcPtr)(void);
//   struct uniaxialPackage *next;
// } UniaxialPackage;

// static UniaxialPackage *theUniaxialPackage = nullptr;


//! @brief Default constructor.
XC::FEM_ObjectBrokerAllClasses::FEM_ObjectBrokerAllClasses(void)
  :lastDomainSolver(nullptr) {}

//! @brief Creates a new actor de la clase cuyo tag being passed as parameter.
XC::Actor *XC::FEM_ObjectBrokerAllClasses::getNewActor(int classTag, Channel *theChannel)
  {
    switch(classTag)
      {
#ifdef _PARALLEL_PROCESSING
      case ACTOR_TAGS_SUBDOMAIN:
        return new ActorSubdomain(*theChannel, *this);
#endif
      default:
        std::cerr << "FEM_ObjectBrokerAllClasses::getNewActor - ";
        std::cerr << " - no ActorType type exists for class tag ";
        std::cerr << classTag << std::endl;
      return 0;
      }
  }

//! @brief Creates a new PartitionedModelBuilder de la clase cuyo tag being passed as parameter.
XC::PartitionedModelBuilder *XC::FEM_ObjectBrokerAllClasses::getPtrNewPartitionedModelBuilder(Subdomain &theSubdomain,int classTag)
  {
    switch(classTag)
      {
        /*
        case PartitionedModelBuilder_TAGS_PartitionedQuick2dFrameModel:
             return new PartitionedQuick2dFrame(theSubdomain);
             */

      default:
        std::cerr << "FEM_ObjectBrokerAllClasses::getPtrNewPartitionedModelBuilder - ";
        std::cerr << " - no PartitionedModelBuilder type exists for class tag ";
        std::cerr << classTag << std::endl;
        return 0;

      }
  }



/*****************************************
 *
 * METHODS TO GET NEW MODELLING CLASSES
 *
 *****************************************/

XC::Element *XC::FEM_ObjectBrokerAllClasses::getNewElement(int classTag)
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
        case ELE_TAG_ElasticBeam3d:
             return new ElasticBeam3d();
        case ELE_TAG_ForceBeamColumn2d:
             return new ForceBeamColumn2d();
        case ELE_TAG_ForceBeamColumn3d:
             return new ForceBeamColumn3d();
        case ELE_TAG_DispBeamColumn2d:
             return new DispBeamColumn2d();
        case ELE_TAG_DispBeamColumn3d:
             return new DispBeamColumn3d();
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
            std::cerr << "FEM_ObjectBrokerAllClasses::getNewElement - ";
            std::cerr << " - no Element type exists for class tag " ;
            std::cerr << classTag << std::endl;
            return 0;
      }
  }

XC::Node *XC::FEM_ObjectBrokerAllClasses::getNewNode(int classTag)
  {
    switch(classTag)
      {
      case NOD_TAG_Node:
        return new Node(classTag);
      default:
        std::cerr << "FEM_ObjectBrokerAllClasses::getNewNode - ";
        std::cerr << " - no Node type exists for class tag ";
        std::cerr << classTag << std::endl;
        return 0;
      }
  }


XC::MFreedom_Constraint *XC::FEM_ObjectBrokerAllClasses::getNewMP(int classTag)
  {
    switch(classTag)
      {
      case CNSTRNT_TAG_MFreedom_Constraint:
        return new MFreedom_Constraint(classTag);

      case CNSTRNT_TAG_MFreedom_Joint2D:                        // Arash
        return new MFreedom_Joint2D();                        // Arash

      default:
        std::cerr << "FEM_ObjectBrokerAllClasses::getNewMP - ";
        std::cerr << " - no MFreedom_Constraint type exists for class tag ";
        std::cerr << classTag << std::endl;
        return 0;
      }
  }


XC::SFreedom_Constraint *XC::FEM_ObjectBrokerAllClasses::getNewSP(int classTag)
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
        std::cerr << "FEM_ObjectBrokerAllClasses::getNewSP - ";
        std::cerr << " - no SFreedom_Constraint type exists for class tag ";
        std::cerr << classTag << std::endl;
        return 0;
      }
  }

XC::NodalLoad *XC::FEM_ObjectBrokerAllClasses::getNewNodalLoad(int classTag)
  {
    switch(classTag)
      {
      case LOAD_TAG_NodalLoad:
        return new NodalLoad(classTag);
      default:
        std::cerr << "FEM_ObjectBrokerAllClasses::getNewNodalLoad - ";
        std::cerr << " - no NodalLoad type exists for class tag ";
        std::cerr << classTag << std::endl;
        return 0;
      }
  }

XC::SectionForceDeformation *XC::FEM_ObjectBrokerAllClasses::getNewSection(int classTag)
  {
    switch(classTag)
      {
      case SEC_TAG_Elastic2d:
        return new ElasticSection2d();
      case SEC_TAG_Elastic3d:
        return new ElasticSection3d();
      case SEC_TAG_Generic1d:
        return new GenericSection1d();

      //case SEC_TAG_GenericNd:
        //return new GenericSectionNd();

      case SEC_TAG_Aggregator:
             return new SectionAggregator();

             //case SEC_TAG_Fiber:
             //return new FiberSection();

        case SEC_TAG_FiberSection2d:
                return new FiberSection2d();

        case SEC_TAG_FiberSection3d:
                return new FiberSection3d();

        case SEC_TAG_ElasticPlateSection:
                return new ElasticPlateSection();

        case SEC_TAG_ElasticMembranePlateSection:
                return new ElasticMembranePlateSection();

        case SEC_TAG_MembranePlateFiberSection:
                return new MembranePlateFiberSection();

        case SEC_TAG_Bidirectional:
                return new Bidirectional();

        default:
             std::cerr << "FEM_ObjectBrokerAllClasses::getNewSection - ";
             std::cerr << " - no section type exists for class tag ";
             std::cerr << classTag << std::endl;
             return nullptr;

         }
}

XC::NDMaterial *XC::FEM_ObjectBrokerAllClasses::getNewNDMaterial(int classTag)
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
//    case ND_TAG_FeapMaterial03:
//      return new FeapMaterial03();
      default:
	std::cerr << "FEM_ObjectBrokerAllClasses::getNewNDMaterial - ";
	std::cerr << " - no NDMaterial type exists for class tag ";
	std::cerr << classTag << std::endl;
	return 0;
    }
  }

XC::ConvergenceTest *XC::FEM_ObjectBrokerAllClasses::getNewConvergenceTest(int classTag)
{
    switch(classTag) {
        case CONVERGENCE_TEST_CTestNormUnbalance:
             return new CTestNormUnbalance();

        case CONVERGENCE_TEST_CTestNormDispIncr:
             return new CTestNormDispIncr();

        case CONVERGENCE_TEST_CTestEnergyIncr:
             return new CTestEnergyIncr();


        default:
             std::cerr << "FEM_ObjectBrokerAllClasses::getNewConvergenceTest - ";
             std::cerr << " - no ConvergenceTest type exists for class tag ";
             std::cerr << classTag << std::endl;
             return 0;

         }
}


XC::GroundMotion *XC::FEM_ObjectBrokerAllClasses::getNewGroundMotion(int classTag)
  {
    switch(classTag)
      {

        case GROUND_MOTION_TAG_GroundMotionRecord:
          return new GroundMotionRecord();

        case GROUND_MOTION_TAG_InterpolatedGroundMotion:
          return new InterpolatedGroundMotion();

        default:
             std::cerr << "FEM_ObjectBrokerAllClasses::getPtrGroundMotion - ";
             std::cerr << " - no Load type exists for class tag ";
             std::cerr << classTag << std::endl;
             return 0;
      }
  }

XC::TimeSeries *XC::FEM_ObjectBrokerAllClasses::getNewTimeSeries(int classTag)
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
             std::cerr << "FEM_ObjectBrokerAllClasses::getPtrTimeSeries - ";
             std::cerr << " - no Load type exists for class tag ";
             std::cerr << classTag << std::endl;
             return 0;

         }
}

XC::TimeSeriesIntegrator *XC::FEM_ObjectBrokerAllClasses::getNewTimeSeriesIntegrator(int classTag)
  {
    switch(classTag) {
    case TIMESERIES_INTEGRATOR_TAG_Trapezoidal:
          return new TrapezoidalTimeSeriesIntegrator();

        default:
             std::cerr << "FEM_ObjectBrokerAllClasses::getPtrTimeSeriesIntegrator - ";
             std::cerr << " - no Load type exists for class tag ";
             std::cerr << classTag << std::endl;
             return 0;

         }
  }


XC::Matrix *XC::FEM_ObjectBrokerAllClasses::getPtrNewMatrix(int classTag, int noRows, int noCols)
  {
    switch(classTag) {
        case MATRIX_TAG_Matrix:
             return new Matrix(noRows,noCols);


        default:
             std::cerr << "FEM_ObjectBrokerAllClasses::getPtrNewMatrix - ";
             std::cerr << " - no NodalLoad type exists for class tag ";
             std::cerr << classTag << std::endl;
             return 0;

         }
  }


XC::Vector *XC::FEM_ObjectBrokerAllClasses::getPtrNewVector(int classTag, int size)
  {
    switch(classTag) {
        case VECTOR_TAG_Vector:
             return new Vector(size);


        default:
             std::cerr << "FEM_ObjectBrokerAllClasses::getPtrNewVector - ";
             std::cerr << " - no Vector type exists for class tag ";
             std::cerr << classTag << std::endl;
             return 0;

         }
  }


XC::ID *XC::FEM_ObjectBrokerAllClasses::getPtrNewID(int classTag, int size)
  {
    switch(classTag) {
        case ID_TAG_ID:
             return new ID(size);


        default:
             std::cerr << "FEM_ObjectBrokerAllClasses::getPtrNewID - ";
             std::cerr << " - no ID type exists for class tag ";
             std::cerr << classTag << std::endl;
             return 0;

         }
  }

/*****************************************
 *
 * METHODS TO GET NEW OUTPUT CLASS OBJECTS
 *
 *****************************************/

XC::Recorder *XC::FEM_ObjectBrokerAllClasses::getPtrNewRecorder(int classTag)
  {
    switch(classTag) {
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
             std::cerr << "FEM_ObjectBrokerAllClasses::getNewRecordr - ";
             std::cerr << " - no ConstraintHandler type exists for class tag ";
             std::cerr << classTag << std::endl;
             return 0;

         }
  }



/*****************************************
 *
 * METHODS TO GET NEW ANALYSIS CLASSES
 *
 *****************************************/

// XC::Accelerator *XC::FEM_ObjectBrokerAllClasses::getAccelerator(int classTag)
//   {
//     switch(classTag) {

//     case ACCELERATOR_TAGS_Krylov:
//       return new KrylovAccelerator;
//     case ACCELERATOR_TAGS_Raphson:
//       return new RaphsonAccelerator;

//     default:
//       std::cerr << "FEM_ObjectBrokerAllClasses::getAccelerator - ";
//       std::cerr << " - no EquiSolnAlgo type exists for class tag ";
//       std::cerr << classTag << std::endl;
//       return 0;

//     }
//   }









XC::DomainSolver *XC::FEM_ObjectBrokerAllClasses::getNewDomainSolver(void)
  { return lastDomainSolver; }

XC::Subdomain *XC::FEM_ObjectBrokerAllClasses::getSubdomainPtr(int classTag)
  {
    std::cerr << "FEM_ObjectBrokerAllClasses: NOT IMPLEMENTED YET";
    return 0;
  }


int XC::FEM_ObjectBrokerAllClasses::addUniaxialMaterial(int classTag,const char *lib,const char *funcName, UniaxialMaterial *(*funcPtr)(void))
  {
//     // check to see if it's already added

//     UniaxialPackage *matCommands = theUniaxialPackage;
//     bool found = false;
//     while (matCommands != nullptr && found == false)
//       {
//         if((strcmp(lib, matCommands->libName) == 0) && (strcmp(funcName, matCommands->funcName) == 0))
//           { return 0; }
//       }

//     //
//     // if funPtr == 0; go get the handle
//     //

//     void *libHandle;
//     if(funcPtr == nullptr)
//       {
//         if(getLibraryFunction(lib, funcName, &libHandle, (void **)&funcPtr) != 0)
//           {
//             std::cerr << "FEM_ObjectBrokerAllClasses::addUniaxialMaterial - could not find function\n";
//             return -1;
//           }
//       }

//     //
//     // add the new funcPtr
//     //

//     char *libNameCopy = new char[strlen(lib)+1];
//     char *funcNameCopy = new char[strlen(funcName)+1];
//     UniaxialPackage *theMat = new UniaxialPackage;
//     if(libNameCopy == 0 || funcNameCopy == 0 || theMat == 0)
//       {
//         std::cerr << "FEM_ObjectBrokerAllClasses::addUniaxialMaterial - could not add lib, out of memory\n";
//         return -1;
//       }
//     strcpy(libNameCopy, lib);
//     strcpy(funcNameCopy, funcName);

//     theMat->classTag = classTag;
//     theMat->funcName = funcNameCopy;
//     theMat->libName = libNameCopy;
//     theMat->funcPtr = funcPtr;
//     theMat->next = theUniaxialPackage;
//     theUniaxialPackage = theMat;
    std::cerr << "FEM_ObjectBrokerAllClasses::addUniaxialMaterial not implemented." << std::endl;
    return 0;
  }


XC::Parameter *XC::FEM_ObjectBrokerAllClasses::getParameter(int classTag)
{
  Parameter *theRes = 0;

  switch(classTag) {
  case  PARAMETER_TAG_Parameter:
    theRes = new Parameter;
    break;

//   case PARAMETER_TAG_MaterialStageParameter:
//     theRes = new MaterialStageParameter();
//     break;

  case PARAMETER_TAG_MatParameter:
    theRes = new MatParameter();
    break;

  default:
    ;
  }

  return theRes;
}


