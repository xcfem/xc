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
//ResponseQuantities.h                                        
                                                                        
#ifndef ResponseQuantities_h
#define ResponseQuantities_h


#include "utility/actor/actor/MovableObject.h"
#include "utility/matrix/Vector.h"

namespace XC {

//! @ingroup TransientIntegrator
//
//! @brief Response quantities.
class ResponseQuantities: public MovableObject
  {
  private:
    Vector R, Rdot, Rdotdot; //!< Response quantities
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    ResponseQuantities(const int &size= 0);

    inline const Vector &get(void) const
      { return R; }
    inline const Vector &getDot(void) const
      { return Rdot; }
    inline const Vector &getDotDot(void) const
      { return Rdotdot; }
    inline Vector &get(void)
      { return R; }
    inline Vector &getDot(void)
      { return Rdot; }
    inline Vector &getDotDot(void)
      { return Rdotdot; }
    inline void set(const Vector &v)
      { R= v; }
    inline void setDot(const Vector &v)
      { Rdot= v; }
    inline void setDotDot(const Vector &v)
      { Rdotdot= v; }

    void setDisp(const ID &,const Vector &);
    void setVel(const ID &,const Vector &);
    void setAccel(const ID &,const Vector &);

    void resize(const int &size);
    void Zero(void);

    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);

    void Print(std::ostream &s);        
  };
} // fin namespace XC

#endif

