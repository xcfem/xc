//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
//  XC is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
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
                                                                        
// $Revision: 1.0 $
// $Date: 2012-05-21 23:03:01 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/LayeredShellFiberSection.cpp,v $

// Yuli Huang (yulihuang@gmail.com) & Xinzheng Lu (luxz@tsinghua.edu.cn)
//
// Layered Shell Section
//
/* Ref: Lu X, Lu XZ, Guan H, Ye LP, Collapse simulation of reinforced 
concrete high-rise building induced by extreme earthquakes, 
Earthquake Engineering & Structural Dynamics, 2013, 42(5): 705-723*/


#include "LayeredShellFiberSection.h"
#include <material/nD/NDMaterial.h>
#include "material/section/ResponseId.h"
#include "domain/mesh/element/utils/Information.h"

//static vector and matrices
XC::ResponseId XC::LayeredShellFiberSection::array(8);

//! @brief Initialize layers.
//! @param thicknesses: vector containing the thickness of each layer.
void XC::LayeredShellFiberSection::setHWgSg(const std::vector<double> &thicknesses)
  {
    // Total thickness.
    h= 0.0;
    const size_t sz= thicknesses.size();
    for(size_t i = 0; i<sz; i++ )
      { h+= thicknesses[i]; }

    // Gauss integration weights.
    if(wg.size()!=sz)
      wg.resize(sz);
    for(size_t i= 0; i<sz; i++ )
      wg[i] = 2.0 * thicknesses[i]/h;
    
    // Gauss integration points.
    if(sg.size()!=sz)
      sg.resize(sz);
    double currLoc = 0.0;
    const double h1= 1.0/h;
    for(size_t i= 0; i<sz; i++ )
      {
        currLoc+= thicknesses[i];
        sg[i]= currLoc * h1 - 1.0;
        currLoc+= thicknesses[i];
      }    
  }

//! @brief Assign the thickness of each layer.
void XC::LayeredShellFiberSection::setThicknesses(const boost::python::list &thicknesses)
  {
    const size_t sz= boost::python::len(thicknesses);
    std::vector<double> thcks(sz);
    for(size_t i= 0;i<sz;i++)
      {
	thcks[i]= boost::python::extract<double>(thicknesses[i]);
      }
    this->setHWgSg(thcks);
  }

//! @brief Assign the thickness and the material of each layer.
void XC::LayeredShellFiberSection::setupPy(const boost::python::list &materialThicknessPairs)
  {
    const size_t sz= boost::python::len(materialThicknessPairs);
    std::vector<double> thcks(sz);
    std::vector<std::string> mat_names(sz);
    // Extract (material, thickness) pairs.
    for(size_t i= 0;i<sz;i++)
      {
	boost::python::tuple tpl= boost::python::extract<boost::python::tuple>(materialThicknessPairs[i]);
        if(boost::python::len(tpl) != 2) throw std::invalid_argument("bad");
        mat_names[i]= boost::python::extract<std::string>(tpl[0]);
        thcks[i]= boost::python::extract<double>(tpl[1]);
      }
    // Setup members.
    this->setMaterial(mat_names);
    this->setHWgSg(thcks);
  }

//! @brief Default constructor
XC::LayeredShellFiberSection::LayeredShellFiberSection(int tag)
  :MembranePlateFiberSectionBase(tag, SEC_TAG_LayeredShellFiberSection), 
    sg(), wg()
  {}

//! @brief Full constructor
XC::LayeredShellFiberSection::LayeredShellFiberSection(int tag, const std::vector<double> &thicknesses, const std::vector<NDMaterial *> &fibers)
  : MembranePlateFiberSectionBase( tag, SEC_TAG_LayeredShellFiberSection, 0.0, fibers), sg(fibers.size()), wg(fibers.size())
  {
    setHWgSg(thicknesses);
  }

//make a clone of this material
XC::SectionForceDeformation  *XC::LayeredShellFiberSection::getCopy(void) const
  { return new LayeredShellFiberSection(*this); }



//send back order of strainResultant in vector form
const XC::ResponseId &XC::LayeredShellFiberSection::getResponseType(void) const 
  {
    static bool initialized = false;
    if(!initialized)
      {
        array(0)= MEMBRANE_RESPONSE_n1; // Axial force per unit length, parallel to axis 1
        array(1)= MEMBRANE_RESPONSE_n2; // Axial force per unit length, parallel to axis 2
        array(2)= MEMBRANE_RESPONSE_n12; // In-plane shear force per unit length.
        array(3)= PLATE_RESPONSE_m1; // Bending moment per unit length, parallel to axis 1.
        array(4)= PLATE_RESPONSE_m2; // Bending moment per unit length, parallel to axis 2.
        array(5)= PLATE_RESPONSE_m12; // Torque per unit length.
        array(6)= PLATE_RESPONSE_q13; // Shear force per unit length, perpendicular to axis 1 and parallel to axis 3.
        array(7)= PLATE_RESPONSE_q23; // Shear force per unit length, perpendicular to axis 2 and parallel to axis 3.
        initialized = true;
      }
    return array;
  }
