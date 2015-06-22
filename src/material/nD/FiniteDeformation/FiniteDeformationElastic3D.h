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
//# DATE:              19AUg2003
//# UPDATE HISTORY:    Sept 2003
//#		       May28, 2004
//#		       
//===============================================================================

#ifndef FiniteDeformationElastic3D_h
#define FiniteDeformationElastic3D_h

#include <utility/handler/ConsoleErrorHandler.h>

#include <material/nD/NDMaterial.h>

namespace XC {
//! @ingroup FDNDMat
//
//! @brief Material elastico con deformación finita en tres dimensiones.
class FiniteDeformationElastic3D : public NDMaterial
  {
  protected:
    double rho;
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    
    FiniteDeformationElastic3D(int tag, int classTag, double );
    FiniteDeformationElastic3D(int tag);      
    FiniteDeformationElastic3D();      
    
    virtual double getRho(void) const;

    virtual int setTrialF(const straintensor &f);
    virtual int setTrialFIncr(const straintensor &df);
    virtual int setTrialC(const straintensor &c);
    virtual int setTrialCIncr(const straintensor &dc);

    virtual const Tensor& getTangentTensor(void) const;	  // Default Lagrangian Tangent Tensor
    virtual const Tensor& getInitialTangentTensor(void) const;

    virtual const straintensor &getStrainTensor(void) const;   // Default Green Lagrangian Strain
    virtual const stresstensor &getStressTensor(void) const;   // Default 2nd Piola Kirchhoff Stress
    virtual const straintensor &getF(void) const;
    virtual const straintensor &getC(void) const;

    virtual int commitState(void);
    virtual int revertToLastCommit(void);
    virtual int revertToStart(void);

    virtual NDMaterial *getCopy(void) const;
    virtual NDMaterial *getCopy(const std::string &) const;

    virtual const std::string &getType(void) const;
    virtual int getOrder (void) const;

    virtual void Print(std::ostream &s, int flag = 0);

    virtual int setParameter(const std::vector<std::string> &argv, Parameter &param);
    virtual int updateParameter(int parameterID, Information &info);

    virtual const  stresstensor getPK1StressTensor(void);
    virtual const  stresstensor getCauchyStressTensor(void);
  };
} // fin namespace XC

#endif

