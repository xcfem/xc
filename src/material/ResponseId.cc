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
//ResponseId.cc

#include "ResponseId.h"
#include "utility/actor/actor/MovableID.h"
#include "utility/utils/misc_utils/colormod.h"

XC::ResponseId::ResponseId(int sz)
  : ID(sz) {}

XC::ResponseId::ResponseId(const std::deque<std::string> &indices)
  : ID(indices.size())
  {
    const size_t sz= indices.size();
    for( size_t i= 0;i<sz;i++)
      (*this)[i]= this->getComponentIdFromString(indices[i]);
  }

XC::ResponseId::ResponseId(const std::vector<std::string> &indices)
  : ID(indices.size())
  {
    const size_t sz= indices.size();
    for( size_t i= 0;i<sz;i++)
      (*this)[i]= getComponentIdFromString(indices[i]);
  }

XC::ResponseId::ResponseId(const boost::python::list &l)
  : ID(boost::python::len(l))
  {
    const size_t sz= this->Size();
    for(size_t i=0; i<sz; i++)
      (*this)[i]= getComponentIdFromString(boost::python::extract<std::string>(l[i]));
  }

// XC::ResponseId::ResponseId(int *dt, int size, bool cleanIt)
//   : ID(dt,size,cleanIt) {}

XC::ResponseId::ResponseId(const ID &other)
  : ID(other) {}

//! @brief Return true if the given response identifier is found in this
//! object.
bool XC::ResponseId::hasResponse(const int &ri) const
  {
    const int i= this->getLocation(ri);
    return (i>0);
  }

//! @brief Return the response identifiers in text form as comma separated
//! values in a string.
std::string XC::ResponseId::getString(void) const
  {
    std::string retval= "";
    const int sz= Size();
    if(sz>0)
      {
        retval+= getStringFromComponentId((*this)(0));
        for(int i= 1;i<sz;i++)
          retval+= "," + getStringFromComponentId((*this)(i));
      }
    return retval;
  }

//! @brief Return the response identifiers in text form in a Python list.
boost::python::list XC::ResponseId::getStringIdentifiers(void) const
  {
    boost::python::list retval;
    const int sz= Size();
    if(sz>0)
      {
        for(int i= 0;i<sz;i++)
          retval.append(getStringFromComponentId((*this)(i)));
      }
    return retval;
  }

//! @brief Return the response identifiers in a Python list.
boost::python::list XC::ResponseId::getIdentifiers(void) const
  {
    boost::python::list retval;
    const int sz= Size();
    if(sz>0)
      {
        for(int i= 0;i<sz;i++)
          retval.append((*this)(i));
      }
    return retval;
  }

//! @brief Return the strain or stress vector indexes corresponding to the given
//! code.
std::deque<int> XC::ResponseId::getComponentIndexesFromCode(const int &defID) const
  {
    std::deque<int> retval;
    const int order= this->Size();
    for(int i= 0;i<order;i++)
      if((*this)(i) == defID)
        retval.push_back(i);
    return retval;
  }

//! @brief Return the strain or stress vector indexes corresponding to the given
//! code.
std::deque<int> XC::ResponseId::getComponentIndexesFromCode(const std::string &code) const
  {
    const int defID= getComponentIdFromString(code);
    return this->getComponentIndexesFromCode(defID);
  }

//! @brief Return the strain or stress vector indexes corresponding to the given
//! code.
boost::python::list XC::ResponseId::getComponentIndexesFromCodePy(const int &defID) const
  {
    const std::deque<int> tmp= this->getComponentIndexesFromCode(defID);
    boost::python::list retval;
    const int sz= tmp.size();
    if(sz>0)
      {
        for(int i= 0;i<sz;i++)
          retval.append(tmp[i]);
      }
    return retval;
  }

//! @brief Return the strain or stress vector indexes corresponding to the given
//! code.
boost::python::list XC::ResponseId::getComponentIndexesFromCodePy(const std::string &code) const
  {
    const int defID= getComponentIdFromString(code);
    return this->getComponentIndexesFromCodePy(defID);
  }

