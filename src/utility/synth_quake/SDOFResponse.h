// -*-c++-*-
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

#ifndef SDOFResponse_h
#define SDOFResponse_h

#include <ostream>
#include <vector>
#include <boost/python.hpp>
#include <boost/python/dict.hpp>

namespace XC {
struct sdof_response
  {
    double max_displ, max_vel, u, up, max_accel, time_max_accel;
    sdof_response(void)
      : max_displ(0.0), max_vel(0.0), u(0.0), up(0.0), max_accel(0.0), time_max_accel(0.0)
    {}
    sdof_response(double md, double mv, double _u, double _up, double ma, double tma)
      : max_displ(md), max_vel(mv), u(_u), up(_up), max_accel(ma), time_max_accel(tma)
    {}
    boost::python::dict getPyDict(void) const;
  };

//! @brief Bilinear elasto-plastic response of an SDF system.
//!
//! Bilinear elasto-plastic response of an SDF system for
//! any given mass, damping, stiffness, yield force, and input
//! ground motion.
//! @ingroup Utils
class SDOFResponse
  {
  private:
    double m; //!< mass.
    double zeta; //!< damping ratio.
    double k; //!< stiffness.
    double Fy; //!< yielding strength.
    double alpha; //!< strain-hardening ratio.
    
    int get_response(double dtF,
		     double dt,
		     double uresidual,
		     double max_prev_displ,
		     const std::vector<double> &forces,
		     struct sdof_response &result);
  public:
    SDOFResponse();
    SDOFResponse(double m,
		 double zeta,
		 double k,
		 double Fy,
		 double alpha);

    inline void setMass(const double &d)
      { this->m= d; }
    inline double getMass() const
      { return this->m; }
    inline void setDampingRatio(const double &d)
      { this->zeta= d; }
    inline double getDampingRatio() const
      { return this->zeta; }
    inline void setStiffness(const double &d)
      { this->k= d; }
    inline double getStiffness() const
      { return this->k; }
    inline void setYieldingStrength(const double &d)
      { this->Fy= d; }
    inline double getYieldingStrength() const
      { return this->Fy; }
    inline void setStrainHardeningRatio(const double &d)
      { this->alpha= d; }
    inline double getStrainHardeningRatio() const
      { return this->alpha; }

    boost::python::dict getResponse(double dtF,
				    double dt,
				    const boost::python::list &forces,
				    double uresidual= 0.0,
				    double max_prev_displ= 0.0);
    
    virtual void Print(std::ostream &s) const;   
    friend std::ostream &operator<<(std::ostream &, const SDOFResponse &);    
  };
std::ostream &operator<<(std::ostream &, const SDOFResponse &);
} // end of XC namespace


#endif

