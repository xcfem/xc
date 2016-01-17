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

// $Revision: 1.7 $
// $Date: 2003/02/25 23:33:37 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/CableMaterial.cpp,v $

// Written: Charles Chadwell
// Created: 07/01
//
// Description: This file contains the class definition for
// CableMaterial. CableMaterial provides the abstraction
// of an elastic uniaxial material,
//
// The input parameters are the Prestress, E, Effective Self Weight (gravity component of
// Weight per volume transverse to the cable), and Length of the cable.
//
// The cable Force Displacement is converted to Stress Strain material for use
// with the truss element.  The stress strain ranges from slack (large strain at zero
// stress) to taught (linear with modulus E).  The material has no history and is not
// path dependent.
//
//
// What: "@(#) CableMaterial.cpp, revA"

#include <material/uniaxial/CableMaterial.h>
#include <utility/matrix/Vector.h>

#include <domain/mesh/element/Information.h>
#include "xc_utils/src/base/any_const_ptr.h"
#include "utility/actor/actor/MovableVector.h"


XC::CableMaterial::CableMaterial(int tag, double PRESTRESS, double e, double UNIT_WEIGHT_EFF, double L_Element)
  :ElasticBaseMaterial(tag,MAT_TAG_CableMaterial, e), Ps(PRESTRESS), Mue(UNIT_WEIGHT_EFF), L(L_Element)
  {}

//! @brief Constructor.
XC::CableMaterial::CableMaterial(int tag)
  :ElasticBaseMaterial(tag,MAT_TAG_CableMaterial, 0.0),
   Ps(0.0), Mue(0.0), L(0.0) {}

void XC::CableMaterial::setLength(const double &l)
  { L= l; }

int XC::CableMaterial::setTrialStrain(double strain, double strainRate)
  {
    trialStrain= strain;
    double derivE, derivG;

    // Check if out side the range of inportance
    double dP, curstrain, e0;
    int i = 0;

    // Perameters for bisection
    double L_bound = 0, U_bound= 0.0, middle = 0;


    if(trialStrain < 0)
      U_bound = Ps;
    else
      U_bound = E * trialStrain + Ps;

    if(L<1e-4)
      std::clog << "CableMaterial::setTrialStrain; la longitud es extremadamente pequeña; L= " << L << std::endl; 

    // Check if slack in cable has been taken out and it is a bar
    e0= Mue*Mue*L*L/(24*Ps*Ps) - Ps/E;
    if(trialStrain > 0 && std::abs(trialStrain - evalStress((trialStrain - e0)*E)) < 10e-9)
      trialStress =  (trialStrain - e0)*E;

    // Check if all slack
    if(trialStrain < - Ps/E*10.0)
      trialStress =  0.0;

    // if stress is in between then do itterations -- Bisection
    dP = U_bound - L_bound;

    while(std::abs(dP)/U_bound > 0.00000001 && i < 100)
      {
        middle = .5 * (U_bound + L_bound);
        curstrain = evalStress(middle);
        if(curstrain <= trialStrain)
          { L_bound = middle; }
        else
          { U_bound = middle; }
        dP = U_bound - L_bound;
        i++;
      }

    // if it did not converge - return near zero stress
    if(i == 100)
      trialStress =  0.0;
    else
      trialStress = middle;

    if(trialStress <= 0.0)
      trialTangent = 0.0;

    // Elastic Part
    derivE = 1 / E * (1. - Mue * Mue * L * L / (24. * trialStress * trialStress) * (1. - 2. * Ps / trialStress));
    // Geometric Part
    derivG = 1 / 12. * Mue * Mue * L * L / (trialStress * trialStress * trialStress);

    if(derivE + derivG != 0.0)
      trialTangent =  1.0 / (derivE + derivG);
    else
      trialTangent =  1e-8;
    return 0;
  }


int XC::CableMaterial::setTrial(double strain, double &stress, double &tangent, double strainRate)
  {
    setTrialStrain(strain, strainRate);
    // set the return values
    stress = trialStress;
    tangent = trialTangent;
    return 0;
  }

double XC::CableMaterial::getStress(void) const
  { return trialStress; }

double XC::CableMaterial::evalStress(double stress)
  {
    double retval= -10;
    // Should never be zero or less than zero
    if(stress>0)
      {
        // Elastic Part
        const double strainE= 1 / E * (stress - Ps) * (1 + Mue * Mue * L * L / (24 * stress));
        // Geometric Part
        const double strainG = 1 / 24 * Mue * Mue * L * L * (1 / (Ps * Ps) - 1 / (stress * stress));
        retval= strainE + strainG;
      }
    return retval;
  }


double XC::CableMaterial::getTangent(void) const
  { return trialTangent; }

int XC::CableMaterial::commitState(void)
  { return 0; }

int XC::CableMaterial::revertToLastCommit(void)
  { return 0; }

int XC::CableMaterial::revertToStart(void)
  {
    trialStrain = 0.0;
    return 0;
  }

XC::UniaxialMaterial *XC::CableMaterial::getCopy(void) const
  { return new CableMaterial(*this); }

//! @brief Devuelve un vector para almacenar los dbTags
//! de los miembros de la clase.
XC::DbTagData &XC::CableMaterial::getDbTagData(void) const
  {
    static DbTagData retval(4);
    return retval;
  }

//! @brief Envía los miembros del objeto a través del canal que se pasa como parámetro.
int XC::CableMaterial::sendData(CommParameters &cp)
  {
    int res= ElasticBaseMaterial::sendData(cp);
    res+= cp.sendDoubles(Ps,Mue,L,trialStress,trialTangent,getDbTagData(),CommMetaData(3));
    return res;
  }

//! @brief Recibe los miembros del objeto a través del canal que se pasa como parámetro.
int XC::CableMaterial::recvData(const CommParameters &cp)
  {
    int res= ElasticBaseMaterial::recvData(cp);
    res+= cp.receiveDoubles(Ps,Mue,L,trialStress,trialTangent,getDbTagData(),CommMetaData(3));
    return res;
  }

//! @brief Envía el objeto a través del canal que se pasa como parámetro.
int XC::CableMaterial::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(4); 
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << "CableMaterial::sendSelf - failed to send data.\n";
    return res;
  }

//! @brief Recibe el objeto a través del canal que se pasa como parámetro.
int XC::CableMaterial::recvSelf(const CommParameters &cp)
  {
    inicComm(4);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << "CableMaterial::recvSelf - failed to receive ids.\n";
    else
      {
        res+= recvData(cp);
        if(res<0)
          std::cerr << "CableMaterial::recvSelf - failed to receive data.\n";
      }
    return res;
  }

void XC::CableMaterial::Print(std::ostream &s, int flag)
  {
    s << "CableMaterial tag: " << this->getTag() << std::endl;
    s << "  E: " << E << " Prestress: " << Ps << std::endl;
  }

//int XC::CableMaterial::setParameter(const std::vector<std::string> &argv, Parameter &param)
//{
//        if (argv[0] == "E") {
//                info.theType = DoubleType;
///                return 1;
//        }
//        else if (argv[0] == "eta") {
//                info.theType = DoubleType;
//                return 2;
//        }
//        else
//                return -1;
//}
//
//int
//XC::CableMaterial::updateParameter(int parameterID, Information &info)
//{
//        switch(parameterID) {
//        case -1:
//                return -1;
//        case 1:
//                E = info.theDouble;
//                return 0;
//        case 2:
//                eta = info.theDouble;
//                return 0;
//        default:
//                return -1;
//        }
//}



