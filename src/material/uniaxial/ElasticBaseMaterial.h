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
//ElasticBaseMaterial.h

#ifndef ElasticBaseMaterial_h
#define ElasticBaseMaterial_h

#include <material/uniaxial/UniaxialMaterial.h>

namespace XC {
//! @ingroup MatUnx
//
//! @brief Clase base para los materiales elásticos uniaxiales.
class ElasticBaseMaterial: public UniaxialMaterial
  {
  protected:
    double trialStrain;
    double E; //!< Módulo elástico.
    double ezero; //!< Deformación inicial.
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
    bool procesa_comando(CmdStatus &status);
    inline virtual double def_total(void) const { return trialStrain-ezero; }
  public:
    ElasticBaseMaterial(int tag= 0, int classtag= 0, double E= 0.0, double e0= 0.0);    

    inline double getE(void) const
      {return E;}
    inline void setE(const double &e)
      {E= e;}

    int setInitialStrain(double strain);
    inline double getStrain(void) const
      {return trialStrain;}
    inline double getInitialStrain(void) const
      {return ezero;}
    inline double getInitialTangent(void) const
      {return E;}

    int sendSelf(CommParameters &);  
    int recvSelf(const CommParameters &);

    virtual any_const_ptr GetProp(const std::string &cod) const;
  };
} // end of XC namespace


#endif
