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
//ElasticPlateBase.cc

#include <material/section/plate_section/ElasticPlateBase.h>
#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "utility/actor/actor/MovableVector.h"


//parameters
const double XC::ElasticPlateBase::five6 = 5.0/6.0 ; //shear correction

//! @brief Constuctor.
XC::ElasticPlateBase::ElasticPlateBase(int tag,int classTag)
  : XC::SectionForceDeformation(tag, classTag), E(0.0), nu(0.0), h(0.0) {}

//null constructor
XC::ElasticPlateBase::ElasticPlateBase(int classTag)
  :XC::SectionForceDeformation( 0, classTag),
   E(0.0), nu(0.0), h(0.0) { }

//full constructor
XC::ElasticPlateBase::ElasticPlateBase(int tag, int classTag,
                                           double young,
                                           double poisson,
                                           double thickness)
  :XC::SectionForceDeformation(tag,classTag), E(young), nu(poisson),h(thickness) {}

//! @brief Lee un objeto XC::ElasticPlateBase desde archivo
bool XC::ElasticPlateBase::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(ElasticPlateBase) Procesando comando: " << cmd << std::endl;
    if(cmd == "E")
      {
        E= interpretaDouble(status.GetString());
        return true;
      }
    else if(cmd == "nu")
      {
        nu= interpretaDouble(status.GetString());
        return true;
      }
    else if(cmd == "h")
      {
        h= interpretaDouble(status.GetString());
        return true;
      }
    else
      return SectionForceDeformation::procesa_comando(status);
  }


//swap history variables
int XC::ElasticPlateBase::commitState(void) 
  { return 0 ; }

//revert to last saved state
int XC::ElasticPlateBase::revertToLastCommit(void)
  { return 0 ; }

//revert to start
int XC::ElasticPlateBase::revertToStart(void)
  { return 0; }

//! @brief Envía los datos a través del canal que se pasa como parámetro.
int XC::ElasticPlateBase::sendData(CommParameters &cp)
  {
    int res= SectionForceDeformation::sendData(cp);
    res+= cp.sendDoubles(E,nu,h,getDbTagData(),CommMetaData(5));
    return res;
  }

//! @brief Recibe los datos a través del canal que se pasa como parámetro.
int XC::ElasticPlateBase::recvData(const CommParameters &cp)
  {
    int res= SectionForceDeformation::recvData(cp);
    res+= cp.receiveDoubles(E,nu,h,getDbTagData(),CommMetaData(5));
    return res;
  }

//! \brief Devuelve la propiedad del objeto cuyo código (de la propiedad) se pasa
//! como parámetro.
any_const_ptr XC::ElasticPlateBase::GetProp(const std::string &cod) const
  {
    if(cod=="getE")
      return any_const_ptr(E);
    else if(cod=="getCoefPoisson")
      return any_const_ptr(nu);
    else if(cod=="getThickness")
      return any_const_ptr(h);
    else if(cod=="getMembraneModulus")
      {
        tmp_gp_dbl= membraneModulus();
        return any_const_ptr(tmp_gp_dbl);
      }
    else if(cod=="getShearModulus")
      {
        tmp_gp_dbl= shearModulus();
        return any_const_ptr(tmp_gp_dbl);
      }
    else if(cod=="getBendingModulus")
      {
        tmp_gp_dbl= bendingModulus();
        return any_const_ptr(tmp_gp_dbl);
      }
    else
      return SectionForceDeformation::GetProp(cod);
  }