//! @brief Return the identifier of the response component from the given
//! label (something like 'Mz' or 'N', etc.)
int XC::ResponseId::getComponentIdFromString(const std::string &str) const
  {
    // Generic ResponseId behaviour. This is needed to make
    // SectionAggregation mechanism to work.
    int retval= -1;
    if(str == "Mz")
      retval= SECTION_RESPONSE_MZ;
    else if((str=="P") or (str=="N"))
      retval= SECTION_RESPONSE_P;
    else if(str =="Vy")
      retval= SECTION_RESPONSE_VY;
    else if(str =="My")
      retval= SECTION_RESPONSE_MY;
    else if(str =="Vz")
      retval= SECTION_RESPONSE_VZ;
    else if(str =="T")
      retval= SECTION_RESPONSE_T;
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
    return retval;
  }

//! @brief Return the label corresponding to the response component identified
//! by the given code.
std::string XC::ResponseId::getStringFromComponentId(const int &code) const
  {
    // Generic ResponseId behaviour. This is needed to make
    // SectionAggregation mechanism to work.
    std::string retval= "nil";
    switch(code)
      {
      case SECTION_RESPONSE_P:
        retval= "P";
        break;
      case SECTION_RESPONSE_MZ:
        retval= "Mz";
        break;
      case SECTION_RESPONSE_VY:
        retval= "Vy";
        break;
      case SECTION_RESPONSE_MY:
        retval= "My";
        break;
      case SECTION_RESPONSE_VZ:
        retval= "Vz";
        break;
      case SECTION_RESPONSE_T:
        retval= "T";
        break;
      case MEMBRANE_RESPONSE_n1:
        retval= "n1";
        break;
      case MEMBRANE_RESPONSE_n2:
        retval= "n2";
        break;
      case MEMBRANE_RESPONSE_n12:
        retval= "n12";
        break;
      case PLATE_RESPONSE_m1:
        retval= "m1";
        break;
      case PLATE_RESPONSE_m2:
        retval= "m2";
        break;
      case PLATE_RESPONSE_m12:
        retval= "m12";
        break;
      case PLATE_RESPONSE_q13:
        retval= "q13";
        break;
      case PLATE_RESPONSE_q23:
        retval= "q23";
        break;
      default:
        std::cerr << Color::red << "ResponseId::" << __FUNCTION__
		  << "; unknown code: '" << code
                  << "'."
                  << Color::def << std::endl;
        retval= "nil";
        break;
      }
    return retval;
   }

//! @brief Constructor; sets the identifiers for the stiffness contribution of the material.
XC::RespP::RespP(void)
  : ResponseId(1)
  {
    (*this)(0)= SECTION_RESPONSE_P; // P is the first and only quantity
  }

//! @brief Return the identifier of the response component from the given
//! label (something like 'Mz' or 'N', etc.)
int XC::RespP::getComponentIdFromString(const std::string &str) const
  {
    int retval= -1;
    if((str=="P") or (str=="N") or (str=="epsilon") or (str=="epsilon_1") or (str=="epsilon_11"))
      retval= SECTION_RESPONSE_P;
    else
      retval= -1;
    return retval;
  }

