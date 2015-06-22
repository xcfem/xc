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
//BodyForces.h

#ifndef BodyForces_h
#define BodyForces_h

#include "utility/actor/actor/MovableObject.h"
#include "utility/matrix/Vector.h"

namespace XC {

//! \ingroup Elem
//
//! @brief Body forces over an element.
class BodyForces: public MovableObject
  {
  protected:
    Vector forces; //!< Body force components.

    DbTagData &getDbTagData(void) const;
    int sendData(CommParameters &);
    int recvData(const CommParameters &);

  public:
    BodyForces(const size_t &sz);

    inline size_t size(void) const
      { return forces.Size(); } 

    inline double &operator[](const size_t &i)
      { return forces[i]; }
    inline const double &operator[](const size_t &i) const
      { return forces[i]; }
    inline double &operator()(const size_t &i)
      { return forces(i); }
    inline const double &operator()(const size_t &i) const
      { return forces(i); }

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);

    virtual void Print(std::ostream &s) const;
  };

inline std::ostream &operator<<(std::ostream &os,const BodyForces &bf)
  {
    bf.Print(os);
    return os;
  }
} //fin namespace XC

#endif
