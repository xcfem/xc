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
//# PURPOSE:           Hyper-spherical Constraint 
//# CLASS:             HSConstraint
//#
//# VERSION:           0.61803398874989 (golden section)
//# LANGUAGE:          C++
//# TARGET OS:         all...
//# DESIGN:            Ritu Jain, Boris Jeremic
//# PROGRAMMER(S):     Ritu, Boris Jeremic
//#
//#
//# DATE:              14Mar2003
//# UPDATE HISTORY:   
//#
//#
//===============================================================================
#ifndef HSConstraint_h
#define HSConstraint_h

#include "solution/analysis/integrator/static/ProtoArcLength.h"
#include "utility/matrix/Matrix.h"

namespace XC {
class LinearSOE;
class AnalysisModel;
class FE_Element;
class Vector;
class Matrix;

//! @ingroup StaticIntegrator
//
//! @brief ??.
class HSConstraint: public ProtoArcLength
  {
  private:
    double psi_u2;
    double psi_f2;
    double u_ref2;
    Matrix scalingMatrix;
  protected:
    double getDLambdaNewStep(void) const;
    double getDLambdaUpdate(void) const;
    int sendData(CommParameters &);
    int recvData(const CommParameters &);

    friend class SoluMethod;
    HSConstraint(SoluMethod *,double arcLength, double psi_u=1.0, double psi_f=1.0, double u_ref=1.0);
    Integrator *getCopy(void) const;
  public:

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);

    void Print(std::ostream &s, int flag =0);    
  };
inline Integrator *HSConstraint::getCopy(void) const
  { return new HSConstraint(*this); }
} // fin namespace XC

#endif

