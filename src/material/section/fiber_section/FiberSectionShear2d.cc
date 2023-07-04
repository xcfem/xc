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
//FiberSectionShear2d.cpp

#include "FiberSectionShear2d.h"
#include "material/uniaxial/UniaxialMaterial.h"
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include <utility/recorder/response/MaterialResponse.h>
#include "preprocessor/prep_handlers/MaterialHandler.h"


#include "classTags.h"
#include "material/section/ResponseId.h"
#include "utility/actor/actor/ArrayCommMetaData.h"
#include "utility/actor/actor/MatrixCommMetaData.h"
#include "utility/geom/d2/2d_polygons/Polygon2d.h"

XC::Vector XC::FiberSectionShear2d::def(3);
XC::Vector XC::FiberSectionShear2d::defzero(3);
XC::Vector XC::FiberSectionShear2d::s(3);
XC::Matrix XC::FiberSectionShear2d::ks(3,3);
XC::Matrix XC::FiberSectionShear2d::fs(3,3);


//! @brief Frees memory occupied by materials that define
//! shear and torsion responses.
void XC::FiberSectionShear2d::freeRespVy(void)
  {
    if(respVy)
      {
        delete respVy;
        respVy= nullptr;
      }
  }

//! @brief Frees memory.
void XC::FiberSectionShear2d::free_mem(void)
  { freeRespVy(); }

//! @brief Sets the material defining the shear response along the y axis.
void XC::FiberSectionShear2d::setRespVy(const UniaxialMaterial *rvy)
  {
    freeRespVy();
    if(rvy)
      respVy= rvy->getCopy();
  }

//! @brief Constructor.
XC::FiberSectionShear2d::FiberSectionShear2d(int tag, MaterialHandler *mat_ldr)
  : FiberSection2d(tag, SEC_TAG_FiberSectionShear2d, mat_ldr),
    respVy(nullptr) {}

//! @brief Copy constructor.
XC::FiberSectionShear2d::FiberSectionShear2d(const FiberSectionShear2d &other)
  : FiberSection2d(other), respVy(nullptr)
   { setRespVy(other.respVy); }

//! @brief Assignment operator.
XC::FiberSectionShear2d &XC::FiberSectionShear2d::operator=(const FiberSectionShear2d &other)
  {
    free_mem();
    FiberSection2d::operator=(other);
    setRespVy(other.respVy);
    respVy->set_owner(this);
    return *this;
  }

