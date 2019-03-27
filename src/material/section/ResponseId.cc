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
//ResponseId.cc

#include "ResponseId.h"
#include "utility/actor/actor/MovableID.h"

XC::ResponseId::ResponseId(int sz)
  : ID(sz) {}

XC::ResponseId::ResponseId(const std::deque<std::string> &indices)
  : ID(indices.size())
  {
    const size_t sz= indices.size();
    for(register size_t i= 0;i<sz;i++)
      (*this)[i]= StringToRespId(indices[i]);
  }

XC::ResponseId::ResponseId(const std::vector<std::string> &indices)
  : ID(indices.size())
  {
    const size_t sz= indices.size();
    for(register size_t i= 0;i<sz;i++)
      (*this)[i]= StringToRespId(indices[i]);
  }

// XC::ResponseId::ResponseId(int *dt, int size, bool cleanIt)
//   : ID(dt,size,cleanIt) {}

XC::ResponseId::ResponseId(const ID &other)
  : ID(other) {}

bool XC::ResponseId::hasResponse(const int &ri) const
  {
    bool retval= false;
    const size_t sz= Size();
    for(size_t i= 0;i<sz;i++)
      if((*this)(i)==ri)
        {
          retval= true;
          break;
        }
    return retval;
  }

//! @brief 
int XC::ResponseId::StringToRespId(const std::string &str)
  {
    if(str == "Mz")
      return SECTION_RESPONSE_MZ;
    else if(str =="P")
      return SECTION_RESPONSE_P;
    else if(str =="Vy")
      return SECTION_RESPONSE_VY;
    else if(str =="My")
      return SECTION_RESPONSE_MY;
    else if(str =="Vz")
      return SECTION_RESPONSE_VZ;
    else if(str =="T")
      return SECTION_RESPONSE_T;
    else if(str =="n1")
      return MEMBRANE_RESPONSE_n1;
    else if(str =="n2")
      return MEMBRANE_RESPONSE_n2;
    else if(str =="n12")
      return MEMBRANE_RESPONSE_n12;
    else if(str =="m1")
      return PLATE_RESPONSE_m1;
    else if(str =="m2")
      return PLATE_RESPONSE_m2;
    else if(str =="m12")
      return PLATE_RESPONSE_m12;
    else if(str =="q13")
      return PLATE_RESPONSE_q13;
    else if(str =="q23")
      return PLATE_RESPONSE_q23;
    else
      {
        std::cerr << "ResponseId::" << __FUNCTION__
	          << "; unknown code: '"
                  << str << "'." << std::endl;
        return 0;
      }
  }

std::string XC::ResponseId::RespIdToString(const int &code)
  {
    switch(code)
      {
      case SECTION_RESPONSE_P:
        return "P";
        break;
      case SECTION_RESPONSE_MZ:
        return "Mz";
        break;
      case SECTION_RESPONSE_VY:
        return "Vy";
        break;
      case SECTION_RESPONSE_MY:
        return "My";
        break;
      case SECTION_RESPONSE_VZ:
        return "Vz";
        break;
      case SECTION_RESPONSE_T:
        return "T";
        break;
      case MEMBRANE_RESPONSE_n1:
        return "n1";
        break;
      case MEMBRANE_RESPONSE_n2:
        return "n2";
        break;
      case MEMBRANE_RESPONSE_n12:
        return "n12";
        break;
      case PLATE_RESPONSE_m1:
        return "m1";
        break;
      case PLATE_RESPONSE_m2:
        return "m2";
        break;
      case PLATE_RESPONSE_m12:
        return "m12";
        break;
      case PLATE_RESPONSE_q13:
        return "q13";
        break;
      case PLATE_RESPONSE_q23:
        return "q23";
        break;
      default:
        std::cerr << "ResponseId::" << __FUNCTION__
		  << "; unknown code: '" << code
                  << "'."
                  << std::endl;
        return "nil";
        break;
      }
  }

std::string XC::ResponseId::getString(void) const
  {
    std::string retval= "";
    const int sz= Size();
    if(sz>0)
      {
        retval+= RespIdToString((*this)(0));
        for(int i= 1;i<sz;i++)
          retval+= "," + RespIdToString((*this)(i));
      }
    return retval;
  }

//! @brief Constructor; sets the identifiers for the stiffness contribution of the material.
XC::RespPMz::RespPMz(void)
  : ResponseId(2)
  {
    (*this)(0)= SECTION_RESPONSE_P; // P is the first quantity
    (*this)(1)= SECTION_RESPONSE_MZ; // Mz is the second
  }

//! @brief Constructor; sets the identifiers for the stiffness contribution of the material.
XC::RespPMzV::RespPMzV(void)
  : ResponseId(3)
  {
    (*this)(0)= SECTION_RESPONSE_P; // P is the first quantity
    (*this)(1)= SECTION_RESPONSE_MZ; // Mz is the second
    (*this)(2)= SECTION_RESPONSE_VY; // Vy is the third
  }

