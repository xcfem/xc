//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
//  XC is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
///*
//################################################################################
//# COPY-YES  (C):     :-))                                                      #
//# PROJECT:           Object Oriented Finite XC::Element Program                    #
//# PURPOSE:           stress XC::BJtensor with all necessary functions                #
//# CLASS:             stresstensor                                              #
//#                                                                              #
//# VERSION:                                                                     #
//# LANGUAGE:          C++.ver >= 2.0 ( Borland C++ ver=3.00, SUN C++ ver=2.1 )  #
//# TARGET OS:         DOS || UNIX || . . .                                      #
//# DESIGNER(S):       Alireza Tabarrei                                          #
//# PROGRAMMER(S):     Alireza Tabarrei                                          #
//#                                                                              #
//#                                                                              #
//# DATE:              June 2004                                                 #
//# UPDATE HISTORY:                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//################################################################################
//*/


#ifndef COSSERATSTRESSTENSOR_CPP
#define COSSERATSTRESSTENSOR_CPP

#include "utility/matrix/nDarray/Cosseratstresst.h"

#include <iomanip>
using std::ios;

// just send appropriate arguments to the base constructor
//##############################################################################
XC::Cosseratstresstensor::Cosseratstresstensor (int rank_of_tensor, double initval):
  BJtensor(rank_of_tensor, Cosserat_def_dim_2, initval) {  } // default constructor


//##############################################################################
XC::Cosseratstresstensor::Cosseratstresstensor ( double *values ):
  BJtensor( 2, Cosserat_def_dim_2, values) {  }

//##############################################################################
XC::Cosseratstresstensor::Cosseratstresstensor ( double initvalue ):
  BJtensor( 2, Cosserat_def_dim_2, initvalue)  {  }

//##############################################################################
XC::Cosseratstresstensor::Cosseratstresstensor( const Cosseratstresstensor & x ):
  BJtensor("NO")
    {
      x.pc_nDarray_rep->n++;  // tell the rval it has another reference
//      x.reference_count(+1);              // we're adding another reference.
      pc_nDarray_rep = x.pc_nDarray_rep;  // point to the new tensor_rep.
// add the indices
      indices1 = x.indices1;
      indices2 = x.indices2;
    }


//##############################################################################
XC::Cosseratstresstensor::Cosseratstresstensor( const XC::BJtensor & x):
  BJtensor( x )
    {  } // copy-initializer

//##############################################################################
XC::Cosseratstresstensor::Cosseratstresstensor( const XC::nDarray & x):
  BJtensor ( x )
    {  } // copy-initializer



 // IT IS NOT INHERITED so must be defined in all derived classes
 // See ARM page 277.
 //##############################################################################
// XC::Cosseratstresstensor::~Cosseratstresstensor()
// {
//   if (reference_count(-1) <= 0)  // Zhaohui changed  == 0 to <= 0 // if reference count  goes to 0
//     {
// // DEallocate memory of the actual XC::nDarray
// //    delete [pc_nDarray_rep->pc_nDarray_rep->total_numb] pc_nDarray_rep->pd_nDdata;
// // nema potrebe za brojem clanova koji se brisu## see ELLIS & STROUSTRUP $18.3
// //                                                and note on the p.65($5.3.4)
//     delete [] data();
//     delete [] dim();
//     delete pc_nDarray_rep;
//   }
// }
//

//##############################################################################
// IT IS NOT INHERITED so must be defined in all derived classes
// See ARM page 306.
//##############################################################################
XC::Cosseratstresstensor XC::Cosseratstresstensor::operator=( const Cosseratstresstensor & rval)
{
    rval.pc_nDarray_rep->n++;  // tell the rval it has another reference
//    rval.reference_count(+1);  // tell the rval it has another reference
//   /*  It is important to increment the reference_counter in the new
//       BJtensor before decrementing the reference_counter in the
//       old tensor_rep to ensure proper operation when assigning a
//       tensor_rep to itself ( after ARKoenig JOOP May/June '90 )  */
 // clean up current value;
    if( reference_count(-1) == 0)  // if nobody else is referencing us.
      {
// DEallocate memory of the actual XC::BJtensor
        delete [] data();
        delete [] dim();
        delete pc_nDarray_rep;
      }
 // connect to new value
    pc_nDarray_rep = rval.pc_nDarray_rep;  // point at the rval tensor_rep
// Temporary out !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// null indices in the rval
//    rval.indices1 = nullptr;
//    rval.indices2 = nullptr;
//    rval.null_indices();
// Temporary out !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    this->null_indices();
    return *this;
}

// IT IS NOT INHERITED so must be defined in all derived classes
// See ARM page 306.
//##############################################################################
XC::Cosseratstresstensor XC::Cosseratstresstensor::operator=( const XC::BJtensor & rval)
{
    rval.pc_nDarray_rep->n++;  // tell the rval it has another reference
//    rval.pc_nDarray_rep->n++;  // tell the rval it has another reference
//    rval.reference_count(+1);  // tell the rval it has another reference
//   /*  It is important to increment the reference_counter in the new
//       BJtensor before decrementing the reference_counter in the
//       old tensor_rep to ensure proper operation when assigning a
//       tensor_rep to itself ( after ARKoenig JOOP May/June '90 )  */

 // clean up current value;
//    if(--pc_nDarray_rep->n == 0)  // if nobody else is referencing us.
    if( reference_count(-1) == 0)  // if nobody else is referencing us.
      {
// DEallocate memory of the actual XC::BJtensor
//      delete [pc_tensor_rep->pc_tensor_rep->total_numb] pc_tensor_rep->pd_nDdata;
// nema potrebe za brojem clanova koji se brisu## see ELLIS & STROUSTRUP $18.3
//                                                and note on the p.65($5.3.4)
//        delete  pc_nDarray_rep->pd_nDdata;
        delete [] data();
        delete [] dim();
// ovo ne smem da brisem jer nije dinamicki alocirano
//        delete pc_tensor_rep->indices;
        delete pc_nDarray_rep;
      }

 // connect to new value
    pc_nDarray_rep = rval.pc_nDarray_rep;  // point at the rval tensor_rep
// Temporary out !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// null indices in the rval
//    rval.indices1 = nullptr;
//    rval.indices2 = nullptr;
//    rval.null_indices();
// Temporary out !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    this->null_indices();
    return *this;
}


