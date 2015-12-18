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

#ifndef Joint2DPhysicalProperties_h
#define Joint2DPhysicalProperties_h

#include "UniaxialMatPhysicalProperties.h"
#include "material/damage/DamageModelVector.h"

namespace XC {

//! \ingroup PhysicalProperties
//
//! @brief Physical properties for shells.
class Joint2DPhysicalProperties : public UniaxialMatPhysicalProperties
  {
  protected:
    DamageModelVector theDamages;
    ID fixedEnd;

    DbTagData &getDbTagData(void) const;
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    Joint2DPhysicalProperties(const size_t &nMat= 0,const UniaxialMaterial *ptr_mat= nullptr); 
    Joint2DPhysicalProperties(const UniaxialMaterial &spring1, const UniaxialMaterial &spring2, const UniaxialMaterial &spring3, const UniaxialMaterial &spring4, const UniaxialMaterial &springC);
    Joint2DPhysicalProperties(const UniaxialMaterial &spring1, const UniaxialMaterial &spring2, const UniaxialMaterial &spring3, const UniaxialMaterial &spring4, const UniaxialMaterial &springC, const DamageModel &dmg1, const DamageModel &dmg2, const DamageModel &dmg3, const DamageModel &dmg4, const DamageModel &dmgC);

    inline const DamageModelVector &getDamageModelVector(void) const
      { return theDamages; }
    inline const ID &getFixedEndInfo(void) const
      { return fixedEnd; }

    // public methods to set the state of the properties
    int commitState(void);
    int revertToLastCommit(void);
    int revertToStart(void);

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
  }; 

} // end of XC namespace
#endif
