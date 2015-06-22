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
//BeamStrainLoad.cpp

#include <domain/load/beam_loads/BeamStrainLoad.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include "xc_utils/src/base/CmdStatus.h"
#include "utility/matrix/ID.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "material/section/ResponseId.h"

XC::BeamStrainLoad::BeamStrainLoad(int tag, const XC::ID &theElementTags)
  :BeamLoad(tag, LOAD_TAG_BeamStrainLoad, theElementTags) {}

XC::BeamStrainLoad::BeamStrainLoad(int tag)
  :BeamLoad(tag, LOAD_TAG_BeamStrainLoad) {}

//! @brief Lee un objeto BeamStrainLoad desde archivo
bool XC::BeamStrainLoad::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(BeamStrainLoad) Procesando comando: " << cmd << std::endl;
    if(cmd == "plano_deformacion_1")
      {
        planoDefDorsal.LeeCmd(status);
        return true;
      }
    else if(cmd == "plano_deformacion_2")
      {
        planoDefFrontal.LeeCmd(status);
        return true;
      }
    return BeamLoad::procesa_comando(status);
  }

//! @brief Envía los datos a través del canal que se pasa como parámetro.
int XC::BeamStrainLoad::sendData(CommParameters &cp)
  {
    int res= BeamLoad::sendData(cp);
    res+= cp.sendMovable(planoDefDorsal,getDbTagData(),CommMetaData(5));
    res+= cp.sendMovable(planoDefFrontal,getDbTagData(),CommMetaData(6));
    return res;
  }

//! @brief Recibe los datos a través del canal que se pasa como parámetro.
int XC::BeamStrainLoad::recvData(const CommParameters &cp)
  {
    int res= BeamLoad::recvData(cp);
    res+= cp.receiveMovable(planoDefDorsal,getDbTagData(),CommMetaData(5));;
    res+= cp.receiveMovable(planoDefFrontal,getDbTagData(),CommMetaData(6));;
    return res;
  }

int XC::BeamStrainLoad::sendSelf(CommParameters &cp)
  {
    static ID data(3);
    int res= sendData(cp);
    const int dataTag= getDbTag();
    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "BeamStrainLoad::sendSelf() - failed to send extra data\n";    
    return res;
  }

int XC::BeamStrainLoad::recvSelf(const CommParameters &cp)
  {
    static ID data(3);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "BeamStrainLoad::recvSelf() - data could not be received\n" ;
    else
      res+= recvData(cp);
    return res;
  }

const XC::Vector &XC::BeamStrainLoad::getSection1Deformation(const size_t &order,const ResponseId &code) const
  { return planoDefDorsal.getDeformation(order,code); }

const XC::Vector &XC::BeamStrainLoad::getSection2Deformation(const size_t &order,const ResponseId &code) const
  { return planoDefFrontal.getDeformation(order,code); }

//! Devuelve la propiedad del objeto cuyo código se pasa
//! como parámetro.
any_const_ptr XC::BeamStrainLoad::GetProp(const std::string &cod) const
  {
    if(cod == "getPlanoDeformacion1")
      return any_const_ptr(&planoDefDorsal);
    if(cod == "getPlanoDeformacion2")
      return any_const_ptr(&planoDefFrontal);
    else
      return BeamLoad::GetProp(cod);
  }
