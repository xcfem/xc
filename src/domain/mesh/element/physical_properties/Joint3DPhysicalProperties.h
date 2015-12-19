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
//SectionFDPhysicalProperties.h

#ifndef Joint3DPhysicalProperties_h
#define Joint3DPhysicalProperties_h

#include "UniaxialMatPhysicalProperties.h"
#include "material/damage/DamageModelVector.h"

namespace XC {

//! \ingroup PhysicalProperties
//
//! @brief Physical properties for shells.
class Joint3DPhysicalProperties : public UniaxialMatPhysicalProperties
  {
  public:
    Joint3DPhysicalProperties(const size_t &nMat= 0,const UniaxialMaterial *ptr_mat= nullptr); 
    Joint3DPhysicalProperties(const UniaxialMaterial &springx, const UniaxialMaterial &springy, const UniaxialMaterial &springz);

    int update(const Vector &,const int &);
    Vector getTangent(void) const;
    Vector getStress(void) const;
    Vector getStrain(void) const;
    Vector getStrainStress(void) const;
    Vector getResponse7(void) const;
  }; 

} // end of XC namespace
#endif
