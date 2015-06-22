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

#ifndef EncapsulatedMaterial_h
#define EncapsulatedMaterial_h

#include <material/uniaxial/UniaxialMaterial.h>

namespace XC {
//! @ingroup MatUnx
//
//! @brief ??.
class EncapsulatedMaterial: public UniaxialMaterial
  {
  protected:
    UniaxialMaterial *theMaterial;

    void libera(void);
    void copia(const UniaxialMaterial *otro);
  public:
    EncapsulatedMaterial(int tag, int classTag, UniaxialMaterial &material); 
    EncapsulatedMaterial(int tag= 0,int classTag= 0);
    EncapsulatedMaterial(const EncapsulatedMaterial &otro);
    EncapsulatedMaterial &operator=(const EncapsulatedMaterial &otro);
    ~EncapsulatedMaterial(void);

    double getStrain(void) const;          
    double getStrainRate(void) const;
    
    int sendData(CommParameters &);  
    int recvData(const CommParameters &);
  };
} // fin namespace XC


#endif

