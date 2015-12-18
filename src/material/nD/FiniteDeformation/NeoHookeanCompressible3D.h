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
//# PURPOSE:           Finite Deformation Hyper-Elastic classes
//# CLASS:
//#
//# VERSION:           0.6_(1803398874989) (golden section)
//# LANGUAGE:          C++
//# TARGET OS:         all...
//# DESIGN:            Zhao Cheng, Boris Jeremic (jeremic@ucdavis.edu)
//# PROGRAMMER(S):     Zhao Cheng, Boris Jeremic
//#
//#
//# DATE:              July 2004
//# UPDATE HISTORY:
//#
//===============================================================================

#ifndef NeoHookeanCompressible3D_h
#define NeoHookeanCompressible3D_h

#include <material/nD/FiniteDeformation/FiniteDeformationElastic3D.h>
#include "utility/matrix/nDarray/straint.h"
#include "utility/matrix/nDarray/stresst.h"

namespace XC {
//! @ingroup FDNDMat
//
//! @brief ??.
class NeoHookeanCompressible3D : public FiniteDeformationElastic3D
  {
  private:
    int ComputeTrials(void);
     
    double rho;
    double K;
    double G;

    straintensor F;
    straintensor C;
    double J;
    straintensor Cinv;

    int FromForC;

    Tensor Stiffness;
    straintensor thisGreenStrain;
    stresstensor thisPK2Stress;
  public:
    NeoHookeanCompressible3D(int tag, int classTag, double, double, double );
    NeoHookeanCompressible3D(int tag, double, double, double );
    NeoHookeanCompressible3D(int tag);
    NeoHookeanCompressible3D();    
    
    double getRho(void) const;

    int setTrialF(const straintensor &f);
    int setTrialFIncr(const straintensor &df);
    int setTrialC(const straintensor &c);
    int setTrialCIncr(const straintensor &dc);

    const Tensor& getTangentTensor(void) const; // Default Lagrangian Tangent Tensor
    const Tensor& getInitialTangentTensor(void) const;
    const straintensor &getStrainTensor(void) const;   // Default Green Lagrangian Strain
    const stresstensor &getStressTensor(void) const;   // Default 2nd Piola Kirchhoff Stress
    const straintensor &getF(void) const;
    const straintensor &getC(void) const;

    int commitState(void) ;
    int revertToLastCommit(void) ;
    int revertToStart(void) ;

    NDMaterial *getCopy(void) const;
    NDMaterial *getCopy(const std::string &) const;

    const std::string &getType(void) const;
    int getOrder (void) const;

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);

    void Print(std::ostream &s, int flag = 0);

//    int setParameter(const std::vector<std::string> &argv, Parameter &param);
//    int updateParameter(int parameterID, Information &info);

    const  stresstensor getPK1StressTensor(void) ;
    const  stresstensor getCauchyStressTensor(void) ;

};
} // end of XC namespace

#endif