//! @brief Sets the material defining the shear response along the y axis.
void XC::FiberSectionShear2d::setRespVyByName(const std::string &material_name)
  {
    const Material *ptr_mat= material_handler->find_ptr(material_name);
    if(ptr_mat)
      {
        const UniaxialMaterial *tmp= dynamic_cast<const UniaxialMaterial *>(ptr_mat);
        if(tmp)
          setRespVy(tmp);
        else
          std::cerr << getClassName() << "::" << __FUNCTION__ << "; "
                    << "material identified by: '" << material_name
                    << "' is not uniaxial.\n";
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__ << "; "
                << "material identified by: '" << material_name
                << "' not found.\n";
  }

//! @brief Returns shear y response.
XC::UniaxialMaterial *XC::FiberSectionShear2d::getRespVy(void)
  { return respVy; }

//! @brief destructor:
XC::FiberSectionShear2d::~FiberSectionShear2d(void)
  { free_mem(); }

//! @brief Asigna la initial strain.
int XC::FiberSectionShear2d::setInitialSectionDeformation(const Vector &def)
  {
    static Vector v(2);
    v(0)= def(0); v(1)= def(1);
    int ret= FiberSection2d::setInitialSectionDeformation(v);
    if(respVy) ret+= respVy->setInitialStrain(def(2));
    return ret;
  }

//! @brief Asigna la trial strain.
int XC::FiberSectionShear2d::setTrialSectionDeformation(const Vector &def)
  {
    static Vector v(2);
    v(0)= def(0); v(1)= def(1);
    int ret= FiberSection2d::setTrialSectionDeformation(v);
    if(respVy) ret+= respVy->setTrialStrain(def(2));
    return ret;
  }

//! @brief Zeroes initial strain of the section.
void XC::FiberSectionShear2d::zeroInitialSectionDeformation(void)
  {
    FiberSection2d::zeroInitialSectionDeformation();
    if(respVy) respVy->setInitialStrain(0.0);
  }

//! @brief Returns the initial strain of the section.
const XC::Vector &XC::FiberSectionShear2d::getInitialSectionDeformation(void) const
  {
    defzero.Zero();
    const Vector &eSec= FiberSection2d::getInitialSectionDeformation();
    defzero(0)= eSec(0); defzero(1)= eSec(1);
    if(respVy) defzero(2)= respVy->getInitialStrain();
    return defzero;
  }

//! @brief Returns material's trial generalized deformation.
const XC::Vector &XC::FiberSectionShear2d::getSectionDeformation(void) const
  {
    def.Zero();
    const Vector &eSec= FiberSection2d::getSectionDeformation();
    def(0)= eSec(0);def(1)= eSec(1);
    if(respVy) def(2)= respVy->getStrain();
    return def;
  }

//! @brief Returns the tangent stiffness matrix.
const XC::Matrix &XC::FiberSectionShear2d::getSectionTangent(void) const
  {
    // Zero before assembly
    ks.Zero();
    const Matrix &kSec= FiberSection2d::getSectionTangent();
    ks(0,0)= kSec(0,0); ks(0,1)= kSec(0,1); 
    ks(1,0)= kSec(1,0); ks(1,1)= kSec(1,1); 
    if(respVy) ks(2,2)= respVy->getTangent();
    return ks;
  }

//! @brief Returns the initial tangent stiffness matrix.
const XC::Matrix &XC::FiberSectionShear2d::getInitialTangent(void) const
  {
    // Zero before assembly
    ks.Zero();
    const Matrix &kSec= FiberSection2d::getInitialTangent();
    ks(0,0)= kSec(0,0); ks(0,1)= kSec(0,1); 
    ks(1,0)= kSec(1,0); ks(1,1)= kSec(1,1); 
    if(respVy) ks(2,2)= respVy->getInitialTangent();
    return ks;
  }

//! @brief Returns the flexibility matrix.
const XC::Matrix &XC::FiberSectionShear2d::getSectionFlexibility(void) const
  {
    fs.Zero(); // Zero before assembly
    const Matrix &fSec= FiberSection2d::getSectionFlexibility();
    fs(0,0)= fSec(0,0); fs(0,1)= fSec(0,1);
    fs(1,0)= fSec(1,0); fs(1,1)= fSec(1,1);
    if(respVy) fs(2,2)= respVy->getFlexibility();
    return fs;
  }

//! @brief Returns the initial flexibility matrix.
const XC::Matrix &XC::FiberSectionShear2d::getInitialFlexibility(void) const
  {
    fs.Zero(); // Zero before assembly
    const Matrix &fSec= FiberSection2d::getInitialFlexibility();
    fs(0,0)= fSec(0,0); fs(0,1)= fSec(0,1); 
    fs(1,0)= fSec(1,0); fs(1,1)= fSec(1,1); 
    if(respVy) fs(2,2)= respVy->getInitialFlexibility();
    return fs;
  }

//! @brief Returns stress resultant.
const XC::Vector &XC::FiberSectionShear2d::getStressResultant(void) const
  {
    s.Zero();
    const Vector &sSec= FiberSection2d::getStressResultant();
    s(0)= sSec(0); s(1)= sSec(1);
    if(respVy) s(2)= respVy->getStress();
    return s;
  }

//! @brief Virtual constructor.
XC::SectionForceDeformation *XC::FiberSectionShear2d::getCopy(void) const
  { return new FiberSectionShear2d(*this); }

//! @brief Section stiffness contribution response identifiers.
const XC::ResponseId &XC::FiberSectionShear2d::getResponseType(void) const
  { return RespFiberSectionSh2d; }

int XC::FiberSectionShear2d::getOrder(void) const
  { return 3; }

//! @brief Commit material state (normally after convergence is achieved).
int XC::FiberSectionShear2d::commitState(void)
  {
    int err= FiberSection2d::commitState();
    if(respVy) err+= respVy->commitState();
    return err;
  }

//! @brief Returns the material to the last committed state.
int XC::FiberSectionShear2d::revertToLastCommit(void)
  {
    int err= FiberSection2d::revertToLastCommit();
    if(respVy) err+= respVy->revertToLastCommit();
    return err;
  }        

int XC::FiberSectionShear2d::revertToStart(void)
  {
    int err= FiberSection2d::revertToStart();
    if(respVy) err+= respVy->revertToStart();
    return err;
  }

//! @brief Send object members through the communicator argument.
int XC::FiberSectionShear2d::sendData(Communicator &comm)
  {
    int res= FiberSection2d::sendData(comm);
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << " - not implemented.\n";
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::FiberSectionShear2d::recvData(const Communicator &comm)
  {
    int res= FiberSection2d::recvData(comm);
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << " - not implemented.\n";
    return res;
  }

//! @brief Sends object through the communicator argument.
int XC::FiberSectionShear2d::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(29);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to send data\n";
    return res;
  }


//! @brief Receives object through the communicator argument.
int XC::FiberSectionShear2d::recvSelf(const Communicator &comm)
  {
    inicComm(29);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << getClassName() << __FUNCTION__
	        << "; failed to receive ids.\n";
    else
      {
        setTag(getDbTagDataPos(0));
        res+= recvData(comm);
        if(res<0)
          std::cerr << getClassName() << __FUNCTION__
		    << "; failed to receive data.\n";
      }
    return res;
  }

//! @brief Print stuff.
void XC::FiberSectionShear2d::Print(std::ostream &s, int flag) const
  {
    if(flag == 2)
      { FiberSectionShear2d::Print(s, flag); }
    else
      {
        s << "\nFiberSectionShear2d, tag: " << this->getTag() << std::endl;
      }
  }

//! @brief Returns the identifier of the variable which name is
//! being passed as parameter.
int XC::FiberSectionShear2d::setVariable(const std::string &argv)
  {
    // Axial strain
    if(argv =="axialStrain")
      return 1;
    else if (argv == "curvatureZ") //Curvature about the section z-axis
      return 2;
    else
      return -1;
  }

//! @brief Returns the value of the variable which name is
//! being passed as parameter.
int XC::FiberSectionShear2d::getVariable(int variableID, double &info)
  {
    int i;
    info= 0.0;

    const int order= getOrder();

    const Vector &e= getSectionDeformation();
    const ResponseId &code= getResponseType();

    switch (variableID)
      {
      case 1: // Axial strain
        // Series model ... add all sources of deformation
        for(i= 0; i < order; i++)
          if(code(i) == SECTION_RESPONSE_P)
            info+= e(i);
        return 0;
      case 2: // Curvature about the section z-axis
        for(i= 0; i < order; i++)
          if(code(i) == SECTION_RESPONSE_MZ)
            info += e(i);
          return 0;
      default:
        return -1;
      }
  }

