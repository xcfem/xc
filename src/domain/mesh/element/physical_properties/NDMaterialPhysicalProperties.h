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
//NDMaterialPhysicalProperties.h
                                                                        
#include "PhysicalProperties.h"
#include "material/nD/NDMaterial.h"

#ifndef NDMaterialPhysicalProperties_h
#define NDMaterialPhysicalProperties_h

namespace XC {
//! \ingroup PysicalProperties
//
//! @brief Physical properties for solid mechanics.
class NDMaterialPhysicalProperties: public PhysicalProperties<NDMaterial>
  {
  protected:
    virtual bool check_material_type(const std::string &type) const;
  public:
    static bool check_material_elast_plana(const std::string &type);

    NDMaterialPhysicalProperties(const size_t &nMat= 0, const NDMaterial *ptr_mat= nullptr);
    NDMaterialPhysicalProperties(const size_t &, NDMaterial &theMaterial, const std::string &type);

    inline const Vector &getCommittedStrain(const size_t &i) const
      { return theMaterial[i]->getCommittedStrain(); }
    inline const Vector &getCommittedStress(const size_t &i) const
      { return theMaterial[i]->getCommittedStress(); }
    Vector getCommittedAvgStrain(void) const;
    Vector getCommittedAvgStress(void) const;
    double getCommittedAvgStrain(const size_t &iComp) const;
    double getCommittedAvgStress(const size_t &iComp) const;

    Matrix getCommittedStrain(void) const;
    Matrix getCommittedStress(void) const;

    bool haveRho(void) const;
    Vector getRhoi(const double &rhoDefault= 0.0) const;
 };

} // end of XC namespace
#endif
