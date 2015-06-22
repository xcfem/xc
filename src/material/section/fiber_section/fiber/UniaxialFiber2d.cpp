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
// $Date: 2003/05/08 22:56:41 $
// $Source: material/section/fiber_section/fiber/UniaxialFiber2d.cpp,v $
                                                                        
                                                                        
// File: ~/section/UniaxialFiber2d.C
//
// Written: Remo Magalhaes de Souza
// Created: 10/98
// Revision: 
//
// Description: This file contains the implementation for the
// UniaxialFiber2d class. UniaxialFiber2d provides the abstraction of a
// uniaXial XC::Fiber whose position  is defined with only one coordinate.
// The XC::UniaxialFiber2d is subjected to a stress state with 
// only one nonzero axial stress and corresponding axial strain.
//
// What: "@(#) UniaxialFiber2d.h, revA"

#include "UniaxialFiber2d.h"
#include <cstdlib>
#include <material/uniaxial/UniaxialMaterial.h>
#include <material/section/SectionForceDeformation.h>
#include <domain/mesh/element/Information.h>
#include <utility/recorder/response/FiberResponse.h>
#include "material/section/ResponseId.h"
#include "xc_utils/src/base/CmdStatus.h"
#include "utility/actor/actor/MovableVector.h"
#include "utility/matrix/Vector.h"
#include "utility/matrix/Matrix.h"
#include "utility/matrix/ID.h"

XC::Matrix XC::UniaxialFiber2d::ks(2,2); 
XC::Vector XC::UniaxialFiber2d::fs(2);

//! @brief Constructor for blank object that recvSelf needs to be invoked upon
XC::UniaxialFiber2d::UniaxialFiber2d(void)
  : UniaxialFiber(FIBER_TAG_Uniaxial2d), y(0.0) {}

//! @brief Constructor.
XC::UniaxialFiber2d::UniaxialFiber2d(int tag, UniaxialMaterial &theMat,const double &Area,const double &position)
  : UniaxialFiber(tag, FIBER_TAG_Uniaxial2d,theMat,Area), y(-position) //XXX signo de la y.
  {}
//! @brief Constructor.
XC::UniaxialFiber2d::UniaxialFiber2d(int tag,const MaterialLoader &ldr,const std::string &nmbMat, const double &Area,const double &position)
  : UniaxialFiber(tag, FIBER_TAG_Uniaxial2d,ldr,nmbMat,Area), y(-position) //XXX signo de la y.
  {}

//! @brief Lee un objeto UniaxialFiber2d desde archivo
bool XC::UniaxialFiber2d::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(UniaxialFiber2d) Procesando comando: " << cmd << std::endl;

    if(cmd == "y")
      {
        y= interpretaDouble(status.GetString());
        return true;
      }
    else
      return UniaxialFiber::procesa_comando(status);
  }

//! @brief Asigna la deformación a la fibra.
int XC::UniaxialFiber2d::setTrialFiberStrain(const Vector &vs)
  {
    // Use the section kinematic matrix to get the fiber strain
    // eps = as * vs;
    const double strain = vs(0) + y*vs(1); // fiber strain
    return theMaterial->setTrialStrain(strain);
  }

//! @brief Devuelve la resultante de tensiones en la fibra.
XC::Vector &XC::UniaxialFiber2d::getFiberStressResultants(void) 
  {
    // Use the section kinematic matrix to get the fiber 
    // stress resultant vector
    // fs = as^ * area * sigma;
    if(isAlive())
      {
        const double df = theMaterial->getStress() * area;
        fs(0)= df;
        fs(1)= y * df;
      }
    else
      fs.Zero();
    return fs;
  }



//! @brief Devuelve la contribución de la fibra a la matriz de
//! rigidez tangente.
XC::Matrix &XC::UniaxialFiber2d::getFiberTangentStiffContr(void) 
  {
    if(isAlive())
      {
        // Use the section kinematic matrix to get the fiber 
        // tangent stiffness matrix
        // ks = (as^as) * area * Et;
        const double value= theMaterial->getTangent() * area;
        const double value_as1= value*y;
        ks(0,0)= value;
        ks(0,1)= value_as1;
        ks(1,0)= value_as1;
        ks(1,1)= value_as1 * y;
      }
    else
      ks.Zero();

    return ks;
  }

//! @brief Constructor virtual.
XC::Fiber *XC::UniaxialFiber2d::getCopy(void) const
  { return new UniaxialFiber2d(*this); }  

//! @brief Devuelve la dimensión del vector resultante de tensiones.
int XC::UniaxialFiber2d::getOrder(void) const
  { return 2; }

const XC::ResponseId &XC::UniaxialFiber2d::getType(void) const
  { return RespElasticSection2d; }

//! @brief Envia los datos a través del canal que se pasa como parámetro.
int XC::UniaxialFiber2d::sendData(CommParameters &cp)
  {
    int res= UniaxialFiber::sendData(cp);
    res+= cp.sendDouble(y,getDbTagData(),CommMetaData(6));
    return res;
  }

//! @brief Recibe los datos a través del canal que se pasa como parámetro.
int XC::UniaxialFiber2d::recvData(const CommParameters &cp)
  {    
    int res= UniaxialFiber::recvData(cp);
    res+= cp.receiveDouble(y,getDbTagData(),CommMetaData(6));
    return res;
  }

//! @brief Envia el objeto a través del canal que se pasa como parámetro.
int XC::UniaxialFiber2d::sendSelf(CommParameters &cp)
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


//! @brief Recibe el objeto a través del canal que se pasa como parámetro.
int XC::UniaxialFiber2d::recvSelf(const CommParameters &cp)
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


void XC::UniaxialFiber2d::Print(std::ostream &s, int flag)
  {
    s << "\nUniaxialFiber2d, tag: " << this->getTag() << std::endl;
    s << "\tArea: " << area << std::endl; 
    s << "\tMatrix as: " << 1.0 << " " << y << std::endl; 
    s << "\tMaterial, tag: " << theMaterial->getTag() << std::endl;
  }

XC::Response *XC::UniaxialFiber2d::setResponse(const std::vector<std::string> &argv, Information &info)
  {
    const int argc= argv.size();
    if(argc == 0)
      return 0;
    if(argv[0] == "force" || argv[0] == "forces")
      return new FiberResponse(this, 1, XC::Vector(2));
    else
      return theMaterial->setResponse(argv, info);
  }

int XC::UniaxialFiber2d::getResponse(int responseID, Information &fibInfo)
  {
    switch(responseID)
      {
        case 1:
          return fibInfo.setVector(this->getFiberStressResultants());
        default:
          return -1;
      }
  }

//! @brief Devuelve la posición de la fibra.
void XC::UniaxialFiber2d::getFiberLocation(double &yLoc, double &zLoc) const
  {
    yLoc= -y;
    zLoc= 0.0;
  }
