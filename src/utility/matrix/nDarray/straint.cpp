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
//################################################################################
//# COPY-YES  (C):     :-))                                                      #
//# PROJECT:           Object Oriented Finite XC::Element Program                    #
//# PURPOSE:           strain XC::BJtensor with all necessary functions                #
//# CLASS:             straintensor                                              #
//#                                                                              #
//# VERSION:                                                                     #
//# LANGUAGE:          C++.ver >= 2.0 ( Borland C++ ver=3.00, SUN C++ ver=2.1 )  #
//# TARGET OS:         DOS || UNIX || . . .                                      #
//# DESIGNER(S):       Boris Jeremic                                             #
//# PROGRAMMER(S):     Boris Jeremic                                             #
//#                                                                              #
//#                                                                              #
//# DATE:              July 25 '93                                               #
//# UPDATE HISTORY:    December 15 '93 replaced polynomial root solver for       #
//#                    principal strains with explicit formula                  #
//#                    August 22-29 '94 choped to separate files and worked on   #
//#                                   const and & issues                         #
//#                    August 30-31 '94 added use_def_dim to full the CC         #
//#                                   resolved problem with temoraries for       #
//#                                   operators + and - ( +=, -= )               #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//################################################################################
//
#ifndef STRAINTENSOR_CC
#define STRAINTENSOR_CC

#include "straint.h"
#include "utility/matrix/Vector.h"

//! @brief Constructor.
XC::straintensor::straintensor(int rank_of_tensor, double initval)
  : stressstraintensor(rank_of_tensor, initval)
    {   } // default constructor

//! @brief Constructor.
XC::straintensor::straintensor(const double *values)
  : stressstraintensor(values)
    {  }

//! @brief Constructor.
XC::straintensor::straintensor(const std::vector<double> &values)
  : stressstraintensor(values)
    {  }

//! @brief Constructor.
XC::straintensor::straintensor(double initvalue)
  : stressstraintensor(initvalue) {}

//! @brief Constructor.
XC::straintensor::straintensor( const straintensor & x )
  : stressstraintensor(x)
  {}

//! @brief Constructor.
XC::straintensor::straintensor(const Vector &x )
  : stressstraintensor(x)
  {}

//! @brief Constructor.
XC::straintensor::straintensor(const BJtensor & x)
  : stressstraintensor(x) {  } // copy-initializer

//! @brief Constructor.
XC::straintensor::straintensor(const nDarray & x)
  : stressstraintensor(x) {  }  // copy-initializer


// IT IS NOT INHERITED so must be defined in all derived classes
// See ARM page 306.
//! @brief Assignment operator.
XC::straintensor XC::straintensor::operator=( const straintensor & rval)
  {
    stressstraintensor::operator=(rval);
    return *this;
  }


// IT IS NOT INHERITED so must be defined in all derived classes
// See ARM page 306.
//! @brief Assignment operator.
XC::straintensor XC::straintensor::operator=( const BJtensor & rval)
  {
    stressstraintensor::operator=(rval);
    return *this;
  }

//! @brief straintensor addition
XC::straintensor &XC::straintensor::operator+=(const straintensor & rval)
  {
    stressstraintensor::operator+=(rval);
    return *this;
  }

//! @brief straintensor subtraction
XC::straintensor &XC::straintensor::operator-=(const straintensor & rval) 
  {
    stressstraintensor::operator-=(rval);
    return *this;
  }
    
//! @brief product.
XC::straintensor &XC::straintensor::operator*=(const double &rval)
  {
    stressstraintensor::operator*=(rval);
    return *this;
  }

//! @brief Scalar multiplication.
XC::straintensor XC::straintensor::operator*(const double &rval) const
  {
    straintensor retval(*this);
    retval*= rval;
    return retval;
  }


//##############################################################################
double XC::straintensor::equivalent(void) const //Zhaohui added 09-02-2000
  {   
    // Evaluating e_eq = sqrt( 2.0 * epsilon_ij * epsilon_ij / 3.0)
    straintensor pstrain =  *this;
    BJtensor temp  = pstrain("ij") * pstrain("ij");
    double tempd = temp.trace();
    double e_eq  = pow( 2.0 * tempd / 3.0, 0.5 );
    return e_eq;
  }


//##############################################################################
XC::straintensor XC::straintensor::principal(void) const
  {
    straintensor ret;
    compute_principal(ret);
    return ret;
  }

//##############################################################################
XC::straintensor XC::straintensor::deviator() const
  {
    straintensor st_dev;
    compute_deviator(st_dev);
    return st_dev;
  }


