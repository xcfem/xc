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
                                                                        
// $Revision: 1.6 $
// $Date: 2003/02/14 23:01:34 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/MembranePlateFiberSection.cpp,v $

// Ed "C++" Love
//
//  Generic Plate Section with membrane
//


#include <material/section/plate_section/MembranePlateFiberSection.h>
#include <material/nD/NDMaterial.h>
#include "material/section/ResponseId.h"


//parameters
const double XC::MembranePlateFiberSection::root56 = sqrt(5.0/6.0); //shear correction

//static vector and matrices
XC::Vector  XC::MembranePlateFiberSection::stressResultant(8);
XC::Matrix  XC::MembranePlateFiberSection::tangent(8,8);


const double  XC::MembranePlateFiberSection::sg[] = { -1, 
                                                  -0.65465367, 
                                                   0, 
                                                   0.65465367, 
                                                   1 };
 
const double  XC::MembranePlateFiberSection::wg[] = { 0.1, 
                                                  0.5444444444, 
                                                  0.7111111111, 
                                                  0.5444444444, 
                                                  0.1  };

/*      from Ham-O
        case 5:
         xi(0,0) = -1.;
         xi(1,0) = -0.65465367;
         xi(2,0) =  0.;
         xi(3,0) =  0.65465367;
         xi(4,0) =  1.;
      
         w(0) =  0.1;
         w(1) =  0.5444444444;
         w(2) =  0.7111111111;
         w(3) =  0.5444444444;
         w(4) =  0.1;
      break;
*/


XC::MembranePlateFiberSection::MembranePlateFiberSection(int tag)
  : XC::SectionForceDeformation( 0, SEC_TAG_MembranePlateFiberSection ), strainResultant(8) 
  { 
    for(int i= 0;i<5;i++ )
      theFibers[i]= nullptr;
  }

//! @brief null constructor
XC::MembranePlateFiberSection::MembranePlateFiberSection(void)
  : XC::SectionForceDeformation(0, SEC_TAG_MembranePlateFiberSection ), 
    strainResultant(8) 
  { 
    for(int i= 0;i<5;i++)
      theFibers[i]= nullptr;
  }



//! @brief full constructor
XC::MembranePlateFiberSection::MembranePlateFiberSection(int tag, double thickness, XC::NDMaterial &Afiber )
  : XC::SectionForceDeformation( tag, SEC_TAG_MembranePlateFiberSection), strainResultant(8)
  {
    this->h= thickness;
    for(int i= 0; i < 5; i++)
      theFibers[i]= Afiber.getCopy("PlateFiber");
  }


//! @brief destructor
XC::MembranePlateFiberSection::~MembranePlateFiberSection(void) 
  { 
    for(int i= 0; i < 5; i++ )
      delete theFibers[i]; 
  } 



//! @brief make a clone of this material
XC::SectionForceDeformation  *XC::MembranePlateFiberSection::getCopy(void) const 
  { return new MembranePlateFiberSection(*this); }



//! @brief send back order of strainResultant in vector form
int XC::MembranePlateFiberSection::getOrder(void) const
  { return 8; }


//! @brief Devuelve las etiquetas de los grados de libertad a los que el
//! material aporta rigidez.
const XC::ResponseId &XC::MembranePlateFiberSection::getType(void) const 
  { return RespShellMat; }



//swap history variables
int XC::MembranePlateFiberSection::commitState(void) 
  {
    int success = 0;
    for(register int i= 0; i < 5; i++ )
      success += theFibers[i]->commitState( );
    return success;
  }



//revert to last saved state
int XC::MembranePlateFiberSection::revertToLastCommit(void)
  {
    int success = 0;

    for(int i= 0; i < 5; i++ )
      success+= theFibers[i]->revertToLastCommit();
    return success;
  }

//revert to start
int XC::MembranePlateFiberSection::revertToStart(void)
  {
    int success = 0;
    for(int i= 0;i<5;i++ )
      success += theFibers[i]->revertToStart( );
    return success;
  }


//mass per unit area
double XC::MembranePlateFiberSection::getRho(void) const
  {
    double weight;
    double rhoH = 0.0;
    for(int i = 0; i < 5; i++ )
      {
        weight = ( 0.5*h ) * wg[i];
        rhoH+= ( theFibers[i]->getRho() ) * weight;
      }
    return rhoH;
  }