// IT IS NOT INHERITED so must be defined in all derived classes
// See ARM page 306.
//##############################################################################
XC::Cosseratstresstensor XC::Cosseratstresstensor::operator=( const XC::nDarray & rval)
{
    rval.pc_nDarray_rep->n++;  // tell the rval it has another reference
//    rval.reference_count(+1);  // tell the rval it has another reference
//   /*  It is important to increment the reference_counter in the new
//       BJtensor before decrementing the reference_counter in the
//       old tensor_rep to ensure proper operation when assigning a
//       tensor_rep to itself ( after ARKoenig JOOP May/June '90 )  */

    if( reference_count(-1) == 0)  // if nobody else is referencing us.
      {
        delete [] data();
        delete [] dim();
        delete pc_nDarray_rep;
      }

 // connect to new value
    pc_nDarray_rep = rval.pc_nDarray_rep;  // point at the rval tensor_rep
    return *this;
}


//##############################################################################
// makes a complete new copy of Cosseratstresstensor!!
XC::Cosseratstresstensor XC::Cosseratstresstensor::deep_copy(void)
  {
    return Cosseratstresstensor(this->data()); // call constructor and return it !
  }
//..//##############################################################################
//../ returns a pointer to the Cosseratstresstensor!!
//..Cosseratstresstensor * XC::Cosseratstresstensor::p_deep_copy(void)
//..  {
//..    Cosseratstresstensor temp = this->deep_copy();
//..    return &temp; // call constructor and return it !
//..  }

//ini  //##############################################################################
//ini  // use "from" and initialize already allocated stress XC::BJtensor from "from" values
//ini  void XC::Cosseratstresstensor::Initialize( const Cosseratstresstensor & from )
//ini    {
//ini  // copy only data because everything else is default
//ini      for ( int i=0 ; i<pc_nDarray_rep->total_numb ; i++ )
//ini        this->pc_nDarray_rep->pd_nDdata[i] = from.pc_nDarray_rep->pd_nDdata[i] ;
//ini    }

//___//##############################################################################
//___//##############################################################################
//___//##############################################################################
//___Cosseratstresstensor & XC::Cosseratstresstensor::operator()(short ir, short is, short it,
//___                                        short tr, short ts, short tt  )
//___// another overloading of operator() . . .  // overloaded for THREE arguments
//___  {
//___    short where = ir - 1;
//___          where = where*ts + is - 1;
//___          where = where*tt + it - 1;
//___
//___//::printf(" w=%ld ",where);
//___    Cosseratstresstensor *p_value = this + where;
//___    return (*p_value);
//___  }

//##############################################################################
// invariants of the stress XC::BJtensor              // Chen XC::W.F. "plasticity for
double XC::Cosseratstresstensor::Iinvariant1() const       // Structural Engineers"
  {
    return (cval(1,1)+cval(2,2)+cval(3,3) + cval(4,4)+cval(5,5)+cval(6,6)   );
  }


////////////////// ALIREZA  from now on
//##############################################################################
double XC::Cosseratstresstensor::Iinvariant2() const
  {
    return (cval(2,2)*cval(3,3)-cval(3,2)*cval(2,3)+
            cval(1,1)*cval(3,3)-cval(3,1)*cval(1,3)+
            cval(1,1)*cval(2,2)-cval(2,1)*cval(1,2));
  }

//##############################################################################
double XC::Cosseratstresstensor::Iinvariant3()  const
  {

    double I3 = cval(1,1)*cval(2,2)*cval(3,3) +
                cval(1,2)*cval(2,3)*cval(3,1) +
                cval(1,3)*cval(2,1)*cval(3,2) -
                cval(1,3)*cval(2,2)*cval(3,1) -
                cval(1,2)*cval(2,1)*cval(3,3) -
                cval(1,1)*cval(2,3)*cval(3,2) ;

    return I3;
//    return ( this->determinant());
  }


//##############################################################################
// invariants of the deviatoric stress XC::BJtensor
double XC::Cosseratstresstensor::Jinvariant1()  const
  {
    return (0.0);
  }

//##############################################################################
double XC::Cosseratstresstensor::Jinvariant2()  const
  {
    double temp1 = (Iinvariant1()*Iinvariant1()-3.0*Iinvariant2())*ONEOVERTHREE;
//    double EPS = sqrt(d_macheps());
//    if ( temp1 < 0.0 || fabs(temp1) < EPS )
    if ( temp1 < 0.0 )
      {                    // this is because it might be close
        temp1 = 0.0;       // to zero ( -1e-19 ) numericaly
      }                    // but sqrt() does not accept it
    return ( temp1 );      // as (-) and theoreticaly J2d>0
  }

