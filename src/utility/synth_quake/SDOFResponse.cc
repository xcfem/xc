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

//===----------------------------------------------------------------------===//
//
//                                   xara
//                              https://xara.so
//
//===----------------------------------------------------------------------===//
//
// Copyright (c) 2025, OpenSees/Xara Developers
// All rights reserved.  No warranty, explicit or implicit, is provided.
//
// This source code is licensed under the BSD 2-Clause License.
// See LICENSE file or https://opensource.org/licenses/BSD-2-Clause
//
//===----------------------------------------------------------------------===//
//
//  =============   ====================================================
//  m               mass
//  zeta            damping ratio
//  k               stiffness
//  Fy              yielding strength
//  alpha           strain-hardening ratio
//  dtF             time step for input data
//  filename        input data file, one force per line
//  dt              time step for analysis
//  uresidual       residual displacement at the end of previous analysis
//                             (optional, default=0)
//  umaxprev        previous displacement (optional, default=0)
//  =============   ====================================================

// The command returns a list of five response quantities.

// =============   =====================================================
// umax            maximum displacement during analysis
// u               displacement at end of analysis
// up              permanent residual displacement at end of analysis
// amax            maximum acceleration during analysis
// tamax           time when maximum accleration occurred
// =============   =====================================================
//
// https://portwooddigital.com/2021/02/14/how-many-clicks-does-it-take/
//
//

#include "SDOFResponse.h"
#include <cmath>

//! @brief Return a Python dictionary with the object members values.
boost::python::dict XC::sdof_response::getPyDict(void) const
  {
    boost::python::dict retval;
    retval["max_displ"]= max_displ;
    retval["max_vel"]= max_vel;
    retval["u"]= u;
    retval["up"]= up;
    retval["max_accel"]= max_accel;
    retval["time_max_accel"]= time_max_accel;
    retval["max_true_accel"]= max_true_accel;
    retval["time_max_true_accel"]= time_max_true_accel;
    return retval;
  }

//! @brief Default constructor.
XC::SDOFResponse::SDOFResponse()
  : m(0.0), zeta(0.0), k(0.0), Fy(0.0), alpha(0.0)
  {}

//! @brief Constructor.
XC::SDOFResponse::SDOFResponse(double _m,
			       double _zeta,
			       double _k,
			       double _Fy,
			       double _alpha)
  : m(_m), zeta(_zeta), k(_k), Fy(_Fy), alpha(_alpha)
  {}

