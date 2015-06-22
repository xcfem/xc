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
//IntegratorVectors.h

#ifndef IntegratorVectors_h
#define IntegratorVectors_h

#include "utility/actor/actor/MovableObject.h"
#include "utility/matrix/Vector.h"

namespace XC {

//! @ingroup Integrator
//
//! @brief Vectores empleados en varios integradores.
class IntegratorVectors: public MovableObject
  {
  protected:
    Vector deltaUhat, deltaUbar, deltaU, deltaUstep;
    Vector phat; // the reference load vector
    double deltaLambdaStep, currentLambda;

    void resize(const int &size);
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    IntegratorVectors(void);

    inline const Vector &getPhat(void) const
      { return phat; }
    inline Vector &getPhat(void)
      { return phat; }

    inline const Vector &getDeltaUhat(void) const
      { return deltaUhat; }
    inline void setDeltaUhat(const Vector &v)
      { deltaUhat= v; }

    inline const Vector &getDeltaUbar(void) const
      { return deltaUbar; }
    inline void setDeltaUbar(const Vector &v)
      { deltaUbar= v; }

    inline const Vector &getDeltaUstep(void) const
      { return deltaUstep; }
    inline void setDeltaUstep(const Vector &v)
      { deltaUstep= v; }

    inline const Vector &getDeltaU(void) const
      { return deltaU; }
    inline void setDeltaU(const Vector &v)
      { deltaU= v; }

    inline const double &getDeltaLambdaStep(void) const
      { return deltaLambdaStep; }
    inline void setDeltaLambdaStep(const double &d)
      { deltaLambdaStep= d; }

    inline const double &getCurrentLambda(void) const
      { return currentLambda; }
    inline double &getCurrentLambda(void)
      { return currentLambda; }
    inline void setCurrentLambda(const double &d)
      { currentLambda= d; }

    void newStep(const double &dLambda,const Vector &);
    void determineUhat(LinearSOE &);
    void distribDetermineUhat(const int &,LinearSOE &);
    void solve(const Vector &,LinearSOE &);
    void update(const double &);
    void domainChanged(const size_t &sz,IncrementalIntegrator &,LinearSOE &);

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
  };
} // fin namespace XC

#endif

