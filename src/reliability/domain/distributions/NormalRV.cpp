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
** (C) Copyright 2001, The Regents of the University of California    **
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
** Reliability module developed by:                                   **
**   Terje Haukaas (haukaas@ce.berkeley.edu)                          **
**   Armen Der Kiureghian (adk@ce.berkeley.edu)                       **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.7 $
// $Date: 2004/08/27 17:51:50 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/domain/distributions/NormalRV.cpp,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu) 
//

#include <reliability/domain/distributions/NormalRV.h>
#include <math.h>
#include <string.h>
#include <classTags.h>


XC::NormalRV::NormalRV(int passedTag, 
    	 double passedMean,
    	 double passedStdv,
    	 double passedStartValue)
:RandomVariable(passedTag, RANDOM_VARIABLE_normal)
{
    tag= passedTag ;
    mju= passedMean;
    sigma= passedStdv;
    startValue= passedStartValue;
}
XC::NormalRV::NormalRV(int passedTag, 
    	 double passedParameter1,
    	 double passedParameter2,
    	 double passedParameter3,
    	 double passedParameter4,
    	 double passedStartValue)
:RandomVariable(passedTag, RANDOM_VARIABLE_normal)
{
    tag= passedTag ;
    mju= passedParameter1;
    sigma= passedParameter2;
    startValue= passedStartValue;
}
XC::NormalRV::NormalRV(int passedTag, 
    	 double passedMean,
    	 double passedStdv)
:RandomVariable(passedTag, RANDOM_VARIABLE_normal)
{
    tag= passedTag ;
    mju= passedMean;
    sigma= passedStdv;
    startValue= getMean();
}
XC::NormalRV::NormalRV(int passedTag, 
    	 double passedParameter1,
    	 double passedParameter2,
    	 double passedParameter3,
    	 double passedParameter4)
:RandomVariable(passedTag, RANDOM_VARIABLE_normal)
{
    tag= passedTag ;
    mju= passedParameter1;
    sigma= passedParameter2;
    startValue= getMean();
}


void XC::NormalRV::Print(std::ostream &s, int flag) const
  {}


double XC::NormalRV::getPDFvalue(double rvValue)
  {
    const double oneOverSigmaRootTwoPi = 1.0/(sigma * sqrt(2.0*M_PI));
    return oneOverSigmaRootTwoPi*exp(-0.5*pow(((rvValue - mju)/sigma),2.0));
  }


double XC::NormalRV::getCDFvalue(double rvValue)
  {
    double result= 0.5 + errorFunction( ((rvValue-mju)/sigma)/sqrt(2.0) )/2.0;
    return result;
  }


double XC::NormalRV::getInverseCDFvalue(double probValue)
  {
    if (probValue < 0.0 || probValue > 1.0) {
    	std::cerr << "WARNING: Illegal probability value input to XC::NormalRV::getInverseCDFvalue()" << std::endl;
    }
    double result= getMean() + getStdv() * sqrt(2.0) * inverseErrorFunction(2*probValue-1.0);
    return result;
}


const std::string XC::NormalRV::getType(void)
{
    return "NORMAL";
}


double 
XC::NormalRV::getMean()
{
    return mju;
}



double 
XC::NormalRV::getStdv()
{
    return sigma;
}


double 
XC::NormalRV::getStartValue()
{
    return startValue;
}

double XC::NormalRV::getParameter1()  {return mju;}
double XC::NormalRV::getParameter2()  {return sigma;}
double XC::NormalRV::getParameter3()  {std::cerr<<"No such parameter in r.v. #"<<tag<<std::endl; return 0.0;}
double XC::NormalRV::getParameter4()  {std::cerr<<"No such parameter in r.v. #"<<tag<<std::endl; return 0.0;}


