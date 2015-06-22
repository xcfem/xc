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
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/ElasticPlateSection.cpp,v $

// Ed "C++" Love
//
//  Elastic Plate Section
//


#include <material/section/plate_section/ElasticPlateSection.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include <cstdio> 
#include <cstdlib> 
#include <cmath> 
#include "material/section/ResponseId.h"

//! @brief Constructor
XC::ElasticPlateSection::ElasticPlateSection(int tag)
  : XC::ElasticPlateProto<5>(tag, SEC_TAG_ElasticPlateSection) {}

//! @brief Default constructor
XC::ElasticPlateSection::ElasticPlateSection(void)
  : XC::ElasticPlateProto<5>(0,SEC_TAG_ElasticPlateSection) {}



//! @brief Constructor
XC::ElasticPlateSection::ElasticPlateSection(  int    tag, double young,double poisson, double thickness)
  : XC::ElasticPlateProto<5>(tag, SEC_TAG_ElasticPlateSection,young,poisson,thickness) {}

//! @brief Constructor virtual
XC::SectionForceDeformation *XC::ElasticPlateSection::getCopy(void) const
  { return new ElasticPlateSection(*this); }



//! @brief Devuelve la resultante de las tensiones en la sección.
const XC::Vector& XC::ElasticPlateSection::getStressResultant(void) const
  {
    const Vector &strain= getSectionDeformation();
    double D=  E * (h*h*h) / 12.0 / ( 1.0 - nu*nu ) ; //bending modulus
    double G=  0.5 * E / ( 1.0 + nu ) ; //shear modulus
    G *= five6 ;
    G *= h ;
    stress(0) = -( D*strain(0) + nu*D*strain(1) ) ;
    stress(1) = -( nu*D*strain(0) + D*strain(1) ) ;
    stress(2) = -0.5*D*( 1.0 - nu )*strain(2) ;
    stress(3) = G*strain(3) ;
    stress(4) = G*strain(4) ;
    return this->stress ;
  }


//! @brief Devuelve la matriz de rigidez tangente.
const XC::Matrix& XC::ElasticPlateSection::getSectionTangent(void) const
  {
    const double D= E * (h*h*h) / 12.0 / ( 1.0 - nu*nu ) ;
    const double G= 0.5 * E / ( 1.0 + nu ) ;
    tangent.Zero() ;
    tangent(0,0) = -D ;
    tangent(1,1) = -D ;
    tangent(0,1) = -nu*D ;
    tangent(1,0) = tangent(0,1) ;
    tangent(2,2) = -0.5 * D * ( 1.0 - nu ) ;
    tangent(3,3) = five6*G*h ;
    tangent(4,4) = tangent(3,3) ;
    return tangent;
  }

//! @brief Devuelve la matriz de rigidez noval.
const XC::Matrix& XC::ElasticPlateSection::getInitialTangent(void) const
  { return getSectionTangent(); }

//! @brief Devuelve las etiquetas de los grados de libertad a los que el
//! elemento aporta rigidez.
const XC::ResponseId &XC::ElasticPlateSection::getType(void) const 
  { return RespPlateMat; }

//! @brief print out data
void  XC::ElasticPlateSection::Print( std::ostream &s, int flag )
  {
    s << "ElasticPlateSection: \n " ;
    s <<  "  Young's Modulus E  = "  <<  E  <<  std::endl; ;
    s <<  "  Poisson's Ratio nu = " <<  nu <<  std::endl; ;
    s <<  "  Thickness h = "        <<  h  <<  std::endl; ;
    return ;
  }

//! @brief Envía el objeto a través del canal que se pasa como parámetro.
int XC::ElasticPlateSection::sendSelf(CommParameters &cp) 
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(8);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << nombre_clase() << "sendSelf() - failed to send data\n";
    return res;
  }


//! @brief Recibe el objeto a través del canal que se pasa como parámetro.
int XC::ElasticPlateSection::recvSelf(const CommParameters &cp)
  {
    inicComm(8);
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
