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

#ifndef UniaxialStateVars_h
#define UniaxialStateVars_h

#include "utility/actor/actor/MovableObject.h"

namespace XC {
//! @ingroup MatUnx
//
//! @brief UniaxialStateVars almacena los valores de la
//! deformación, la tensión y la rigidez del material.
class UniaxialStateVars: public MovableObject
  {
  private:
    double strain;
    double stress;
    double tangent;

  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    UniaxialStateVars(void);

    inline const double &getStrain(void) const
      { return strain; }
    inline const double &getStress(void) const
      { return stress; }
    inline const double &getTangent(void) const
      { return tangent; }
    inline double &Strain(void)
      { return strain; }
    inline double &Stress(void)
      { return stress; }
    inline double &Tangent(void)
      { return tangent; }

    int revertToStart(const double &);

    int sendSelf(CommParameters &);  
    int recvSelf(const CommParameters &);
    
    void Print(std::ostream &s, int flag =0);
    
  };
} // end of XC namespace


#endif

