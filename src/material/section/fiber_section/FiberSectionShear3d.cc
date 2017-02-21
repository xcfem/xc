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
//FiberSectionShear3d.cpp

#include "FiberSectionShear3d.h"
#include "material/uniaxial/UniaxialMaterial.h"
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include <utility/recorder/response/MaterialResponse.h>
#include "preprocessor/loaders/MaterialLoader.h"


#include "classTags.h"
#include "material/section/ResponseId.h"
#include "utility/actor/actor/ArrayCommMetaData.h"
#include "utility/actor/actor/MatrixCommMetaData.h"
#include "xc_utils/src/geom/d2/poligonos2d/Poligono2d.h"

XC::Vector XC::FiberSectionShear3d::def(6);
XC::Vector XC::FiberSectionShear3d::defzero(6);
XC::Vector XC::FiberSectionShear3d::s(6);
XC::Matrix XC::FiberSectionShear3d::ks(6,6);
XC::Matrix XC::FiberSectionShear3d::fs(6,6);


//! @brief Frees memory occupied by materials that define
//! shear and torsion responses.
void XC::FiberSectionShear3d::liberaRespVyVzT(void)
  {
    if(respVy)
      {
        delete respVy;
        respVy= nullptr;
      }
    if(respVz)
      {
        delete respVz;
        respVz= nullptr;
      }
    if(respT)
      {
        delete respT;
        respT= nullptr;
      }
  }

//! @brief Libera la memoria reservada dinámicamente.
void XC::FiberSectionShear3d::libera(void)
  { liberaRespVyVzT(); }

//! @brief Asigna el material que define la respuesta a cortante según «y».
void XC::FiberSectionShear3d::setRespVy(const UniaxialMaterial *rvy)
  {
    if(respVy)
      delete respVy;
    respVy= rvy->getCopy();
  }

//! @brief Asigna el material que define la respuesta a cortante según «z».
void XC::FiberSectionShear3d::setRespVz(const UniaxialMaterial *rvz)
  {
    if(respVz)
      delete respVz;
    respVz= rvz->getCopy();
  }

//! @brief Asigna el material que define la respuesta a torsión.
void XC::FiberSectionShear3d::setRespT(const UniaxialMaterial *rt)
  {
    if(respT)
      delete respT;
    respT= rt->getCopy();
  }

void XC::FiberSectionShear3d::setRespVyVzT(const UniaxialMaterial *rvy,const UniaxialMaterial *rvz,const UniaxialMaterial *rt)
  {
    liberaRespVyVzT();
    if(rvy)
      respVy= rvy->getCopy();
    else
      std::cerr << "FiberSectionShear3d::setRespVyVzT; "
                << " no se ha definido la respuesta al cortante según «y»." << std::endl;
    if(rvz)
      respVz= rvz->getCopy();
    else
      std::cerr << "FiberSectionShear3d::setRespVyVzT; "
                << " no se ha definido la respuesta al cortante según «z»." << std::endl;
    if(rt)
      respT= rt->getCopy();
    else
      std::cerr << "FiberSectionShear3d::setRespVyVzT; "
                << " no se ha definido la respuesta al torsor." << std::endl;
  }

//! @brief Constructor.
XC::FiberSectionShear3d::FiberSectionShear3d(int tag, MaterialLoader *mat_ldr)
  : FiberSection3d(tag, SEC_TAG_FiberSectionShear3d,mat_ldr),
    respVy(nullptr), respVz(nullptr), respT(nullptr) {}

//! @brief Copy constructor.
XC::FiberSectionShear3d::FiberSectionShear3d(const FiberSectionShear3d &otro)
  : FiberSection3d(otro), respVy(nullptr), respVz(nullptr), respT(nullptr)
   { setRespVyVzT(otro.respVy,otro.respVz,otro.respT); }

//! @brief Assignment operator.
XC::FiberSectionShear3d &XC::FiberSectionShear3d::operator=(const FiberSectionShear3d &otro)
  {
    libera();
    FiberSection3d::operator=(otro);
    setRespVyVzT(otro.respVy,otro.respVz,otro.respT);
    respVy->set_owner(this); respVz->set_owner(this); respT->set_owner(this);
    return *this;
  }