//! @brief Computes bilinear elasto-plastic response of an SDF system for
//! the given ground motion.
//! @param dtF: time step for input data.
//! @param dt: time step for analysis.
//! @param uresidual: residual displacement at the end of previous analysis
//!                   (optional, default=0).
//! @param max_prev_displ: previous displacement (optional, default=0).
//! @param accelerations: input accelerations.
int XC::SDOFResponse::get_response(double dtF,
				   double dt,
				   double uresidual,
				   double max_prev_displ,
				   const std::vector<double> &accelerations,
				   struct sdof_response &result)
  {
    const double gamma= 0.5;
    const double beta= 0.25;
    const double tol= 1.0e-8;
    const int maxIter= 10;
 
    double c= this->zeta*2*sqrt(this->k*this->m);
    double Hkin= this->alpha/(1.0-this->alpha)*this->k;

    double p0= 0.0;
    double u0= uresidual;
    double v0= 0.0;
    double fs0= 0.0;
    double a0= (p0-c*v0-fs0)/this->m;

    double a1= this->m/(beta*dt*dt) + (gamma/(beta*dt))*c;
    double a2= this->m/(beta*dt) + (gamma/beta-1.0)*c;
    double a3= (0.5/beta-1.0)*this->m + dt*(0.5*gamma/beta-1.0)*c;

    double au= 1.0/(beta*dt*dt);
    double av= 1.0/(beta*dt);
    double aa= 0.5/beta-1.0;

    double vu= gamma/(beta*dt);
    double vv= 1.0-gamma/beta;
    double va= dt*(1.0-0.5*gamma/beta);
    
    double kT0= this->k;

    double max_displ= fabs(max_prev_displ);
    double max_vel= 0.0;
    double max_accel= 0.0;
    double time_max_accel= 0.0;
    double max_true_accel= 0.0;
    double time_max_true_accel= 0.0;
    double up= uresidual;
    double up0= up;

    //int i= 0;
    double u=0, du, v, a, fs, zs, ftrial, kT, kTeff, dg, phat, R, R0;

    const size_t sz= accelerations.size();
    for(size_t j= 0;j<sz;j++)
      {
	const double &inputAccel= accelerations[j];
        //i++;
    
        u= u0;
      
        fs= fs0;
        kT= kT0;
        up= up0;
      
        phat= inputAccel + a1*u0 + a2*v0 + a3*a0;
      
        R= phat - fs - a1*u;
        R0= R;
        if (R0 == 0.0)
	  { R0= 1.0; }
    
        int iter= 0;

        while (iter < maxIter && fabs(R/R0) > tol)
	  {
            iter++;

            kTeff= kT + a1;

            du= R/kTeff;

            u= u + du;

            fs= this->k*(u-up0);
            zs= fs-Hkin*up0;
            ftrial= fabs(zs)-this->Fy;
            if (ftrial > 0)
	      {
                dg= ftrial/(this->k+Hkin);
                if (fs < 0)
		  {
                    fs= fs + dg*this->k;
                    up= up0 - dg;
		  }
		else
		  {
                    fs= fs - dg*this->k;
                    up= up0 + dg;
		  }
                kT= this->k*Hkin/(this->k+Hkin);
	      }
	    else
	      {
                kT= this->k;
	      }
      
            R= phat - fs - a1*u;
	  }

        v= vu*(u-u0) + vv*v0 + va*a0;
        a= au*(u-u0) - av*v0 - aa*a0;
	const double true_accel= a-inputAccel;

        u0= u;
        v0= v;
        a0= a;
        fs0= fs;
        kT0= kT;
        up0= up;

        if(fabs(u) > max_displ)
	  {
            max_displ= fabs(u);
	  }
        if(fabs(v) > max_vel)
	  {
            max_vel= fabs(v);
	  }
        if(fabs(a) > max_accel)
	  {
            max_accel= fabs(a);
            time_max_accel= (j+1)*dt;
	  }
        if(fabs(true_accel) > max_true_accel)
	  {
            max_true_accel= fabs(true_accel);
            time_max_true_accel= (j+1)*dt;
	  }
      }
  
    result= sdof_response(max_displ, max_vel, u, up, max_accel, time_max_accel, max_true_accel, time_max_true_accel);

    return 0;
  }

//! @brief Computes bilinear elasto-plastic response of an SDF system for
//! the given ground motion.
//! @param dtF: time step for input data.
//! @param dt: time step for analysis.
//! @param uresidual: residual displacement at the end of previous analysis
//!                   (optional, default=0).
//! @param max_prev_displ: previous displacement (optional, default=0).
//! @param accelerations: input accelerations.
boost::python::dict XC::SDOFResponse::getResponse(double dtF,
						  double dt,
						  const boost::python::list &accelerations,
						  double uresidual,
						  double max_prev_displ)
  {
    const size_t sz= boost::python::len(accelerations);
    std::vector<double> tmp(sz);
    for(size_t i= 0; i<sz; i++)
      tmp[i]= boost::python::extract<double>(accelerations[i]);
    sdof_response resp;
    this->get_response(dtF, dt, uresidual, max_prev_displ, tmp, resp);
    return resp.getPyDict();
  }

//! @brief Print stuff.
void XC::SDOFResponse::Print(std::ostream &s) const
  {
    s << "mass: " << this->m
      << " damping ratio: " << this->zeta
      << " stiffness: " << this->k
      << " yielding strength: " << this->Fy
      << " strain-hardening ratio: " << this->alpha
      << std::endl;
  }    

//! Invokes {\em Print(s)} on the SDOFResponse object \p E. 
std::ostream &XC::operator<<(std::ostream &s, const SDOFResponse &r)
  {
    r.Print(s);
    return s;
  }