//! @brief Return the label corresponding to the response component identified
//! by the given code.
std::string XC::RespP::getStringFromComponentId(const int &code) const
  {
    std::string retval= "nil";
    switch(code)
      {
      case SECTION_RESPONSE_P:
        return "P";
        break;
      default:
        std::cerr << Color::red << "ResponseId::" << __FUNCTION__
		  << "; unknown code: '" << code
                  << "'."
                  << Color::def << std::endl;
        retval= "nil";
        break;
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

//! @brief Return the identifier of the response component from the given
//! label (something like 'Mz' or 'N', etc.)
int XC::RespPMz::getComponentIdFromString(const std::string &str) const
  {
    int retval= -1;
    if((str == "Mz") or (str == "kappa_z") or (str == "kappa_1") or (str == "M") or (str == "kappa"))
      retval= SECTION_RESPONSE_MZ;
    else if((str=="P") or (str=="N") or (str=="epsilon") or (str=="epsilon_1") or (str=="epsilon_11"))
      retval= SECTION_RESPONSE_P;
    else
      retval= -1;
    return retval;
  }

//! @brief Return the label corresponding to the response component identified
//! by the given code.
std::string XC::RespPMz::getStringFromComponentId(const int &code) const
  {
    std::string retval= "nil";
    switch(code)
      {
      case SECTION_RESPONSE_P:
        return "P";
        break;
      case SECTION_RESPONSE_MZ:
        return "Mz";
        break;
      default:
        std::cerr << Color::red << "ResponseId::" << __FUNCTION__
		  << "; unknown code: '" << code
                  << "'."
                  << Color::def << std::endl;
        retval= "nil";
        break;
      }
    return retval;
   }

//! @brief Constructor; sets the identifiers for the stiffness contribution of the material.
XC::RespPMzV::RespPMzV(void)
  : ResponseId(3)
  {
    (*this)(0)= SECTION_RESPONSE_P; // P is the first quantity
    (*this)(1)= SECTION_RESPONSE_MZ; // Mz is the second
    (*this)(2)= SECTION_RESPONSE_VY; // Vy is the third
  }

//! @brief Return the identifier of the response component from the given
//! label (something like 'Mz' or 'N', etc.)
int XC::RespPMzV::getComponentIdFromString(const std::string &str) const
  {
    int retval= -1;
    if((str == "Mz") or (str == "kappa_z") or (str == "kappa_1") or (str == "M") or (str == "kappa"))
      retval= SECTION_RESPONSE_MZ;
    else if((str=="P") or (str=="N") or (str=="epsilon") or (str=="epsilon_1") or (str=="epsilon_11"))
      retval= SECTION_RESPONSE_P;
    else if((str == "Vy") or (str == "gamma_y") or (str == "V") or (str == "gamma"))
      retval= SECTION_RESPONSE_VY;
    else
      retval= -1;
    return retval;
  }

//! @brief Return the label corresponding to the response component identified
//! by the given code.
std::string XC::RespPMzV::getStringFromComponentId(const int &code) const
  {
    std::string retval= "nil";
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
      default:
        std::cerr << Color::red << "ResponseId::" << __FUNCTION__
		  << "; unknown code: '" << code
                  << "'."
                  << Color::def << std::endl;
        retval= "nil";
        break;
      }
    return retval;
   }

//! @brief Constructor; sets the identifiers for the stiffness contribution of the material.
XC::RespVyP::RespVyP(void)
  : ResponseId(2)
  {
    (*this)(0)= SECTION_RESPONSE_VY; // Vy is the first quantity
    (*this)(1)= SECTION_RESPONSE_P; // P is the second
  }

//! @brief Return the identifier of the response component from the given
//! label (something like 'Mz' or 'N', etc.)
int XC::RespVyP::getComponentIdFromString(const std::string &str) const
  {
    int retval= -1;
    if((str=="P") or (str=="N") or (str=="epsilon") or (str=="epsilon_1") or (str=="epsilon_11"))
      retval= SECTION_RESPONSE_P;
    else if((str == "Vy") or (str == "gamma_y") or (str == "V") or (str == "gamma"))
      retval= SECTION_RESPONSE_VY;
    else
      retval= -1;
    return retval;
  }

//! @brief Return the label corresponding to the response component identified
//! by the given code.
std::string XC::RespVyP::getStringFromComponentId(const int &code) const
  {
    std::string retval= "nil";
    switch(code)
      {
      case SECTION_RESPONSE_P:
        return "P";
        break;
      case SECTION_RESPONSE_VY:
        return "Vy";
        break;
      default:
        std::cerr << Color::red << "ResponseId::" << __FUNCTION__
		  << "; unknown code: '" << code
                  << "'."
                  << Color::def << std::endl;
        retval= "nil";
        break;
      }
    return retval;
   }

//! @brief Constructor; sets the identifiers for the stiffness contribution of the material.
XC::RespPMzMy::RespPMzMy(void)
  : ResponseId(3)
  {
    (*this)(0)= SECTION_RESPONSE_P; // P is the first quantity
    (*this)(1)= SECTION_RESPONSE_MZ; // Mz is the second
    (*this)(2)= SECTION_RESPONSE_MY; // My is the third 
  }

//! @brief Return the identifier of the response component from the given
//! label (something like 'Mz' or 'N', etc.)
int XC::RespPMzMy::getComponentIdFromString(const std::string &str) const
  {
    int retval= -1;
    if((str == "Mz") or (str == "kappa_z") or (str == "kappa_1") or (str == "M") or (str == "kappa"))
      retval= SECTION_RESPONSE_MZ;
    else if((str=="P") or (str=="N") or (str=="epsilon") or (str=="epsilon_1") or (str=="epsilon_11"))
      retval= SECTION_RESPONSE_P;
    else if((str =="My") or (str == "kappa_y"))
      retval= SECTION_RESPONSE_MY;
    else
      retval= -1;
    return retval;
  }

//! @brief Return the label corresponding to the response component identified
//! by the given code.
std::string XC::RespPMzMy::getStringFromComponentId(const int &code) const
  {
    std::string retval= "nil";
    switch(code)
      {
      case SECTION_RESPONSE_P:
        return "P";
        break;
      case SECTION_RESPONSE_MZ:
        return "Mz";
        break;
      case SECTION_RESPONSE_MY:
        return "My";
        break;
      default:
        std::cerr << Color::red << "ResponseId::" << __FUNCTION__
		  << "; unknown code: '" << code
                  << "'."
                  << Color::def << std::endl;
        retval= "nil";
        break;
      }
    return retval;
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

//! @brief Return the identifier of the response component from the given
//! label (something like 'Mz' or 'N', etc.)
int XC::RespPMzMyT::getComponentIdFromString(const std::string &str) const
  {
    int retval= -1;
    if((str == "Mz") or (str == "kappa_z") or (str == "kappa_1")  or (str == "M") or (str == "kappa"))
      retval= SECTION_RESPONSE_MZ;
    else if((str=="P") or (str=="N") or (str=="epsilon") or (str=="epsilon_1") or (str=="epsilon_11"))
      retval= SECTION_RESPONSE_P;
    else if((str =="My") or (str == "kappa_y"))
      retval= SECTION_RESPONSE_MY;
    else if(str =="T")
      retval= SECTION_RESPONSE_T;
    else
      retval= -1;
    return retval;
  }

//! @brief Return the label corresponding to the response component identified
//! by the given code.
std::string XC::RespPMzMyT::getStringFromComponentId(const int &code) const
  {
    std::string retval= "nil";
    switch(code)
      {
      case SECTION_RESPONSE_P:
        return "P";
        break;
      case SECTION_RESPONSE_MZ:
        return "Mz";
        break;
      case SECTION_RESPONSE_MY:
        return "My";
        break;
      case SECTION_RESPONSE_T:
        return "T";
        break;
      default:
        std::cerr << Color::red << "ResponseId::" << __FUNCTION__
		  << "; unknown code: '" << code
                  << "'."
                  << Color::def << std::endl;
        retval= "nil";
        break;
      }
    return retval;
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

//! @brief Return the identifier of the response component from the given
//! label (something like 'Mz' or 'N', etc.)
int XC::RespPMzVyMyVzT::getComponentIdFromString(const std::string &str) const
  {
    int retval= -1;
    if((str == "Mz") or (str == "kappa_z") or (str == "kappa_1") or (str == "M") or (str == "kappa"))
      retval= SECTION_RESPONSE_MZ;
    else if((str=="P") or (str=="N") or (str=="epsilon") or (str=="epsilon_1") or (str=="epsilon_11"))
      retval= SECTION_RESPONSE_P;
    else if((str == "Vy") or (str == "gamma_y") or (str == "V") or (str == "gamma"))
      retval= SECTION_RESPONSE_VY;
    else if((str =="My") or (str == "kappa_y"))
      retval= SECTION_RESPONSE_MY;
    else if((str == "Vz") or (str == "gamma_z"))
      retval= SECTION_RESPONSE_VZ;
    else if(str =="T")
      retval= SECTION_RESPONSE_T;
    else
      retval= -1;
    return retval;
  }

//! @brief Return the label corresponding to the response component identified
//! by the given code.
std::string XC::RespPMzVyMyVzT::getStringFromComponentId(const int &code) const
  {
    std::string retval= "nil";
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
      default:
        std::cerr << Color::red << "ResponseId::" << __FUNCTION__
		  << "; unknown code: '" << code
                  << "'."
                  << Color::def << std::endl;
        retval= "nil";
        break;
      }
    return retval;
   }

//! @brief Constructor; sets the identifiers for the stiffness contribution of the material.
XC::RespFiberSectionShear2d::RespFiberSectionShear2d(void)
  : RespPMzV()
  {
    // The order doesn't change so no need to
    // repeat the initializtion.
    // (*this)(0)= SECTION_RESPONSE_P;
    // (*this)(1)= SECTION_RESPONSE_MZ;
    // (*this)(2)= SECTION_RESPONSE_VY;
  }

//! @brief Constructor; sets the identifiers for the stiffness contribution of the material.
XC::RespFiberSectionShear3d::RespFiberSectionShear3d(void)
  : RespPMzVyMyVzT()
  {
    // IMPORTANT: The order is diferent from its
    // base class.
    (*this)(0)= SECTION_RESPONSE_P;
    (*this)(1)= SECTION_RESPONSE_MZ;
    (*this)(2)= SECTION_RESPONSE_MY;
    (*this)(3)= SECTION_RESPONSE_VY;
    (*this)(4)= SECTION_RESPONSE_VZ;
    (*this)(5)= SECTION_RESPONSE_T;
  }

//! @brief Constructor; sets the identifiers for the stiffness contribution of the material.
XC::RespPVyMz::RespPVyMz(void)
  : RespPMzV()
  {
    // The order changes with respect its
    // base class.
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

//! @brief Return the identifier of the response component from the given
//! label (something like 'Mz' or 'N', etc.)
int XC::RespMembraneMaterial::getComponentIdFromString(const std::string &str) const
  {
    int retval= -1;
    if((str =="n1") or (str == "epsilon") or (str == "epsilon_1") or (str == "epsilon_x") or (str == "epsilon_11") or (str == "epsilon_xx"))
      retval= MEMBRANE_RESPONSE_n1;
    else if((str =="n2") or (str == "epsilon_2") or (str == "epsilon_y") or (str == "epsilon_22") or (str == "epsilon_yy"))
      retval= MEMBRANE_RESPONSE_n2;
    else if((str =="n12") or (str == "epsilon_12") or (str == "epsilon_xy"))
      retval= MEMBRANE_RESPONSE_n12;
    else
      retval= -1;
    return retval;
  }

//! @brief Return the label corresponding to the response component identified
//! by the given code.
std::string XC::RespMembraneMaterial::getStringFromComponentId(const int &code) const
  {
    std::string retval= "nil";
    switch(code)
      {
      case MEMBRANE_RESPONSE_n1:
        return "n1";
        break;
      case MEMBRANE_RESPONSE_n2:
        return "n2";
        break;
      case MEMBRANE_RESPONSE_n12:
        return "n12";
        break;
      default:
        std::cerr << Color::red << "ResponseId::" << __FUNCTION__
		  << "; unknown code: '" << code
                  << "'."
                  << Color::def << std::endl;
        retval= "nil";
        break;
      }
    return retval;
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

//! @brief Return the identifier of the response component from the given
//! label (something like 'Mz' or 'N', etc.)
int XC::RespPlateMaterial::getComponentIdFromString(const std::string &str) const
  {
    int retval= -1;
    if((str == "m1") or (str == "kappa_1") or (str == "kappa_z"))
      retval= PLATE_RESPONSE_m1;
    else if((str == "m2") or (str == "kappa_2"))
      retval= PLATE_RESPONSE_m2;
    else if((str == "m12") or (str == "kappa_12"))
      retval= PLATE_RESPONSE_m12;
    else if((str == "q13") or (str == "gamma_13"))
      retval= PLATE_RESPONSE_q13;
    else if((str == "q23") or (str == "gamma_23"))
      retval= PLATE_RESPONSE_q23;
    else
      retval= -1;
    return retval;
  }

//! @brief Return the label corresponding to the response component identified
//! by the given code.
std::string XC::RespPlateMaterial::getStringFromComponentId(const int &code) const
  {
    std::string retval= "nil";
    switch(code)
      {
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
        std::cerr << Color::red << "ResponseId::" << __FUNCTION__
		  << "; unknown code: '" << code
                  << "'."
                  << Color::def << std::endl;
        retval= "nil";
        break;
      }
    return retval;
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

int XC::RespShellMaterial::getComponentIdFromString(const std::string &str) const
  {
    int retval= -1;
    if((str =="n1") or (str == "epsilon") or (str == "epsilon_1") or (str == "epsilon_x") or (str == "epsilon_11") or (str == "epsilon_xx"))
      retval= MEMBRANE_RESPONSE_n1;
    else if((str =="n2") or (str == "epsilon_2") or (str == "epsilon_y") or (str == "epsilon_22") or (str == "epsilon_yy"))
      retval= MEMBRANE_RESPONSE_n2;
    else if((str =="n12") or (str == "epsilon_12") or (str == "epsilon_xy"))
      retval= MEMBRANE_RESPONSE_n12;
    else if((str == "m1") or (str == "kappa_1") or (str == "kappa_z"))
      retval= PLATE_RESPONSE_m1;
    else if((str == "m2") or (str == "kappa_2"))
      retval= PLATE_RESPONSE_m2;
    else if((str == "m12") or (str == "kappa_12"))
      retval= PLATE_RESPONSE_m12;
    else if((str == "q13") or (str == "gamma_13"))
      retval= PLATE_RESPONSE_q13;
    else if((str == "q23") or (str == "gamma_23"))
      retval= PLATE_RESPONSE_q23;
    else
      retval= -1;
    return retval;
  }

//! @brief Return the label corresponding to the response component identified
//! by the given code.
std::string XC::RespShellMaterial::getStringFromComponentId(const int &code) const
  {
    std::string retval= "nil";
    switch(code)
      {
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
        std::cerr << Color::red << "ResponseId::" << __FUNCTION__
		  << "; unknown code: '" << code
                  << "'."
                  << Color::def << std::endl;
        retval= "nil";
        break;
      }
    return retval;
   }

XC::RespSolidMecanics1DMaterial::RespSolidMecanics1DMaterial(void)
  : ResponseId(1)
  {
    (*this)(0)= SOLID_MECHANICS_RESPONSE_sigma_11;
  };

int XC::RespSolidMecanics1DMaterial::getComponentIdFromString(const std::string &str) const
  {
    int retval= -1;
    if((str =="sigma_11") or (str=="sigma") or (str=="epsilon_11") or (str=="epsilon") or (str=="epsilon_1"))
      retval= SOLID_MECHANICS_RESPONSE_sigma_11;
    else if((str =="sigma_xx") or (str=="epsilon_xx"))
      retval= SOLID_MECHANICS_RESPONSE_sigma_11;
    else
      retval= -1;
    return retval;
  }

//! @brief Return the label corresponding to the response component identified
//! by the given code.
std::string XC::RespSolidMecanics1DMaterial::getStringFromComponentId(const int &code) const
  {
    std::string retval= "nil";
    switch(code)
      {
      case SOLID_MECHANICS_RESPONSE_sigma_11:
	retval= "sigma_11";
	break;
      default:
        std::cerr << Color::red << "ResponseId::" << __FUNCTION__
		  << "; unknown code: '" << code
                  << "'."
                  << Color::def << std::endl;
        retval= "nil";
        break;
      }
    return retval;
  }

XC::RespSolidMecanics2DMaterial::RespSolidMecanics2DMaterial(void)
  : ResponseId(3)
  {
    (*this)(0)= SOLID_MECHANICS_RESPONSE_sigma_11;
    (*this)(1)= SOLID_MECHANICS_RESPONSE_sigma_22;
    (*this)(2)= SOLID_MECHANICS_RESPONSE_sigma_12;
  };

int XC::RespSolidMecanics2DMaterial::getComponentIdFromString(const std::string &str) const
  {
    int retval= -1;
    if((str =="sigma_11") or (str=="sigma") or (str=="epsilon_11") or (str=="epsilon") or (str=="epsilon_1"))
      retval= SOLID_MECHANICS_RESPONSE_sigma_11;
    else if((str =="sigma_22") or (str=="epsilon_22"))
      retval= SOLID_MECHANICS_RESPONSE_sigma_22;
    else if((str =="sigma_12") or (str=="epsilon_12"))
      retval= SOLID_MECHANICS_RESPONSE_sigma_12;
    else if((str =="sigma_xx") or (str=="epsilon_xx"))
      retval= SOLID_MECHANICS_RESPONSE_sigma_11;
    else if((str =="sigma_yy") or (str=="epsilon_yy"))
      retval= SOLID_MECHANICS_RESPONSE_sigma_22;
    else if((str =="sigma_xy") or (str=="epsilon_xy"))
      retval= SOLID_MECHANICS_RESPONSE_sigma_12;
    else
      retval= -1;
    return retval;
  }

//! @brief Return the label corresponding to the response component identified
//! by the given code.
std::string XC::RespSolidMecanics2DMaterial::getStringFromComponentId(const int &code) const
  {
    std::string retval= "nil";
    switch(code)
      {
      case SOLID_MECHANICS_RESPONSE_sigma_11:
	retval= "sigma_11";
	break;
      case SOLID_MECHANICS_RESPONSE_sigma_22:
	retval= "sigma_22";
	break;
      case SOLID_MECHANICS_RESPONSE_sigma_12:
	retval= "sigma_12";
	break;
      default:
        std::cerr << Color::red << "ResponseId::" << __FUNCTION__
		  << "; unknown code: '" << code
                  << "'."
                  << Color::def << std::endl;
        retval= "nil";
        break;
      }
    return retval;
  }

XC::RespSolidMecanics3DMaterial::RespSolidMecanics3DMaterial(void)
  : ResponseId(6)
  {
    (*this)(0)= SOLID_MECHANICS_RESPONSE_sigma_11;
    (*this)(1)= SOLID_MECHANICS_RESPONSE_sigma_22;
    (*this)(2)= SOLID_MECHANICS_RESPONSE_sigma_33;
    (*this)(3)= SOLID_MECHANICS_RESPONSE_sigma_12;
    (*this)(4)= SOLID_MECHANICS_RESPONSE_sigma_23;
    (*this)(5)= SOLID_MECHANICS_RESPONSE_sigma_13;
  };

int XC::RespSolidMecanics3DMaterial::getComponentIdFromString(const std::string &str) const
  {
    int retval= -1;
    if((str =="sigma_11") or (str=="sigma") or (str=="epsilon_11") or (str=="epsilon") or (str=="epsilon_1"))
      retval= SOLID_MECHANICS_RESPONSE_sigma_11;
    else if((str =="sigma_22") or (str=="epsilon_22"))
      retval= SOLID_MECHANICS_RESPONSE_sigma_22;
    else if((str =="sigma_33") or (str=="epsilon_33"))
      retval= SOLID_MECHANICS_RESPONSE_sigma_33;
    else if((str =="sigma_12") or (str=="epsilon_12"))
      retval= SOLID_MECHANICS_RESPONSE_sigma_12;
    else if((str =="sigma_13") or (str=="epsilon_13"))
      retval= SOLID_MECHANICS_RESPONSE_sigma_13;
    else if((str =="sigma_23") or (str=="epsilon_23"))
      retval= SOLID_MECHANICS_RESPONSE_sigma_23;
    else if((str =="sigma_xx") or (str=="epsilon_xx"))
      retval= SOLID_MECHANICS_RESPONSE_sigma_11;
    else if((str =="sigma_yy") or (str=="epsilon_yy"))
      retval= SOLID_MECHANICS_RESPONSE_sigma_22;
    else if((str =="sigma_zz") or (str=="epsilon_zz"))
      retval= SOLID_MECHANICS_RESPONSE_sigma_33;
    else if((str =="sigma_xy") or (str=="epsilon_xy"))
      retval= SOLID_MECHANICS_RESPONSE_sigma_12;
    else if((str =="sigma_xz") or (str=="epsilon_xz"))
      retval= SOLID_MECHANICS_RESPONSE_sigma_13;
    else if((str =="sigma_yz") or (str=="epsilon_yz"))
      retval= SOLID_MECHANICS_RESPONSE_sigma_23;
    return retval;
  }

//! @brief Return the label corresponding to the response component identified
//! by the given code.
std::string XC::RespSolidMecanics3DMaterial::getStringFromComponentId(const int &code) const
  {
    std::string retval= "nil";
    switch(code)
      {
      case SOLID_MECHANICS_RESPONSE_sigma_11:
	retval= "sigma_11";
	break;
      case SOLID_MECHANICS_RESPONSE_sigma_22:
	retval= "sigma_22";
	break;
      case SOLID_MECHANICS_RESPONSE_sigma_33:
	retval= "sigma_33";
	break;
      case SOLID_MECHANICS_RESPONSE_sigma_12:
	retval= "sigma_12";
	break;
      case SOLID_MECHANICS_RESPONSE_sigma_23:
	retval= "sigma_23";
	break;
      case SOLID_MECHANICS_RESPONSE_sigma_13:
	retval= "sigma_13";
	break;
      default:
        std::cerr << Color::red << "ResponseId::" << __FUNCTION__
		  << "; unknown code: '" << code
                  << "'."
                  << Color::def << std::endl;
        retval= "nil";
        break;
      }
    return retval;
  }

