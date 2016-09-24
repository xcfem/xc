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

#include "utility/actor/actor/MovableVector.h"


//parameters
const double XC::ElasticPlateBase::five6 = 5.0/6.0 ; //shear correction

//! @brief Constuctor.
XC::ElasticPlateBase::ElasticPlateBase(int tag,int classTag)
  : XC::PlateBase(tag, classTag), E(0.0), nu(0.0) {}

//null constructor
XC::ElasticPlateBase::ElasticPlateBase(int classTag)
  :XC::PlateBase( 0, classTag),
   E(0.0), nu(0.0) { }

//full constructor
XC::ElasticPlateBase::ElasticPlateBase(int tag, int classTag,
                                           double young,
                                           double poisson,
                                           double thickness)
  :XC::PlateBase(tag,classTag,h), E(young), nu(poisson) {}

//swap history variables
int XC::ElasticPlateBase::commitState(void) 
  { return 0 ; }

//revert to last saved state
int XC::ElasticPlateBase::revertToLastCommit(void)
  { return 0 ; }

//revert to start
int XC::ElasticPlateBase::revertToStart(void)
  { return 0; }

//! @brief Envía los datos through the channel being passed as parameter.
int XC::ElasticPlateBase::sendData(CommParameters &cp)
  {
    int res= PlateBase::sendData(cp);
    res+= cp.sendDoubles(E,nu,h,getDbTagData(),CommMetaData(5));
    return res;
  }

//! @brief Recibe los datos through the channel being passed as parameter.
int XC::ElasticPlateBase::recvData(const CommParameters &cp)
  {
    int res= PlateBase::recvData(cp);
    res+= cp.receiveDoubles(E,nu,h,getDbTagData(),CommMetaData(5));
    return res;
  }
