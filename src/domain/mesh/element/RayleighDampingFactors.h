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
//RayleighDampingFactors.h

#ifndef RayleighDampingFactors_h
#define RayleighDampingFactors_h

#include "xc_utils/src/nucleo/EntCmd.h"
#include "utility/actor/actor/MovableObject.h"

class TritrizPos3d;

namespace XC {

class Vector;
class Information;

//! \ingroup Elem
//
//! @brief Rayleigh damping factors
class RayleighDampingFactors: public EntCmd, public MovableObject
  {
    double alphaM;
    double betaK;
    double betaK0;
    double betaKc; 
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);

  public:
    RayleighDampingFactors(void);
    RayleighDampingFactors(const double &alphaM,const double &betaK,const double &betaK0,const double &betaKc);
    explicit RayleighDampingFactors(const Vector &);

    inline const double &getAlphaM(void) const
      { return alphaM; }
    inline const double &getBetaK(void) const
      { return betaK; }
    inline const double &getBetaK0(void) const
      { return betaK0; }
    inline const double &getBetaKc(void) const
      { return betaKc; }
    inline bool Nulos(void) const
      { return (alphaM == 0.0 && betaK == 0.0 && betaK0 == 0.0 && betaKc == 0.0); }
    inline bool KNulos(void) const
      { return (betaK == 0.0 && betaK0 == 0.0 && betaKc == 0.0); }
    int updateParameter(int parameterID, Information &info);

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);

    void Print(std::ostream &s, int flag= 0) const;

  };

std::ostream &operator<<(std::ostream &os,const RayleighDampingFactors &rF);

} // end of XC namespace

#endif