//##############################################################################
double XC::Cosseratstresstensor::Jinvariant3()  const
  {
    double I1 = Iinvariant1();
    double I2 = Iinvariant2();
    double I3 = Iinvariant3();

    return ( (2.0*I1*I1*I1 - 9.0*I1*I2 + 27.0*I3)/27.0 );
  }

//##############################################################################
XC::Cosseratstresstensor XC::Cosseratstresstensor::principal()  const
  {
    Cosseratstresstensor ret;

    double p_     = this->p_hydrostatic();
    double q_     = this->q_deviatoric();
    double theta_ = this->theta();
//old    while ( theta_ >= 2.0*PI )
//old      theta_ = theta_ - 2.0*PI; // if bigger than full cycle
//old    while ( theta_ >= 4.0*PI/3.0 )
//old      theta_ = theta_ - 4.0*PI/3.0; // if bigger than four thirds of half cycle
//old    while ( theta_ >= 2.0*PI/3.0 )
//old      theta_ = theta_ - 2.0*PI/3.0; // if bigger than two third of half cycle
//old    while ( theta_ >= PI/3.0 )
//old      theta_ = 2.0*PI/3.0 - theta_; // if bigger than one third of half cycle

//    ret.report("ret");
//    double sqrtJ2D = q/3.0;
//    double 2osqrt3 = 2.0/sqrt(3.0);
    double temp = q_*TWOOVERTHREE;

    double ct  = cos( theta_  );
    double ctm = cos( theta_ - TWOOVERTHREE*PI );
    double ctp = cos( theta_ + TWOOVERTHREE*PI );

    ret.val(1,1) = - p_ + temp*ct;  // - because p is p = -1/3 sigma_ij delta_ii
    ret.val(2,2) = - p_ + temp*ctm;
    ret.val(3,3) = - p_ + temp*ctp;

//    ret.report("ret");
    return ret;

//..    static complex ac[4];               // Chen XC::W.F. "plasticity for
//..    static complex roots[4];            // Structural Engineers"
//..    int polish = 1 ;                    // page 53
//..    int m = 3;
//..
//..    ac[0] = complex( -(this->Iinvariant3()), 0.0 );
//..    ac[1] = complex(  (this->Iinvariant2()), 0.0);
//..    ac[2] = complex( -(this->Iinvariant1()), 0.0);
//..    ac[3] = complex(  1.0, 0.0);
//..
//..// what was obtained for coefficients
//..//DEBUGprint ::printf("ac[0].r = %lf  ac[0].i = %lf\n", real(ac[0]), imag(ac[0]));
//..//DEBUGprint ::printf("ac[1].r = %lf  ac[1].i = %lf\n", real(ac[1]), imag(ac[1]));
//..//DEBUGprint ::printf("ac[2].r = %lf  ac[2].i = %lf\n", real(ac[2]), imag(ac[2]));
//..//DEBUGprint ::printf("ac[3].r = %lf  ac[3].i = %lf\n", real(ac[3]), imag(ac[3]));
//..//DEBUGprint ::printf("m  = %d\n",m);
//..
//..    zroots( ac, m, roots, polish);
//..
//..//DEBUGprint ::printf("\nroots[1].r=%lf  roots[1].i=%lf\n",real(roots[1]),imag(roots[1]));
//..//DEBUGprint ::printf("roots[2].r=%lf  roots[2].i=%lf\n",real(roots[2]),imag(roots[2]));
//..//DEBUGprint ::printf("roots[3].r=%lf  roots[3].i=%lf\n",real(roots[3]),imag(roots[3]));
//..
//..
//..    Cosseratstresstensor principal(0.0);
//..
//..    principal.val(1,1) = real(roots[3]); // since they are sorted by
//..    principal.val(2,2) = real(roots[2]); // the zroot function in ascending
//..    principal.val(3,3) = real(roots[1]); // order . . .
//..                                         // sig1>sig2>sig3
//..    return principal;
//..
//..
  }

//##############################################################################
XC::Cosseratstresstensor XC::Cosseratstresstensor::deviator() const
  {
    BJtensor I2("I", 2, def_dim_2); // Kronecker delta  \delta_{ij}
    Cosseratstresstensor st_vol = I2 * (this->trace()*(0.333333333));
    Cosseratstresstensor st_dev = (*this) - st_vol;

    return st_dev;
  }



//##############################################################################
double XC::Cosseratstresstensor::sigma_octahedral() const // Chen XC::W.F. "plasticity for
  {                                            // Structural Engineers"
    return ( this->Iinvariant1()*ONEOVERTHREE );        // page 59-60
  }

//##############################################################################
double XC::Cosseratstresstensor::tau_octahedral() const    // Chen XC::W.F. "plasticity for
  {                                             // Structural Engineers"
    return(sqrt(TWOOVERTHREE*(this->Jinvariant2())));// page 59-60
  }


//##############################################################################
double XC::Cosseratstresstensor::ksi() const                     // Chen XC::W.F. "plasticity for
  {                                            // Structural Engineers"
    return( (this->Iinvariant1())/sqrt(3.0) ); // page 66
  }

//##############################################################################
double XC::Cosseratstresstensor::xi() const                // Chen XC::W.F. "plasticity for
  {                                            // Structural Engineers"
    return( (this->Iinvariant1())/sqrt(3.0) ); // page 66
  }

