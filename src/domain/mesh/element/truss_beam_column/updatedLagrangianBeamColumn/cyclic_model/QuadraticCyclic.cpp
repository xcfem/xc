//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
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
//QuadraticCyclic.cpp

#include "QuadraticCyclic.h"
#include <cmath>

XC::Matrix XC::QuadraticCyclic::X(3,3);
XC::Vector XC::QuadraticCyclic::Y(3);
XC::Vector XC::QuadraticCyclic::A(3);

XC::QuadraticCyclic::QuadraticCyclic(int tag, double wt, double fac_y)
:CyclicModel(tag, -1), weightFactor(wt), facty(fac_y),
 qx1(0.0), qy1(0.0), qx2(0.0), qy2(0.0), qx3(0.0), qy3(0.0)
 {}

int XC::QuadraticCyclic::createFullCycleTask()
  {
    int res = this->XC::CyclicModel::createFullCycleTask();
    res+= createTask();
    return res;
  }

int XC::QuadraticCyclic::createHalfCycleTask()
  {
    int res = this->XC::CyclicModel::createHalfCycleTask();
    res+= createTask();
    return res;
  }


XC::CyclicModel *XC::QuadraticCyclic::getCopy()
  {
    XC::CyclicModel *newModel = new XC::QuadraticCyclic(getTag(), weightFactor, facty);	
    return newModel;
  }


int XC::QuadraticCyclic::createTask()
  {
    if(f_bgn*f_end < 0)
      {
		double k0 = k_init;
		double delx0 = f_bgn/(resFactor*k0);  //say +ive

		double qy = facty;
		double x1 = d_bgn,                   y1 = f_bgn;

//		double x2 = d_bgn + delx0*(qy-1),    y2 = qy*f_bgn;
//		double x3 = qx*(d_bgn-delx0),        y3 = 0.0;

//		double x2 = qx*(d_bgn - delx0),      y2 = 0.0;
//		double x3 = d_end;
//		double y3 = f_end;



		double delx2 = delx0*(1 - qy);   // +ive
		double x2 = d_bgn - delx2;       // +ive
		double y2 = qy*f_bgn;            // +ive

		double x0 = d_bgn - delx0;       // +ive (at least 0)
		double y0 = 0.0;  // always - don't change
		
		double R  = sqrt( (x2 - x0)*(x2 - x0) + (y2 - y0)*(y2 - y0));
		double delx_end = d_end - x0;    // -ive
        double dely_end = f_end - y0;    // -ive
		
		double R_end = sqrt(delx_end*delx_end + dely_end*dely_end);

		double delx3 = delx_end*R/R_end; // -ive
		double y3 = f_end*R/R_end;       // -ive
		double x3 = x0 + delx3;          // +ive
		
//        std::cerr << "R = " << R << ", Re = " << R_end << ", R/Re = " << R/R_end << std::endl;
//        std::cerr << "x1, y1 = " << x1 << ", " << y1 << std::endl;
//        std::cerr << "x2, y2 = " << x2 << ", " << y2 << std::endl;
//        std::cerr << "x0, y0 = " << x0 << ", " << y0 << std::endl;
//        std::cerr << "x3, y3 = " << x3 << ", " << y3 << std::endl;
//
//        std::cerr << *this;
//        std::cerr << "\a";

        qx1 = x1; qy1 = y1;
        qx2 = x2; qy2 = y2;
        qx3 = x3; qy3 = y3;
                                       
		solveQuad(x1, y1, x2, y2, x3, y3);
	}

	return 0;
}


int XC::QuadraticCyclic::solveQuad(double x1, double y1, double x2,
                               double y2, double x3, double y3)
{

	X(0,0) = x1*x1; X(0,1) = x1; X(0,2) = 1.0;
	X(1,0) = x2*x2; X(1,1) = x2; X(1,2) = 1.0;
	X(2,0) = x3*x3; X(2,1) = x3; X(2,2) = 1.0;

	Y(0) = y1; Y(1) = y2; Y(2) = y3;

	A = Y/X;
	a = A(0);
	b = A(1);
	c = A(2);
	
	// std::cerr << A;

	return 0;
}

double XC::QuadraticCyclic::getQuadFactor(double x1, double y1, double dx)
{
// double dx = (d_curr - d_hist)/2;
double x_nxt = x1 + dx;
double y_nxt = a*x_nxt*x_nxt + b*x_nxt + c;
double x_prv = x1 - dx;
double y_prv = a*x_prv*x_prv + b*x_prv + c;

//	std::cerr << "x1, y1 = " << x_nxt << ", " <<  y_nxt << std::endl;
//	std::cerr << "x2, y2 = " << x_prv << ", " <<  y_prv << std::endl;
	return (rationalize(x_prv, y_prv, x_nxt, y_nxt));

}


double XC::QuadraticCyclic::getTaskFactor()
{
double tfactor;
//	// redundant - only for print
//	if(d_curr >= 0 && !initYieldPos)
//		return 1.0;
//	if(d_curr  < 0 && !initYieldNeg)
//		return 1.0;
//	// end redundant



	if(yielding /*&& fabs(d_curr) >= fabs(d_end) */)
//		return resFactor; // will eventually unload
		tfactor = cycFactor_hist;
    else
    {
    	if(f_bgn*f_end < 0)
    	{
    		// if(contains(0.0, f_bgn, f_curr))
			if(contains(qy1, qy3, f_curr))  
    			tfactor = getQuadFactor(d_curr, f_curr, (d_curr - d_hist)/2);
    		else
			{
				tfactor=rationalize(d_curr, f_curr, d_end, f_end);
				tfactor = weightFactor*tfactor + (1 - weightFactor)*resFactor;
		    }
    	}
    	else // half-cycle
		{
    		tfactor = rationalize(d_bgn, f_bgn, d_end, f_end);
            tfactor = weightFactor*tfactor + (1 - weightFactor)*resFactor;
		}
	}

//	std::cerr << "tfactor = " << tfactor << std::endl;
	return tfactor;
}

void XC::QuadraticCyclic::Print (std::ostream &s, int flag)
{
	this->XC::CyclicModel::Print (s, flag);
	s << "+QuadraticCyclic\n";
	s << "   taskFactor = " << cycFactor << std::endl;
	s << "   a=" << a <<", b=" << b <<", c=" << c << std::endl;
	s << "----------------------------------------"
      << "----------------------------------------"
	  << std::endl;
}