int XC::MembranePlateFiberSection::setInitialSectionDeformation(const Vector &strainResultant_from_element)
  {
    std::cerr << "MembranePlateFiberSection::setInitialSectionDeformation no implementada." << std::endl;
    return 0;
  }
const XC::Vector &XC::MembranePlateFiberSection::getInitialSectionDeformation(void) const
  {
    std::cerr << "MembranePlateFiberSection::getInitialSectionDeformation no implementada." << std::endl;
    return strainResultant;
  }

//receive the strainResultant 
int XC::MembranePlateFiberSection::setTrialSectionDeformation( const XC::Vector &strainResultant_from_element)
  {
    this->strainResultant = strainResultant_from_element;

    static XC::Vector strain(5);

    int success= 0;
    double z;

    for(int i = 0; i < 5; i++ )
      {
        z= ( 0.5*h ) * sg[i];
        strain(0) =  strainResultant(0)  - z*strainResultant(3);
        strain(1) =  strainResultant(1)  - z*strainResultant(4);
        strain(2) =  strainResultant(2)  - z*strainResultant(5);
        strain(3) =  root56*strainResultant(6);
        strain(4) =  root56*strainResultant(7);
        success += theFibers[i]->setTrialStrain( strain );
      } //end for i
    return success;
  }


//! @brief Returns material's trial generalized deformation.
const XC::Vector& XC::MembranePlateFiberSection::getSectionDeformation(void) const
  { return strainResultant; }


//send back the stressResultant 
const XC::Vector&  XC::MembranePlateFiberSection::getStressResultant(void) const
  {
    static XC::Vector stress(5);
    int i;
    double z, weight;
    stressResultant.Zero( );

    for( i = 0; i < 5; i++ )
      {
        z = ( 0.5*h ) * sg[i];
        weight = ( 0.5*h ) * wg[i];
        stress = theFibers[i]->getStress( );
        //membrane
        stressResultant(0)  +=  stress(0)*weight;
        stressResultant(1)  +=  stress(1)*weight;
        stressResultant(2)  +=  stress(2)*weight;
        //bending moments
        stressResultant(3)  +=  ( z*stress(0) ) * weight;
        stressResultant(4)  +=  ( z*stress(1) ) * weight;
        stressResultant(5)  +=  ( z*stress(2) ) * weight;
        //shear
        stressResultant(6)  += stress(3)*weight;
        stressResultant(7)  += stress(4)*weight;
      } //end for i
    //modify shear 
    stressResultant(6) *= root56;  
    stressResultant(7) *= root56;
    return this->stressResultant;
  }


