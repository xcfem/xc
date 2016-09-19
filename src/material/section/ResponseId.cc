//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
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

// XC::ResponseId::ResponseId(int *data, int size, bool cleanIt)
//   : ID(data,size,cleanIt) {}

XC::ResponseId::ResponseId(const ID &otro)
  : ID(otro) {}

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
        std::cerr << "ResponseId::StringToRespId; la cadena: '"
                  << str << "' no corresponde a ningún esfuerzo." << std::endl;
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
        std::cerr << "El código: '" << code
                  << "' no corresponde a nigún esfuerzo."
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

//! @brief Constructor; asigna los códigos a los gdl a los que el material aporta rigidez.
XC::RespPMz::RespPMz(void)
  : ResponseId(2)
  {
    (*this)(0)= SECTION_RESPONSE_P; // P is the first quantity
    (*this)(1)= SECTION_RESPONSE_MZ; // Mz is the second
  }

//! @brief Constructor; asigna los códigos a los gdl a los que el material aporta rigidez.
XC::RespPMzV::RespPMzV(void)
  : ResponseId(3)
  {
    (*this)(0)= SECTION_RESPONSE_P; // P is the first quantity
    (*this)(1)= SECTION_RESPONSE_MZ; // Mz is the second
    (*this)(2)= SECTION_RESPONSE_VY; // Vy is the third
  }

//! @brief Constructor; asigna los códigos a los gdl a los que el material aporta rigidez.
XC::RespVyP::RespVyP(void)
  : ResponseId(2)
  {
    (*this)(0)= SECTION_RESPONSE_VY; // Vy is the first quantity
    (*this)(1)= SECTION_RESPONSE_P; // P is the second
  }

//! @brief Constructor; asigna los códigos a los gdl a los que el material aporta rigidez.
XC::RespPMzMy::RespPMzMy(void)
  : ResponseId(3)
  {
    (*this)(0)= SECTION_RESPONSE_P; // P is the first quantity
    (*this)(1)= SECTION_RESPONSE_MZ; // Mz is the second
    (*this)(2)= SECTION_RESPONSE_MY; // My is the third 
  }

//! @brief Constructor; asigna los códigos a los gdl a los que el material aporta rigidez.
XC::RespPMzMyT::RespPMzMyT(void)
  : ResponseId(4)
  {
    (*this)(0)= SECTION_RESPONSE_P; // P is the first quantity
    (*this)(1)= SECTION_RESPONSE_MZ; // Mz is the second
    (*this)(2)= SECTION_RESPONSE_MY; // My is the third 
    (*this)(3)= SECTION_RESPONSE_T; // T is the fourth
  }

//! @brief Constructor; asigna los códigos a los gdl a los que el material aporta rigidez.
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

//! @brief Constructor; asigna los códigos a los gdl a los que el material aporta rigidez.
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

//! @brief Constructor; asigna los códigos a los gdl a los que el material aporta rigidez.
XC::RespPVyMz::RespPVyMz(void)
  : ResponseId(3)
  {
    (*this)(0)= SECTION_RESPONSE_P; // P is the first quantity
    (*this)(1)= SECTION_RESPONSE_VY; // Vy is the second
    (*this)(2)= SECTION_RESPONSE_MZ; // Mz is the third 
  }

//! @brief Constructor; asigna las etiquetas a los gdl a los que el material aporta rigidez.
XC::RespMembraneMaterial::RespMembraneMaterial(void)
  : ResponseId(3)
  {
    (*this)(0)= MEMBRANE_RESPONSE_n1;
    (*this)(1)= MEMBRANE_RESPONSE_n2;
    (*this)(2)= MEMBRANE_RESPONSE_n12;
  }

//! @brief Constructor; asigna las etiquetas a los gdl a los que el material aporta rigidez.
XC::RespPlateMaterial::RespPlateMaterial(void)
  : ResponseId(5)
  {
    (*this)(0)= PLATE_RESPONSE_m1;
    (*this)(1)= PLATE_RESPONSE_m2;
    (*this)(2)= PLATE_RESPONSE_m12;
    (*this)(3)= PLATE_RESPONSE_q13;
    (*this)(4)= PLATE_RESPONSE_q23;
  }

//! @brief Constructor; asigna las etiquetas a los gdl a los que el material aporta rigidez.
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
