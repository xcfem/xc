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

#ifndef STRESSSTRAINTENSOR_CPP
#define STRESSSTRAINTENSOR_CPP

#include "stress_strain_tensor.h"
#include "utility/matrix/Vector.h"

#include <iomanip>

// just send appropriate arguments to the base constructor
//##############################################################################
XC::stressstraintensor::stressstraintensor(int rank_of_tensor, double initval)
  : BJtensor(rank_of_tensor, def_dim_2, initval) {  } // default constructor


//! @brief Constructor.
XC::stressstraintensor::stressstraintensor(const double *values )
  : BJtensor( 2, def_dim_2, values) {  }

//! @brief Constructor.
XC::stressstraintensor::stressstraintensor(const std::vector<double> &values )
  : BJtensor( 2, def_dim_2, values) {  }

//! @brief Constructor
XC::stressstraintensor::stressstraintensor(const boost::python::list &l)
  : BJtensor( 2, def_dim_2, l) {  }

//##############################################################################
XC::stressstraintensor::stressstraintensor ( double initvalue ):
  BJtensor( 2, def_dim_2, initvalue)  {  }

//##############################################################################
XC::stressstraintensor::stressstraintensor( const stressstraintensor & x )
  : BJtensor("NO")
    {
      x.pc_nDarray_rep->n++;  // tell the rval it has another reference
//      x.reference_count(+1);              // we're adding another reference.
      pc_nDarray_rep = x.pc_nDarray_rep;  // point to the new tensor_rep.
// add the indices
      indices1 = x.indices1;
      indices2 = x.indices2;
    }


//##############################################################################
XC::stressstraintensor::stressstraintensor( const XC::BJtensor & x)
  : BJtensor( x )
    {  } // copy-initializer

//##############################################################################
XC::stressstraintensor::stressstraintensor( const XC::nDarray & x)
  : BJtensor ( x )
    {  } // copy-initializer


XC::stressstraintensor::stressstraintensor(const Vector &x)
  : BJtensor(2, def_dim_2, 0.0)
  {
    const int sz= x.Size();
    if(sz==3)
      {
	val(1,1)= x[0]; 
	val(2,2)= x[1];
	val(2,1)= x[2]; val(1,2)= x[2];
      }
    else if(sz==6)
      {
	val(1,1)= x[0]; 
	val(2,2)= x[1];
	val(3,3)= x[2];
	val(2,3)= x[3]; val(3,2)= x[3];
	val(1,3)= x[4]; val(3,1)= x[4];
	val(1,2)= x[5]; val(2,1)= x[5];
      }
    else
      std::cerr << "stressstraintensor::" << __FUNCTION__
	        << "; invalid dimension: " << sz << std::endl;
  }


//##############################################################################
// IT IS NOT INHERITED so must be defined in all derived classes
// See ARM page 306.
//##############################################################################
XC::stressstraintensor XC::stressstraintensor::operator=( const stressstraintensor & rval)
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
        clear_dim();
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
XC::stressstraintensor XC::stressstraintensor::operator=(const XC::BJtensor &rval)
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
        clear_dim();
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
XC::stressstraintensor XC::stressstraintensor::operator=( const XC::nDarray & rval)
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
        clear_dim();
        delete pc_nDarray_rep;
      }

 // connect to new value
    pc_nDarray_rep = rval.pc_nDarray_rep;  // point at the rval tensor_rep
    return *this;
  }

//! @brief stressstraintensor addition
XC::stressstraintensor &XC::stressstraintensor::operator+=(const stressstraintensor & rval)
  {
    BJtensor::operator+=(rval);
    return *this;
  }

//! @brief stressstraintensor subtraction
XC::stressstraintensor &XC::stressstraintensor::operator-=(const stressstraintensor & rval) 
  {
    BJtensor::operator-=(rval);
    return *this;
  }
    
//! @brief Scalar multiplication.
XC::stressstraintensor &XC::stressstraintensor::operator*=(const double &rval)
  {
    BJtensor::operator*=(rval);
    return *this;
  }

//! @brief Scalar multiplication.
XC::stressstraintensor XC::stressstraintensor::operator*(const double &rval) const
  {
    stressstraintensor retval(*this);
    retval*= rval;
    return retval;
  }


