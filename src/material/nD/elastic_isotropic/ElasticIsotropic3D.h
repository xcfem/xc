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
//===============================================================================
//# COPYRIGHT (C): Woody's license (by BJ):
//                 ``This    source  code is Copyrighted in
//                 U.S.,  for  an  indefinite  period,  and anybody
//                 caught  using it without our permission, will be
//                 mighty good friends of ourn, cause we don't give
//                 a  darn.  Hack it. Compile it. Debug it. Run it.
//                 Yodel  it.  Enjoy it. We wrote it, that's all we
//                 wanted to do.''
//
//# PROJECT:           Object Oriented Finite Element Program
//# PURPOSE:           Elastic Isotropic Material implementation:
//# CLASS:             ElasticIsotropic3D
//#
//# VERSION:           0.61803398874989 (golden section)
//# LANGUAGE:          C++
//# TARGET OS:         all...
//# DESIGN:            Zhaohui Yang, Boris Jeremic (jeremic@ucdavis.edu)
//# PROGRAMMER(S):     Zhaohui Yang, Boris Jeremic
//#
//#
//# DATE:              10Oct2000
//# UPDATE HISTORY:    22Nov2002 small fixes 
//#
//#
//===============================================================================
                                                                        
                                                                        
#ifndef ElasticIsotropic3D_h
#define ElasticIsotropic3D_h
	 
#include <material/nD/ElasticIsotropicMaterial.h>

#include <utility/matrix/nDarray/straint.h>
#include <utility/matrix/nDarray/stresst.h>
#include <utility/matrix/nDarray/Tensor.h>


namespace XC {
//! @ingroup NDMat
//
//! @brief Material elásticos e isótropos para problemas tridimensionales.
class ElasticIsotropic3D : public ElasticIsotropicMaterial
  {
  private:
    static Vector sigma;        // Stress vector
    static Matrix D;            // Elastic constantsVector sigma;

    mutable Tensor *Dt;			// Elastic constants tensor
    static stresstensor Stress;	// Stress tensor    
    straintensor Strain;	// Strain tensor    
  public:
    ElasticIsotropic3D(int tag, double E, double nu, double rho);
    ElasticIsotropic3D(int tag);
    ElasticIsotropic3D(void);
    ~ElasticIsotropic3D(void);

    int setTrialStrainIncr(const Vector &v);
    int setTrialStrainIncr(const Vector &v, const Vector &r);
    const Matrix &getTangent(void) const;
    const Matrix &getInitialTangent(void) const;

    const Vector &getStress(void) const;
    
    int setTrialStrain(const Tensor &v);
    int setTrialStrain(const Tensor &v, const Tensor &r);
    int setTrialStrainIncr(const Tensor &v);
    int setTrialStrainIncr(const Tensor &v, const Tensor &r);
    const Tensor &getTangentTensor (void) const;
    const stresstensor &getStressTensor(void) const;
    const straintensor &getStrainTensor(void) const;
    const straintensor &getPlasticStrainTensor(void) const;

    int commitState (void);
    int revertToLastCommit (void);
    int revertToStart (void);
    
    NDMaterial *getCopy(void) const;
    const std::string &getType(void) const;
    int getOrder(void) const;

    void Print(std::ostream &s, int flag =0);
    void setInitElasticStiffness(void) const;
  };
} // fin namespace XC


#endif


