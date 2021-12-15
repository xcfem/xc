//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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
                                                                        
// $Revision: 1.4 $                                                              
// $Date: 2005/10/21 22:02:39 $                                                                  
// $Source: /usr/local/cvs/OpenSees/SRC/nDarray/BJtensor.cpp,v $                                                                
                                                                        
                                                                        
//############################################################################
//#                                                                          #
//#             /~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/~~\              #
//#            |                                          |____|             #
//#            |                                          |                  #
//#            |                                          |                  #
//#            |                 B A S E                  |                  #
//#            |                                          |                  #
//#            |                                          |                  #
//#            |              C L A S S E S               |                  #
//#            |                                          |                  #
//#            |                                          |                  #
//#            |          C + +     S O U R C E           |                  #
//#            |                                          |                  #
//#            |                                          |                  #
//#            |                                          |                  #
//#            |                                          |                  #
//#            |                                          |                  #
//#         /~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/   |                  #
//#        |                                         |    |                  #
//#         \_________________________________________\__/                   #
//#                                                                          #
//#                                                                          #
//############################################################################
//
//   "C makes it easy to shoot yourself in the foot, C++ makes it harder,
//   but when you do, it blows away your whole leg" -- Bjarne Stroustrup
//
///*
//################################################################################
//# COPY-YES  (C):     :-))                                                      #
//# PROJECT:           Object Oriented Finite XC::Element Program                    #
//# PURPOSE:                                                                     #
//# CLASS:             BJtensor                                                    #
//#                                                                              #
//# VERSION:                                                                     #
//# LANGUAGE:          C++.ver >= 2.0 ( Borland C++ ver=3.00, SUN C++ ver=2.1 )  #
//# TARGET OS:         DOS || UNIX || . . .                                      #
//# DESIGNER(S):       Boris Jeremic                                             #
//# PROGRAMMER(S):     Boris Jeremic                                             #
//#                                                                              #
//#                                                                              #
//# DATE:              May 28. - July 21 '93                                     #
//# UPDATE HISTORY:    july 8. '93. BJtensor02 - BJtensor multiplication             #
//#                                 inner and outer products                     #
//#                    august 17-19 '93 fixed default constructor that wasted    #
//#                                     memory ###                               #
//#                    october 11 '93 added transpose0110, transpose0101,        #
//#                                   transpose0111 so the creation of           #
//#                                   isotropic XC::BJtensor is much easier and         #
//#                                   understandable !                           #
//#                    january 06 '93  added BJtensor2BJmatrix_1, BJtensor2BJmatrix_2     #
//#                                   inverse_1, inverse_2, inverse_3            #
//#                    january 20 '93  added inverse  TRUE ONE                    #
//#                    August 22-29 '94 choped to separate files and worked on   #
//#                                   const and & issues                         #
//#                    August 30-31 '94 added use_def_dim to full the CC         #
//#                                   resolved problem with temoraries for       #
//#                                   operators + and - ( +=, -= )               #
//#                    28June2004     added val4 for efficiency still            #
//#                                   to be worked on                            #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//################################################################################
//*/
//

#include "utility/matrix/nDarray/BJtensor.h"
#include "utility/matrix/nDarray/BJmatrix.h"
#include <iostream>

//! @brief Default constructor.
XC::BJtensor::BJtensor(int rank_of_BJtensor,const double &initval)
  : nDarray(rank_of_BJtensor, initval), indices1(""), indices2("") {}

//! @brief Constructor
XC::BJtensor::BJtensor(const std::vector<int> &pdim, const double *values)
  : nDarray(pdim, values), indices1(""), indices2("") {}

//! @brief Constructor
XC::BJtensor::BJtensor(const std::vector<int> &pdim, const std::vector<double> &values)
  : nDarray(pdim, values), indices1(""), indices2("") {}

//! @brief Constructor
XC::BJtensor::BJtensor(const std::vector<int> &pdim, const boost::python::list &l)
  : nDarray(pdim, l), indices1(""), indices2("") {}

//! @brief Constructor
XC::BJtensor::BJtensor(const std::vector<int> &pdim,const double &initvalue)
  : nDarray(pdim, initvalue), indices1(""), indices2("") {}

//! @brief Constructor
XC::BJtensor::BJtensor(const std::string &flag, const std::vector<int> &pdim)
  : nDarray( flag, pdim), indices1(""), indices2("") {}


//! @brief Constructor
XC::BJtensor::BJtensor(const std::string &flag)
  : nDarray(flag) { } //this one used to send "NO" message



//ZhaoOct2005 re-wrote the copy constructor 
//##############################################################################
XC::BJtensor::BJtensor(const BJtensor & x)
  : nDarray(x) {}


//##############################################################################
XC::BJtensor::BJtensor(const nDarray &x)
  : nDarray(x) {} // copy-initializer
                  // DO NOT delete ( nullptr ) indices because return
                  // from operator*( BJtensor & arg ) this one
                  // is invoked and so I need this indices.




// IT IS NOT INHERITED so must be defined in all derived classes
// See ARM page 306.
XC::BJtensor &XC::BJtensor::operator=(const BJtensor & rval)
  {
    if(&rval == this) // if assign an BJtensor to itself
      return *this;
    
    nDarray::operator=(rval);
    this->null_indices();
    return *this;
  }

//! @brief Addition.
XC::BJtensor &XC::BJtensor::operator+=(const BJtensor & rval)
  {
    nDarray::operator+=(rval);
    return *this;
  }

//! @brief Subtraction.
XC::BJtensor &XC::BJtensor::operator-=(const BJtensor &rval)
  {
    nDarray::operator-=(rval);
    return *this;
  }



//##############################################################################
// this is supposed to fill in the string indices1 or the string indices2
// array in XC::BJtensor object
// so that they can be checked for matching later on when operations like
// single contraction (.), double contraction (:), dyadic product (otimes)
// are performed the object can choose the right operator.
// Also when multiplying XC::BJtensor by itself ( like for example:
//               D("ij")*D("kl")
// the other string indices2 is defined to take the other array of
// indices
// so that mulitplication will be O.K.
// Since only two BJtensors can be multiplied at the time ( binary operation )
// only indices1 and indices2 are needed #
// WATCH OUT THIS IS NOT STANDRAD AS YOU CANNOT QUARANTY THE ORDER OF EXECUTION!!!!
const XC::BJtensor &XC::BJtensor::operator()(const std::string &indices_from_user) const
  {
    if(indices1.empty())
      {
        indices1= indices_from_user;
        return *this;
      }
    else
      {
        indices2= indices_from_user;
        return *this;
      }
    return *this;
  }



//##############################################################################
void XC::BJtensor::null_indices(void) const
  {
    indices1= "";
    indices2= "";
  }




//! @brief Scalar multiplication. Optimized by  Zhao Oct2005
XC::BJtensor &XC::BJtensor::operator*=(const double &rval)
  {
    nDarray::operator*=(rval);
    this->null_indices();
    return *this;
  }