//##############################################################################
double XC::Cosseratstresstensor::ro()  const                      // Chen XC::W.F. "plasticity for
  {                                             // Structural Engineers"
    double temp1 = this->Jinvariant2();         // page 68
    double EPS = pow(d_macheps(),(1./2.));
    if ( temp1 < 0.0 && fabs(temp1) < EPS )
      {
        temp1 = 0.0;
      }
    return( sqrt(2.0*(temp1)));
  }
//##############################################################################
double XC::Cosseratstresstensor::rho()  const                      // Chen XC::W.F. "plasticity for
  {                                             // Structural Engineers"
    double temp1 = this->Jinvariant2();         // page 68
    double EPS = pow(d_macheps(),(1./2.));
    if ( temp1 < 0.0 && fabs(temp1) < EPS )
      {
        temp1 = 0.0;
      }
    return( sqrt(2.0*(temp1)));
  }

//##############################################################################
double XC::Cosseratstresstensor::p_hydrostatic() const        // Desai "Constitutive Laws
  {                                         // for Engineering Materials"
    return( - (this->Iinvariant1())*ONEOVERTHREE );  // page 283
  }  //sign (-) because tension is positive


//##############################################################################
double XC::Cosseratstresstensor::q_deviatoric() const       // Desai "Constitutive Laws
  {                                       // for Engineering Materials"
    double temp1 = this->Jinvariant2();   // page 283
//    double EPS = pow(d_macheps(),(1./2.));
//    if ( temp1 < 0.0 || fabs(temp1) < EPS )
    if ( temp1 < 0.0 )
      {
        temp1 = 0.0;
      }

    return( sqrt(3.0*temp1) );
  }

//##############################################################################
double XC::Cosseratstresstensor::theta( ) const              // Chen XC::W.F. "plasticity for
  {                                             // Structural Engineers"
                                                // page 70
    double EPS = pow(d_macheps(),(1./4.));
    double temp1 = (3.0*sqrt(3.0)/2.0);
    double J3D = (this->Jinvariant3());
    double J2D = (this->Jinvariant2());
    if ( J2D < 0.0 || fabs(J2D) < EPS )
      {
        J2D = 0.0;
      }
    double temp4 = J2D * J2D * J2D;
    double temp5 = temp1 * J3D;
    double temp6 = sqrt(temp4);
    if ( (fabs(temp6)) <= fabs(temp5 * EPS) ) return ( 0.000001 );// slight perturbation because of 1/0 KERU06sep96
    if ( (fabs(temp6)) < EPS  ) return ( 0.000001 );// slight perturbation because of 1/0 KERU06sep96
    double temp7 = temp5 / temp6;
    double tempabs1 = (fabs(temp7-1.0));
    double tempabs2 = (fabs(temp7+1.0));
    if ( tempabs1 < 0.1 ) return ( 0.000001/3.0 );// slight perturbation because of 1/0 KERU06sep96
//    if ( tempabs2 < 0.001 ) return ( PI/3.0 );
    if ( tempabs2 < 0.1 ) return ( 3.14159/3.0 );// slight perturbation because of 1/0 KERU06sep96
    if ( temp7>1.0 || temp7<-1.0 )
      {
        ::fprintf(stderr,"\n something is wrong in XC::Cosseratstresstensor::theta() (temp7>1.0||temp7<-1.0)\n");
        ::fprintf(stderr,"returning Pi/3.\n");

        ::fprintf(stderr,"temp1 = %.20e\n", temp1);
        ::fprintf(stderr,"J3D = %.20e\n", J3D);
        ::fprintf(stderr,"J2D = %.20e\n", J2D);
        ::fprintf(stderr,"temp4 = %.20e\n", temp4);
        ::fprintf(stderr,"tempabs1 = %.20e\n", tempabs1);
        ::fprintf(stderr,"tempabs2 = %.20e\n", tempabs2);
        ::fprintf(stderr,"temp5 = %.20e\n", temp5);
        ::fprintf(stderr,"temp6 = %.20e\n", temp6);
        ::fprintf(stderr,"temp7 = %.20e\n", temp7);

        ::fprintf(stdout,"\n something is wrong in XC::Cosseratstresstensor::theta() (temp7>1.0||temp7<-1.0)\n");
        ::fprintf(stdout,"returning Pi/3.\n");
        this->print("s","Cosseratstresstensor s");
        ::fprintf(stdout,"temp1 = %.20e\n", temp1);
        ::fprintf(stdout,"J3D = %.20e\n", J3D);
        ::fprintf(stdout,"J2D = %.20e\n", J2D);
        ::fprintf(stdout,"temp4 = %.20e\n", temp4);
        ::fprintf(stdout,"tempabs1 = %.20e\n", tempabs1);
        ::fprintf(stdout,"tempabs2 = %.20e\n", tempabs2);
        ::fprintf(stdout,"temp5 = %.20e\n", temp5);
        ::fprintf(stdout,"temp6 = %.20e\n", temp6);
        ::fprintf(stdout,"temp7 = %.20e\n", temp7);

        return (  3.14159/ 3.0 );// slight perturbation because of 1/0 KERU06sep96
//        exit(1);
      }
    double temp8 = acos(temp7);
    double temp9 = temp8 * ONEOVERTHREE;

    return ( temp9 );
  }

//##############################################################################
double XC::Cosseratstresstensor::thetaPI()  const
  {
    double thetaPI = theta() / PI;
    return thetaPI;
  }


