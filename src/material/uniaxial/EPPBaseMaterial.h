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
//EPPBaseMaterial.h

#ifndef EPPBaseMaterial_h
#define EPPBaseMaterial_h

#include <material/uniaxial/ElasticBaseMaterial.h>

namespace XC {
//! @ingroup MatUnx
//
//! @brief Clase base para los materiales elasto-plásticos
//! perfectos. 
class EPPBaseMaterial : public ElasticBaseMaterial
  {
  protected:
    double trialStress; //!< current trial stress
    double trialTangent; //!< current trial tangent
    double commitStrain; //!< plastic strain at last commit
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
    bool procesa_comando(CmdStatus &status);
  public:
    EPPBaseMaterial(int tag, int classtag, double E= 0.0, double e0= 0.0);


    inline double getStress(void) const
      { return trialStress; }
    inline double getTangent(void) const
      { return trialTangent; }

    virtual any_const_ptr GetProp(const std::string &cod) const;
  };
} // end of XC namespace


#endif