//mass per unit area
double XC::LayeredShellFiberSection::getRho(void) const 
  {

    double weight;
    double rhoH = 0.0;
    const size_t sz= theFibers.size();
    for(size_t i = 0; i < sz; i++ )
      {
        weight = ( 0.5*h ) * wg[i];
        rhoH += ( theFibers[i]->getRho() ) * weight;
      }
    return rhoH;
  }

XC::Response *XC::LayeredShellFiberSection::setResponse(const std::vector<std::string> &argv, Information &info)
  { return MembranePlateFiberSectionBase::setResponse(argv, info); }

int XC::LayeredShellFiberSection::getResponse(int responseID, Information &secInfo)
  {
    return MembranePlateFiberSectionBase::getResponse(responseID, secInfo);
  }

int XC::LayeredShellFiberSection::setParameter(const  std::vector<std::string> &argv, Parameter &param)
  {
    return MembranePlateFiberSectionBase::setParameter(argv, param);
  }

int XC::LayeredShellFiberSection::updateParameter(int parameterID, Information& info)
  {
    // placeholder for future implementations: if we will have parameters for this class, update them here
    return MembranePlateFiberSectionBase::updateParameter(parameterID, info);
  }


//receive the strainResultant 
int XC::LayeredShellFiberSection::setTrialSectionDeformation(const Vector &strainResultant_from_element)
  {
    this->strainResultant = strainResultant_from_element;

    static Vector strain(5);

    int success = 0;

    double z;
    const size_t sz= theFibers.size();
    for(size_t i = 0; i < sz; i++ )
      {
	z = ( 0.5*h ) * sg[i];

	strain(0) =  strainResultant(0)  - z*strainResultant(3);
	strain(1) =  strainResultant(1)  - z*strainResultant(4);
	strain(2) =  strainResultant(2)  - z*strainResultant(5);
	strain(3) =  strainResultant(6);
	strain(4) =  strainResultant(7);
	success += theFibers[i]->setTrialStrain( strain );

      } //end for i
    return success;
  }


//send back the stressResultant 
const XC::Vector &XC::LayeredShellFiberSection::getStressResultant(void) const
  {
    static Vector stress(5);

    double z, weight;

    stressResultant.Zero( );
    const size_t sz= theFibers.size();
    for(size_t i= 0; i < sz; i++ )
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
    return this->stressResultant;
  }


