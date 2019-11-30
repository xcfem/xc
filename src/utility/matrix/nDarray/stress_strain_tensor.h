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

#ifndef STRESSSTRAINTENSOR_H
#define STRESSSTRAINTENSOR_H

#include "utility/matrix/nDarray/BJtensor.h"
#include <iostream>

namespace XC {
class Vector;


//! @ingroup Matrix
//
class stressstraintensor : public BJtensor
  {
  public:
    // just send appropriate arguments to the base constructor
    stressstraintensor(int rank_of_tensor=2, double initval=0.0); // default constructor
    stressstraintensor(const boost::python::list &l);
    stressstraintensor(const double *values );
    stressstraintensor(const std::vector<double> &values );
    stressstraintensor(double initvalue );
    explicit stressstraintensor(const Vector &);

    stressstraintensor(const stressstraintensor & x );
    stressstraintensor(const BJtensor & x); // copy-initializer
    stressstraintensor(const nDarray & x); // copy-initializer

    stressstraintensor operator=(const stressstraintensor & rval);
    stressstraintensor operator=(const BJtensor & rval);
    stressstraintensor operator=(const nDarray & rval);

    Vector getVector(const size_t &dim) const;

    double Iinvariant1(void) const;
    double Iinvariant2(void) const;
    double Iinvariant3(void) const;

    double Jinvariant1(void) const;
    double Jinvariant2(void) const;
    double Jinvariant3(void) const;

    void compute_deviator(stressstraintensor &) const;
    void compute_principal(stressstraintensor &) const;

    double sigma_octahedral(void) const;
    double tau_octahedral(void) const;

    double ksi(void) const;
    double ro(void) const;
    double rho(void) const;
    double theta() const;
    double thetaPI(void) const;

    double p_hydrostatic(void) const;
    double q_deviatoric(void) const;
  };

} // end of XC namespace

#endif

