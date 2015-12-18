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
#ifndef FiniteDeformationEP3D_H
#define FiniteDefornationEP3D_H

#include <material/nD/NDMaterial.h>

namespace XC {
  class FDEPState;
  class fdYield;
  class fdFlow;
  class fdEvolution_S;
  class fdEvolution_T;
//! @ingroup NDMat
//
//! @defgroup FDNDMat Materiales con deformación finita.
//
//! @ingroup FDNDMat
//
//! @brief Material elastoplástico con deformación finita en tres dimensiones.
class FiniteDeformationEP3D: public NDMaterial
  {

  private:
    NDMaterial *fde3d;
    fdYield *fdy;
    fdFlow *fdf;
    fdEvolution_S *fdEvolutionS;
    fdEvolution_T *fdEvolutionT;

    //material input
    straintensor F;

    //material response
    straintensor iniGreen;
    stresstensor iniPK2;
    BJtensor iniTangent;
  
    stresstensor B_PK2;
    straintensor Fe;
    stresstensor cauchystress;
  
    FDEPState *fdeps;
  private:

    NDMaterial *getFDE3D() const;
    fdYield *getFDY() const;
    fdFlow *getFDF() const;
    fdEvolution_S *getFDEvolutionS() const;
    fdEvolution_T *getFDEvolutionT() const;  
    FDEPState *getFDEPState() const; 

    int ImplicitAlgorithm();
    int SemiImplicitAlgorithm();
  public:
    FiniteDeformationEP3D(int tag);
    // Constructor 00
    FiniteDeformationEP3D( );
    // Constructor 01
    FiniteDeformationEP3D(int tag,
                          NDMaterial *fde3d_in,
			fdYield *fdy_in,
			fdFlow *fdf_in,
			fdEvolution_S *fdEvolutionS_in,
			fdEvolution_T *fdEvolutionT_in);
    // Constructor 02
    FiniteDeformationEP3D(int tag,
                          NDMaterial *fde3d_in,
			fdYield *fdy_in,
			fdFlow *fdf_in,
			fdEvolution_S *fdEvolutionS_in);
    // Constructor 03
    FiniteDeformationEP3D(int tag,
                          NDMaterial *fde3d_in,
			fdYield *fdy_in,
			fdFlow *fdf_in,
			fdEvolution_T *fdEvolutionT_in);
    // Constructor 04
    FiniteDeformationEP3D(int tag,
                          NDMaterial *fde3d_in,
			fdYield *fdy_in,
			fdFlow *fdf_in);
    // Destructor
    virtual ~FiniteDeformationEP3D( );
    
    double getRho(void) const;  

    int setTrialF(const straintensor &f);
    int setTrialFIncr(const straintensor &df);

    const Tensor &getTangentTensor(void) const;
    const straintensor &getStrainTensor(void) const;  // Default Green Strain
    const stresstensor &getStressTensor(void) const ;  // Default 2nd Piola Kirchhoff Stress
    const straintensor &getF(void) const;
    const straintensor getFp(void);

    int commitState(void) ;
    int revertToLastCommit(void) ;
    int revertToStart(void) ;

    NDMaterial *getCopy(void) const;
    NDMaterial *getCopy(const std::string &) const;

    const std::string &getType(void) const;
    int getOrder (void) const;

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);

    void Print(std::ostream &s, int flag);

    const  stresstensor getCauchyStressTensor(void);
  };
} // end of XC namespace

#endif