//##############################################################################
//// scalar multiplication (BJtensor * scalar)
//check what happens if you have tenosr A=B and then you do B*5.13
// does it change A as well !!!!!!!!!!!!!!!!!!!!!!!!!!
// TS KR!!!!
//
XC::BJtensor XC::BJtensor::operator*(const double &rval) const // Added const here!
 {
//ZC// construct XC::BJtensor using the same control numbers as for the
//ZC// original one.
//ZC    BJtensor mult(this->rank(), dim(), 0.0);
//ZC
//ZC    mult.indices1 = this->indices1;
//ZC
//ZC    switch(this->rank())
//ZC      {
//ZC        case 0:
//ZC          {
//ZC            mult(1) = val(1) * rval;
//ZC            break;
//ZC          }
//ZC
//ZC        case 1:
//ZC          {
//ZC            for ( int i1=1 ; i1<=this->dim()[0] ; i1++ )
//ZC              mult(i1) = val(i1) * rval;
//ZC            break;
//ZC          }
//ZC
//ZC        case 2:
//ZC          {
//ZC            for ( int i2=1 ; i2<=this->dim()[0] ; i2++ )
//ZC              for ( int j2=1 ; j2<=this->dim()[1] ; j2++ )
//ZC                mult(i2, j2) = val(i2, j2) * rval;
//ZC            break;
//ZC          }
//ZC
//ZC        case 3:
//ZC          {
//ZC            for ( int i3=1 ; i3<=this->dim()[0] ; i3++ )
//ZC              for ( int j3=1 ; j3<=this->dim()[1] ; j3++ )
//ZC                for ( int k3=1 ; k3<=this->dim()[2] ; k3++ )
//ZC                  mult(i3, j3, k3) = val(i3, j3, k3) * rval;
//ZC            break;
//ZC          }
//ZC
//ZC        case 4:
//ZC          {
//ZC            for ( int i4=1 ; i4<=this->dim()[0] ; i4++ )
//ZC              for ( int j4=1 ; j4<=this->dim()[1] ; j4++ )
//ZC                for ( int k4=1 ; k4<=this->dim()[2] ; k4++ )
//ZC                  for ( int l4=1 ; l4<=this->dim()[3] ; l4++ )
//ZC                    mult(i4,j4,k4,l4)=val(i4,j4,k4,l4)*rval;
//ZC            break;
//ZC          }
//ZC      }
//ZC
//ZC    null_indices();
//ZC
//ZC    return mult;

    BJtensor mult(*this);
    mult *= rval;
    return mult;
 }

// //! @brief scalar multiplication
// XC::BJtensor XC::operator*(const double &lval,const XC::BJtensor &rval)
//   { return rval*lval; }

