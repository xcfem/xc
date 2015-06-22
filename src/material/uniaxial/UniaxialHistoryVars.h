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

#ifndef UniaxialHistoryVars_h
#define UniaxialHistoryVars_h

#include "utility/actor/actor/MovableObject.h"

namespace XC {
//! @ingroup MatUnx
//
//! @brief UniaxialHistoryVars almacena los valores de la
//! deformación, la tensión y la rigidez del material.
class UniaxialHistoryVars: public MovableObject
  {
  private:
    double minStrain; //!< Smallest previous strain (compression)
    double unloadSlope; //!< Unloading (reloading) slope from CminStrain
    double endStrain; //!< Strain at the end of unloading from CminStrain

  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    UniaxialHistoryVars(void);

    inline const double &getMinStrain(void) const
      { return minStrain; }
    inline const double &getUnloadSlope(void) const
      { return unloadSlope; }
    inline const double &getEndStrain(void) const
      { return endStrain; }
    inline double &MinStrain(void)
      { return minStrain; }
    inline double &UnloadSlope(void)
      { return unloadSlope; }
    inline double &EndStrain(void)
      { return endStrain; }

    int revertToStart(const double &);
    void zero(void);

    int sendSelf(CommParameters &);  
    int recvSelf(const CommParameters &);
    
    void Print(std::ostream &s, int flag =0);
    
  };
} // fin namespace XC


#endif

