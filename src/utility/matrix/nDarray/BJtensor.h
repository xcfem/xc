// -*-c++-*-
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
 
// $Revision: 1.4 $                                                              
// $Date: 2005/10/21 22:02:39 $                                                                  
// $Source: /usr/local/cvs/OpenSees/SRC/nDarray/BJtensor.h,v $                                                                

//#############################################################################
//                                                                            #
//                                                                            #
//             /~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/~~\                #
//            |                                          |____|               #
//            |                                          |                    #
//            |                                          |                    #
//            |                                          |                    #
//            |                                          |                    #
//            |        B A S E   C L A S S E S           |                    #
//            |                                          |                    #
//            |                                          |                    #
//            |                                          |                    #
//            |                                          |                    #
//            |          C + +     H E A D E R           |                    #
//            |                                          |                    #
//            |                                          |                    #
//            |                                          |                    #
//            |                                          |                    #
//         /~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/   |                    #
//         \_________________________________________\__/                     #
//                                                                            #
//                                                                            #
//#############################################################################
//#############################################################################
///*
//################################################################################
//# COPYRIGHT (C):     :-))                                                      #
//# PROJECT:           Object Oriented Finite Element Program                    #
//# PURPOSE:                                                                     #
//# CLASS:             BJtensor                                                    #
//#                                                                              #
//# VERSION:                                                                     #
//# LANGUAGE:          C++.ver >= 2.0 ( Borland C++ ver=3.10, SUN C++ ver=2.1 )  #
//# TARGET OS:         DOS || UNIX || . . .                                      #
//# DESIGNER(S):       Boris Jeremic                                             #
//# PROGRAMMER(S):     Boris Jeremic                                             #
//#                                                                              #
//#                                                                              #
//# DATE:              May 28. - July  __  '93                                   #
//# UPDATE HISTORY:    july 8. '93. BJtensor02 - BJtensor multiplication             #
//#                                 inner and outer products                     #
//#                    august 17-19 '93 fixed default constructor that wasted    #
//#                                     memory ###                               #
//#                    october 11 '93 added transpose0110, transpose0101,        #
//#                                   transpose0111 so the creation of           #
//#                                   isotropic BJtensor is much easier and         #
//#                                   understandable!                            #
//#                    january 06 '93  added BJtensor2BJmatrix_1, BJtensor2BJmatrix_2     #
//#                                   BJtensor2BJmatrix_3, inverse_1, inverse_2,     #
//#                                   inverse_3                                  #
//#                    january 20 '93  added inverse  TRUE ONE                    #
//#                    August 22-29 '94 choped to separate files and worked on   #
//#                                   const and & issues                         #
//#                    August 30-31 '94 added use_def_dim to full the CC         #
//#                                   resolved problem with temoraries for       #
//#                                   operators + and - ( +=, -= )               #
//#                    Oktobar 15 '96  added  trasposeoverbar trasposeunderbar   #
//#                                    ( the same as transpose0110 and           #
//#                                    transpose0101) but just to make it        #
//#                                    easier to work with Runesson's notation   #
//#                    28June2004     added val4 for efficiency still            #
//#                                   to be worked on                            #
//#                                                                              #
//#                                                                              #
//################################################################################
//*/

#ifndef TENSOR_HH
#define TENSOR_HH


#include "utility/matrix/nDarray/nDarray.h"