//##############################################################################
//##############################################################################
//##############################################################################
//##############################################################################
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
//
//                                _____..---========+^+=========---.._____
//   ______________________ __,_='=====____  ================== _____=====`=
//  (._____________________I__) _ __=_/    `--------=+=--------'
//      /      /--...---===='---+----'
//     '------'---.--- _  _ =   _._'    "Make it so..."
//                    `--------'                  Captain Jean-Luc Picard
//                                                USS Enterprise, NCC-1701D
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
XC::BJtensor XC::BJtensor::operator*(const BJtensor &arg) const
  {
    //XC::DEBUGprint::printf("before block in operator* coreleft is: %lu bytes\n", (unsigned long) ::coreleft());
    //   const int MAX_TENS_ORD = 8;
    const int MAX_TENS_ORD = 4;
    //....   int results_rank = 0;

    //XC::DEBUGprint::printf("this->rank()=%d\n",
    //DEBUGprint          this->rank());
    //XC::DEBUGprint::printf("arg.rank()=%d\n",
    //DEBUGprint          arg.rank());

    // space for CONTRACTED indices
    std::vector<int> this_contr(MAX_TENS_ORD);
    std::vector<int> arg_contr(MAX_TENS_ORD);

    // space for UN-CONTRACTED indices
    std::vector<int> this_uncontr(MAX_TENS_ORD);
    std::vector<int> arg_uncontr(MAX_TENS_ORD);

    for(int this_ic=0 ; this_ic<MAX_TENS_ORD ; this_ic++ )
      {
        this_contr[this_ic] = 0;
        this_uncontr[this_ic] = 0;
      }
    for( int arg_ic=0 ; arg_ic<MAX_TENS_ORD ; arg_ic++ )
      {
        arg_contr[arg_ic]  = 0;
        arg_uncontr[arg_ic]  = 0;
      }

// now make the right choice for indices for this and arg XC::BJtensor !!!
//
//                  |||~          |||~          |||~
//                 (0 0)         (0 0)         (0 0)
//--------------ooO-(_)-Ooo---ooO-(_)-Ooo---ooO-(_)-Ooo----------------------
//
// WATCH OUT THIS IS NOT STANDRAD AS YOU CANNOT QUARANTY THE ORDER OF EXECUTION!!!!

    std::string this_indices = this->indices1;
    std::string arg_indices  = arg.indices1;

//fprintf(stdout,"\n\n\n\n  this->indices1 = %s   ; this->indices2 = %s\n", this->indices1,this->indices2);
// if the BJtensors are same then split indices :-)
   if( this == &arg )
     {
// WATCH OUT THIS IS NOT STANDRAD AS YOU CANNOT QUARANTY THE ORDER OF EXECUTION!!!!
       this_indices= this->indices2; // this is changed because
       arg_indices= arg.indices1;  // it reads indices from
                                    // right so "in" indices1 are
                                   // the right ( arg ) and "in"
                                  // indices2 are the
                                 // left indices
// WATCH OUT THIS IS NOT STANDRAD AS YOU CANNOT QUARANTY THE ORDER OF EXECUTION!!!!
//       this_indices = this->indices1; // Now it turns out that it is 
//       arg_indices  = arg.indices2;  // reading indices from left!!!!!!
//                                    // Not any more as per g++ (12 may 2000
//                                   // 
//                                  // 
//                                 //  
// WATCH OUT THIS IS NOT STANDRAD AS YOU CANNOT QUARANTY THE ORDER OF EXECUTION!!!!
     }
//XC::DEBUGprint::printf("this_indices=%s\n", this_indices);
//XC::DEBUGprint::printf("arg_indices=%s\n",  arg_indices);


   int this_indices_number= this_indices.size();
   int  arg_indices_number = arg_indices.size();

//XC::DEBUGprint::printf("this_indices_number=%d\n",
//DEBUGprint          this_indices_number);
//XC::DEBUGprint::printf("arg_indices_number=%d\n",
//DEBUGprint          arg_indices_number);

// check for indices number: should be EQUAL than rank of XC::BJtensor
   if(this_indices_number != this->rank() )
     {
       std::cerr << "\a\n 'this' has more/less indices than expected:\n"
                 << "this_indices_number= " << this_indices_number << std::endl
                 << "this_indices= " << this_indices << std::endl
                 << "this->rank()= " << this->rank() << std::endl;
       ::exit(1);
     }

   if( arg_indices_number != arg.rank() )
     {
       std::cerr << "\a\n 'arg' has more/less indices than expected:\n"
                 << "arg_indices_number= " << arg_indices_number << std::endl
                 << "arg_indices= " << arg_indices << std::endl
                 << "arg->rank()= " << arg.rank() << std::endl;
       ::exit(1);
     }

// counter for contracted indices
   int contr_counter = contracted_ind(this_indices,
                                      arg_indices,
                                      this_contr,
                                      arg_contr,
                                      this_indices_number,
                                      arg_indices_number  );

   int this_uni_count = uncontracted_ind( this_uncontr,
                                          this_contr,
                                          this_indices_number);

   int arg_uni_count = uncontracted_ind( arg_uncontr,
                                         arg_contr,
                                         arg_indices_number);

//number of UNcontractions
   const int uncontr_counter= this_uni_count + arg_uni_count;

//TEMP  this is just the test because right now only up to order = 4
   if ( uncontr_counter > MAX_TENS_ORD )
     {
       std::cerr << "\a\a\n\n  OOOPS product of multiplication has order "
                 << uncontr_counter << std::endl;
       ::exit(1);
     }

//DEBUGprint ::printf("..... this_uni_count = %d, arg_uni_count = %d\n",
//DEBUGprint                 this_uni_count,      arg_uni_count);
//DEBUGprint ::printf(" uncontr_counter %d\n",uncontr_counter);
//DEBUGprint for( int pthis_ic=0 ;
//DEBUGprint      pthis_ic<(this->rank()+1) ;
//DEBUGprint      pthis_ic++ )
//DEBUGprint   ::printf("this_uncontr[%d]=%d\n",pthis_ic,this_uncontr[pthis_ic]);
//DEBUGprint for( int parg_ic=0 ;
//DEBUGprint      parg_ic<(arg.rank()+1) ;
//DEBUGprint      parg_ic++ )
//DEBUGprint   ::printf("             arg_uncontr[%d]=%d\n",parg_ic,arg_uncontr[parg_ic]);

/////////////////////////////////////////////////////
// let's make result BJtensor

   int t = 0;
   int a = 0;
   std::vector<int> results_dims(uncontr_counter);
   for( t=0 ; t < this_uni_count ; t++ )
     results_dims[t]=this->dim()[this_uncontr[t]-1];
   for( a=0 ; a < arg_uni_count ; a++ )
     results_dims[a+t]=arg.dim()[arg_uncontr[a]-1];

// Indices  pa sada indeksi ( index )
   std::string results_indices(uncontr_counter+1, '\0');
   for( t=0 ; t < this_uni_count ; t++ )
     results_indices[t]= this_indices[this_uncontr[t]-1];
   for( a=0 ; a < arg_uni_count ; a++ )
     results_indices[a+t]= arg_indices[arg_uncontr[a]-1];
// the last one . . .
   results_indices[uncontr_counter]= '\0';


//DEBUGprint  ::printf("\n\n......  uncontr_counter+1=%d\n", uncontr_counter+1);
//DEBUGprintfor( int pa=0 ; pa < MAX_TENS_ORD ; pa++ )
//DEBUGprint  ::printf("   results_dims[%d]=%d\n",pa,results_dims[pa]);

   BJtensor result(results_dims, 0.0);
   result(results_indices);  // initialize indices in result
//DEBUGprint  ::printf("\n\n..................   results_indices=%s\n",results_indices);

//////////////////////////////////////////////////
// check dimensions in each XC::BJtensor order
   for( t=0 ; t<contr_counter ; t++ )
     if( this->dim()[this_contr[t]-1] != arg.dim()[arg_contr[t]-1] )
       {
         std::cerr << "\a\a\n t = " << t << std::endl
                   << "this_contr[t]-1= " << this_contr[t]-1 << std::endl
                   << "this->dim()[ " << this_contr[t]-1 << "]= " << this->dim()[this_contr[t]-1] << std::endl
                   << "arg_contr[t]-1= " << arg_contr[t]-1 << std::endl
                   << "arg.dim()[" << arg_contr[t]-1 << "]= " << arg.dim()[arg_contr[t]-1] << std::endl;
         ::exit(1);
       }

/////////////////////////////////////////////////////#*%
//////////////////////////////////////////////////#*%
///////////////////////////////////////////////#*%
////////////////////////////////////////////#*%
/////////////////////////////////////////#*%
//////////////////////////////////////#*%
///////////////////////////////////#*%
////////////////////////////////#*%
/////////////////////////////#*%
//////////////////////////#*%
///////////////////////#*%
////////////////////#*%
/////////////////#*%
//////////////#*%
///////////#*%
////////#*%
/////#*%
//#*%

   std::vector<int> inerr_dims(MAX_TENS_ORD);
   for( t=0 ; t<contr_counter ; t++ )
     {
       inerr_dims[t] = this->dim()[this_contr[t]-1];
//DEBUGprint       ::printf("    inerr_dims[%d] = %d\n",t,inerr_dims[t]);
     }
   for( ; t<MAX_TENS_ORD ; t++ )
     {
       inerr_dims[t] = 1;
//DEBUGprint       ::printf("          the rest of inerr_dims[%d] = %d\n",t,inerr_dims[t]);
     }


   std::vector<int> lid(MAX_TENS_ORD);
   for( t=0 ; t<MAX_TENS_ORD ; t++ )
     {
       lid[t] = 1;
//DEBUGprint       ::printf("    lid[%d] = %d\n",t,lid[t]);
     }

   std::vector<int> rid(MAX_TENS_ORD);
   for( t=0 ;  t<MAX_TENS_ORD ; t++ )
     {
       rid[t] = 1;
//DEBUGprint       ::printf("    rid[%d] = %d\n",t,rid[t]);
     }

   std::vector<int> cd(MAX_TENS_ORD);
   std::vector<int> rd(MAX_TENS_ORD);

   std::vector<int> resd(MAX_TENS_ORD);
   for( t=0 ;  t<MAX_TENS_ORD ; t++ )
     {
       resd[t] = 1;
//DEBUGprint       ::printf("    resd[%d] = %d\n",t,resd[t]);
     }

  double inerr=0.0;

// obrnuti red zbog brzeg izvrsenja ( rd[0] ide do najveceg broja . . . )
//  for ( rd[7]=1 ; rd[7]<=results_dims[7] ; rd[7]++ )
//   for ( rd[6]=1 ; rd[6]<=results_dims[6] ; rd[6]++ )
//    for ( rd[5]=1 ; rd[5]<=results_dims[5] ; rd[5]++ )
//     for ( rd[4]=1 ; rd[4]<=results_dims[4] ; rd[4]++ )
      for ( rd[3]=1 ; rd[3]<=results_dims[3] ; rd[3]++ )
       for ( rd[2]=1 ; rd[2]<=results_dims[2] ; rd[2]++ )
        for ( rd[1]=1 ; rd[1]<=results_dims[1] ; rd[1]++ )
         for ( rd[0]=1 ; rd[0]<=results_dims[0] ; rd[0]++ )
          {
            inerr = 0.0;

//            for ( cd[7]=1 ; cd[7]<=inerr_dims[7] ; cd[7]++ )
//             for ( cd[6]=1 ; cd[6]<=inerr_dims[6] ; cd[6]++ )
//              for ( cd[5]=1 ; cd[5]<=inerr_dims[5] ; cd[5]++ )
//               for ( cd[4]=1 ; cd[4]<=inerr_dims[4] ; cd[4]++ )
                for ( cd[3]=1 ; cd[3]<=inerr_dims[3] ; cd[3]++ )
                 for ( cd[2]=1 ; cd[2]<=inerr_dims[2] ; cd[2]++ )
                  for ( cd[1]=1 ; cd[1]<=inerr_dims[1] ; cd[1]++ )
                   for ( cd[0]=1 ; cd[0]<=inerr_dims[0] ; cd[0]++ )
                    {

// contracted indices
                      for ( t=0 ; t<contr_counter ; t++ )
                        {
                          lid[ this_contr[t]-1 ]  = cd[t];
//DEBUGprint                          ::printf("this contracted  t=%d  lid[%d]=%d     ",
//DEBUGprint                                    t,this_contr[t]-1,lid[this_contr[t]-1]);
                        }
//DEBUGprint                      ::printf("\n");
                      for ( t=0 ; t<contr_counter ; t++ )
                        {
                          rid[ arg_contr[t]-1 ]  = cd[t];
//DEBUGprint                          ::printf("arg contracted  t=%d  rid[%d]=%d    ",
//DEBUGprint                                    t,arg_contr[t]-1,rid[arg_contr[t]-1]);
                        }
//DEBUGprint                      ::printf("\n");


// uncontracted indices
                      for ( t=0 ; t<this_uni_count ; t++ )
                        {
                          lid[ this_uncontr[t]-1 ]  = rd[t];
//DEBUGprint                          ::printf("this uncontracted ----- t=%d lid[%d]=%d  ",
//DEBUGprint                                    t,this_uncontr[t]-1,lid[this_uncontr[t]-1]);
                        }
//DEBUGprint                      ::printf("\n");

//                      for ( t=0 ; t<arg_uni_count ; t++ )
                      for ( ; t<(this_uni_count+arg_uni_count) ; t++ )
                        {
                          rid[ arg_uncontr[t-this_uni_count]-1 ] = rd[t];
//DEBUGprint                          ::printf("arg uncontracted ----- t=%d  rid[%d]=%d  ",
//DEBUGprint                                    t,
//DEBUGprint                                    arg_uncontr[t-this_uni_count]-1,
//DEBUGprint                                    rid[arg_uncontr[t-this_uni_count]-1]);
                        }
//DEBUGprint                      ::printf("\n");



//                      inerr =
//                      this->val(lid[0],lid[1],lid[2],lid[3],
//                                       lid[4],lid[5],lid[6],lid[7]) *
//                      arg.val(rid[0],rid[1],rid[2],rid[3],
//                                     rid[4],rid[5],rid[6],rid[7]);

                      inerr = inerr +
			(*this)(lid[0],lid[1],lid[2],lid[3]) * arg(rid[0],rid[1],rid[2],rid[3]);
//::printf(" inerr = %12.4lf \n   this[%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d] = %12.4lf arg[%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d] = %12.4lf\n",
//      inerr,lid[0],lid[1],lid[2],lid[3],lid[4],lid[5],lid[6],lid[7],
//      this->val(lid[0],lid[1],lid[2],lid[3],lid[4],lid[5],lid[6],lid[7]),
//      rid[0],rid[1],rid[2],rid[3],rid[4],rid[5],rid[6],rid[7],
//      arg.val(rid[0],rid[1],rid[2],rid[3],rid[4],rid[5],rid[6],rid[7]));

//XC::DEBUGprint::printf("inerr=%6.2lf this[%1d,%1d,%1d,%1d]=%6.2lf arg[%1d,%1d,%1d,%1d]=%6.2lf\n",
//DEBUGprint      inerr,lid[0],lid[1],lid[2],lid[3],
//DEBUGprint      (*this)(lid[0],lid[1],lid[2],lid[3]),
//DEBUGprint      rid[0],rid[1],rid[2],rid[3],
//DEBUGprint      arg(rid[0],rid[1],rid[2],rid[3]));

                    }

                                                           // might be optimized 
            for ( t=0 ; t<this_uni_count ; t++ )           // put first on in second
              {                                            // loop TS
//..                resd[ this_uncontr[t]-1 ] = rd[t];
                resd[t] = rd[t];
//DEBUGprint                ::printf("##### t=%d  resd[%d]=%d\n",
//DEBUGprint                          t,this_uncontr[t]-1,resd[t]);
//DEBUGprint                          t,t,resd[t]);
              }
            for ( ; t<(this_uni_count+arg_uni_count) ; t++ )
              {
//..                resd[ arg_uncontr[t]-1 ] = rd[t];
                resd[t] = rd[t];
//DEBUGprint                ::printf("##### t=%d  resd[%d]=%d\n",
//DEBUGprint                          t,arg_uncontr[t]-1,resd[t]);
//DEBUGprint                          t,t,resd[t]);
              }

//          result(resd[0],resd[1],resd[2],resd[3],
//                 resd[4],resd[5],resd[6],resd[7]) = inerr ;
//::printf(" -----------result[%1d,%1d,%1d,%1d,%1d,%1d,%1d,%1d] = %lf\n",



//      resd[0],resd[1],resd[2],resd[3],resd[4],resd[5],resd[6],resd[7],
//      result(resd[0],resd[1],resd[2],resd[3],resd[4],resd[5],resd[6],resd[7]));

          result(resd[0],resd[1],resd[2],resd[3]) = inerr ;
//XC::DEBUGprint::printf("##################################### result[%1d,%1d,%1d,%1d]=%8.2lf\n",
//DEBUGprint             resd[0],resd[1],resd[2],resd[3],
//DEBUGprint      result(resd[0],resd[1],resd[2],resd[3]));


          }

// nullptrification of indices

    null_indices();
    arg.null_indices();

//XC::DEBUGprint::printf("     after delete in operator* coreleft is: %lu bytes\n", (unsigned long) ::coreleft());

    return result; // Returning a local variable ??
                   // copy-initializer happens before the destructor,
                   // so reference count is 2 when destructor is called,
                   // thus destructor doesn't free the memory.
  }