////##############################################################################
//// routine used by root finder, takes an alfa and returns the
//// yield function value for that alfa
//    double XC::Cosseratstresstensor::func( Cosseratstresstensor & start_stress,
//                               Cosseratstresstensor & end_stress,
//                               Material_Model & YC,
//                               double alfa )
//      {
//        Cosseratstresstensor delta_stress = end_stress - start_stress;
//        Cosseratstresstensor intersection_stress = start_stress + delta_stress*alfa;
//        double f = YC.f(intersection_stress);
//        return f;
//      }




//#############################################################################
XC::BJtensor XC::Cosseratstresstensor::dpoverds( void ) const
  {
    BJtensor ret(2, def_dim_2, 0.0);
    BJtensor I2("I", 2, def_dim_2);
    ret = I2*(-1.0/3.0);
    ret.null_indices();
    return ret;
  }

//#############################################################################
XC::BJtensor XC::Cosseratstresstensor::dqoverds( void ) const
  {
    
//    Cosseratstresstensor stress = EPS->getStress();
    
    BJtensor ret(2, def_dim_2, 0.0);
    double q = this->q_deviatoric();
    Cosseratstresstensor s( 0.0);
//...    double J2D = stress.Jinvariant2();
//...    double temp1 = sqrt(J2D);
//...    double temp2 = sqrt(3.0)/(2.0*temp1);
    double temp2 = (3.0/2.0)*(1/q);
    s = this->deviator();
    ret = s*temp2;
    ret.null_indices();
    return ret;
  }

//#############################################################################
XC::BJtensor XC::Cosseratstresstensor::dthetaoverds( void ) const
  {
//    Cosseratstresstensor stress = EPS->getStress();

    BJtensor ret(2, def_dim_2, 0.0);
    Cosseratstresstensor s( 0.0);
    Cosseratstresstensor t( 0.0);
    BJtensor I2("I", 2, def_dim_2);

//    double EPS = pow(d_macheps(),(1./3.));
    double J2D   = this->Jinvariant2();
    double q     = this->q_deviatoric();
    double theta = this->theta();

//out    while ( theta >= 2.0*PI )
//out      theta = theta - 2.0*PI; // if bigger than full cycle
//out    while ( theta >= 4.0*PI/3.0 )
//out      theta = theta - 4.0*PI/3.0; // if bigger than four thirds of half cycle
//out    while ( theta >= 2.0*PI/3.0 )
//out      theta = theta - 2.0*PI/3.0; // if bigger than two third of half cycle
//out    while ( theta >= PI/3.0 )
//out      theta = 2.0*PI/3.0 - theta; // if bigger than one third of half cycle
//out
//out    if ( theta < 0.0001 )
//out      {
//out        ::printf("theta = %.10e in XC::CAMPotentialSurface::dthetaoverds(stress)\n",
//out                           theta);
//out//>><<>><<>><<        return ret;
//out      }

    double c3t = cos(3.0*theta);
    double s3t = sin(3.0*theta);

    double tempS = (3.0/2.0)*(c3t/(q*q*s3t));
    double tempT = (9.0/2.0)*(1.0/(q*q*q*s3t));

    s = this->deviator();
    t = s("qk")*s("kp") - I2*(J2D*(2.0/3.0));

    s.null_indices();
    t.null_indices();

    ret = s*tempS - t*tempT;
    ret.null_indices();
    return ret;
  }

//#############################################################################
//!..........................................................................
//!  BJtensor d2poverds2( 4, def_dim_4, 0.0); //second derivative of p over
//!                                         // d sigma_pq  d sigma_mn
//!  d2poverds2 = 0.0; //IDENTICALLY EQUAL TO ZERO
//!..........................................................................
//#############################################################################
XC::BJtensor XC::Cosseratstresstensor::d2poverds2( void ) const
  {
    BJtensor ret(4, def_dim_4, 0.0);
    return ret;
    //!!!!! this one is equivalent to zero at all times so no need to call it !!!
  }


//#############################################################################
XC::BJtensor XC::Cosseratstresstensor::d2qoverds2( void ) const
  {
//    Cosseratstresstensor stress = EPS->getStress();


    //first the return XC::BJtensor in order not to fragment the memory
    BJtensor ret( 4, def_dim_4, 0.0); //  second derivative of q over
                                    //  d sigma_pq  d sigma_mn
    //setting up some constants

    BJtensor I2("I", 2, def_dim_2);      // To check out this three fourth-order
    BJtensor I_pqmn("I", 4, def_dim_4);  // isotropic XC::BJtensor please see
    I_pqmn = I2("pq")*I2("mn");        // W.Michael Lai, David Rubin,
    I_pqmn.null_indices();
    BJtensor I_pmqn("I", 4, def_dim_4);  // Erhard Krempl
    I_pmqn = I_pqmn.transpose0110();   // " Introduction to Continuum Mechanics"
                                       // QA808.2  ;   ISBN 0-08-022699-X

    double q_dev   = this->q_deviatoric();

    Cosseratstresstensor s = this->deviator();
    s.null_indices();

    BJtensor iso1( 4, def_dim_4, 0.0); //this will be d_pm*d_nq-d_pq*d_nm*(1/3)
    iso1 = I_pmqn  - I_pqmn*(1.0/3.0);

    double tempiso1  = (3.0/2.0)*(1/q_dev);
    double tempss    = (9.0/4.0)*(1.0/( q_dev * q_dev * q_dev ));

    ret = iso1*tempiso1 - (s("pq")*s("mn"))*tempss;
    ret.null_indices();
    return ret;
  }