//! @brief Devuelve la matriz de rigidez tangente.
const XC::Matrix&  XC::MembranePlateFiberSection::getSectionTangent(void) const
  {
    static XC::Matrix dd(5,5);
    static XC::Matrix Aeps(5,8);
    static XC::Matrix Asig(8,5);

    double z, weight;
    tangent.Zero( );

    for(register int i = 0; i < 5; i++)
      {
        z = ( 0.5*h ) * sg[i];
        weight = (0.5*h) * wg[i];

/*      //compute Aeps

      Aeps.Zero( );

      Aeps(0,0) = 1.0;
      Aeps(0,3) = -z;

      Aeps(1,1) = 1.0;
      Aeps(1,4) = -z;

      Aeps(2,2) = 1.0;
      Aeps(2,5) = -z;

      Aeps(3,6) = root56;
      Aeps(4,7) = root56;

      //compute Asig

      Asig.Zero( );

      Asig(0,0) = 1.0;
      Asig(3,0) = z;

      Asig(1,1) = 1.0;
      Asig(4,1) = z;

      Asig(2,2) = 1.0;
      Asig(5,2) = z;

      Asig(6,3) = root56;
      Asig(7,4) = root56;
*/
      //compute the tangent
      dd = theFibers[i]->getTangent( );
      dd *= weight;

      //tangent +=  ( Asig * dd * Aeps );   

//from MATLAB : tangent = 
//[      d11,           d12,           d13,        -z*d11,        -z*d12,        -z*d13,    d14*root56,    d15*root56]
//[      d21,           d22,           d23,        -z*d21,        -z*d22,        -z*d23,    d24*root56,    d25*root56]
//[      d31,           d32,           d33,        -z*d31,        -z*d32,        -z*d33,    d34*root56,    d35*root56]
//[     z*d11,         z*d12,         z*d13,      -z^2*d11,      -z^2*d12,      -z^2*d13,  z*d14*root56,  z*d15*root56]
//[     z*d21,         z*d22,         z*d23,      -z^2*d21,      -z^2*d22,      -z^2*d23,  z*d24*root56,  z*d25*root56]
//[     z*d31,         z*d32,         z*d33,      -z^2*d31,      -z^2*d32,      -z^2*d33,  z*d34*root56,  z*d35*root56]
//[  root56*d41,    root56*d42,    root56*d43, -root56*d41*z, -root56*d42*z, -root56*d43*z,  root56^2*d44,  root56^2*d45]
//[  root56*d51,    root56*d52,    root56*d53, -root56*d51*z, -root56*d52*z, -root56*d53*z,  root56^2*d54,  root56^2*d55]
 
      //row 1
//[      d11,           d12,           d13,        -z*d11,        -z*d12,        -z*d13,    d14*root56,    d15*root56]
      tangent(0,0) +=  dd(0,0);
      tangent(0,1) +=  dd(0,1);
      tangent(0,2) +=  dd(0,2);      
      tangent(0,3) +=  -z*dd(0,0);      
      tangent(0,3) +=  -z*dd(0,1);
      tangent(0,5) +=  -z*dd(0,2);
      tangent(0,6) +=  root56*dd(0,3);
      tangent(0,7) +=  root56*dd(0,4);

      //row 2
//[      d21,           d22,           d23,        -z*d21,        -z*d22,        -z*d23,    d24*root56,    d25*root56]
      tangent(1,0) +=  dd(1,0);
      tangent(1,1) +=  dd(1,1);
      tangent(1,2) +=  dd(1,2);      
      tangent(1,3) +=  -z*dd(1,0);      
      tangent(1,4) +=  -z*dd(1,1);
      tangent(1,5) +=  -z*dd(1,2);
      tangent(1,6) +=  root56*dd(1,3);
      tangent(1,7) +=  root56*dd(1,4);

      //row 3
//[      d31,           d32,           d33,        -z*d31,        -z*d32,        -z*d33,    d34*root56,    d35*root56]
      tangent(2,0) +=  dd(2,0);
      tangent(2,1) +=  dd(2,1);
      tangent(2,2) +=  dd(2,2);      
      tangent(2,3) +=  -z*dd(2,0);      
      tangent(2,4) +=  -z*dd(2,1);
      tangent(2,5) +=  -z*dd(2,2);
      tangent(2,6) +=  root56*dd(2,3);
      tangent(2,7) +=  root56*dd(2,4);

      //row 4
//[     z*d11,         z*d12,         z*d13,      -z^2*d11,      -z^2*d12,      -z^2*d13,  z*d14*root56,  z*d15*root56]
      tangent(3,0) +=  z*dd(0,0);
      tangent(3,1) +=  z*dd(0,1);
      tangent(3,2) +=  z*dd(0,2);      
      tangent(3,3) +=  -z*z*dd(0,0);      
      tangent(3,4) +=  -z*z*dd(0,1);
      tangent(3,5) +=  -z*z*dd(0,2);
      tangent(3,6) +=  z*root56*dd(0,3);
      tangent(3,7) +=  z*root56*dd(0,4);

      //row 5
//[     z*d21,         z*d22,         z*d23,      -z^2*d21,      -z^2*d22,      -z^2*d23,  z*d24*root56,  z*d25*root56]
      tangent(4,0) +=  z*dd(1,0);
      tangent(4,1) +=  z*dd(1,1);
      tangent(4,2) +=  z*dd(1,2);      
      tangent(4,3) +=  -z*z*dd(1,0);      
      tangent(4,4) +=  -z*z*dd(1,1);
      tangent(4,5) +=  -z*z*dd(1,2);
      tangent(4,6) +=  z*root56*dd(1,3);
      tangent(4,7) +=  z*root56*dd(1,4);

      //row 6
//[     z*d31,         z*d32,         z*d33,      -z^2*d31,      -z^2*d32,      -z^2*d33,  z*d34*root56,  z*d35*root56]
      tangent(5,0) +=  z*dd(2,0);
      tangent(5,1) +=  z*dd(2,1);
      tangent(5,2) +=  z*dd(2,2);      
      tangent(5,3) +=  -z*z*dd(2,0);      
      tangent(5,4) +=  -z*z*dd(2,1);
      tangent(5,5) +=  -z*z*dd(2,2);
      tangent(5,6) +=  z*root56*dd(2,3);
      tangent(5,7) +=  z*root56*dd(2,4);

      //row 7
//[  root56*d41,    root56*d42,    root56*d43, -root56*d41*z, -root56*d42*z, -root56*d43*z,  root56^2*d44,  root56^2*d45]
      tangent(6,0) +=  root56*dd(3,0);
      tangent(6,1) +=  root56*dd(3,1);
      tangent(6,2) +=  root56*dd(3,2);      
      tangent(6,3) +=  -root56*z*dd(3,0);      
      tangent(6,4) +=  -root56*z*dd(3,1);
      tangent(6,5) +=  -root56*z*dd(3,2);
      tangent(6,6) +=  root56*root56*dd(3,3);
      tangent(6,7) +=  root56*root56*dd(3,4);

      //row 8 
//[  root56*d51,    root56*d52,    root56*d53, -root56*d51*z, -root56*d52*z, -root56*d53*z,  root56^2*d54,  root56^2*d55]
      tangent(7,0) +=  root56*dd(4,0);
      tangent(7,1) +=  root56*dd(4,1);
      tangent(7,2) +=  root56*dd(4,2);      
      tangent(7,3) +=  -root56*z*dd(4,0);      
      tangent(7,4) +=  -root56*z*dd(4,1);
      tangent(7,5) +=  -root56*z*dd(4,2);
      tangent(7,6) +=  root56*root56*dd(4,3);
      tangent(7,7) +=  root56*root56*dd(4,4);

  } //end for i

  return this->tangent;
}


