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
//# PROJECT:           Object Oriented Finite XC::Element Program                    #
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
//#                                                                              #
//################################################################################
//*/

#include "RMC01.h"
#include <cmath>

//#############################################################################
double XC::g_0( double theta, double e )
  {
    double g = 0.0;

    double ct = cos(theta);
    double e_ = e;

    double temp3 = 2.0 * e_ - 1.0;
    double temp4 = 4.0 * ( 1.0 - e_ * e_ ) * ct * ct;

    double upper = temp4 + temp3 * temp3;
    double lower =
      2.0 * ( 1.0 - e_ *e_ ) * ct +
      temp3 *sqrt ( temp4 + 5.0 * e_ * e_ - 4. * e_ );

    g = upper/lower;
    return g;
  }

//#############################################################################
double XC::g_prime( double theta, double e ) 
  {
    double g_prime = 0.0;

    double ct = cos(theta);
    double st = sin(theta);
    double e_ = e;

    double temp2 = 1.0 - e_ * e_;
    double temp3 = 2.0 * e_ - 1.0;
    double temp4 = 4.0 * temp2 * ct * ct;

    double N = temp4 + temp3*temp3;

    double tempsqrt = temp4 + 5.0*e_*e_ - 4.0 * e_ ;
//    double EPS = sqrt(d_macheps());
// this is because it might be close
// to zero ( -1e-19 ) numericaly
// but sqrt() does not accept it
//    if ( tempsqrt < 0.0 || fabs(tempsqrt) < EPS )
//      {
//::fprintf(stdout,"tempsqrt < 0.0 || fabs(tempsqrt) < EPS in ");
//::fprintf(stdout,"double XC::Material_Model::g_Willam_Warnke_prime( double theta, double e )
//const \a\a\n");
//::fprintf(stderr,"tempsqrt < 0.0 || fabs(tempsqrt) < EPS in ");
//::fprintf(stderr,"double XC::Material_Model::g_Willam_Warnke_prime( double theta, double e )
//const \a\a\n");
//        ::exit(1);
//      }


    double temp1 = sqrt( tempsqrt );
    double D     = 2.0*temp2*ct + temp3*temp1;

    double N_prime = -8.0*temp2*ct*st;

    double temp5   = 2.0*temp2*st;
    double temp6   = 4.0*temp3*temp2*ct*st;
    double D_prime = - temp5 - temp6/temp1;

    g_prime = - N*D_prime/(D*D) + N_prime/D;

    return g_prime;
  }

//#############################################################################
double XC::g_second( double theta, double e ) 
  {
    double g_second = 0.0;

    double ct = cos(theta);
    double st = sin(theta);
    double e_ = e;


    double temp2 = 1.0-e_*e_;
    double temp3 = 2.0*e_-1.0;
    double temp4 = 4.0*temp2*ct*ct;

    double N = temp4 + temp3*temp3;

    double tempsqrt = temp4+5.0*e_*e_-4.0*e_ ;
//    double EPS = sqrt(d_macheps());
// this is because it might be close
// to zero ( -1e-19 ) numericaly
// but sqrt() does not accept it
//    if ( tempsqrt < 0.0 || fabs(tempsqrt) < EPS )
//      {
//::fprintf(stdout,"tempsqrt < 0.0 || fabs(tempsqrt) < EPS in ");
//::fprintf(stdout," double XC::Material_Model::g_Willam_Warnke_second( double theta, double e )
//const \a\a\n");
//::fprintf(stderr,"tempsqrt < 0.0 || fabs(tempsqrt) < EPS in ");
//::fprintf(stderr," double XC::Material_Model::g_Willam_Warnke_second( double theta, double e )
//const \a\a\n");
//        ::exit(1);
//      }

    double temp1 = sqrt(tempsqrt);
    double D = 2.0*temp2*ct + temp3*temp1;

    double N_prime = -8.0*temp2*ct*st;

    double temp5 = 2.0*temp2*st;
    double temp6 = 4.0*temp3*temp2*ct*st;
    double D_prime = - temp5 - temp6/temp1;

    double N_second = - 8.0*temp2*ct*ct + 8.0*temp2*st*st;

    double temp7  = 4.0*temp3*temp2*ct*ct;
    double temp8  = 16.0*temp3*temp2*temp2*ct*ct*st*st;
    double temp9  = 4.0*temp3*temp2*st*st;
    double temp10 = 2.0*temp2*ct;
    double D_second = - temp10 -
                        temp7/temp1 -
                        temp8/(temp1*temp1*temp1) +
                        temp9/temp1;

    g_second = 2.0 * N * D_prime * D_prime / (D*D*D) -
               2.0 * D_prime * N_prime / (D*D) -
               N * D_second / (D*D) +
               N_second / D;

    return g_second;
  }