//#############################################################################
XC::BJtensor XC::Cosseratstresstensor::d2thetaoverds2( void ) const
  {
    BJtensor ret( 4, def_dim_4, 0.0);

    BJtensor I2("I", 2, def_dim_2);
    BJtensor I_pqmn("I", 4, def_dim_4);  // To check out this three fourth-order
    I_pqmn = I2("pq")*I2("mn");        // isotropic XC::BJtensor please see
    I_pqmn.null_indices();
    BJtensor I_pmqn("I", 4, def_dim_4);  // W.Michael Lai, David Rubin,
    I_pmqn = I_pqmn.transpose0110();   // Erhard Krempl
//no    BJtensor I_pnqm("I", 4, def_dim_4);  // " Introduction to Continuum Mechanics"
//no    I_pnqm = I_pqmn.transpose0111();   // QA808.2  ;   ISBN 0-08-022699-X

    double J2D = this->Jinvariant2();

//    double EPS = pow(d_macheps(),(1./3.));

    BJtensor s( 2, def_dim_2, 0.0);
    BJtensor t( 2, def_dim_2, 0.0);
    s = this->deviator();
    t = s("qk")*s("kp") - I2*(J2D*(2.0/3.0));
//s.print("s"," \n\n XC::BJtensor s \n\n");
//t.print("t"," \n\n XC::BJtensor t \n\n");
    s.null_indices();
    t.null_indices();

    BJtensor p( 4, def_dim_4, 0.0); //this will be d_mp*d_nq - d_pq*d_nm*(1/3)
    BJtensor w( 4, def_dim_4, 0.0);

    double theta = this->theta();
//out    while ( theta >= 2.0*PI )
//out      theta = theta - 2.0*PI; // if bigger than full cycle
//out    while ( theta >= 4.0*PI/3.0 )
//out      theta = theta - 4.0*PI/3.0; // if bigger than four thirds of half cycle
//out    while ( theta >= 2.0*PI/3.0 )
//out      theta = theta - 2.0*PI/3.0; // if bigger than two third of half cycle
//out    while ( theta >= PI/3.0 )
//out      theta = 2.0*PI/3.0 - theta; // if bigger than one third of half cycle
//out
//out    if ( theta < 0.0001 )
//out      {
//out        ::printf("\n\ntheta = %.10e in XC::CAMPotentialSurface::d2thetaoverds2(stress)\n",
//out                           theta);
//out//>><<>><<>><<        return ret;
//out      }
//out

    double q_dev = this->q_deviatoric();

  //setting up some constants
//......    double sqrt3 = sqrt(3.0);
    double c3t    = cos(3*theta);
    double s3t    = sin(3*theta);
    double s3t3   = s3t*s3t*s3t;
    double q3     = q_dev * q_dev * q_dev;
    double q4     = q_dev * q_dev * q_dev * q_dev;
    double q5     = q_dev * q_dev * q_dev * q_dev * q_dev;
    double q6     = q_dev * q_dev * q_dev * q_dev * q_dev * q_dev;


    double tempss = - (9.0/2.0)*(c3t)/(q4*s3t) - (27.0/4.0)*(c3t/(s3t3*q4));

    double tempst = (81.0/4.0)*(1.0)/(s3t3*q5);

    double tempts = (81.0/4.0)*(1.0/(s3t*q5)) + (81.0/4.0)*(c3t*c3t)/(s3t3*q5);

    double temptt = - (243.0/4.0)*(c3t/(s3t3*q6));

    double tempp  = +(3.0/2.0)*(c3t/(s3t*q_dev*q_dev));

    double tempw  = -(9.0/2.0)*(1.0/(s3t*q3));


//))))))))
//))))))))    double tempss = 0.0;
//))))))))
//))))))))    double tempst = 0.0;
//))))))))
//))))))))    double tempts = 0.0;
//))))))))
//))))))))    double temptt = 0.0;
//))))))))
//))))))))    double tempp  = 0.0;
//))))))))
//))))))))    double tempw  = 0.0;
//))))))))


// fourth order tensors in the final equation for second derivative
// of theta over ( d \sigma_{pq} d \sigma_{mn} )
// BE CAREFUL order is   PQ MN

    BJtensor s_pq_d_mn( 4, def_dim_4, 0.0);
    BJtensor s_pn_d_mq( 4, def_dim_4, 0.0);
//...    BJtensor s_pm_d_nq( 4, def_dim_4, 0.0);

    BJtensor d_pq_s_mn( 4, def_dim_4, 0.0);
    BJtensor d_pn_s_mq( 4, def_dim_4, 0.0);
//...    BJtensor d_pm_s_nq( 4, def_dim_4, 0.0);

    p = I_pmqn  - I_pqmn*(1.0/3.0);

// privremena stampa
//..........................................................................
//.......p.print("p"," BJtensor p = I_pmqn  - I_pqmn*(1.0/3.0)");


    s_pq_d_mn = s("pq")*I2("mn");
    s_pq_d_mn.null_indices();
    s_pn_d_mq = s_pq_d_mn.transpose0101();
//...    s_pm_d_nq = s_pn_d_mq.transpose0110();

    d_pq_s_mn = I2("pq")*s("mn");
    d_pq_s_mn.null_indices();
    d_pn_s_mq = d_pq_s_mn.transpose0101();
//...    d_pm_s_nq = d_pn_s_mq.transpose0110();

//// privremena stampa
////..........................................................................
//s_pq_d_mn.print("sd"," BJtensor s_pq_d_mn = s(\"pq\")*I2(\"mn\") ");
//s_pn_d_mq.print("sd"," BJtensor s_pn_d_mq = s_pq_d_mn.transpose0101()");
////s_pm_d_nq.print("sd"," BJtensor s_pm_d_nq = s_pn_d_mq.transpose0110()");
//d_pq_s_mn.print("ds"," BJtensor d_pq_s_mn = I2(\"pq\")*s(\"mn\") ");
//d_pn_s_mq.print("ds"," BJtensor d_pn_s_mq = d_pq_s_mn.transpose0101()");
////d_pm_s_nq.print("ds"," BJtensor d_pm_s_nq = d_pn_s_mq.transpose0110()");
////..........................................................................

    w =  s_pn_d_mq + d_pn_s_mq - s_pq_d_mn*(2.0/3.0) - d_pq_s_mn*(2.0/3.0);

//.....// symmetric w
//.....    w = ( s_pn_d_mq + s_pm_d_nq ) * 0.5 +
//.....        ( d_pn_s_mq + d_pm_s_nq ) * 0.5
//.....        - s_pq_d_mn*(2.0/3.0) - d_pq_s_mn*(2.0/3.0);
//.....

//// privremena stampa
////..........................................................................
//w.print("w","w=s_pn_d_mq+d_pn_s_mq-s_pq_d_mn*(2.0/3.0)-d_pq_s_mn*(2.0/3.0)");
////..........................................................................

////...// privremena stampa
////...//..........................................................................
//BJtensor ss = s("pq")*s("mn");
//BJtensor st = s("pq")*t("mn");
//BJtensor ts = t("pq")*s("mn");
//BJtensor tt = t("pq")*t("mn");
//ss.print("ss","\n XC::BJtensor ss ");
//st.print("st","\n XC::BJtensor st ");
//ts.print("ts","\n XC::BJtensor ts ");
//tt.print("tt","\n XC::BJtensor tt ");
////...

// finally
    ret = (   s("pq")*s("mn") * tempss
            + s("pq")*t("mn") * tempst
            + t("pq")*s("mn") * tempts
            + t("pq")*t("mn") * temptt
            + p               * tempp
            + w               * tempw );
	        
    ret.null_indices();
    return ret;
  }