XC::Vector XC::stressstraintensor::getVector(const size_t &dim) const
  {
    Vector retval(dim);
    if(dim==3)
      {
	retval[0]= val(1,1); 
	retval[1]= val(2,2);
	retval[2]= val(2,1);
      }
    else if(dim==6)
      {
	retval[0]= val(1,1); 
	retval[1]= val(2,2);
	retval[2]= val(3,3);
	retval[3]= val(2,3);
	retval[4]= val(1,3);
	retval[5]= val(1,2);
      }
    else
      std::cerr << "stressstraintensor::" << __FUNCTION__
	        << "; invalid dimension: " << dim << std::endl;
    return retval;
  }

//! @brief invariants of the stressstrain XC::BJtensor
//! Chen XC::W.F. "plasticity for Structural Engineers"
double XC::stressstraintensor::Iinvariant1() const
  {
    return (cval(1,1)+cval(2,2)+cval(3,3));
  }

//##############################################################################
double XC::stressstraintensor::Iinvariant2() const
  {
    return (cval(2,2)*cval(3,3)-cval(3,2)*cval(2,3)+
            cval(1,1)*cval(3,3)-cval(3,1)*cval(1,3)+
            cval(1,1)*cval(2,2)-cval(2,1)*cval(1,2));
  }

//##############################################################################
double XC::stressstraintensor::Iinvariant3()  const
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

//! @brief invariants of the deviatoric stressstrain XC::BJtensor
double XC::stressstraintensor::Jinvariant1()  const
  {
    return (0.0);
  }

//##############################################################################
double XC::stressstraintensor::Jinvariant2()  const
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
double XC::stressstraintensor::Jinvariant3()  const
  {
    double I1 = Iinvariant1();
    double I2 = Iinvariant2();
    double I3 = Iinvariant3();

    return ( (2.0*I1*I1*I1 - 9.0*I1*I2 + 27.0*I3)/27.0 );
  }

//##############################################################################
void XC::stressstraintensor::compute_principal(stressstraintensor &ret)  const
  {

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
//..    stressstraintensor principal(0.0);
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
void XC::stressstraintensor::compute_deviator(stressstraintensor &st_dev) const
  {
    BJtensor I2("I", 2, def_dim_2); // Kronecker delta  \delta_{ij}
    stressstraintensor st_vol(I2 * (this->trace()*(1./3.)));
    st_dev= (*this) - st_vol;
  }



//##############################################################################
double XC::stressstraintensor::sigma_octahedral() const // Chen XC::W.F. "plasticity for
  {                                            // Structural Engineers"
    return ( this->Iinvariant1()*ONEOVERTHREE );        // page 59-60
  }

//##############################################################################
double XC::stressstraintensor::tau_octahedral() const    // Chen XC::W.F. "plasticity for
  {                                             // Structural Engineers"
    return(sqrt(TWOOVERTHREE*(this->Jinvariant2())));// page 59-60
  }


//##############################################################################
double XC::stressstraintensor::ksi() const                     // Chen XC::W.F. "plasticity for
  {                                            // Structural Engineers"
    return( (this->Iinvariant1())/sqrt(3.0) ); // page 66
  }

//##############################################################################
double XC::stressstraintensor::ro()  const                      // Chen XC::W.F. "plasticity for
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
double XC::stressstraintensor::rho()  const                      // Chen XC::W.F. "plasticity for
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
double XC::stressstraintensor::p_hydrostatic() const        // Desai "Constitutive Laws
  {                                         // for Engineering Materials"
    return( - (this->Iinvariant1())*ONEOVERTHREE );  // page 283
  }  //sign (-) because tension is positive


//##############################################################################
double XC::stressstraintensor::q_deviatoric() const       // Desai "Constitutive Laws
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
double XC::stressstraintensor::theta( ) const              // Chen XC::W.F. "plasticity for
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
        ::fprintf(stderr,"\n something is wrong in XC::stressstraintensor::theta() (temp7>1.0||temp7<-1.0)\n");
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

        ::fprintf(stdout,"\n something is wrong in XC::stressstraintensor::theta() (temp7>1.0||temp7<-1.0)\n");
        ::fprintf(stdout,"returning Pi/3.\n");
        this->print("s","stressstraintensor s");
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
double XC::stressstraintensor::thetaPI()  const
  {
    double thetaPI = theta() / PI;
    return thetaPI;
  }


#endif