double XC::NormalRV::errorFunction(double x)
  {

    // ErrorFunction(x)= 2/sqrt(pi) * integral from 0 to x of exp(-t^2) dt.

    double a1,a2,a3,a4,a5;
    double b1,b2,b3,b4;
    double c1,c2,c3,c4,c5,c6,c7,c8,c9;
    double d1,d2,d3,d4,d5,d6,d7,d8;
    double p1,p2,p3,p4,p5,p6;
    double q1,q2,q3,q4,q5;
    double y,z,xnum,xden,del,result= 0;


    // Evaluate  errorFunction  for  |x| <= 0.46875
    if(fabs(x) <= 0.46875)
      {
    	a1= 3.16112374387056560e00;
    	a2= 1.13864154151050156e02;
    	a3= 3.77485237685302021e02; 
    	a4= 3.20937758913846947e03;
    	a5= 1.85777706184603153e-1;
    	b1= 2.36012909523441209e01; 
    	b2= 2.44024637934444173e02;
    	b3= 1.28261652607737228e03; 
    	b4= 2.84423683343917062e03;
    	y= fabs(x);
    	z= y * y;
    	xnum= a5 * z;
    	xden= z;
    	xnum= (xnum + a1) * z;
    	xden= (xden + b1) * z;
    	xnum= (xnum + a2) * z;
    	xden= (xden + b2) * z;
    	xnum= (xnum + a3) * z;
    	xden= (xden + b3) * z;
    	result= x * (xnum + a4) / (xden + b4);
      }
    // Evaluate  errorFunction  for 0.46875 <= |x| <= 4.0
    else if ( fabs(x) > 0.46875 && fabs(x) <= 4.0 )
      {
    	c1= 5.64188496988670089e-1; 
    	c2= 8.88314979438837594e00;
    	c3= 6.61191906371416295e01; 
    	c4= 2.98635138197400131e02;
    	c5= 8.81952221241769090e02; 
    	c6= 1.71204761263407058e03;
    	c7= 2.05107837782607147e03; 
    	c8= 1.23033935479799725e03;
    	c9= 2.15311535474403846e-8;
    	d1= 1.57449261107098347e01; 
    	d2= 1.17693950891312499e02;
    	d3= 5.37181101862009858e02; 
    	d4= 1.62138957456669019e03;
    	d5= 3.29079923573345963e03; 
    	d6= 4.36261909014324716e03;
    	d7= 3.43936767414372164e03; 
    	d8= 1.23033935480374942e03;
    	y= fabs(x);
    	xnum= c9 * y;
    	xden= y;
    	xnum= (xnum + c1) * y;
    	xden= (xden + d1) * y;
    	xnum= (xnum + c2) * y;
    	xden= (xden + d2) * y;
    	xnum= (xnum + c3) * y;
    	xden= (xden + d3) * y;
    	xnum= (xnum + c4) * y;
    	xden= (xden + d4) * y;
    	xnum= (xnum + c5) * y;
    	xden= (xden + d5) * y;
    	xnum= (xnum + c6) * y;
    	xden= (xden + d6) * y;
    	xnum= (xnum + c7) * y;
    	xden= (xden + d7) * y;
    	result= (xnum + c8) / (xden + d8);
    	z= floor(y*16)/16;
    	del= (y-z) * (y+z);
    	result= exp(-z * z) * exp(-del) * result;
      }
    // Evaluate  erfc  for |x| > 4.0
    else if ( fabs(x) > 4.0 )
      {
    	p1= 3.05326634961232344e-1; 
    	p2= 3.60344899949804439e-1;
    	p3= 1.25781726111229246e-1; 
    	p4= 1.60837851487422766e-2;
    	p5= 6.58749161529837803e-4; 
    	p6= 1.63153871373020978e-2;
    	q1= 2.56852019228982242e00; 
    	q2= 1.87295284992346047e00;
    	q3= 5.27905102951428412e-1; 
    	q4= 6.05183413124413191e-2;
    	q5= 2.33520497626869185e-3;
    	y= fabs(x);
    	z= 1 / (y * y);
    	xnum= p6 * z;
    	xden= z;
    	xnum= (xnum + p1) * z;
    	xden= (xden + q1) * z;
    	xnum= (xnum + p2) * z;
    	xden= (xden + q2) * z;
    	xnum= (xnum + p3) * z;
    	xden= (xden + q3) * z;
    	xnum= (xnum + p4) * z;
    	xden= (xden + q4) * z;
    	result= z * (xnum + p5) / (xden + q5);
    	result= (1/sqrt(M_PI) -  result) / y;
    	z= floor(y*16)/16;
    	del= (y-z) * (y+z);
    	result= exp(-z * z) * exp(-del) * result;
      }

    // Final computations
    if(x>0.46875)
      result= (0.5 - result) + 0.5;
    if(x<-0.46875)
      result= (-0.5 + result) - 0.5;
    return result;
  }

double XC::NormalRV::inverseErrorFunction(double y)
  {
    double a1,a2,a3,a4;
    double b1,b2,b3,b4;
    double c1,c2,c3,c4;
    double d1,d2;
    double x= 0,z;

    // Coefficients in rational approximations.
    a1= 0.886226899; 
    a2= -1.645349621;  
    a3= 0.914624893;
    a4= 0.140543331;
    b1= -2.118377725;
    b2= 1.442710462;
    b3= -0.329097515;
    b4= 0.012229801;
    c1= -1.970840454;
    c2= -1.624906493;
    c3= 3.429567803;
    c4= 1.641345311;
    d1= 3.543889200;  
    d2= 1.637067800;

    // Central range
    if(fabs(y)<=0.7)
      {
    	z= y * y;
    	x= y * (((a4*z+a3)*z+a2)*z+a1) / ((((b4*z+b3)*z+b2)*z+b1)*z+1);
     }

    // Near end-points of range
    if ( y > 0.7  &&  y < 1 )
    {
    	z= sqrt(-log((1-y)/2));
    	x= (((c4*z+c3)*z+c2)*z+c1) / ((d2*z+d1)*z+1);
    }

    if ( y < -0.7  &&  y > -1 )
    {
    	z= sqrt(-log((1+y)/2));
    	x= -(((c4*z+c3)*z+c2)*z+c1) / ((d2*z+d1)*z+1);
    }


    // Two steps of Newton-Raphson correction to full accuracy.
    // Without these steps, erfinv(y) would be about 3 times
    // faster to compute, but accurate to only about 6 digits.
    x= x - (errorFunction(x) - y) / (2.0/sqrt(M_PI) * exp(-pow(x,2.0)));
    x= x - (errorFunction(x) - y) / (2.0/sqrt(M_PI) * exp(-pow(x,2.0)));

    // Exceptional cases not treated for now
    // k= find(y== -1);
    // if ~isempty(k), x(k)= -inf*k; end
    // k= find(y== 1);
    // if ~isempty(k), x(k)= inf*k; end
    // k= find(std::abs(y) > 1);
    // if ~isempty(k), x(k)= NaN; end
    return x;
  }








