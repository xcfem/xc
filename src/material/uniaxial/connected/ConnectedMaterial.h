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
//ConnectedMaterial.h                                                                        
                                                                        
#ifndef ConnectedMaterial_h
#define ConnectedMaterial_h

#include <material/uniaxial/UniaxialMaterial.h>
#include <material/uniaxial/DqUniaxialMaterial.h>

namespace XC{
//! @ingroup MatUnx
//
//! @brief Connected uniaxial materials (parallel or serial).
class ConnectedMaterial : public UniaxialMaterial
  {
  protected:
    DqUniaxialMaterial theModels; //!< an array of pointers to the UniaxialMaterials
  public:
    ConnectedMaterial(int tag, int classTag,const DqUniaxialMaterial &theMaterials);
    ConnectedMaterial(int tag, int classTag);
    ConnectedMaterial(const ConnectedMaterial &otro);
    ConnectedMaterial &operator=(const ConnectedMaterial &otro);

    int sendData(CommParameters &);  
    int recvData(const CommParameters &);
  };
} // end of XC namespace


#endif