//! @brief Asigna el material que define la respuesta a cortante según «y».
void XC::FiberSectionShear3d::setRespVyByName(const std::string &nmb_mat)
  {
    const Material *ptr_mat= material_loader->find_ptr(nmb_mat);
    if(ptr_mat)
      {
        const UniaxialMaterial *tmp= dynamic_cast<const UniaxialMaterial *>(ptr_mat);
        if(tmp)
          setRespVy(tmp);
        else
          std::cerr << "FiberSectionShear3d::setRespVyByName; "
                    << "el material de código: '" << nmb_mat
                    << "' no corresponde a un material uniaxial.\n";
      }
    else
      std::cerr << "FiberSectionShear3d::setRespVyByName; "
                << "no se encontró el material de código: '" << nmb_mat
                << "'.\n";
  }

//! @brief Returns shear y response.
XC::UniaxialMaterial *XC::FiberSectionShear3d::getRespVy(void)
  { return respVy; }

//! @brief Asigna el material que define la respuesta a cortante según «y».
void XC::FiberSectionShear3d::setRespVzByName(const std::string &nmb_mat)
  {
    const Material *ptr_mat= material_loader->find_ptr(nmb_mat);
    if(ptr_mat)
      {
        const UniaxialMaterial *tmp= dynamic_cast<const UniaxialMaterial *>(ptr_mat);
        if(tmp)
          setRespVz(tmp);
        else
          std::cerr << "FiberSectionShear3d::setRespVzByName; "
                    << "el material de código: '" << nmb_mat
                    << "' no corresponde a un material uniaxial.\n";
      }
    else
      std::cerr << "FiberSectionShear3d::setRespVzByName; "
                << "no se encontró el material de código: '" << nmb_mat
                << "'.\n";
  }

//! @brief Returns shear z response.
XC::UniaxialMaterial *XC::FiberSectionShear3d::getRespVz(void)
  { return respVz; }

//! @brief Asigna el material que define la respuesta a cortante según «y».
void XC::FiberSectionShear3d::setRespTByName(const std::string &nmb_mat)
  {
    const Material *ptr_mat= material_loader->find_ptr(nmb_mat);
    if(ptr_mat)
      {
        const UniaxialMaterial *tmp= dynamic_cast<const UniaxialMaterial *>(ptr_mat);
        if(tmp)
          setRespT(tmp);
        else
          std::cerr << "FiberSectionShear3d::setRespTByName; "
                    << "el material de código: '" << nmb_mat
                    << "' no corresponde a un material uniaxial.\n";
      }
    else
      std::cerr << "FiberSectionShear3d::setRespTByName; "
                << "no se encontró el material de código: '" << nmb_mat
                << "'.\n";
  }

//! @brief Returns torsion response.
XC::UniaxialMaterial *XC::FiberSectionShear3d::getRespT(void)
  { return respT; }

void XC::FiberSectionShear3d::setRespVyVzTByName(const std::string &rvy,const std::string &rvz,const std::string &rt)
  {
    setRespVyByName(rvy);
    setRespVzByName(rvz);
    setRespTByName(rt);
  }

//! @brief destructor:
XC::FiberSectionShear3d::~FiberSectionShear3d(void)
  { libera(); }

//! @brief Asigna la initial strain.
int XC::FiberSectionShear3d::setInitialSectionDeformation(const Vector &def)
  {
    static Vector v(3);
    v(0)= def(0); v(1)= def(1); v(2)= def(2);
    int ret= FiberSection3d::setInitialSectionDeformation(v);
    if(respVy) ret+= respVy->setInitialStrain(def(3));
    if(respVz) ret+= respVz->setInitialStrain(def(4));
    if(respT) ret+= respT->setInitialStrain(def(5));
    return ret;
  }

//! @brief Asigna la trial strain.
int XC::FiberSectionShear3d::setTrialSectionDeformation(const Vector &def)
  {
    static Vector v(3);
    v(0)= def(0); v(1)= def(1); v(2)= def(2);
    int ret= FiberSection3d::setTrialSectionDeformation(v);
    if(respVy) ret+= respVy->setTrialStrain(def(3));
    if(respVz) ret+= respVz->setTrialStrain(def(4));
    if(respT) ret+= respT->setTrialStrain(def(5));
    return ret;
  }