//send back the tangent 
const XC::Matrix &XC::LayeredShellFiberSection::getSectionTangent(void) const
  {
    static Matrix dd(5,5);

  //  static Matrix Aeps(5,8);

  //  static Matrix Asig(8,5);

    double z, weight;
    tangent.Zero( );
    const size_t sz= theFibers.size();
    for(size_t i = 0; i < sz; i++ )
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

	dd= theFibers[i]->getTangent( );
	dd*= weight;

	//tangent +=  ( Asig * dd * Aeps );   

  //from MATLAB : tangent = 
  //[      d11,           d12,           d13,        -z*d11,        -z*d12,        -z*d13,    d14,    d15]
  //[      d21,           d22,           d23,        -z*d21,        -z*d22,        -z*d23,    d24,    d25]
  //[      d31,           d32,           d33,        -z*d31,        -z*d32,        -z*d33,    d34,    d35]
  //[     z*d11,         z*d12,         z*d13,      -z^2*d11,      -z^2*d12,      -z^2*d13,  z*d14,  z*d15]
  //[     z*d21,         z*d22,         z*d23,      -z^2*d21,      -z^2*d22,      -z^2*d23,  z*d24,  z*d25]
  //[     z*d31,         z*d32,         z*d33,      -z^2*d31,      -z^2*d32,      -z^2*d33,  z*d34,  z*d35]
  //[       d41,           d42,           d43,        -d41*z,        -d42*z,        -d43*z,    d44,    d45]
  //[       d51,           d52,           d53,        -d51*z,        -d52*z,        -d53*z,    d54,    d55]

	//row 1
  //[      d11,           d12,           d13,        -z*d11,        -z*d12,        -z*d13,    d14,    d15]
	tangent(0,0) +=     dd(0,0);
	tangent(0,1) +=     dd(0,1);
	tangent(0,2) +=     dd(0,2);      
	tangent(0,3) +=  -z*dd(0,0);      
	tangent(0,4) +=  -z*dd(0,1);
	tangent(0,5) +=  -z*dd(0,2);
	tangent(0,6) +=     dd(0,3);
	tangent(0,7) +=     dd(0,4);

	//row 2
  //[      d21,           d22,           d23,        -z*d21,        -z*d22,        -z*d23,    d24,    d25]
	tangent(1,0) +=     dd(1,0);
	tangent(1,1) +=     dd(1,1);
	tangent(1,2) +=     dd(1,2);      
	tangent(1,3) +=  -z*dd(1,0);      
	tangent(1,4) +=  -z*dd(1,1);
	tangent(1,5) +=  -z*dd(1,2);
	tangent(1,6) +=     dd(1,3);
	tangent(1,7) +=     dd(1,4);

	//row 3
  //[      d31,           d32,           d33,        -z*d31,        -z*d32,        -z*d33,    d34,    d35]
	tangent(2,0) +=     dd(2,0);
	tangent(2,1) +=     dd(2,1);
	tangent(2,2) +=     dd(2,2);      
	tangent(2,3) +=  -z*dd(2,0);      
	tangent(2,4) +=  -z*dd(2,1);
	tangent(2,5) +=  -z*dd(2,2);
	tangent(2,6) +=     dd(2,3);
	tangent(2,7) +=     dd(2,4);

	//row 4
  //[     z*d11,         z*d12,         z*d13,      -z^2*d11,      -z^2*d12,      -z^2*d13,  z*d14,  z*d15]
	tangent(3,0) +=     z*dd(0,0);
	tangent(3,1) +=     z*dd(0,1);
	tangent(3,2) +=     z*dd(0,2);      
	tangent(3,3) +=  -z*z*dd(0,0);      
	tangent(3,4) +=  -z*z*dd(0,1);
	tangent(3,5) +=  -z*z*dd(0,2);
	tangent(3,6) +=     z*dd(0,3);
	tangent(3,7) +=     z*dd(0,4);

	//row 5
  //[     z*d21,         z*d22,         z*d23,      -z^2*d21,      -z^2*d22,      -z^2*d23,  z*d24,  z*d25]
	tangent(4,0) +=     z*dd(1,0);
	tangent(4,1) +=     z*dd(1,1);
	tangent(4,2) +=     z*dd(1,2);      
	tangent(4,3) +=  -z*z*dd(1,0);      
	tangent(4,4) +=  -z*z*dd(1,1);
	tangent(4,5) +=  -z*z*dd(1,2);
	tangent(4,6) +=     z*dd(1,3);
	tangent(4,7) +=     z*dd(1,4);

	//row 6
  //[     z*d31,         z*d32,         z*d33,      -z^2*d31,      -z^2*d32,      -z^2*d33,  z*d34,  z*d35]
	tangent(5,0) +=     z*dd(2,0);
	tangent(5,1) +=     z*dd(2,1);
	tangent(5,2) +=     z*dd(2,2);      
	tangent(5,3) +=  -z*z*dd(2,0);      
	tangent(5,4) +=  -z*z*dd(2,1);
	tangent(5,5) +=  -z*z*dd(2,2);
	tangent(5,6) +=     z*dd(2,3);
	tangent(5,7) +=     z*dd(2,4);

	//row 7
  //[  d41,    d42,    d43, -d41*z, -d42*z, -d43*z,  d44,  d45]
	tangent(6,0) +=     dd(3,0);
	tangent(6,1) +=     dd(3,1);
	tangent(6,2) +=     dd(3,2);      
	tangent(6,3) +=  -z*dd(3,0);      
	tangent(6,4) +=  -z*dd(3,1);
	tangent(6,5) +=  -z*dd(3,2);
	tangent(6,6) +=     dd(3,3);
	tangent(6,7) +=     dd(3,4);

	//row 8 
  //[  d51,    d52,    d53, -d51*z, -d52*z, -d53*z,  d54,  d55]
	tangent(7,0) +=     dd(4,0);
	tangent(7,1) +=     dd(4,1);
	tangent(7,2) +=     dd(4,2);      
	tangent(7,3) +=  -z*dd(4,0);      
	tangent(7,4) +=  -z*dd(4,1);
	tangent(7,5) +=  -z*dd(4,2);
	tangent(7,6) +=     dd(4,3);
	tangent(7,7) +=     dd(4,4);
      } //end for i

    return this->tangent;
  }


//print out data
void XC::LayeredShellFiberSection::Print(std::ostream &s, int flag ) const
  { MembranePlateFiberSectionBase::Print(s, flag); }

