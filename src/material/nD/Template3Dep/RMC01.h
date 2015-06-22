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
//================================================================================
//# COPY LEFT and RIGHT:                                                         #
//# Commercial    use    of    this  program without express permission of the   #
//# University  of  California, is strictly encouraged. Copyright and Copyleft   #
//# are covered by the following clause:                                         #
//#                                                                              #
//# Woody's license:                                                             #
//# ``This    source    code is Copyrighted in U.S., by the The Regents of the   #
//# University  of  California,  for  an indefinite period, and anybody caught   #
//# using  it  without  our  permission,  will be mighty good friends of ourn,   #
//# cause  we  don't give a darn. Hack it. Compile it. Debug it. Run it. Yodel   #
//# it. Enjoy it. We wrote it, that's all we wanted to do.'' bj                  #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//# PROJECT:           Object Oriented Finite Element Program                    #
//# PURPOSE:           Rounded Mohr Coulomb base functions                       #
//# CLASS:                                                                       #
//#                                                                              #
//# VERSION:                                                                     #
//# LANGUAGE:          C++                                                       #
//# TARGET OS:         DOS || UNIX || . . .                                      #
//# DESIGNER(S):       Boris Jeremic jeremic@ucdavis.edu                         #
//#                    Zhao Cheng,                                               #
//#                    Zhaohui Yang                                              #
//# PROGRAMMER(S):     Zhao Cheng, Boris Jeremic                                 #
//#                                                                              #
//#                                                                              #
//# DATE:              12 Feb. 2003                                              #
//# UPDATE HISTORY:                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//# SHORT EXPLANATION: base functions for rounded Mohr-Coulomb yield surface     #
//#                    and potential function                                    #
//#                                                                              #
//================================================================================

#ifndef RMC01_H
#define RMC01_H

namespace XC{
double g_0( double, double ) ;
double g_prime( double, double ); 
double g_second( double, double ); 
} //fin namespace XC
#endif