namespace XC {
//! @ingroup Matrix
//!
//! @brief Boris Jeremic tensor class.
class BJtensor: public nDarray
  {
    friend class stressstraintensor;
    friend class stresstensor;
    friend class straintensor;
//  private:
  public:
    mutable std::string indices1;   //!<  character array of indices
    mutable std::string indices2;   //  they should be checked for matching and
                                    //  then operator will be identified
  public: // just send appropriate arguments to the base constructor
    BJtensor(int rank_of_BJtensor=1,const double &initval=0); // default constructor
    BJtensor(int rank_of_BJtensor, const int *pdim, const double *values);
    BJtensor(int rank_of_BJtensor, const int *pdim, const std::vector<double> &values);
    BJtensor(int rank_of_BJtensor, const int *pdim, const boost::python::list &l);    
    BJtensor(int rank_of_BJtensor, const int *pdim,const double &initvalue);
    BJtensor(const std::string &flag, int rank_of_BJtensor, const int *pdim);  // create a unit nDarray
    BJtensor(const std::string &flag);   //this one used to send "NO" message
    BJtensor(const BJtensor &x);  // instead of: "BJtensor(nDarray & x):"   :-)
    explicit BJtensor(const nDarray &x);

    //~BJtensor( );

    BJtensor&  operator=( const BJtensor & rval);       // BJtensor assignment

    friend BJtensor operator+(const BJtensor & , const BJtensor & ); // BJtensor addition
    friend BJtensor operator-(const BJtensor & , const BJtensor & ); // BJtensor subtraction

    BJtensor& operator*=(const double &rval);     // Added Zhao Oct2005
    BJtensor operator*(const double &rval) const; // scalar multiplication, Added const Zhao Oct2005
    BJtensor operator*(const BJtensor &rval) const;       // inner/outer product
    BJtensor operator/(const BJtensor &rval) const;       // BJtensor division rval MUST BE BJtensor of
                                            // order 0 ( i.e. scalar in BJtensor form )

    BJtensor transpose0110( ) const;      // transpose ijkl  -->> ikjl
    BJtensor transposeoverbar( ) const;   // transpose ijkl  -->> ikjl
    BJtensor transpose0101( ) const;      // transpose ijkl  -->> ilkj
    BJtensor transpose0111( ) const;      // transpose ijkl  -->> iljk
    BJtensor transposeunderbar( ) const;  // transpose ijkl  -->> iljk
    BJtensor transpose1100( ) const;      // transpose ijkl  -->> jikl    First minor symm
    BJtensor transpose0011( ) const;      // transpose ijkl  -->> ijlk    Second minor symm
    BJtensor transpose1001( ) const;      // transpose ijkl  -->> ljki
    BJtensor transpose11( ) const;        // transpose ij  -->> ji

    BJtensor symmetrize11( ) const;   // symmetrize with respect to ij
    double determinant(void) const;  // determinant of a BJtensor
    BJmatrix BJtensor2BJmatrix_1(void) const;// convert BJtensor of even order to BJmatrix
                              // to be used in inversion process
                              // I_ijkl scheme
    BJmatrix BJtensor2BJmatrix_2(void) const;// convert BJtensor of even order to BJmatrix
                              // to be used in inversion process
                              // I_ikjl scheme
    BJmatrix BJtensor2BJmatrix_3(void) const;// convert BJtensor of even order to BJmatrix
                              // to be used in inversion process
                               // I_iljk scheme
    BJtensor inverse(void) const;  // inverse of a BJtensor
                                 // I_ikjl scheme TRUE ONE         ____
                                                         //           |
    BJtensor inverse_2(void) const;    // inverse of a BJtensor   <--|
                                       // I_ikjl scheme

    const BJtensor &operator()(const std::string &indices_from_user) const;

    void null_indices(void) const;
    const std::string &f_indices1(void) const;
    const std::string &f_indices2(void) const;

  private:
//K    int BJtensor::contracted_ind(const std::string &, const std::string &, int *, int *, int , int );
//K    int BJtensor::uncontracted_ind(int *, int *, int);
    int contracted_ind(const std::string &,const std::string &, int *, int *, int , int ) const;
    int uncontracted_ind(int *, int *, int) const;
  };

// GLOBAL
BJtensor operator*(const double &lval,const BJtensor &rval);  // REVIEWER global *
BJtensor operator+(const BJtensor & , const BJtensor & ); // BJtensor addition
BJtensor operator-(const BJtensor & , const BJtensor & ); // BJtensor subtraction

} // end of XC namespace

#endif



