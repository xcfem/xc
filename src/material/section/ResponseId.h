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


const int MEMBRANE_RESPONSE_n1= 11; // Axil per unit length, parallel to axis 1
const int MEMBRANE_RESPONSE_n2= 12; // Axil per unit length, parallel to axis 2
const int MEMBRANE_RESPONSE_n12= 13; // Esfuerzo rasante per unit length.
const int PLATE_RESPONSE_m1= 14; // Moment per unit length, parallel to axis 1.
const int PLATE_RESPONSE_m2= 15; // Moment per unit length, parallel to axis 2.
const int PLATE_RESPONSE_m12= 16; // Esfuerzo torsor per unit length.
const int PLATE_RESPONSE_q13= 17; // Shear force per unit length, perpendicular to axis 1 and parallel to axis 3.
const int PLATE_RESPONSE_q23= 18; // Shear force per unit length, perpendicular to axis 2 and parallel to axis 3.

//! @ingroup MATSCC
//
//! @brief Stiffness material contribution response identifiers.
class ResponseId: public ID
  {
  public:
    ResponseId(int sz);
    ResponseId(const std::deque<std::string> &indices);
    ResponseId(const std::vector<std::string> &indices);
    //ResponseId(int *, int size, bool cleanIt = false);
    explicit ResponseId(const ID &);
    bool hasResponse(const int &ri) const;
    static int StringToRespId(const std::string &str);
    static std::string RespIdToString(const int &ri);
    std::string getString(void) const;
  };

class RespPMz: public ResponseId
  {
  public:
    RespPMz(void);
  };

class RespPMzV: public ResponseId
  {
  public:
    RespPMzV(void);
  };

class RespVyP: public ResponseId
  {
  public:
    RespVyP(void);
  };

class RespPMzMy: public ResponseId
  {
  public:
    RespPMzMy(void);
  };

class RespPMzMyT: public ResponseId
  {
  public:
    RespPMzMyT(void);
  };

class RespPMzVyMyVzT: public ResponseId
  {
  public:
    RespPMzVyMyVzT(void);
  };

class RespFiberSectionShear3d: public ResponseId
  {
  public:
    RespFiberSectionShear3d(void);
  };

class RespPVyMz: public ResponseId
  {
  public:
    RespPVyMz(void);
  };

class RespMembraneMaterial: public ResponseId
  {
  public:
    RespMembraneMaterial(void);
  };

class RespPlateMaterial: public ResponseId
  {
  public:
    RespPlateMaterial(void);
  };

class RespShellMaterial: public ResponseId
  {
  public:
    RespShellMaterial(void);
  };

const RespPMz RespElasticSection2d;
const RespPMzV RespElasticShSection2d;
const RespVyP RespBidirectional;
const RespPMzMy RespFiberSection3d;
const RespPMzMyT RespElasticSection3d;
const RespPMzVyMyVzT RespElasticShSection3d;
const RespFiberSectionShear3d RespFiberSectionSh3d;
const RespPVyMz RespIsolator2spring;
const RespMembraneMaterial RespMembraneMat;
const RespPlateMaterial RespPlateMat;
const RespShellMaterial RespShellMat;

} // end of XC namespace

#endif
