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
** file 'COPYRIGHT'  in XC::main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.8 $
// $Date: 2003/02/14 23:01:33 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/fiber_section/ElasticSection2d.cpp,v $
                                                                        
                                                                        
///////////////////////////////////////////////////////
// File:  ~/Src/element/hinge/fiber_section/ElasticSection2d.cpp
//
// Written by Matthew Peavy
//
// Written:  Feb 13, 2000
// Debugged: Feb 14, 2000
// Revised:  May 2000 -- MHS (to include elastic shear)
//
//
// Purpose:  This file contains the function definitions
// for the XC::ElasticSection2d class.

#include <material/section/elastic_section/ElasticSection2d.h>
#include "material/section/repres/geom_section/GeomSection.h"
#include "preprocessor/loaders/MaterialLoader.h"
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include "xc_utils/src/base/any_const_ptr.h"
#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/base/utils_any.h"
#include "material/section/ResponseId.h"

XC::Vector XC::ElasticSection2d::s(2);

XC::ElasticSection2d::ElasticSection2d(int tag, MaterialLoader *mat_ldr)
  : BaseElasticSection2d(tag,SEC_TAG_Elastic2d,2,mat_ldr) {}

XC::ElasticSection2d::ElasticSection2d(void)
  : BaseElasticSection2d(0,SEC_TAG_Elastic2d,2) {}


XC::ElasticSection2d::ElasticSection2d(int tag, double E, double A, double I)
  : BaseElasticSection2d(tag,SEC_TAG_Elastic2d,2,E,A,I,0.0,0.0)
  {}

XC::ElasticSection2d::ElasticSection2d(int tag, double EA, double EI)
  :BaseElasticSection2d(tag, SEC_TAG_Elastic2d,2,1,EA,EI,0.0,0.0)
  {}

//! @brief Lee un objeto XC::ElasticSection2d desde archivo
bool XC::ElasticSection2d::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(ElasticSection2d) Procesando comando: " << cmd << std::endl;

    return BaseElasticSection2d::procesa_comando(status);
  }

//! @brief Devuelve la resultante de las tensiones en la sección.
const XC::Vector &XC::ElasticSection2d::getStressResultant(void) const
  {
    const Vector &e= getSectionDeformation();
    s(0)= ctes_scc.EA()*e(0);
    s(1)= ctes_scc.EI()*e(1);
    return s;
  }

//! @brief Devuelve la matriz de rigidez tangente.
const XC::Matrix &XC::ElasticSection2d::getSectionTangent(void) const
  { return ctes_scc.getSectionTangent2x2(); }


//! @brief Devuelve la matriz de rigidez noval.
const XC::Matrix &XC::ElasticSection2d::getInitialTangent(void) const
  { return getSectionTangent(); }

//! @brief Devuelve la matriz de flexibilidad.
const XC::Matrix &XC::ElasticSection2d::getSectionFlexibility(void) const
  { return ctes_scc.getSectionFlexibility2x2(); }

//! @brief Devuelve la matriz de flexibilidad noval.
const XC::Matrix &XC::ElasticSection2d::getInitialFlexibility(void) const
  { return getSectionFlexibility(); }


XC::SectionForceDeformation *XC::ElasticSection2d::getCopy(void) const
  { return new XC::ElasticSection2d(*this); }

//! @brief Devuelve los códigos de los esfuerzos para los que la
//! sección aporta rigidez.
const XC::ResponseId &XC::ElasticSection2d::getType(void) const
  { return RespElasticSection2d; }

int XC::ElasticSection2d::getOrder(void) const
  { return 2; }

int XC::ElasticSection2d::sendSelf(CommParameters &cp)
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

int XC::ElasticSection2d::recvSelf(const CommParameters &cp)
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

//! \brief Devuelve la propiedad del objeto cuyo código (de la propiedad) se pasa
//! como parámetro.
any_const_ptr XC::ElasticSection2d::GetProp(const std::string &cod) const
  {
    if(cod=="paramSeccion")
      return any_const_ptr(&ctes_scc);
    else
      return BaseElasticSection2d::GetProp(cod);
  }
 
void XC::ElasticSection2d::Print(std::ostream &s, int flag) const
  {
    s << "ElasticSection2d, tag: " << this->getTag() << std::endl;
    ctes_scc.Print(s,flag);
  }