//##############################################################################
XC::straintensor XC::straintensor::pqtheta2strain( double p, double q, double theta)
  {
    straintensor ret;

//    double sqrtJ2D = q/3.0;
//    double 2osqrt3 = 2.0/sqrt(3.0);
    double temp = (2.0*q)/(3.0);

    while ( theta >= 2.0*PI )
      theta = theta - 2.0*PI; // if bigger than full cycle
    while ( theta >= 4.0*PI/3.0 )
      theta = theta - 4.0*PI/3.0; // if bigger than four thirds of half cycle
    while ( theta >= 2.0*PI/3.0 )
      theta = theta - 2.0*PI/3.0; // if bigger than two third of half cycle
    while ( theta >= PI/3.0 )
      theta = 2.0*PI/3.0 - theta; // if bigger than one third of half cycle

    double ct  = cos( theta  );
    double ctm = cos( theta - 2.0*PI/3.0 );
    double ctp = cos( theta + 2.0*PI/3.0 );

    ret.val(1,1) = p + temp*ct;
    ret.val(2,2) = p + temp*ctm;
    ret.val(3,3) = p + temp*ctp;

    return ret;

  }


//##############################################################################
XC::straintensor XC::straintensor::evoleq2strain( double evol, double eq )
  {
    straintensor ret;

    ret.val(1,1) = 1./3.*evol + 1./2.*eq;
    ret.val(2,2) = 1./3.*evol + 1./2.*eq;
    ret.val(3,3) = 1./3.*evol - eq;

    return ret;

  }



//##############################################################################
void XC::straintensor::report(const std::string &msg) const
  {
    std::cerr << "\n****************  strain XC::BJtensor report ****************\n";
    std::cerr << msg;

    this->print("st","straintensor st");

    ::printf("I1 = %.8e ; I2 = %.8e ; I3 = %.8e \n",
              Iinvariant1(),Iinvariant2(),Iinvariant3());

    printf("st_trace = %.8e,  mean pressure p = %.8e\n",
             trace(),  trace()/3.0);

    BJtensor I2("I", 2, def_dim_2);

    straintensor st_vol = straintensor(I2 * trace() * (1./3.));
    st_vol.print("st_v","BJtensor st_vol (volumetric part of the st XC::BJtensor)");

    straintensor st_dev = this->deviator();  // - st_vol;
    st_dev.print("st_d","BJtensor st_dev (strain deviator)");

    ::printf("J1 = %.8e ; J2 = %.8e ; J3 = %.8e ;\n",
              Jinvariant1(),Jinvariant2(),Jinvariant3());
//...  straintensor Jinv2 = st_dev("ij")*st_dev("ji")*0.5;

    straintensor st_principal = principal();
    st_principal.print("st_p","principal strain XC::BJtensor");


    ::printf("sig_oct = %.8e  , tau_oct = %.8e\n",
              sigma_octahedral(), tau_octahedral());


    ::printf("ksi=%.6e, ro=%.6e, theta=%.6e=%.6e*PI \n",
              ksi(),    ro(),    theta(),  thetaPI());

    std::cerr << msg;
    std::cerr << "\n############  end of strain XC::BJtensor report ############\n";
  }


//##############################################################################
void XC::straintensor::reportshort(const std::string &msg) const
  {
//    ::printf("\n         ****************** short strain XC::BJtensor report ***\n");
//    if ( msg ) ::printf("         %s",msg);

    this->print("st"," ");

//    ::printf("ksi = %.8e ,ro = %.8e ,theta = %.8e\n",
//              ksi(),       ro(),      theta());

//    ::printf("p=%.12e , q=%.12e , theta=%.12e*PI\n",
//              p_hydrostatic(), q_deviatoric(), thetaPI());

  }

std::ostream &XC::operator<<(std::ostream &os,const XC::straintensor & rhs)
  {
    os << "straintensor: I1 = " << rhs.Iinvariant1() << ", I2 = " << rhs.Iinvariant2() << ", I3 = " << 
      rhs.Iinvariant3() << std::endl;
    os << "st_trace = " << rhs.trace() << ", mean pressure p = " << rhs.trace()/3.0 << std::endl;
    return os;
  }

XC::BJtensor XC::operator+(const BJtensor &lval, const straintensor &rval)
  { return XC::operator+(lval,static_cast<const BJtensor &>(rval)); }
XC::BJtensor XC::operator-(const BJtensor &lval, const straintensor &rval)
  { return XC::operator-(lval,static_cast<const BJtensor &>(rval)); }
XC::BJtensor XC::operator+(const straintensor &lval, const BJtensor &rval)
  { return XC::operator+(static_cast<const BJtensor &>(lval),rval); }
XC::BJtensor XC::operator-(const straintensor &lval, const BJtensor &rval)
  { return XC::operator-(static_cast<const BJtensor &>(lval),rval); }


#endif