//......//##############################################################################
//......//##############################################################################
//......//##############################################################################
//......//##############################################################################
//......//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//......//
//......//
//......//                                _____..---========+^+=========---.._____
//......//   ______________________ __,_='=====____  ================== _____=====`=
//......//  (._____________________I__) _ __=_/    `--------=+=--------'
//......//      /      /--...---===='---+----'
//......//     '------'---.--- _  _ =   _._'    "Make it so..."
//......//                    `--------'                  Captain Jean-Luc Picard
//......//                                                USS Enterprise, NCC-1701D
//......//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

//! @brief counter for contracted indices
int XC::BJtensor::contracted_ind(const std::string &argleft_indices,
                                 const std::string &argright_indices,
                                 std::vector<int> &argleft_contr,
                                 std::vector<int> &argright_contr,
                                 int   argleft_ind_numb,
                                 int     argright_ind_numb) const
  {
    int contr_counter = 0;
    int argleft_i_count = 0;
    int argright_i_count  = 0;

    for( int argleft_indices_counter = 1 ;
          argleft_indices_counter<=argleft_ind_numb ;
          argleft_indices_counter++ )
      {
        int t_i_c = argleft_indices_counter - 1;
        for ( int argright_indices_counter = 1 ;
              argright_indices_counter<= argright_ind_numb ;
              argright_indices_counter++ )
          {
            int a_i_c = argright_indices_counter - 1;
            if ( argleft_indices[t_i_c] ==
                 argright_indices[a_i_c] )
              {
                argleft_contr[argleft_i_count] = argleft_indices_counter;
                argright_contr[argright_i_count] = argright_indices_counter;

                argleft_i_count++;
                argright_i_count++;

                contr_counter++;
              }
          }
      }


//DEBUGprint ::printf("..... argleft_i_count = %d, argright_i_count = %d\n",
//DEBUGprint                 argleft_i_count,      argright_i_count);
//DEBUGprint ::printf(" contr_counter %d\n",contr_counter);
//DEBUGprint for(int pthis_ic=0 ;
//DEBUGprint     pthis_ic<(argleft->rank()+1) ;
//DEBUGprint     pthis_ic++ )
//DEBUGprint   ::printf("argleft_contr[%d]=%d\n",pthis_ic,argleft_contr[pthis_ic]);
//DEBUGprint for( int parg_ic=0 ;
//DEBUGprint      parg_ic<(argright->rank()+1) ;
//DEBUGprint      parg_ic++ )
//DEBUGprint   ::printf("                 argright_contr[%d]=%d\n",
//DEBUGprint                              parg_ic,argright_contr[parg_ic]);


    return contr_counter;
  }

