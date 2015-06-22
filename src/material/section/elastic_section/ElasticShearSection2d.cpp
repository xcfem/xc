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
/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 1999, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.3 $
// $Date: 2008/08/26 16:49:19 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/ElasticShearSection2d.cpp,v $

#include "ElasticShearSection2d.h"
#include "material/section/repres/geom_section/GeomSection.h"
#include "material/section/ResponseId.h"
#include "modelador/loaders/MaterialLoader.h"
#include <utility/matrix/Matrix.h>
#include "xc_utils/src/base/any_const_ptr.h"
#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/base/utils_any.h"

#include <cstdlib>

XC::Vector XC::ElasticShearSection2d::s(3);

XC::ElasticShearSection2d::ElasticShearSection2d(int tag, MaterialLoader *mat_ldr)
  : BaseElasticSection2d(tag,SEC_TAG_ElasticShear2d,3,mat_ldr) {}

XC::ElasticShearSection2d::ElasticShearSection2d(void)
  :BaseElasticSection2d(0, SEC_TAG_ElasticShear2d,3),parameterID(0) {}

const XC::Vector &XC::ElasticShearSection2d::getStressResultant(void) const
  {
    const Vector e= getSectionDeformation();
    s(0) = ctes_scc.EA()*e(0);
    s(1) = ctes_scc.EI()*e(1);    
    s(2) = ctes_scc.GAAlpha()*e(2);
    return s;
  }

const XC::Matrix &XC::ElasticShearSection2d::getSectionTangent(void) const
  { return ctes_scc.getSectionTangent3x3(); }

const XC::Matrix &XC::ElasticShearSection2d::getInitialTangent(void) const
  { return getSectionTangent(); }

const XC::Matrix &XC::ElasticShearSection2d::getSectionFlexibility(void) const
  { return ctes_scc.getSectionFlexibility3x3(); }

const XC::Matrix &XC::ElasticShearSection2d::getInitialFlexibility(void) const
  { return getSectionFlexibility(); }


XC::SectionForceDeformation *XC::ElasticShearSection2d::getCopy(void) const
  { return new ElasticShearSection2d(*this); }

const XC::ResponseId &XC::ElasticShearSection2d::getType(void) const
  { return RespElasticShSection2d; }

int XC::ElasticShearSection2d::getOrder(void) const
  { return 3; }

//! @brief Envía los miembros del objeto a través del canal que se pasa como parámetro.
int XC::ElasticShearSection2d::sendData(CommParameters &cp)
  {
    int res= BaseElasticSection2d::sendData(cp);
    setDbTagDataPos(9,parameterID);
    return res;
  }

//! @brief Recibe los miembros del objeto a través del canal que se pasa como parámetro.
int XC::ElasticShearSection2d::recvData(const CommParameters &cp)
  {
    int res= BaseElasticSection2d::recvData(cp);
    parameterID= getDbTagDataPos(9);
    return res;
  }

int XC::ElasticShearSection2d::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(3);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << nombre_clase() << "sendSelf() - failed to send data\n";
    return res;
  }

int XC::ElasticShearSection2d::recvSelf(const CommParameters &cp)
  {
    inicComm(3);
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
 
int XC::ElasticShearSection2d::setParameter(const std::vector<std::string> &argv,Parameter &param)
  { return ctes_scc.setParameter(argv,param,this); }

int XC::ElasticShearSection2d::updateParameter(int paramID, Information &info)
  { return ctes_scc.updateParameter(paramID,info); }

int XC::ElasticShearSection2d::activateParameter(int paramID)
  {
    parameterID = paramID;
    return 0;
  }

const XC::Vector &XC::ElasticShearSection2d::getStressResultantSensitivity(int gradIndex,bool conditional)
  {
    s.Zero();

    const Vector &e= getSectionDeformation();
    if(parameterID == 1)
      { // E
        s(0) = ctes_scc.A()*e(0);
        s(1) = ctes_scc.I()*e(1);
      }
    if(parameterID == 2)
      { // A
        s(0)= ctes_scc.E()*e(0);
        s(2)= ctes_scc.G()*ctes_scc.Alpha()*e(2);
      }
    if(parameterID == 3) // I
      s(1)= ctes_scc.E()*e(1);
    if(parameterID == 4) // G
      s(2)= ctes_scc.A()*ctes_scc.Alpha()*e(2);
    if(parameterID == 5) // alpha
      s(2)= ctes_scc.G()*ctes_scc.A()*e(2);
    return s;
  }

const XC::Matrix& XC::ElasticShearSection2d::getInitialTangentSensitivity(int gradIndex)
  { return ctes_scc.getInitialTangentSensitivity3x3(gradIndex); }
