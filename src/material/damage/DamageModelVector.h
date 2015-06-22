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
//  bajo los términos de l Licencia Pública General GNU publicada 
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
//DamageModelVector.h

#ifndef DamageModelVector_h
#define DamageModelVector_h

#include "material/MaterialVector.h"
#include "material/uniaxial/UniaxialMaterial.h"
#include "xc_utils/src/nucleo/EntCmd.h"
#include "xc_utils/src/base/utils_any.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "utility/actor/actor/MovableID.h"
#include "utility/matrix/Vector.h"


namespace XC {

class DamageModel;

//! @ingroup damage
//
//! @brief Vector de punteros a damage models.
//! se emplea en Joint2D
class DamageModelVector: public std::vector<DamageModel *>, public EntCmd, public MovableObject
  {
  protected:
    void borra_dmg_models(void);
    void clearAll(void);
    void alloc(const std::vector<DamageModel *> &);

    DbTagData &getDbTagData(void) const;
    int sendData(CommParameters &);  
    int recvData(const CommParameters &);
  public:
    typedef MaterialVector<UniaxialMaterial> material_vector;
    typedef typename std::vector<DamageModel *> dmg_model_vector;
    typedef typename dmg_model_vector::iterator iterator;
    typedef typename dmg_model_vector::reference reference;
    typedef typename dmg_model_vector::const_reference const_reference;

    DamageModelVector(const size_t &nDamageModels,const DamageModel *dmgModel= nullptr);
    DamageModelVector(const DamageModelVector &);
    DamageModelVector &operator=(const DamageModelVector &);
    ~DamageModelVector(void)
      { clearAll(); }

    void setDamageModel(const DamageModel *);
    void setDamageModel(size_t i,DamageModel *);
    bool empty(void) const;
    int commitState(const material_vector &);
    int revertToLastCommit(void);
    int revertToStart(void);

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
  };



} // fin namespace XC

#endif