//print out data
void  XC::MembranePlateFiberSection::Print( std::ostream &s, int flag )
  {
    s << "MembranePlateFiberSection: \n ";
    s <<  "  Thickness h = "        <<  h  <<  std::endl;
    theFibers[0]->Print( s, flag );
    return;
  }

//! @brief Envía los miembros del objeto a través del canal que se pasa como parámetro.
int XC::MembranePlateFiberSection::sendData(CommParameters &cp)
  {
    int res= SectionForceDeformation::sendData(cp);
    res+= cp.sendDouble(h,getDbTagData(),CommMetaData(6));
    res+= cp.sendBrokedPtr(theFibers[0],getDbTagData(),BrokedPtrCommMetaData(7,8,9));
    res+= cp.sendBrokedPtr(theFibers[1],getDbTagData(),BrokedPtrCommMetaData(10,11,12));
    res+= cp.sendBrokedPtr(theFibers[2],getDbTagData(),BrokedPtrCommMetaData(13,14,15));
    res+= cp.sendBrokedPtr(theFibers[3],getDbTagData(),BrokedPtrCommMetaData(16,17,18));
    res+= cp.sendBrokedPtr(theFibers[4],getDbTagData(),BrokedPtrCommMetaData(19,20,21));
    res+= cp.sendVector(strainResultant,getDbTagData(),CommMetaData(22));
    return res;
  }

//! @brief Recibe los miembros del objeto a través del canal que se pasa como parámetro.
int XC::MembranePlateFiberSection::recvData(const CommParameters &cp)
  {
    int res= SectionForceDeformation::recvData(cp);
    res+= cp.receiveDouble(h,getDbTagData(),CommMetaData(6));
    theFibers[0]= cp.getBrokedMaterial(theFibers[0],getDbTagData(),BrokedPtrCommMetaData(7,8,9));
    theFibers[1]= cp.getBrokedMaterial(theFibers[1],getDbTagData(),BrokedPtrCommMetaData(10,11,12));
    theFibers[2]= cp.getBrokedMaterial(theFibers[2],getDbTagData(),BrokedPtrCommMetaData(13,14,15));
    theFibers[3]= cp.getBrokedMaterial(theFibers[3],getDbTagData(),BrokedPtrCommMetaData(16,17,18));
    theFibers[4]= cp.getBrokedMaterial(theFibers[4],getDbTagData(),BrokedPtrCommMetaData(19,20,21));
    res+= cp.receiveVector(strainResultant,getDbTagData(),CommMetaData(22));
    return res;
  }

int XC::MembranePlateFiberSection::sendSelf(CommParameters &cp)
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


int XC::MembranePlateFiberSection::recvSelf(const CommParameters &cp)
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
 


