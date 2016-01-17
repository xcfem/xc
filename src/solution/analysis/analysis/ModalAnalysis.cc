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
//ModalAnalysis.cpp


#include "ModalAnalysis.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "xc_utils/src/base/utils_any.h"
#include "xc_utils/src/nucleo/InterpreteRPN.h"
#include "utility/matrix/Vector.h"
#include "utility/matrix/Matrix.h"

//! @brief Constructor.
XC::ModalAnalysis::ModalAnalysis(SoluMethod *metodo)
  :EigenAnalysis(metodo), espectro() {}

//! @brief Devuelve la aceleración que corresponde al periodo
//! que se pasa como parámetro.
double XC::ModalAnalysis::getAcceleration(const double &T) const
  { return espectro(T); }

//! @brief Devuelve las aceleraciones que corresponden a los periodos
//! que se pasan como parámetro.
XC::Vector XC::ModalAnalysis::getAccelerations(const Vector &periodos) const
  {
    const int sz= periodos.Size();
    Vector retval(sz);
    for(int i= 0;i<sz;i++)
      retval[i]= getAcceleration(periodos(i));
    return retval;
  }

//! @brief Devuelve las aceleraciones que corresponden a los modos
//! calculados.
XC::Vector XC::ModalAnalysis::getModalAccelerations(void) const
  { return getAccelerations(getPeriodos()); }

//! @brief Devuelve la fuerzas estática equivalente para el modo
//! que se pasa como parámetro.
XC::Vector XC::ModalAnalysis::getEquivalentStaticLoad(int mode) const
  {
    const double accel= getAcceleration(getPeriodo(mode));
    return EigenAnalysis::getEquivalentStaticLoad(mode,accel);
  }

//! @brief Devuelve los coeficientes de correlación entre modos que
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