//--//##############################################################################
//--// trying to find intersection point
//--// according to M. Crisfield's book
//--// " Non-linear Finite XC::Element XC::Analysis of Solids and Structures "
//--// chapter 6.6.1 page 168.
//--Cosseratstresstensor XC::Cosseratstresstensor::yield_surface_cross(Cosseratstresstensor & end_stress,
//--                                               Material_Model & YC)
//--{
//--// bounds
//--  double x1 = 0.0;
//--  double x2 = 1.0;
//--// accuracy
//--  double const TOL = 1.0e-9;
//--  double a = zbrentstress( *this, end_stress, YC, x1, x2, TOL );
//--//  ::printf("\n****\n a = %lf \n****\n",a);
//--  Cosseratstresstensor delta_stress = end_stress - *this;
//--  Cosseratstresstensor intersection_stress = *this + delta_stress * a;
//--//***  intersection_this->reportshort("FINAL Intersection stress\n");
//--
//--  return intersection_stress;
//--
//--}

//##############################################################################
XC::Cosseratstresstensor XC::Cosseratstresstensor::pqtheta2stress( double p, double q, double theta)
  {
    Cosseratstresstensor ret;
//    ret.report("ret");
//    double sqrtJ2D = q/3.0;
//    double 2osqrt3 = 2.0/sqrt(3.0);
    double temp = q*TWOOVERTHREE;

//    while ( theta >= 2.0*PI )
//      theta = theta - 2.0*PI; // if bigger than full cycle
//    while ( theta >= 4.0*PI/3.0 )
//      theta = theta - 4.0*PI/3.0; // if bigger than four thirds of half cycle
//    while ( theta >= 2.0*PI/3.0 )
//      theta = theta - 2.0*PI/3.0; // if bigger than two third of half cycle
//    while ( theta >= PI/3.0 )
//      theta = 2.0*PI/3.0 - theta; // if bigger than one third of half cycle

    double ct  = cos( theta  );
    double ctm = cos( theta - TWOOVERTHREE*PI );
    double ctp = cos( theta + TWOOVERTHREE*PI );

    ret.val(1,1) = temp*ct  - p;
    ret.val(2,2) = temp*ctm - p;
    ret.val(3,3) = temp*ctp - p;

//    ret.report("ret");
    return ret;

  }

//##############################################################################
void XC::Cosseratstresstensor::report(const std::string &msg) const
  {
    ::printf("****************  stress XC::BJtensor report ****************\n");
    std::cerr << msg;

    this->print("st","Cosseratstresstensor st");

    ::fprintf(stdout,"I1 = %.8e ; I2 = %.8e ; I3 = %.8e ;\n",
              Iinvariant1(),Iinvariant2(),Iinvariant3());

    fprintf(stdout,"st_trace = %.8e,  mean pressure p = %.8e\n ",
             trace(),  trace()/3.0);

    BJtensor I2("I", 2, def_dim_2);

    Cosseratstresstensor st_vol = I2 * trace() * (1./3.);
    st_vol.print("st_v","BJtensor st_vol (volumetric part of the st XC::BJtensor)");

    Cosseratstresstensor st_dev = this->deviator();   // - st_vol;
    st_dev.print("st_d","BJtensor st_dev (stress deviator)");

    ::fprintf(stdout,"J1 = %.8e ; J2 = %.8e ; J3 = %.8e ;\n",
              Jinvariant1(),Jinvariant2(),Jinvariant3());
//...  Cosseratstresstensor Jinv2 = st_dev("ij")*st_dev("ji")*0.5;

    Cosseratstresstensor st_principal = principal();
    st_principal.print("st_p","principal stress XC::BJtensor");


    ::fprintf(stdout,"sig_oct = %.8e  , tau_oct = %.8e\n",
              sigma_octahedral(), tau_octahedral());


    ::printf("ksi = %.8e ,  ro = %.8e , theta = %.8e\n",
              ksi(),       ro(),      theta());

    ::printf("p=%.12e , q=%.12e , theta=%.12e*PI\n",
              p_hydrostatic(), q_deviatoric(), thetaPI());

    std::cerr << msg;
    ::printf("############  end of stress XC::BJtensor report ############\n");
  }