//! @brief Returns the initial strain de la sección.
const XC::Vector &XC::FiberSectionShear3d::getInitialSectionDeformation(void) const
  {
    defzero.Zero();
    const Vector &eSec= FiberSection3d::getInitialSectionDeformation();
    defzero(0)= eSec(0);defzero(1)= eSec(1);defzero(2)= eSec(2);
    if(respVy) defzero(3)= respVy->getInitialStrain();
    if(respVz) defzero(4)= respVz->getInitialStrain();
    if(respT) defzero(5)= respT->getInitialStrain();
    return defzero;
  }

//! @brief Returns material's trial generalized deformation.
const XC::Vector &XC::FiberSectionShear3d::getSectionDeformation(void) const
  {
    def.Zero();
    const Vector &eSec= FiberSection3d::getSectionDeformation();
    def(0)= eSec(0);def(1)= eSec(1);def(2)= eSec(2);
    if(respVy) def(3)= respVy->getStrain();
    if(respVz) def(4)= respVz->getStrain();
    if(respT) def(5)= respT->getStrain();
    return def;
  }

//! @brief Returns the tangent stiffness matrix.
const XC::Matrix &XC::FiberSectionShear3d::getSectionTangent(void) const
  {
    // Zero before assembly
    ks.Zero();
    const Matrix &kSec= FiberSection3d::getSectionTangent();
    ks(0,0)= kSec(0,0); ks(0,1)= kSec(0,1); ks(0,2)= kSec(0,2); 
    ks(1,0)= kSec(1,0); ks(1,1)= kSec(1,1); ks(1,2)= kSec(1,2); 
    ks(2,0)= kSec(2,0); ks(2,1)= kSec(2,1); ks(2,2)= kSec(2,2); 
    if(respVy) ks(3,3)= respVy->getTangent();
    if(respVz) ks(4,4)= respVz->getTangent();
    if(respT) ks(5,5)= respT->getTangent();
    return ks;
  }

//! @brief Returns the initial tangent stiffness matrix.
const XC::Matrix &XC::FiberSectionShear3d::getInitialTangent(void) const
  {
    // Zero before assembly
    ks.Zero();
    const Matrix &kSec= FiberSection3d::getInitialTangent();
    ks(0,0)= kSec(0,0); ks(0,1)= kSec(0,1); ks(0,2)= kSec(0,2); 
    ks(1,0)= kSec(1,0); ks(1,1)= kSec(1,1); ks(1,2)= kSec(1,2); 
    ks(2,0)= kSec(2,0); ks(2,1)= kSec(2,1); ks(2,2)= kSec(2,2); 
    if(respVy) ks(3,3)= respVy->getInitialTangent();
    if(respVz) ks(4,4)= respVz->getInitialTangent();
    if(respT) ks(5,5)= respT->getInitialTangent();
    return ks;
  }

//! @brief Returns the flexibility matrix.
const XC::Matrix &XC::FiberSectionShear3d::getSectionFlexibility(void) const
  {
    fs.Zero(); // Zero before assembly
    const Matrix &fSec= FiberSection3d::getSectionFlexibility();
    fs(0,0)= fSec(0,0); fs(0,1)= fSec(0,1); fs(0,2)= fSec(0,2); 
    fs(1,0)= fSec(1,0); fs(1,1)= fSec(1,1); fs(1,2)= fSec(1,2); 
    fs(2,0)= fSec(2,0); fs(2,1)= fSec(2,1); fs(2,2)= fSec(2,2); 
    if(respVy) fs(3,3)= respVy->getFlexibility();
    if(respVz) fs(4,4)= respVz->getFlexibility();
    if(respT) fs(5,5)= respT->getFlexibility();
    return fs;
  }

//! @brief Returns the initial flexibility matrix.
const XC::Matrix &XC::FiberSectionShear3d::getInitialFlexibility(void) const
  {
    fs.Zero(); // Zero before assembly
    const Matrix &fSec= FiberSection3d::getInitialFlexibility();
    fs(0,0)= fSec(0,0); fs(0,1)= fSec(0,1); fs(0,2)= fSec(0,2); 
    fs(1,0)= fSec(1,0); fs(1,1)= fSec(1,1); fs(1,2)= fSec(1,2); 
    fs(2,0)= fSec(2,0); fs(2,1)= fSec(2,1); fs(2,2)= fSec(2,2); 
    if(respVy) fs(3,3)= respVy->getInitialFlexibility();
    if(respVz) fs(4,4)= respVz->getInitialFlexibility();
    if(respT) fs(5,5)= respT->getInitialFlexibility();
    return fs;
  }

