// -*-c++-*-
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
//ResponseId.h
//Stiffness material contribution response identifiers.

#ifndef ResponseId_h
#define ResponseId_h

#include "utility/matrix/ID.h"

namespace XC {
 class Channel;
 class FEM_ObjectBroker;


const int SECTION_RESPONSE_MZ= 1; // Curvature about the section z-axis
const int SECTION_RESPONSE_P= 2; // Axial strain
const int SECTION_RESPONSE_VY= 3;
const int SECTION_RESPONSE_MY= 4; // Curvature about the section y-axis
const int SECTION_RESPONSE_VZ= 5;
const int SECTION_RESPONSE_T= 6; // Torsion.


const int MEMBRANE_RESPONSE_n1= 11; // Axial force per unit length, parallel to axis 1
const int MEMBRANE_RESPONSE_n2= 12; // Axial force per unit length, parallel to axis 2
const int MEMBRANE_RESPONSE_n12= 13; // In-plane shear force per unit length.
const int PLATE_RESPONSE_m1= 14; // Bending moment per unit length, parallel to axis 1.
const int PLATE_RESPONSE_m2= 15; // Bending moment per unit length, parallel to axis 2.
const int PLATE_RESPONSE_m12= 16; // Torque per unit length.
const int PLATE_RESPONSE_q13= 17; // Shear force per unit length, perpendicular to axis 1 and parallel to axis 3.
const int PLATE_RESPONSE_q23= 18; // Shear force per unit length, perpendicular to axis 2 and parallel to axis 3.
  
const int SOLID_MECHANICS_RESPONSE_sigma_11= 19; // Solid mechanics.
const int SOLID_MECHANICS_RESPONSE_sigma_22= 20;
const int SOLID_MECHANICS_RESPONSE_sigma_33= 21;
const int SOLID_MECHANICS_RESPONSE_sigma_12= 22;
const int SOLID_MECHANICS_RESPONSE_sigma_23= 23;
const int SOLID_MECHANICS_RESPONSE_sigma_13= 24;

//
//! @brief Stiffness material contribution response identifiers.
//! @ingroup MATSCC
class ResponseId: public ID
  {
  public:
    ResponseId(int sz= 0);
    ResponseId(const std::deque<std::string> &);
    ResponseId(const std::vector<std::string> &);
    explicit ResponseId(const boost::python::list &);
    //ResponseId(int *, int size, bool cleanIt = false);
    explicit ResponseId(const ID &);
    bool hasResponse(const int &ri) const;
    virtual int getComponentIdFromString(const std::string &str) const;
    virtual std::string getStringFromComponentId(const int &ri) const;
    std::string getString(void) const;
    boost::python::list getStringIdentifiers(void) const;
    boost::python::list getIdentifiers(void) const;

    std::deque<int> getComponentIndexesFromCode(const int &) const;
    std::deque<int> getComponentIndexesFromCode(const std::string &) const;
    boost::python::list getComponentIndexesFromCodePy(const int &code) const;
    boost::python::list getComponentIndexesFromCodePy(const std::string &code) const;
  };

class RespP: public ResponseId
  {
  public:
    RespP(void);
    int getComponentIdFromString(const std::string &str) const;
    std::string getStringFromComponentId(const int &ri) const;    
  };
  
class RespPMz: public ResponseId
  {
  public:
    RespPMz(void);
    int getComponentIdFromString(const std::string &str) const;
    std::string getStringFromComponentId(const int &ri) const;    
  };

class RespPMzV: public ResponseId
  {
  public:
    RespPMzV(void);
    int getComponentIdFromString(const std::string &str) const;
    std::string getStringFromComponentId(const int &ri) const;    
  };

class RespVyP: public ResponseId
  {
  public:
    RespVyP(void);
    int getComponentIdFromString(const std::string &str) const;
    std::string getStringFromComponentId(const int &ri) const;    
  };

class RespPMzMy: public ResponseId
  {
  public:
    RespPMzMy(void);
    int getComponentIdFromString(const std::string &str) const;
    std::string getStringFromComponentId(const int &ri) const;    
  };

class RespPMzMyT: public ResponseId
  {
  public:
    RespPMzMyT(void);
    int getComponentIdFromString(const std::string &str) const;
    std::string getStringFromComponentId(const int &ri) const;    
  };

class RespPMzVyMyVzT: public ResponseId
  {
  public:
    RespPMzVyMyVzT(void);
    int getComponentIdFromString(const std::string &str) const;
    std::string getStringFromComponentId(const int &ri) const;    
  };

class RespFiberSectionShear2d: public RespPMzV
  {
  public:
    RespFiberSectionShear2d(void);
  };
  
class RespFiberSectionShear3d: public RespPMzVyMyVzT
  {
  public:
    RespFiberSectionShear3d(void);
  };

class RespPVyMz: public RespPMzV
  {
  public:
    RespPVyMz(void);
  };

class RespMembraneMaterial: public ResponseId
  {
  public:
    RespMembraneMaterial(void);
    int getComponentIdFromString(const std::string &str) const;
    std::string getStringFromComponentId(const int &ri) const;    
  };

class RespPlateMaterial: public ResponseId
  {
  public:
    RespPlateMaterial(void);
    int getComponentIdFromString(const std::string &str) const;
    std::string getStringFromComponentId(const int &ri) const;    
  };

class RespShellMaterial: public ResponseId
  {
  public:
    RespShellMaterial(void);
    int getComponentIdFromString(const std::string &str) const;
    std::string getStringFromComponentId(const int &ri) const;    
  };
  
class RespSolidMecanics1DMaterial: public ResponseId
  {
  public:
    RespSolidMecanics1DMaterial(void);
    int getComponentIdFromString(const std::string &str) const;
    std::string getStringFromComponentId(const int &ri) const;    
  };
  
class RespSolidMecanics2DMaterial: public ResponseId
  {
  public:
    RespSolidMecanics2DMaterial(void);
    int getComponentIdFromString(const std::string &str) const;
    std::string getStringFromComponentId(const int &ri) const;    
  };
  
class RespSolidMecanics3DMaterial: public ResponseId
  {
  public:
    RespSolidMecanics3DMaterial(void);
    int getComponentIdFromString(const std::string &str) const;
    std::string getStringFromComponentId(const int &ri) const;    
  };

const RespP RespElasticSection1d;
const RespPMz RespElasticSection2d;
const RespPMzV RespElasticShSection2d;
const RespVyP RespBidirectional;
const RespPMzMy RespFiberSection3d;
const RespPMzMyT RespElasticSection3d;
const RespPMzVyMyVzT RespElasticShSection3d;
const RespFiberSectionShear2d RespFiberSectionSh2d;
const RespFiberSectionShear3d RespFiberSectionSh3d;
const RespPVyMz RespIsolator2spring;
const RespMembraneMaterial RespMembraneMat;
const RespPlateMaterial RespPlateMat;
const RespShellMaterial RespShellMat;
const RespSolidMecanics1DMaterial RespSolidMechanics1D;
const RespSolidMecanics2DMaterial RespSolidMechanics2D;
const RespSolidMecanics3DMaterial RespSolidMechanics3D;

} // end of XC namespace

#endif