//##############################################################################
// counter for UNcontracted indices
int XC::BJtensor::uncontracted_ind(std::vector<int> &tens_uncontr,
				     std::vector<int> &tens_contr,
                                     int tens_ind_numb) const
  {
// counter for UNcontracted indices
    int tens_uni_count = 0;

    for ( int tens_unindices_counter = 1 ;
          tens_unindices_counter<=tens_ind_numb ;
          tens_unindices_counter++ )
      {
        tens_uncontr[tens_uni_count] = tens_unindices_counter;
        for ( int tens_indices_counter = 1 ;
              tens_indices_counter<=tens_ind_numb ;
              tens_indices_counter++ )
          {
            int t_i_c = tens_indices_counter - 1;
//XC::DEBUGprint::printf("CHECK tens_uncontr[%d]=%d",tens_uni_count, tens_uncontr[tens_uni_count]);
//XC::DEBUGprint::printf("  WITH tens_contr[%d]=%d\n", t_i_c,tens_contr[t_i_c]);
            if ( tens_uncontr[tens_uni_count] == tens_contr[t_i_c] )
              {
                tens_uncontr[tens_uni_count] = 0;
                tens_uni_count--;
                tens_indices_counter=tens_ind_numb;//out of inner for loop
              }
          }
        tens_uni_count++;
      }
    return tens_uni_count;
  }

//##############################################################################
// BJtensorial division THE rval MUST BE 0 ORDER XC::BJtensor
XC::BJtensor XC::BJtensor::operator/(const BJtensor &rval) const
 {
// construct XC::BJtensor using the same control numbers as for the
// original one.
    BJtensor div(dim(), 0.0);
    double rvalDouble = rval.trace();
    if ( rval.rank() != 1 ) ::printf("rval.rank() != 1 for XC::BJtensor XC::BJtensor::operator/( BJtensor & rval)\n");

    div.indices1 = this->indices1;

    switch(this->rank())
      {
        case 0:
          {
            div(1) = val(1)/rvalDouble;
            break;
          }

        case 1:
          {
            for ( int i1=1 ; i1<=3 ; i1++ )
              div(i1) = val(i1)/rvalDouble;
            break;
          }

        case 2:
          {
            for ( int i2=1 ; i2<=3 ; i2++ )
              for ( int j2=1 ; j2<=3 ; j2++ )
                div(i2, j2) = val(i2, j2)/rvalDouble;
            break;
          }

        case 3:
          {
            for ( int i3=1 ; i3<=3 ; i3++ )
              for ( int j3=1 ; j3<=3 ; j3++ )
                for ( int k3=1 ; k3<=3 ; k3++ )
                  div(i3, j3, k3) = val(i3, j3, k3)/rvalDouble;
            break;
          }

        case 4:
          {
            for ( int i4=1 ; i4<=3 ; i4++ )
              for ( int j4=1 ; j4<=3 ; j4++ )
                for ( int k4=1 ; k4<=3 ; k4++ )
                  for ( int l4=1 ; l4<=3 ; l4++ )
                    div(i4,j4,k4,l4)=val(i4,j4,k4,l4)/rvalDouble;
            break;
          }
      }

    null_indices();

    return div;
 }



//##############################################################################
///* transpose function for 4th rank BJtensors:
//     ijkl  -->> ikjl    */
XC::BJtensor XC::BJtensor::transpose0110() const
  {
// construct XC::BJtensor using the same control numbers as for the
// original one and than transpose it.
   BJtensor trans1(dim(), 0.0);

   for ( int i=1 ; i<=dim()[0] ; i++ )
     for ( int j=1 ; j<=dim()[1] ; j++ )
       for ( int k=1 ; k<=dim()[2] ; k++ )
         for ( int l=1 ; l<=dim()[3] ; l++ )
           trans1(i, j, k, l) = (*this)(i, k, j, l);

    trans1.null_indices();
//    null_indices();
    return trans1;
 }

//##############################################################################
///* transpose function for 4th rank BJtensors:
//     ijkl  -->> ikjl    */
XC::BJtensor XC::BJtensor::transposeoverbar() const // same as transpose0110
  {
// construct XC::BJtensor using the same control numbers as for the
// original one and than transpose it.
   BJtensor trans1(dim(), 0.0);

   for ( int i=1 ; i<=dim()[0] ; i++ )
     for ( int j=1 ; j<=dim()[1] ; j++ )
       for ( int k=1 ; k<=dim()[2] ; k++ )
         for ( int l=1 ; l<=dim()[3] ; l++ )
           trans1(i, j, k, l) = (*this)(i, k, j, l);

    trans1.null_indices();
//    null_indices();
    return trans1;
 }
//##############################################################################
///* transpose function for 4th rank BJtensors:
//     ijkl  -->> ilkj    */
XC::BJtensor XC::BJtensor::transpose0101() const
  {
// construct XC::BJtensor using the same control numbers as for the
// original one and than transpose it.
   BJtensor trans1(dim(), 0.0);

   for ( int i=1 ; i<=dim()[0] ; i++ )
     for ( int j=1 ; j<=dim()[1] ; j++ )
       for ( int k=1 ; k<=dim()[2] ; k++ )
         for ( int l=1 ; l<=dim()[3] ; l++ )
           trans1(i, j, k, l) = (*this)(i, l, k, j);

    trans1.null_indices();
//    null_indices();
    return trans1;
 }

//##############################################################################
///* transpose function for 4th rank BJtensors:
//     ijkl  -->> iljk    */
XC::BJtensor XC::BJtensor::transpose0111() const
  {
// construct XC::BJtensor using the same control numbers as for the
// original one and than transpose it.
   BJtensor trans1(dim(), 0.0);

   for ( int i=1 ; i<=dim()[0] ; i++ )
     for ( int j=1 ; j<=dim()[1] ; j++ )
       for ( int k=1 ; k<=dim()[2] ; k++ )
         for ( int l=1 ; l<=dim()[3] ; l++ )
           trans1(i, j, k, l) = (*this)(i, l, j, k);

    trans1.null_indices();
//    null_indices();
    return trans1;
 }
//##############################################################################
///* transpose function for 4th rank BJtensors:
//     ijkl  -->> iljk    */
XC::BJtensor XC::BJtensor::transposeunderbar() const   // transpose0111() 
  {
// construct XC::BJtensor using the same control numbers as for the
// original one and than transpose it.
   BJtensor trans1(dim(), 0.0);

   for ( int i=1 ; i<=dim()[0] ; i++ )
     for ( int j=1 ; j<=dim()[1] ; j++ )
       for ( int k=1 ; k<=dim()[2] ; k++ )
         for ( int l=1 ; l<=dim()[3] ; l++ )
           trans1(i, j, k, l) = (*this)(i, l, j, k);

    trans1.null_indices();
//    null_indices();
    return trans1;
 }

