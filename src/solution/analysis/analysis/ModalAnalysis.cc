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
XC::ModalAnalysis::ModalAnalysis(SoluMethod *metodo)
  :EigenAnalysis(metodo), espectro() {}

//! @brief Return the aceleración que corresponde al periodo
//! being passed as parameter.
double XC::ModalAnalysis::getAcceleration(const double &T) const
  { return espectro(T); }

//! @brief Returns the aceleraciones que corresponden a los periodos
//! que se pasan como parámetro.
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

//! @brief Return the fuerzas estática equivalente para el modo
//! being passed as parameter.
XC::Vector XC::ModalAnalysis::getEquivalentStaticLoad(int mode) const
  {
    const double accel= getAcceleration(getPeriodo(mode));
    return EigenAnalysis::getEquivalentStaticLoad(mode,accel);
  }

//! @brief Returns the coeficientes de correlación entre modos que
//! se emplean en el método CQC.
//! @param zeta: Amortiguamiento en cada modo analizado.
//! Ver expresión 26-107 del libro Dynamics of Structurs de Clough and Penzien
XC::Matrix XC::ModalAnalysis::getCQCModalCrossCorrelationCoefficients(const Vector &zeta) const
  {
    const Vector omega= getPulsaciones();
    const size_t sz= omega.Size();
    Matrix retval(sz,sz);
    for(size_t i= 0;i<sz;i++)
      for(size_t j= i;j<sz;j++)
        {
          const double r= omega[i]/omega[j];
          if(r>1)
	    std::cerr << "ModalAnalysis::getCQCModalCrossCorrelationCoefficients; ERROR"
                      << " las pulsaciones deberían estar en orden creciente." << std::endl;
          retval(i,j)= 8*sqrt(zeta[i]*zeta[j])*(zeta[i]+r*zeta[j])*pow(r,1.5)/(sqr(1-r*r)+4*zeta[i]*zeta[j]*r*(1+r*r)+4*(sqr(zeta[i])+sqr(zeta[j]))*r*r);
          if(i!=j)
            retval(j,i)= retval(i,j);
        }
    return retval;
  }

