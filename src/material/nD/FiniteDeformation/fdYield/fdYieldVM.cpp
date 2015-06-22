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

#ifndef fdYieldVM_CPP
#define fdYieldVM_CPP

#include "material/nD/FiniteDeformation/fdYield/fdYieldVM.h"
#include "material/nD/FiniteDeformation/FDEPState.h"


//--------------------------------------------------------------------
XC::fdYieldVM::fdYieldVM(double Y0_in) :Y0(Y0_in)
{

}

//--------------------------------------------------------------------
 XC::fdYield * XC::fdYieldVM::newObj()
{
    fdYield *newfdyd = new XC::fdYieldVM(Y0);
    return newfdyd;
}

int XC::fdYieldVM::getNumRank()
{
    //return 1;
    return 2;
}

double XC::fdYieldVM::getTolerance()
{
    double tol = (Y0*(1.0e-8)) * (Y0*(1.0e-8));
    return tol > 1.0e-8?  tol : 1.0e-8;
    //return Y0*1.0e-8 > 1.0e-8? Y0*1.0e-8 : 1.0e-8;
}

//--------------------------------------------------------------------------------------
// Yd =  3.0*(J2) - (Y0+q)*(Y0+q) = 0, Note here NumRank = 2: No Kinematic hardening
// Yd =  |S_ij| - sqrt(2/3)*(Y0+q) = 0, Note here NumRank = 1: No Kinematic hardening
// Yd =  1.5 * (S_ij - a_ij)*(S_ij-a_ij) - (Y0+q)*(Y0+q) = 0, Note here NumRank = 2
//--------------------------------------------------------------------------------------

double XC::fdYieldVM::Yd(const XC::stresstensor &sts, const XC::FDEPState &fdepstate ) const
{
    //// NumRank=2, No Ki Hardeing
    //double J2 = sts.Jinvariant2();
    //double q = fdepstate.getStressLikeInVar();
    //return 3.0*J2 - (Y0+q)*(Y0+q);
    
    //// NumRank=1, No Ki Hardeing
    //return sqrt(2.0*J2) - sqrt(2.0/3.0);
    
    // NumRank=2, With Ki Hardeing
    XC::stresstensor a = fdepstate.getStressLikeKiVar();
    double q = fdepstate.getStressLikeInVar();
    XC::stresstensor dev = sts.deviator() - a;
    BJtensor st = dev("ij")*dev("ij");   
      st.null_indices();
    double x = st.trace();
    return 1.5*x - (Y0+q)*(Y0+q);
}

//--------------------------------------------------------------------
XC::stresstensor XC::fdYieldVM::dYods(const XC::stresstensor &sts, const XC::FDEPState &fdepstate ) const
{   
    //// NumRank=2, No Ki Hardeing
    //return sts.deviator() * 3.0;

    //// NumRank=1, No Ki Hardeing
    //double J2 = sts.Jinvariant2();
    //return sts.deviator()/(sqrt(8.0*J2);

    // NumRank=2, With Ki Hardeing
    XC::stresstensor a = fdepstate.getStressLikeKiVar();
    XC::stresstensor dev = sts.deviator() - a;
    return dev *3.0;
}					      

//--------------------------------------------------------------------
double XC::fdYieldVM::dYodq(const XC::stresstensor &sts, const XC::FDEPState &fdepstate ) const
{  
    //// NumRank=2, No Ki Hardeing
    //double q = fdepstate.getStressLikeInVar();
    //return -2.0 * (Y0+q);
    
    //// NumRank=1, No Ki Hardeing
    //return sqrt(2.0/3.0);

    // NumRank=2, With Ki Hardeing
    double q = fdepstate.getStressLikeInVar();
    return -2.0 * (Y0+q);
}

//--------------------------------------------------------------------
XC::stresstensor XC::fdYieldVM::dYoda(const XC::stresstensor &sts, const XC::FDEPState &fdepstate ) const
{   
    //// NumRank=2, No Ki Hardeing
    //return sts.deviator() * 3.0;

    //// NumRank=1, No Ki Hardeing
    //double J2 = sts.Jinvariant2();
    //return sts.deviator()/(sqrt(8.0*J2);

    // NumRank=2, With Ki Hardeing
    XC::stresstensor a = fdepstate.getStressLikeKiVar();
    XC::stresstensor dev = sts.deviator() - a;
    return dev *(-3.0);
}	

//--------------------------------------------------------------------
std::ostream& XC::operator<<(std::ostream& os, const XC::fdYieldVM &fdydVM)
{
    os << "fdYieldVM Parameters: " << "\n";
    os << "Y0: " << fdydVM.Y0 << "\n";
    return os;
}

void XC::fdYieldVM::print(void)
  { std::cerr << *this; }

#endif