//##############################################################################
///* transpose function for 4th rank BJtensors:
//     ijkl  -->> jikl
XC::BJtensor XC::BJtensor::transpose1100() const
  {
// construct XC::BJtensor using the same control numbers as for the
// original one and than transpose it.
   BJtensor trans1(dim(), 0.0);

   for ( int i=1 ; i<=dim()[0] ; i++ )
     for ( int j=1 ; j<=dim()[1] ; j++ )
       for ( int k=1 ; k<=dim()[2] ; k++ )
         for ( int l=1 ; l<=dim()[3] ; l++ )
           trans1(i, j, k, l) = (*this)(j, i, k, l);

    trans1.null_indices();
//    null_indices();
    return trans1;
 }

//##############################################################################
///* transpose function for 4th rank BJtensors:
//     ijkl  -->> ijlk
XC::BJtensor XC::BJtensor::transpose0011() const
  {
// construct XC::BJtensor using the same control numbers as for the
// original one and than transpose it.
   BJtensor trans1(dim(), 0.0);

   for ( int i=1 ; i<=dim()[0] ; i++ )
     for ( int j=1 ; j<=dim()[1] ; j++ )
       for ( int k=1 ; k<=dim()[2] ; k++ )
         for ( int l=1 ; l<=dim()[3] ; l++ )
           trans1(i, j, k, l) = (*this)(i, j, l, k);

    trans1.null_indices();
//    null_indices();
    return trans1;
 }

//##############################################################################
///* transpose function for 4th rank BJtensors:
//     ijkl  -->> iilk
XC::BJtensor XC::BJtensor::transpose1001() const
  {
// construct XC::BJtensor using the same control numbers as for the
// original one and than transpose it.
   BJtensor trans1(dim(), 0.0);

   for ( int i=1 ; i<=dim()[0] ; i++ )
     for ( int j=1 ; j<=dim()[1] ; j++ )
       for ( int k=1 ; k<=dim()[2] ; k++ )
         for ( int l=1 ; l<=dim()[3] ; l++ )
           trans1(i, j, k, l) = (*this)(l, j, k, i);

    trans1.null_indices();
//    null_indices();
    return trans1;
 }




//##############################################################################
///* transpose function for 2th rank BJtensors:
//     ij  -->> ji    */
XC::BJtensor XC::BJtensor::transpose11() const
  {
// construct XC::BJtensor using the same control numbers as for the
// original one and than transpose it.
   BJtensor trans1(dim(), 0.0);

   for ( int i=1 ; i<=dim()[0] ; i++ )
     for ( int j=1 ; j<=dim()[1] ; j++ )
       trans1(i, j) = (*this)(j, i);

    trans1.null_indices();
//    null_indices();
    return trans1;
 }

//##############################################################################
///* symmterize function for 2th rank BJtensors:
//     ij  
XC::BJtensor XC::BJtensor::symmetrize11() const
  {
// construct XC::BJtensor using the same control numbers as for the
// original one and than symmetrize it.
//   BJtensor sym(this->rank(), dim(), 0.0);


    BJtensor temp(*this);
    BJtensor sym= (temp + temp.transpose11());
    sym*= 0.5;
    
    sym.null_indices();
    return sym;
 }

//..//##############################################################################
//..///* symmterize function for 2th rank BJtensors:
//..//     ij  
//..BJtensor XC::BJtensor::symmetrize11() const
//..  {
//..// construct XC::BJtensor using the same control numbers as for the
//..// original one and than symmetrize it.
//..//   BJtensor sym(this->rank(), dim(), 0.0);
//..
//..
//..   BJtensor temp(*this);
//..
//..   BJtensor sym = (temp + temp.transpose11())*0.5;
//..
//..   sym.null_indices();
//..   return sym;
//.. }
//..


// moved to ndarray april 3 1995
//--//##############################################################################
//--// trace function: trace of a second rank XC::BJtensor
//--// what about fourth ( 4th ) rank XC::BJtensor trace or any other rank ??
//--double XC::BJtensor::trace() const 
//--  {
//--    double tr = 0.0;
//--// ovaj case ne treba vec moze sve do 4-tog reda ( ili kasnije osmog # )
//--    switch(this->rank())
//--      {
//--        case 0:
//--          {
//--//            ::printf(" trace=%.4e  ", val(1));
//--//            ::printf("\n");
//--            tr = (*this)(1);
//--            break;
//--          }
//--
//--        case 1:
//--          {
//--            if(dim()[0] != 1)
//--              {
//--::printf("\a\nERROR in trace function : not a squared 1-st rank XC::BJtensor\n");
//--::exit( 1 );
//--              }
//--            tr = (*this)(1);
//--            break;
//--          }
//--
//--        case 2:
//--          {
//--            if(dim()[0] != dim()[1])
//--              {
//--::printf("\a\nERROR in trace function : not a sqared 2nd-rank XC::BJtensor\n");
//--::exit( 1 );
//--              }
//--            for ( int i2=1 ; i2<=dim()[0] ; i2++ )
//--              tr += (*this)(i2, i2);
//--            break;
//--          }
//--
//--        case 3:
//--          {
//--            if( dim()[0] != dim()[1] ||
//--                dim()[1] != dim()[2] ||
//--                dim()[2] != dim()[0]    )
//--              {
//--::printf("\a\nERROR in trace function : not a sqared 3nd-rank XC::BJtensor\n");
//--::exit( 1 );
//--              }
//--            for ( int i3=1 ; i3<=dim()[0] ; i3++ )
//--              tr += (*this)(i3, i3, i3);
//--            break;
//--          }
//--
//--        case 4:
//--          {
//--            if( dim()[0] != dim()[1] ||
//--                dim()[1] != dim()[2] ||
//--                dim()[2] != dim()[3] ||
//--                dim()[3] != dim()[0]    )
//--              {
//--::printf("\a\nERROR in trace function : not a sqared 4nd-rank XC::BJtensor\n");
//--::exit( 1 );
//--              }
//--            for ( int i4=1 ; i4<=dim()[0] ; i4++ )
//--              tr += (*this)(i4, i4, i4, i4);
//--            break;
//--          }
//--      }
//--
//..........    null_indices();
//--
//--    return tr;
//--  }


//##############################################################################
// determinant function
double XC::BJtensor::determinant() const
  {
    double det = 0.0;

    switch(this->rank())
      {
        case 0:
          {
            det = (*this)(1);
            break;
          }

        case 1:
          {
            if(dim()[0] != 1)
              {
                ::fprintf(stderr,"\a\n ERROR: only for square XC::BJvector (1) \n");
                ::exit(1);
              }
            det = (*this)(1);
            break;
          }

        case 2:
          {
            BJmatrix converted = this->BJtensor2BJmatrix_2();
            det = converted.determinant();
            break;
          }

        case 3:
          {
            ::fprintf(stderr,"\a\n SORRY: not implemented \n");
            ::exit(1);
            break;
          }

        case 4:
          {
            ::fprintf(stderr,"\a\n SORRY: not implemented \n");
            ::exit(1);
            break;
          }
      }

//..........    null_indices();

    return det;
  }


