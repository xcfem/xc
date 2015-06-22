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
//# PROJECT:           Object Oriented Finite XC::Element Program
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

#ifndef fdEvolution_SLS_CPP
#define fdEvolution_SLS_CPP

#include "material/nD/FiniteDeformation/fdEvolution/fdEvolution_SLS.h"

using namespace XC;

// Linear and / or Saturated isotropic hardening

//------------------------------------------------------------------------
XC::fdEvolution_SLS::fdEvolution_SLS(double H_linear_in,
		                 double q_saturated_in,
		                 double delta_in)
:H_linear(H_linear_in), q_saturated(q_saturated_in), delta(delta_in)
{
    
}

//------------------------------------------------------------------------
 XC::fdEvolution_S * XC::fdEvolution_SLS::newObj() 
{   
    fdEvolution_S *newEL = new XC::fdEvolution_SLS( *this );    
    return newEL;
}

//------------------------------------------------------------------------
double XC::fdEvolution_SLS::HModulus(const XC::stresstensor &sts, const XC::FDEPState &fdepstate) const 
{
    double xi = fdepstate.getStrainLikeInVar();
    return H_linear + delta*q_saturated*exp(-delta*xi);
}

//------------------------------------------------------------------------
void XC::fdEvolution_SLS::print()
{
    std::cerr << (*this);
}

//------------------------------------------------------------------------
std::ostream& XC::operator<< (std::ostream& os, const XC::fdEvolution_SLS & fdesl)
{
   os.precision(5);
   os.width(10);
   os << "Linear & Saturate Scalar XC::Linear Evolution Law's Modulus: " << "\n";
   os << "H_linear = " << fdesl.H_linear << "; " << "\n";
   os << "H_initial = " << fdesl.q_saturated << "; " << "\n";
   os << "Delta= " << fdesl.delta << "; " << "\n";
   
   return os;
}

#endif