//##############################################################################
void XC::Cosseratstresstensor::reportshort(const std::string &msg) const
  {
//..    ::printf("\n** short stress XC::BJtensor report ****************\n");
//    if ( msg ) ::printf("** %s",msg);

    this->print("st"," ");
//    ::printf("** ksi = %.8e ,  ro = %.8e , theta = %.8e\n",
//              ksi(),       ro(),      theta());
//    ::printf(" p=%.8e  q=%.8e  theta=%.8e\n",
//              p_hydrostatic(), q_deviatoric(), theta() );
  }

//##############################################################################
void XC::Cosseratstresstensor::reportshortpqtheta(const std::string &msg) const
  {
//..    ::printf("\n** short stress XC::BJtensor report ****************\n");
    std::cerr << msg;

//..    this->print("st","Cosseratstresstensor st");
//..    ::printf("** ksi = %.8e ,  ro = %.8e , theta = %.8e\n",
//..              ksi(),       ro(),      theta());

    double p = p_hydrostatic();
    double q = q_deviatoric();
    double t = theta();
    ::printf(" p= %+.6e q= %+.6e theta= %+.6e \n", p, q, t);
  }
//##############################################################################
void XC::Cosseratstresstensor::reportSHORTpqtheta(const std::string &msg) const
  {
//..    ::printf("\n** short stress XC::BJtensor report ****************\n");
    std::cerr << msg;

//..    this->print("st","Cosseratstresstensor st");
//..    ::printf("** ksi = %.8e ,  ro = %.8e , theta = %.8e\n",
//..              ksi(),       ro(),      theta());

    double p = p_hydrostatic();
    double q = q_deviatoric();
    double t = theta();
    ::printf(" p= %+.6e q= %+.6e theta= %+.6e ", p, q, t);
  }
//##############################################################################
void XC::Cosseratstresstensor::reportSHORTs1s2s3(const std::string &msg) const
  {
//..    ::printf("\n** short stress XC::BJtensor report ****************\n");
    std::cerr << msg;

//..    this->print("st","Cosseratstresstensor st");
//..    ::printf("** ksi = %.8e ,  ro = %.8e , theta = %.8e\n",
//..              ksi(),       ro(),      theta());

    this->print("st","");
  }
//##############################################################################
void XC::Cosseratstresstensor::reportKLOTpqtheta(const std::string &msg) const
  {
//..    ::printf("\n** short stress XC::BJtensor report ****************\n");
    std::cerr << msg;

//..    this->print("st","Cosseratstresstensor st");
//..    ::printf("** ksi = %.8e ,  ro = %.8e , theta = %.8e\n",
//..              ksi(),       ro(),      theta());

    double p = p_hydrostatic();
    double q = q_deviatoric();
    double t = theta();
    ::printf(" %+.6e %+.6e %+.6e  ", p, q, t);
  }
//##############################################################################
void XC::Cosseratstresstensor::reportshortI1J2J3(const std::string &msg) const
  {
//..    ::printf("\n** short stress XC::BJtensor report ****************\n");
    std::cerr << msg;

//..    this->print("st","Cosseratstresstensor st");
//..    ::printf("** ksi = %.8e ,  ro = %.8e , theta = %.8e\n",
//..              ksi(),       ro(),      theta());

    double I1 = Iinvariant1();
    double J2 = Jinvariant2();
    double J3 = Jinvariant3();
    ::printf(" I1= %.6e J2= %.6e J3= %.6e \n", I1, J2, J3);
  }
//##############################################################################
void XC::Cosseratstresstensor::reportAnim(void) const
  {
    ::printf("Anim p= %f ; q= %f ; theta= %f  \n",
              p_hydrostatic(), q_deviatoric(), theta());
  }
//##############################################################################
void XC::Cosseratstresstensor::reportTensor(const std::string &msg) const
  {
//..    ::printf("\n** short stress XC::BJtensor report ****************\n");
    std::cerr << msg;

    ::fprintf(stdout," %+.6e %+.6e %+.6e %+.6e %+.6e %+.6e \n",
    		      this->cval(1,1),
    		      this->cval(1,2),
    		      this->cval(1,3),
    		      this->cval(2,2),
    		      this->cval(2,3),
    		      this->cval(3,3));
  }

//##############################################################################
std::ostream &XC::operator<<(std::ostream &os,const XC::Cosseratstresstensor &rhs)
  {
    //if ( msg ) ::printf("%s",msg);
  
    //os.setf( ios::showpos | ios::scientific);

    os.precision(4);
    os.width(10);

    os << " "<< rhs.p_hydrostatic();
    os.width(10);
    //os << " q = " << rhs.q_deviatoric();
    os << " " << rhs.q_deviatoric();
    os.width(10);
    //os << " theta = " << rhs.theta();

    return os;
  }


#endif