//##############################################################################
XC::BJmatrix XC::BJtensor::BJtensor2BJmatrix_1() const // convert XC::BJtensor of even order to XC::BJmatrix
                                // to be used in inversion process
                                // in the beginning only XC::BJtensor of
                                // 2 and 4 order
                                // I_ijkl scheme
  {
    int BJtensor_order = this->rank();
    static int BJmatrix_dims[] = {0,0};

    if ( BJtensor_order == 2 )
      {
        BJmatrix_dims[0] = this->dim(1);
        BJmatrix_dims[1] = this->dim(2);
      }
    else if ( BJtensor_order == 4 )
      {
        BJmatrix_dims[0] = (this->dim(1))*(this->dim(3));
        BJmatrix_dims[1] = (this->dim(2))*(this->dim(4));
      }
    else
      {
        ::fprintf(stderr,"\a\n only XC::BJtensor rank 2 and 4 #\n");
        ::exit(1);
      }

    BJmatrix converted(BJmatrix_dims[0], BJmatrix_dims[1], 0.0);


   int m41 = 0;
   int m42 = 0;
// filling out the converted XC::BJmatrix
    if ( BJtensor_order == 2 )
      {
        for ( int c22=1 ; c22<=this->dim(2) ; c22++ )
          for ( int c21=1 ; c21<=this->dim(1) ; c21++ )
            converted(c21,c22)=(*this)(c21,c22);
      }
    else if ( BJtensor_order == 4 )
      {
        for ( int c44=1 ; c44<=this->dim(4) ; c44++ )
          for ( int c43=1 ; c43<=this->dim(3) ; c43++ )
            for ( int c42=1 ; c42<=this->dim(2) ; c42++ )
              for ( int c41=1 ; c41<=this->dim(1) ; c41++ )
                {
                  m41 = this->dim(1)*(c41-1)+c43;
                  m42 = this->dim(2)*(c42-1)+c44;

                  converted( m41, m42 ) = (*this)( c41, c42, c43, c44 );

                }
      }
    else
      {
        ::fprintf(stderr,"\a\n only XC::BJtensor rank 2 and 4 #\n");
        ::exit(1);
      }
//    converted.print("t","\n\n converted \n");
    return converted;
  }


//##############################################################################
XC::BJmatrix XC::BJtensor::BJtensor2BJmatrix_2() const // convert XC::BJtensor of even order to XC::BJmatrix
                                // to be used in inversion process
                                // in the beginning only XC::BJtensor of
                                // 2 and 4 order
                                // I_ikjl scheme
  {
    int BJtensor_order = this->rank();
    static int BJmatrix_dims[] = {0,0};

    if ( BJtensor_order == 2 )
      {
        BJmatrix_dims[0] = this->dim(1);
        BJmatrix_dims[1] = this->dim(2);
      }
    else if ( BJtensor_order == 4 )
      {
        BJmatrix_dims[0] = (this->dim(1))*(this->dim(2));
        BJmatrix_dims[1] = (this->dim(3))*(this->dim(4));
      }
    else
      {
        ::fprintf(stderr,"\a\n only XC::BJtensor rank 2 and 4 #\n");
        ::exit(1);
      }

    BJmatrix converted(BJmatrix_dims[0], BJmatrix_dims[1], 0.0);


   int m41 = 0;
   int m42 = 0;
// filling out the converted XC::BJmatrix
    if ( BJtensor_order == 2 )
      {
        for ( int c22=1 ; c22<=this->dim(2) ; c22++ )
          for ( int c21=1 ; c21<=this->dim(1) ; c21++ )
            converted(c21,c22)=(*this)(c21,c22);
      }
    else if ( BJtensor_order == 4 )
      {
        for ( int c44=1 ; c44<=this->dim(4) ; c44++ )
          for ( int c43=1 ; c43<=this->dim(3) ; c43++ )
            for ( int c42=1 ; c42<=this->dim(2) ; c42++ )
              for ( int c41=1 ; c41<=this->dim(1) ; c41++ )
                {
                  m41 = this->dim(1)*(c41-1)+c42;
                  m42 = this->dim(3)*(c43-1)+c44;

                  converted( m41, m42 ) = (*this)( c41, c42, c43, c44 );

                }
      }
    else
      {
        ::fprintf(stderr,"\a\n only XC::BJtensor rank 2 and 4 #\n");
        ::exit(1);
      }

//    converted.print("t","\n\n converted \n");

    return converted;
  }


//~~~~//##############################################################################
//~~~~BJmatrix XC::BJtensor::BJtensor2BJmatrix_3()  // convert XC::BJtensor of even order to XC::BJmatrix
//~~~~                                // to be used in inversion process
//~~~~                                // in the beginning only XC::BJtensor of
//~~~~                                // 2 and 4 order
//~~~~                                // I_iljk scheme
//~~~~  {
//~~~~    int BJtensor_order = this->rank();
//~~~~    static int BJmatrix_dims[] = {0,0};
//~~~~
//~~~~    if ( BJtensor_order == 2 )
//~~~~      {
//~~~~        BJmatrix_dims[0] = this->dim(1);
//~~~~        BJmatrix_dims[1] = this->dim(2);
//~~~~      }
//~~~~    else if ( BJtensor_order == 4 )
//~~~~      {
//~~~~        BJmatrix_dims[0] = (this->dim(1))*(this->dim(4));
//~~~~        BJmatrix_dims[1] = (this->dim(2))*(this->dim(3));
//~~~~      }
//~~~~    else
//~~~~      {
//~~~~        ::fprintf(stderr,"\a\n only XC::BJtensor rank 2 and 4 #\n");
//~~~~        ::exit(1);
//~~~~      }
//~~~~
//~~~~    BJmatrix converted(BJmatrix_dims[0], BJmatrix_dims[1], 0.0);
//~~~~
//~~~~
//~~~~   int m41 = 0;
//~~~~   int m42 = 0;
//~~~~// filling out the converted XC::BJmatrix
//~~~~    if ( BJtensor_order == 2 )
//~~~~      {
//~~~~        for ( int c22=1 ; c22<=this->dim(2) ; c22++ )
//~~~~          for ( int c21=1 ; c21<=this->dim(1) ; c21++ )
//~~~~            converted(c21,c22)=(*this)(c21,c22);
//~~~~      }
//~~~~    else if ( BJtensor_order == 4 )
//~~~~      {
//~~~~        for ( int c44=1 ; c44<=this->dim(4) ; c44++ )
//~~~~          for ( int c43=1 ; c43<=this->dim(3) ; c43++ )
//~~~~            for ( int c42=1 ; c42<=this->dim(2) ; c42++ )
//~~~~              for ( int c41=1 ; c41<=this->dim(1) ; c41++ )
//~~~~                {
//~~~~                  m41 = this->dim(1)*(c41-1)+c44;
//~~~~                  m42 = this->dim(2)*(c42-1)+c43;
//~~~~
//~~~~                  converted( m41, m42 ) = (*this)( c41, c42, c43, c44 );
//~~~~
//~~~~                }
//~~~~      }
//~~~~    else
//~~~~      {
//~~~~        ::fprintf(stderr,"\a\n only XC::BJtensor rank 2 and 4 #\n");
//~~~~        ::exit(1);
//~~~~      }
//~~~~
//~~~~//    converted.print("t","\n\n converted \n");
//~~~~
//~~~~    return converted;
//~~~~  }
//~~~~
//~~~~
//~~~~

