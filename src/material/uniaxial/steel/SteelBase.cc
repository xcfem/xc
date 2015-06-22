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
//SteelBase.cc


#include <material/uniaxial/steel/SteelBase.h>
#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "utility/actor/actor/MovableVector.h"

//! @brief Constructor.
XC::SteelBase::SteelBase(int tag,int classTag,const double &Fy,const double &e0,const double &B,const double &A1,const double &A2,const double &A3,const double &A4)
  : UniaxialMaterial(tag,classTag), fy(Fy),E0(e0),b(B),a1(A1),a2(A2),a3(A3),a4(A4) {}

XC::SteelBase::SteelBase(int tag,int classTag)
  :UniaxialMaterial(tag,classTag), fy(0.0),E0(0.0),b(0.0),a1(0.0), a2(0.0), a3(0.0), a4(0.0) {}

//! @brief Assigns intial Young's modulus.
void XC::SteelBase::setInitialTangent(const double &d)
  {
    E0= d;
    setup_parameters(); //Inicializa las variables históricas.
  }

//! @brief Returns intial Young's modulus.
double XC::SteelBase::getInitialTangent(void) const
  { return E0; }

//! @brief Assigns yield stress.
void XC::SteelBase::setFy(const double &d)
  {
    fy= d;
    setup_parameters(); //Inicializa las variables históricas.
  }

//! @brief Returns yield stress.
double XC::SteelBase::getFy(void) const
  { return fy; }


//! @brief Lee un objeto XC::SteelBase desde archivo
bool XC::SteelBase::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(SteelBase) Procesando comando: " << cmd << std::endl;
    if(cmd == "fy")
      {
        setFy(interpretaDouble(status.GetString()));
        return true;
      }
    else if(cmd == "E")
      {
        setInitialTangent(interpretaDouble(status.GetString()));
        return true;
      }
    else if(cmd == "b")
      {
        b= interpretaDouble(status.GetString());
        return true;
      }
    else if(cmd == "a1")
      {
        a1= interpretaDouble(status.GetString());
        return true;
      }
    else if(cmd == "a2")
      {
        a2= interpretaDouble(status.GetString());
        return true;
      }
    else if(cmd == "a3")
      {
        a3= interpretaDouble(status.GetString());
        return true;
      }
    else if(cmd == "a4")
      {
        a4= interpretaDouble(status.GetString());
        return true;
      }
    else
      return UniaxialMaterial::procesa_comando(status);
  }

//! @brief Envía los miembros del objeto a través del canal que se pasa como parámetro.
int XC::SteelBase::sendData(CommParameters &cp)
  {
    int res= UniaxialMaterial::sendData(cp);
    res+= cp.sendDoubles(fy,E0,b,getDbTagData(),CommMetaData(2));
    res+= cp.sendDoubles(a1,a2,a3,a4,getDbTagData(),CommMetaData(3));
    return res;
  }

//! @brief Recibe los miembros del objeto a través del canal que se pasa como parámetro.
int XC::SteelBase::recvData(const CommParameters &cp)
  {
    int res= UniaxialMaterial::recvData(cp);
    res+= cp.receiveDoubles(fy,E0,b,getDbTagData(),CommMetaData(2));
    res+= cp.receiveDoubles(a1,a2,a3,a4,getDbTagData(),CommMetaData(3));
    return res;
  }

//! \brief Devuelve la propiedad del objeto cuyo código (de la propiedad) se pasa
//! como parámetro.
//!
//! Soporta los códigos:
any_const_ptr XC::SteelBase::GetProp(const std::string &cod) const
  {
    if(cod=="fy")
      return any_const_ptr(fy);
    else if(cod=="E0")
      return any_const_ptr(E0);
    else if(cod=="b")
      return any_const_ptr(b);
    else if(cod=="a1")
      return any_const_ptr(a1);
    else if(cod=="a2")
      return any_const_ptr(a2);
    else if(cod=="a3")
      return any_const_ptr(a3);
    else if(cod=="a4")
      return any_const_ptr(a4);
    else if(cod== "Esh")
      return any_const_ptr(getEsh());
    else if(cod== "epsy")
      return any_const_ptr(getEpsy());
    else
      return UniaxialMaterial::GetProp(cod);
  }
