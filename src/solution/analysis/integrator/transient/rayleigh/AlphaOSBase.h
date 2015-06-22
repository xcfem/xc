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
//AlphaOSBase.h


#ifndef AlphaOSBase_h
#define AlphaOSBase_h

#include "solution/analysis/integrator/transient/rayleigh/HHTBase.h"

namespace XC {
class Vector;

//! @ingroup RayleighIntegrator
//
//! @brief AlphaOSBase is an algorithmic class for performing a transient analysis
//! using the Alpha-Operator-Splitting integration scheme.
//! The parameter alpha corresponds to 1+alpha_{HHT}.
class AlphaOSBase : public HHTBase
  {
  protected:
    int updateCount; //! method should only have one update per step
    ResponseQuantities Upt; //!< predictor quantities at time t
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);

    AlphaOSBase(SoluMethod *,int classTag);
    AlphaOSBase(SoluMethod *,int classTag,double beta, double gamma);
    AlphaOSBase(SoluMethod *,int classTag,double beta, double gamma,const RayleighDampingFactors &rF);
  public:
    int newStep(double deltaT);    
    int revertToLastStep(void);
  };
} // fin namespace XC

#endif
