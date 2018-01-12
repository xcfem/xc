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
//ModalAnalysis.cpp


#include "ModalAnalysis.h"



#include "utility/matrix/Vector.h"
#include "utility/matrix/Matrix.h"

//! @brief Constructor.
XC::ModalAnalysis::ModalAnalysis(AnalysisAggregation *analysis_aggregation)
  :EigenAnalysis(analysis_aggregation), espectro() {}

//! @brief Returns the acceleration that corresponds to the period
//! being passed as parameter.
double XC::ModalAnalysis::getAcceleration(const double &T) const
  { return espectro(T); }

//! @brief Returns the accelerations that correspond to the periods
//! being passed as parameters.
XC::Vector XC::ModalAnalysis::getAccelerations(const Vector &periodos) const
  {
    const int sz= periodos.Size();
    Vector retval(sz);
    for(int i= 0;i<sz;i++)
      retval[i]= getAcceleration(periodos(i));
    return retval;
  }

//! @brief Returns the aceleraciones que corresponden a los modos
//! calculados.
XC::Vector XC::ModalAnalysis::getModalAccelerations(void) const
  { return getAccelerations(getPeriodos()); }

//! @brief Return the equivalent static load for the mode
//! being passed as parameter.
XC::Vector XC::ModalAnalysis::getEquivalentStaticLoad(int mode) const
  {
    const double accel= getAcceleration(getPeriodo(mode));
    return EigenAnalysis::getEquivalentStaticLoad(mode,accel);
  }

//! @brief Returns the correlation coefficients between modes
//! being used in CQC method.
//! @param zeta: Dumping for each mode.
//! See expression 26-107 from book "Dynamics of Structures" from Clough and Penzien
XC::Matrix XC::ModalAnalysis::getCQCModalCrossCorrelationCoefficients(const Vector &zeta) const
  {
    const Vector omega= getAngularFrequencies();
    const size_t sz= omega.Size();
    Matrix retval(sz,sz);
    for(size_t i= 0;i<sz;i++)
      for(size_t j= i;j<sz;j++)
        {
          const double r= omega[i]/omega[j];
          if(r>1)
	    std::cerr << "ModalAnalysis::getCQCModalCrossCorrelationCoefficients; ERROR"
                      << " the angular frequencies should be in increasing order." << std::endl;
          retval(i,j)= 8*sqrt(zeta[i]*zeta[j])*(zeta[i]+r*zeta[j])*pow(r,1.5)/(sqr(1-r*r)+4*zeta[i]*zeta[j]*r*(1+r*r)+4*(sqr(zeta[i])+sqr(zeta[j]))*r*r);
          if(i!=j)
            retval(j,i)= retval(i,j);
        }
    return retval;
  }

