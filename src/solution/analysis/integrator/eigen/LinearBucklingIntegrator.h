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
//LinearBucklingIntegrator.h
// Description: This file contains the class definition of LinearBucklingIntegrator.
// LinearBucklingIntegrator is an algorithmic class for setting up the finite element 
// equations for a eigen problem analysis. 
//
// This class is inheritanted from the base class of Integrator which was
// created by fmk (Frank).


#ifndef LinearBucklingIntegrator_h
#define LinearBucklingIntegrator_h

#include <solution/analysis/integrator/EigenIntegrator.h>

namespace XC {

//! @ingroup EigenIntegrator
//
//! @brief LinearBucklingIntegrator is an algorithmic class for setting up the finite element 
//! equations for a linear buckling analysis. 
class LinearBucklingIntegrator : public EigenIntegrator
  {
  protected:

    friend class SoluMethod;
    LinearBucklingIntegrator(SoluMethod *);
    Integrator *getCopy(void) const;
  public:
     
    // methods to form the K(t) and K(t+Dt) matrices.
    virtual int formKt(void);
    virtual int formKtplusDt(void);
    void Print(std::ostream &,int);
  };
inline Integrator *LinearBucklingIntegrator::getCopy(void) const
  { return new LinearBucklingIntegrator(*this); }
} // end of XC namespace

#endif