//! @brief Returns stress resultant.
const XC::Vector &XC::FiberSectionShear3d::getStressResultant(void) const
  {
    s.Zero();
    const Vector &sSec= FiberSection3d::getStressResultant();
    s(0)= sSec(0); s(1)= sSec(1); s(2)= sSec(2);
    if(respVy) s(3)= respVy->getStress();
    if(respVz) s(4)= respVz->getStress();
    if(respT) s(5)= respT->getStress();
    return s;
  }

//! @brief Virtual constructor.
XC::SectionForceDeformation *XC::FiberSectionShear3d::getCopy(void) const
  { return new FiberSectionShear3d(*this); }

//! @brief Section stiffness contribution response identifiers.
const XC::ResponseId &XC::FiberSectionShear3d::getType(void) const
  { return RespFiberSectionSh3d; }

int XC::FiberSectionShear3d::getOrder(void) const
  { return 6; }

//! @brief Commit material state (normally after convergence is achieved).
int XC::FiberSectionShear3d::commitState(void)
  {
    int err= FiberSection3d::commitState();
    if(respVy) err+= respVy->commitState();
    if(respVz) err+= respVz->commitState();
    if(respT) err+= respT->commitState();
    return err;
  }

//! @brief Returns the material to the last commited state.
int XC::FiberSectionShear3d::revertToLastCommit(void)
  {
    int err= FiberSection3d::revertToLastCommit();
    if(respVy) err+= respVy->revertToLastCommit();
    if(respVz) err+= respVz->revertToLastCommit();
    if(respT) err+= respT->revertToLastCommit();
    return err;
  }        

int XC::FiberSectionShear3d::revertToStart(void)
  {
    int err= FiberSection3d::revertToStart();
    if(respVy) err+= respVy->revertToStart();
    if(respVz) err+= respVz->revertToStart();
    if(respT) err+= respT->revertToStart();
    return err;
  }

//! @brief Send members del objeto through the channel being passed as parameter.
int XC::FiberSectionShear3d::sendData(CommParameters &cp)
  {
    int res= FiberSection3d::sendData(cp);
    std::cerr << "FiberSectionShear3d::sendData - no implementada.\n";
    return res;
  }

//! @brief Receives members del objeto through the channel being passed as parameter.
int XC::FiberSectionShear3d::recvData(const CommParameters &cp)
  {
    int res= FiberSection3d::recvData(cp);
    std::cerr << "FiberSectionShear3d::recvData - no implementada.\n";
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::FiberSectionShear3d::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(29);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << nombre_clase() << "sendSelf() - failed to send data\n";
    return res;
  }


//! @brief Receives object through the channel being passed as parameter.
int XC::FiberSectionShear3d::recvSelf(const CommParameters &cp)
  {
    inicComm(29);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << nombre_clase() << "::recvSelf - failed to receive ids.\n";
    else
      {
        setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
          std::cerr << nombre_clase() << "::recvSelf - failed to receive data.\n";
      }
    return res;
  }

//! @brief Imprime el objeto.
void XC::FiberSectionShear3d::Print(std::ostream &s, int flag) const
  {
    if(flag == 2)
      { FiberSectionShear3d::Print(s, flag); }
    else
      {
        s << "\nFiberSectionShear3d, tag: " << this->getTag() << std::endl;
      }
  }

//! @brief Returns the identificador de la variable cuyo nombre se pasa como parámetro.
int XC::FiberSectionShear3d::setVariable(const std::string &argv)
  {
    // Axial strain
    if(argv =="axialStrain")
      return 1;
    else if (argv == "curvatureZ") //Curvature about the section z-axis
      return 2;
    else if(argv == "curvatureY") // Curvature about the section y-axis
      return 3;
    else
      return -1;
  }

//! @brief Returns the valor de la variable cuyo nombre se pasa como parámetro.
int XC::FiberSectionShear3d::getVariable(int variableID, double &info)
  {
    int i;
    info= 0.0;

    const int order= getOrder();

    const Vector &e= getSectionDeformation();
    const ResponseId &code= getType();

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
      case 3:// Curvature about the section y-axis
        for(i= 0;i<order;i++)
          if(code(i) == SECTION_RESPONSE_MY)
            info += e(i);
        return 0;
      default:
        return -1;
      }
  }

