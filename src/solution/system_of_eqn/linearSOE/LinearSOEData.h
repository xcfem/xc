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
//LinearSOEData.h
                                                                        
                                                                        
#ifndef LinearSOEData_h
#define LinearSOEData_h

#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include "utility/matrix/Vector.h"

namespace XC {

//! @ingroup SOE
//
//! @brief Datos (tamaño, arrays,...) para los sistemas de
//! ecuaciones lineales.
class LinearSOEData : public LinearSOE
  {
  protected:
    int size; //! order of A
    Vector B, X;   //! 1d arrays containing coefficients of B and X  

    void inic(const size_t &);
    inline const double &getB(const size_t &i) const
      { return B[i]; }
    inline double &getB(const size_t &i)
      { return B(i); }
    inline const double &getX(const size_t &i) const
      { return X(i); }
    inline double &getX(const size_t &i)
      { return X(i); }
    inline double *getPtrB(void)
      { return B.getDataPtr(); }
    inline const double *getPtrB(void) const
      { return B.getDataPtr(); }
    inline double *getPtrX(void)
      { return X.getDataPtr(); }
    inline const double *getPtrX(void) const
      { return X.getDataPtr(); }

    LinearSOEData(SoluMethod *,int classTag,int N= 0);
  public:
    virtual int getNumEqn(void) const;
    virtual void zeroB(void);
    virtual void zeroX(void);
    virtual void zero(void);

    virtual int addB(const Vector &, const double &fact= 1.0);    
    virtual int addB(const Vector &, const ID &,const double &fact= 1.0);    
    virtual int setB(const XC::Vector &v,const double &fact= 1.0);
    virtual void setX(int loc, double value);
    virtual void setX(const Vector &x);
    virtual const Vector &getX(void) const;
    virtual Vector &getX(void);
    virtual const Vector &getB(void) const;
    virtual Vector &getB(void);
    virtual double normRHS(void) const;

    void receiveB(const CommParameters &);
    void receiveX(const CommParameters &);
    void receiveBX(const CommParameters &);
    void sendB(CommParameters &) const;
    void sendBX(CommParameters &) const;
  };
} // fin namespace XC


#endif

