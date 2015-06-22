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

#ifndef fdEvolution_TL_CPP
#define fdEvolution_TL_CPP

#include "material/nD/FiniteDeformation/fdEvolution/fdEvolution_TL.h"

using namespace XC;

//------------------------------------------------------------------------
XC::fdEvolution_TL::fdEvolution_TL(double H_linear_in)
:H_linear(H_linear_in)
{
    
}

//------------------------------------------------------------------------
 XC::fdEvolution_T * XC::fdEvolution_TL::newObj() 
{   
    fdEvolution_T *newEL = new XC::fdEvolution_TL( *this );    
    return newEL;
}

//------------------------------------------------------------------------
 XC::BJtensor XC::fdEvolution_TL::HModulus(const XC::stresstensor &sts, const XC::FDEPState &fdepstate) const 
{
   BJtensor eta = fdepstate.getStrainLikeKiVar();
   BJtensor I2("I", 2 , def_dim_2);
   BJtensor I4 = I2("ij")*I2("kl"); I4.null_indices();
    //I4 = (I4.transpose0110()+I4.transpose0111())*0.5; //For symmetric XC::BJtensor
    I4 = I4.transpose0110();  //For general XC::BJtensor
    return I4*H_linear;
}

//------------------------------------------------------------------------
void XC::fdEvolution_TL::print()
{
    std::cerr << (*this);
}

//------------------------------------------------------------------------
std::ostream& XC::operator<< (std::ostream& os, const XC::fdEvolution_TL & fdetl)
{
   os.precision(5);
   os.width(10);
   os << "XC::Tensor XC::Linear Evolution Law's Modulus: " << "\n";
   
   return os;
}

#endif