//! @brief Constructor; sets the identifiers for the stiffness contribution of the material.
XC::RespVyP::RespVyP(void)
  : ResponseId(2)
  {
    (*this)(0)= SECTION_RESPONSE_VY; // Vy is the first quantity
    (*this)(1)= SECTION_RESPONSE_P; // P is the second
  }

//! @brief Constructor; sets the identifiers for the stiffness contribution of the material.
XC::RespPMzMy::RespPMzMy(void)
  : ResponseId(3)
  {
    (*this)(0)= SECTION_RESPONSE_P; // P is the first quantity
    (*this)(1)= SECTION_RESPONSE_MZ; // Mz is the second
    (*this)(2)= SECTION_RESPONSE_MY; // My is the third 
  }

//! @brief Constructor; sets the identifiers for the stiffness contribution of the material.
XC::RespPMzMyT::RespPMzMyT(void)
  : ResponseId(4)
  {
    (*this)(0)= SECTION_RESPONSE_P; // P is the first quantity
    (*this)(1)= SECTION_RESPONSE_MZ; // Mz is the second
    (*this)(2)= SECTION_RESPONSE_MY; // My is the third 
    (*this)(3)= SECTION_RESPONSE_T; // T is the fourth
  }

//! @brief Constructor; sets the identifiers for the stiffness contribution of the material.
XC::RespPMzVyMyVzT::RespPMzVyMyVzT(void)
  : ResponseId(6)
  {
    (*this)(0)= SECTION_RESPONSE_P;        // P is the first quantity
    (*this)(1)= SECTION_RESPONSE_MZ;        // Mz is the second
    (*this)(2)= SECTION_RESPONSE_VY;        // Vy is the third 
    (*this)(3)= SECTION_RESPONSE_MY;        // My is the fourth
    (*this)(4)= SECTION_RESPONSE_VZ;        // Vz is the fifth
    (*this)(5)= SECTION_RESPONSE_T;        // T is the sixth
  }

//! @brief Constructor; sets the identifiers for the stiffness contribution of the material.
XC::RespFiberSectionShear3d::RespFiberSectionShear3d(void)
  : ResponseId(6)
  {
    (*this)(0)= SECTION_RESPONSE_P;
    (*this)(1)= SECTION_RESPONSE_MZ;
    (*this)(2)= SECTION_RESPONSE_MY;
    (*this)(3)= SECTION_RESPONSE_VY;
    (*this)(4)= SECTION_RESPONSE_VZ;
    (*this)(5)= SECTION_RESPONSE_T;
  }

//! @brief Constructor; sets the identifiers for the stiffness contribution of the material.
XC::RespPVyMz::RespPVyMz(void)
  : ResponseId(3)
  {
    (*this)(0)= SECTION_RESPONSE_P; // P is the first quantity
    (*this)(1)= SECTION_RESPONSE_VY; // Vy is the second
    (*this)(2)= SECTION_RESPONSE_MZ; // Mz is the third 
  }

//! @brief Constructor; sets the identifiers for the stiffness contribution of the material.
XC::RespMembraneMaterial::RespMembraneMaterial(void)
  : ResponseId(3)
  {
    (*this)(0)= MEMBRANE_RESPONSE_n1;
    (*this)(1)= MEMBRANE_RESPONSE_n2;
    (*this)(2)= MEMBRANE_RESPONSE_n12;
  }

//! @brief Constructor; sets the identifiers for the stiffness contribution of the material.
XC::RespPlateMaterial::RespPlateMaterial(void)
  : ResponseId(5)
  {
    (*this)(0)= PLATE_RESPONSE_m1;
    (*this)(1)= PLATE_RESPONSE_m2;
    (*this)(2)= PLATE_RESPONSE_m12;
    (*this)(3)= PLATE_RESPONSE_q13;
    (*this)(4)= PLATE_RESPONSE_q23;
  }

//! @brief Constructor; sets the identifiers for the stiffness contribution of the material.
XC::RespShellMaterial::RespShellMaterial(void)
  : ResponseId(8)
  {
    (*this)(0)= MEMBRANE_RESPONSE_n1;
    (*this)(1)= MEMBRANE_RESPONSE_n2;
    (*this)(2)= MEMBRANE_RESPONSE_n12;
    (*this)(3)= PLATE_RESPONSE_m1;
    (*this)(4)= PLATE_RESPONSE_m2;
    (*this)(5)= PLATE_RESPONSE_m12;
    (*this)(6)= PLATE_RESPONSE_q13;
    (*this)(7)= PLATE_RESPONSE_q23;
  }