//..//##############################################################################
//..XC::BJtensor XC::BJtensor::inverse()  // invert XC::BJtensor of even rank by
//..                          // converting it to XC::BJmatrix
//..                          // and than inverting the XC::BJmatrix
//..                          // and at the end putting it back
//..                          // again ( back conversion )
//..  {
//..    static int BJmatrix_dims[] = {0,0};
//..    if ( this->rank() == 2 )
//..      {
//..        BJmatrix_dims[0] = this->dim(1);
//..        BJmatrix_dims[1] = this->dim(2);
//..      }
//..    else if ( this->rank() == 4 )
//..      {
//..        BJmatrix_dims[0] = (this->dim(1))*(this->dim(2));
//..        BJmatrix_dims[1] = (this->dim(3))*(this->dim(4));
//..      }
//..    else
//..      {
//..        ::fprintf(stderr,"\a\n only XC::BJtensor rank 2 and 4 #\n");
//..        ::exit(1);
//..      }
//..    BJmatrix converted(BJmatrix_dims[0], BJmatrix_dims[1], 0.0);
//..
//..   int m41 = 0;
//..   int m42 = 0;
//..
//..// filling out the converted XC::BJmatrix
//..    if ( this->rank() == 2 )
//..      {
//..        for ( int c22=1 ; c22<=this->dim(2) ; c22++ )
//..          for ( int c21=1 ; c21<=this->dim(1) ; c21++ )
//..            converted(c21,c22)=(*this)(c21,c22);
//..      }
//..    else if ( this->rank() == 4 )
//..      {
//..        for ( int c44=1 ; c44<=this->dim(4) ; c44++ )
//..          for ( int c43=1 ; c43<=this->dim(3) ; c43++ )
//..            for ( int c42=1 ; c42<=this->dim(2) ; c42++ )
//..              for ( int c41=1 ; c41<=this->dim(1) ; c41++ )
//..                {
//..                  m41 = this->dim(1)*(c41-1)+c42;
//..                  m42 = this->dim(3)*(c43-1)+c44;
//..
//..                  converted( m41, m42 ) = (*this)( c41, c42, c43, c44 );
//..                }
//..      }
//..    else
//..      {
//..        ::fprintf(stderr,"\a\n only XC::BJtensor rank 2 and 4 #\n");
//..        ::exit(1);
//..      }
//..
//..//  converted.print("c","\n Converted:\n");
//..  BJmatrix converted_inverse = converted.inverse();
//..
//..// building up a XC::BJtensor result
//..  BJtensor result(this->rank(), this->dim(), 0.0);
//..
//..// filling back the inverted XC::BJmatrix to XC::BJtensor
//..    if ( this->rank() == 2 )
//..      {
//..        for ( int c22=1 ; c22<=this->dim(2) ; c22++ )
//..          for ( int c21=1 ; c21<=this->dim(1) ; c21++ )
//..            result(c21,c22)=converted_inverse(c21,c22);
//..      }
//..    else if ( this->rank() == 4 )
//..      {
//..        for ( int c44=1 ; c44<=this->dim(4) ; c44++ )
//..          for ( int c43=1 ; c43<=this->dim(3) ; c43++ )
//..            for ( int c42=1 ; c42<=this->dim(2) ; c42++ )
//..              for ( int c41=1 ; c41<=this->dim(1) ; c41++ )
//..                {
//..                  m41 = this->dim(1)*(c41-1)+c42;
//..                  m42 = this->dim(3)*(c43-1)+c44;
//..
//..                  result(c41,c42,c43,c44) = converted_inverse(m41,m42);
//..                }
//..      }
//..//    result.print("t","back to XC::BJtensor result");
//..    return result;
//..  }
//..

//~~~~//##############################################################################
//~~~~BJtensor XC::BJtensor::inverse_1()  // invert XC::BJtensor of even rank by
//~~~~                            // converting it to XC::BJmatrix
//~~~~                            // and than inverting the XC::BJmatrix
//~~~~                            // and at the end putting it back
//~~~~                            // again ( back conversion )
//~~~~                            // I_ijkl scheme
//~~~~
//~~~~  {
//~~~~
//~~~~  BJmatrix converted = this->BJtensor2BJmatrix_1();
//~~~~
//~~~~//  converted.print("c","\n Converted:\n");
//~~~~
//~~~~  BJmatrix converted_inverse = converted.inverse();
//~~~~
//~~~~//  converted_inverse.print("t","\n\n converted \n");
//~~~~
//~~~~  BJtensor result = converted_inverse.BJmatrix2BJtensor_1();
//~~~~
//~~~~//    result.print("t","back to XC::BJtensor result");
//~~~~
//~~~~    return result;
//~~~~  }


//##############################################################################
XC::BJtensor XC::BJtensor::inverse_2()  const // invert XC::BJtensor of even rank by
                            // converting it to XC::BJmatrix
                            // and than inverting the XC::BJmatrix
                            // and at the end putting it back
                            // again ( back conversion )
                            // I_ikjl scheme

{

  BJmatrix converted = this->BJtensor2BJmatrix_2();

//  converted.print("c","\n Converted:\n");

  BJmatrix converted_inverse = converted.inverse();

//  converted_inverse.print("t","\n\n converted_inverse \n");

  BJtensor result = converted_inverse.BJmatrix2BJtensor_2();

//  result.print("t","back to XC::BJtensor result");

  return result;
}


//##############################################################################
XC::BJtensor XC::BJtensor::inverse()  const // invert XC::BJtensor of even rank by
                          // converting it to XC::BJmatrix
                          // and than inverting the XC::BJmatrix
                          // and at the end putting it back
                          // again ( back conversion )
                          // I_ikjl scheme   TRUE ONE

  {

  int BJmatrix_or_BJtensor4 = this->rank();

  BJmatrix converted = this->BJtensor2BJmatrix_2();

  BJtensor result;

//converted.print("c","\n Converted:\n");

  BJmatrix converted_inverse = converted.inverse();

//converted_inverse.print("t","\n\n Converted_inverse \n");

  if ( BJmatrix_or_BJtensor4 == 4 ) 
    {
      result = converted_inverse.BJmatrix2BJtensor_2();
    }
  else if ( BJmatrix_or_BJtensor4 == 2 ) 
    {
      result = converted_inverse.BJmatrix2BJtensor_22();
//result.print("r","\n\n result  = converted_inverse.BJmatrix2BJtensor_2();\n");
    }
  else 
    {
      ::fprintf(stderr,"only 2 and 4 BJtensors \n");
      ::exit(1);
    }

//    result.print("t","back to XC::BJtensor result");
#ifdef SASA
//***** Dodo Sasa
//  result.pc_nDarray_rep->dim=this->dim();
// Pretty dirty (if it kills you should see that the string is copied
// and not the pointer as this can be a problem if one of the strings
// is destroyed !!!
// Also, if the dimensions of the tensors are different, there will be problems
  if(BJmatrix_or_BJtensor4==4)
    {
      result.pc_nDarray_rep->dim[0]=this->dim()[2];
      result.pc_nDarray_rep->dim[1]=this->dim()[3];
      result.pc_nDarray_rep->dim[2]=this->dim()[0];
      result.pc_nDarray_rep->dim[3]=this->dim()[1];
    }
  else
    {
      result.pc_nDarray_rep->dim[0]=this->dim()[1];
      result.pc_nDarray_rep->dim[1]=this->dim()[0];
    }
#endif
// ***************** End of changes added by Sasa
    return result;
  }


//##############################################################################
const std::string &XC::BJtensor::f_indices1( void ) const
  { return( this->indices1 ); }
//##############################################################################
const std::string &XC::BJtensor::f_indices2( void ) const
  { return( this->indices2 ); }

