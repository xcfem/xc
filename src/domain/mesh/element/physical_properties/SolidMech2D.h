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
//SolidMech2D.h
                                                                        
#include "NDMaterialPhysicalProperties.h"

#ifndef SolidMech2D_h
#define SolidMech2D_h

namespace XC {
//! \ingroup PysicalProperties
//
//! @brief Physical properties for solid mechanics.
class SolidMech2D: public NDMaterialPhysicalProperties
  {
  protected:
    double thickness; //!< Element thickness
    double rho; //!< Mass per unit volume (ya se define en el material ¿por qué repetirla aquí?).

    DbTagData &getDbTagData(void) const;
    int sendData(CommParameters &);
    int recvData(const CommParameters &);

  public:
    SolidMech2D(const size_t &nMat= 0, const NDMaterial *ptr_mat= nullptr, const double &t=0.0, const double &r= 0.0);
    SolidMech2D(const size_t &, NDMaterial &,const std::string &, const double &t=0.0, const double &r= 0.0);

    inline double getThickness(void) const
      { return thickness; }
    inline void setThickness(const double &t)
      { thickness= t; }
    inline double getRho(void) const
      { return rho; }
    inline void setRho(const double &r)
      { rho= r; }
    Vector getRhoi(void) const;

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
  };

} // end of XC namespace
#endif
