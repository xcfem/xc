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
//##############################################################################
//# COPY-YES  (C):     :-))                                                    #
//# PROJECT:           Object Oriented Finite XC::Element Program                  #
//# PURPOSE:                                                                   #
//# CLASS:             skymatrix class                                         #
//#                                                                            #
//# VERSION:                                                                   #
//# LANGUAGE:          C++.ver >= 2.0 ( Borland C++ ver=3.00, SUN C++ ver=2.1 )#
//# TARGET OS:         DOS || UNIX || . . .                                    #
//# PROGRAMMER(S):     Boris Jeremic ( with parts of fortran COLSOL which      #
//#                                    is ported to C++ by BJ                  #
//#                                                                            #
//# DATE:              Nov. 12. 1992.                                          #
//# UPDATE HISTORY:    Nov. 14. 1992.  Improved operator=                      #
//#                    Nov. 16. 1992.  More print options ( upper, lower, full)#
//#                    ___. __. 199_.  derived class from XC::nDarray class        #
//#                    August 22-29 '94 choped to separate files and worked on #
//#                                   const and & issues                       #
//#                    August 30-31 '94 added use_def_dim to full the CC       #
//#                                   resolved problem with temoraries for     #
//#                                   operators + and - ( +=, -= )             #
//#                                                                            #
//#                    September 09 '94 starting to rewrite things after a talk#
//#                    by Stephen Jonson " Objecting the Objects". The point is#
//#                    to forget about inheriting skymatrix from XC::nDarray and   #
//#                    start from separate branch!                             #
//#                    September 11 '94 it works                               #
//#                    September 12-13 '94 looking for the solver for symmetric#
//#                                        and unsymmetric sparse matrices.    #
//#                                        One solution is Taylor's profile    #
//#                                        solver ( see FEM4ed by O.Z. and R.T.#
//#                    September 27 '94 profile solver for symmetric and       #
//#                                     Nonsymmetric systems works!            #
//#                                     (from FEM4ed by O.Z. and R.T.)         #
//#                                                                            #
//##############################################################################
#ifndef SKYMATRIX_CC
#define SKYMATRIX_CC

#include "skymatr.h"
#include "utility/matrix/nDarray/basics.h"

// SYMSKYMATRIX.CC  Symmetric Skyline Sparse Matrix Class



//.. //##########################################################################
//.. // empty constructor ( with default values )
//.. XC::skymatrix::skymatrix( int matrix_order, double init_val)
//.. {
//..  // create the structure:
//..    pc_skymatrix_rep=new skymatrix_rep; // this 'new' is overloaded
//.. 
//.. 
//..    pc_skymatrix_rep->square_dim= matrix_order;
//.. 
//.. // get space for the MAXA vector
//..    pc_skymatrix_rep->p_maxa=new int[matrix_order];
//.. // put all 1 in the MAXA ( somewhat superficial! )
//..    for ( int j=0 ; j<pc_skymatrix_rep->square_dim ; j++ )
//..      pc_skymatrix_rep->p_maxa[j]= 1;
//.. 
//..    pc_skymatrix_rep->total_numb= total_numb;
//.. 
//..  // allocate memory for the actual skymatrix as skymatrix
//..    pc_skymatrix_rep->pd_nDdata=new double [(size_t) pc_skymatrix_rep->total_numb];
//..      if (!pc_skymatrix_rep->pd_nDdata)
//..        {
//..          ::printf("\a\nInsufficient memory for skymatrix_rep\n");
//..          ::exit(1);
//..        }
//.. 
//..    pc_skymatrix_rep->n= 1;  // so far, there's one reference
//.. 
//..    for ( int i=0 ; i<pc_skymatrix_rep->total_numb ; i++ )
//..      pc_skymatrix_rep->pd_nDdata[i]= init_val;
//.. }
//.. 

//XC::tempoutskymatrix::skymatrix(FEModelData & FEMD, Brick3D * b3d, Node *  node)































































































































































//##########################################################################
//##########################################################################
XC::skymatrix::skymatrix(int Number_of_DOFs, int *MAXA, double *initval)
  {
 // create the structure:
    pc_skymatrix_rep=new skymatrix_rep; // this 'new' is overloaded

    pc_skymatrix_rep->square_dim= Number_of_DOFs;

// get space for the MAXA vector
    pc_skymatrix_rep->maxa=new int[Number_of_DOFs+1];
// put all maxa's in the MAXA
    for ( int j=0 ; j<=Number_of_DOFs ; j++ )  
//      pc_skymatrix_rep->maxa[0]= 1;
//    for ( int j=1 ; j<=Number_of_DOFs+1 ; j++ )  
      pc_skymatrix_rep->maxa[j]= MAXA[j];

   int Total_K_length= pc_skymatrix_rep->maxa[Number_of_DOFs];
 // allocate memory for the actual skymatrix as skymatrix
    pc_skymatrix_rep->data=new double [(size_t) Total_K_length];
      if (!pc_skymatrix_rep->data)
        {
          ::printf("\a\nInsufficient memory for skymatrix_rep\n");
          ::exit(1);
        }

    for ( int i=0 ; i<Total_K_length ; i++ )
      pc_skymatrix_rep->data[i]= initval[i];
  }


//XC::outOLDskymatrix::skymatrix(const skymatrix & x)   // copy initializer







//oldDestructor//##########################################################################
//XC::oldDestructorskymatrix::~skymatrix()
//oldDestructor  {
//oldDestructor    if (--pc_skymatrix_rep->n == 0)  // if reference count  goes to 0
//oldDestructor  {
//oldDestructor// DEallocate memory of the actual XC::nDarray
//oldDestructor//    delete [pc_nDarray_rep->pc_nDarray_rep->total_numb] pc_nDarray_rep->pd_nDdata;
//oldDestructor//  see ELLIS & STROUSTRUP $18.3
//oldDestructor//  and note on the p.65($5.3.4)
//oldDestructor//  and the page 276 ($12.4)
//oldDestructor    delete [] pc_skymatrix_rep->pd_nDdata;
//oldDestructor    delete [] pc_skymatrix_rep->p_maxa;
//oldDestructor    delete pc_skymatrix_rep;
//oldDestructor  }
//oldDestructor}

//##########################################################################
XC::skymatrix::~skymatrix()
  {
        ::printf(" ------------  XC::skymatrix::~skymatrix()   \n ");
         delete [] pc_skymatrix_rep->columnheight;
         delete [] pc_skymatrix_rep->maxa;
         delete [] pc_skymatrix_rep->data;
  }

























////#############################################################################
// Assemble global stiffness matrix from local contributions

//        ::printf("ColHeigth= \n");
//    for (int count= 1 ; count <= Number_of_DOFs ; count++)
//      {
//        ::printf("Col %d height=  %d \n", count, ColumnHeight[count] );
//      }

//##########################################################################
int XC::skymatrix::dimension_of_sky_M(void) const   // dimension of  sky matrix
  {
    return pc_skymatrix_rep->square_dim;
  }

//##########################################################################
int * XC::skymatrix::get_MAXA(void) const  // get pointer to array of
  {                                 // Locations of Diagonals
    return pc_skymatrix_rep->maxa;
  }



















































































































































































































//##########################################################################
double & XC::skymatrix::mval(int row, int col) const
  {
    if( row>col )         // Now we can call the matrix
      {                   // as if it is full matrix.
        int temp= row;   // This makes small overhead
        row= col;        // and it will be removed latter on.
        col= temp;
      }
    int diagonal_member_n= *(pc_skymatrix_rep->maxa+col-1);  //-1:starts from 0
   int member_of_sky_n= diagonal_member_n + col - row -1; //-1:starts from 0
    double * member_of_sky= &(pc_skymatrix_rep->data[member_of_sky_n]) ;
    return( * member_of_sky );
// put this latter after debugging
//          return(*( ps_sky_m_rep->pd_nDdata[*(ps_sky_m_rep->p_maxa+col)+col-row-1]));
  }



//##########################################################################
// full_val function allows you to treat skymatrix as if it is full
// float matrix. The function will calculate position inside sky matrix
// and return appropriate number if row and col are bellow skyline or
// return zero (0) if row and col are above sky line
double XC::skymatrix::full_val(int row, int col) const
  {
    if( row>col )
      {
        int temp= row;
        row= col;
        col= temp;
      }
  int total_column_height= col;
  int actual_column_position= total_column_height - row + 1;
  int real_column_height= pc_skymatrix_rep->maxa[col]-pc_skymatrix_rep->maxa[col-1]; 
  int how_much_above_skyline=actual_column_position-real_column_height; //  adding -1 -Zhaohui
  if ( how_much_above_skyline > 0 )
    {
      double back= 0.0;    ////    // not very smart ???????
      return ( back );
    }
  else
    {
      int diagonal_member_n= *(pc_skymatrix_rep->maxa+col-1);  //-1:starts from 0 // deleting -1 --Zhaohui
      int member_of_sky_n= diagonal_member_n + col - row-1 ; //-1:starts from 0 // deleting -1  --Zhaohui
      double member_of_sky= pc_skymatrix_rep->data[member_of_sky_n] ;
      return( member_of_sky );
// put this latter after debugging
//        return(*( ps_sky_m_rep->pd_nDdata[*(ps_sky_m_rep->p_maxa+col)+col-row-1]));
    }
  }
     // used by skymatrix functions which KNOW they aren't
     // exceeding the boundaries





//##########################################################################
double & XC::skymatrix::val(int row, int col)
  {
//    double zero=0.0;
    if ( row<=0 && row>=dimension_of_sky_M() && col>=dimension_of_sky_M() )
      {
        error("index out of range");
//        return zero;
      }
//    else
      return (mval(row,col));
  }

//##########################################################################
double XC::skymatrix::cval(int row, int col) const
  {
    double zero=0.0;
    if ( row<=0 && row>=dimension_of_sky_M() && col>=dimension_of_sky_M() )
      {
        error("index out of range");
        return zero;
      }
//    else
      return (mval(row,col));
  }


//##########################################################################
double XC::skymatrix::mmin()
  {
    double temp= 0.0;
    if ( dimension_of_sky_M()<=0 )
      {
        error("bad skymatrix size for min ()");
        return 0.0;
      }
    double minimum= mval(1,1);
    for ( int row=1 ; row<=dimension_of_sky_M() ; row++ )
      for ( int col=1 ; col<=dimension_of_sky_M() ; col++ )
        if ( (temp=mval(row,col)) < minimum )
          minimum= temp;
    return minimum;
  }

//##########################################################################
double XC::skymatrix::mmax()
  {
    double temp= 0.0;
    if( dimension_of_sky_M()<=0 )
      {
        error("bad skymatrix size for max()");
        double zero=0.0;
        return zero;
      }
    double maximum= mval(1,1);
    for ( int row=1 ; row<=dimension_of_sky_M() ; row++ )
      for ( int col=1 ; col<=dimension_of_sky_M() ; col++ )
        {
          if ( (temp=mval(row,col)) > maximum )
          maximum= temp;
        }
    return maximum;
  }














//##########################################################################
void XC::skymatrix::lower_print(const std::string &msg)
  {
    if(!msg.empty()) printf("%s\n",msg.c_str());
    for ( int row=1 ; row<=dimension_of_sky_M() ; row++ )
      {
        int total_column_height= row;
        int real_column_height= pc_skymatrix_rep->maxa[row] - pc_skymatrix_rep->maxa[row-1];
        int numb_of_voids= total_column_height - real_column_height;
        int n_of_voids_to_reach_number= numb_of_voids;
        for ( int col=1 ; col<=row ; col++ )
          {
            if( n_of_voids_to_reach_number > 0 )
              {
                 for(int void_count=1; void_count<=numb_of_voids; void_count++)
                  {
                    printf("********* ");
                    col++;
                    n_of_voids_to_reach_number--;
                  }
              }
            printf( "%+6.2e ", cval(row,col) );
          }
        printf("\n");
      }
  }

//##########################################################################
void XC::skymatrix::upper_print(const std::string &msg)
  {
    if(!msg.empty()) printf("%s\n",msg.c_str());
    for ( int row=1 ; row<=dimension_of_sky_M() ; row++ )
      {
        for ( int voids=0 ; voids<row-1 ; voids++ ) printf("        ");
        for ( int col=row ; col<=dimension_of_sky_M() ; col++ )
          {
            int total_column_height= col;
            int actual_column_position= total_column_height - row + 1;
            int real_column_height= pc_skymatrix_rep->maxa[col] - pc_skymatrix_rep->maxa[col-1];
            int how_much_above_skyline=actual_column_position-real_column_height;
            if ( how_much_above_skyline > 0 )
              {
                printf("********* ");
              }
            else
              {
                printf( "%+6.2e ", cval(col,row) );
              }
          }
        printf("\n");
      }
  }


//##########################################################################
void XC::skymatrix::full_print(const std::string &msg)
  {
    if(!msg.empty()) printf("%s\n",msg.c_str());
    for( int row=1 ; row<=dimension_of_sky_M() ; row++ )
      {
        for ( int col=1 ; col<=dimension_of_sky_M() ; col++ )
          {
            printf( "%+6.2e ", full_val(row,col) );
          }
        printf("\n");
      }
  }


//##########################################################################
void XC::skymatrix::error(const std::string &msg1,const std::string &msg2) const
  {
    ::fprintf(stderr,"skymatrix error: %s %s\n", msg1.c_str(), msg2.c_str());
    exit( 1 );
  }



























































































///****************************************************************************
//* COPY-YES  (C):   1990,1991,1992
//* PROJECT:
//* FILE:
//* FUNCTION:
//* PURPOSE:
//* VERSION
//* LANGUAGE:        Microsoft C 6.0 , Borland C++ 3.1
//* TARGET OS:       DOS
//* PROGRAMMER:      Jeremic Boris
//* DATE:
//* UPDATE HISTORY:  Nov. 12. 1992. C++ ver
//****************************************************************************/
///*....................................................................*/
///*.                                                                  .*/
///*.                    C  O  L  S  O  L                              .*/
///*.                                                                  .*/
///*. Program to solve Finite XC::Element Static Equilibrium Equations     .*/
///*. in Core, using compacted and column reduction scheme             .*/
///*.                                                                  .*/
///*.                                                                  .*/
///*. Input Variables                                                  .*/
///*.                                                                  .*/
///*.       a[nwk]     = stiffness matrix stored in compacted form    .*/
///*.       v[nn]      = right_hand_side load vector                  .*/
///*.       maxa[nn+1] = vector containing addresses of diagonal      .*/
///*.                     elements of stiffness matrix in a            .*/
///*.       nn         = number of equations                          .*/
///*.       nwk        = number of elements below skyline of matrix   .*/
///*.                                                                  .*/
///*. Output                                                           .*/
///*.                                                                  .*/
///*.       a[nwk]     = D and L - factors of stiffness matrix        .*/
///*.       v[nn]      = displacement vector                          .*/
///*.                                                                  .*/
///*....................................................................*/
///*....................................................................*/
///*.      Fortran source code was taken from the book:                .*/
///*.                                                                  .*/
///*.      Klaus-Jürgen Bathe ;                                        .*/
///*.                                                                  .*/
///*.          Finite XC::Element Procedures In Engineering XC::Analysis       .*/
///*.                                                                  .*/
///*....................................................................*/
///*.      Rearanged for C language by Jeremic Boris                   .*/
///*.      Bekhme dam site, August & September 1990.                   .*/
///*.      Beograd, home,   January & February 1991.                   .*/
///*....................................................................*/
///*.      Rearanged for C++ language by Jeremic Boris                 .*/
///*.      Boulder, home,   Nov. 12. 1992.                             .*/
///*....................................................................*/
///*....................................................................*/
///* void function v_ldl_factorize will                                 */
///* factorize matrix a in L_D_L factors                                */
///* arguments are : a    - pointer (double) to vector of name a */
///*                 maxa - pointer (int) to vector maxa         */
///*                 nn   - pointer (int) to number nn           */
///*                                                                    */
///* 03. august.   1990. Bekhme Dam Site          0. revision           */
///* 04. september 1990. Bekhme Dam Site          1. revision           */
///* 14. september 1990. Bekhme Dam Site          2. revision           */
///* 01. february  1991. E.P. Beograd             3. revision           */
///* 12. November  1992. Boulder CU ( home)       4. revision           */
///* 28. May       1999. Clarkson, Potsdam)       5. revision           */
///*....................................................................*/
XC::skymatrix & XC::skymatrix::v_ldl_factorize()
{
  int kn = 0;
  int kl = 0;
  int ku = 0;
  int kh = 0;
  int k  = 0;
  int ic = 0;
  int klt= 0;
  int ki = 0;
  int nd = 0;
  int kk = 0;
  int n  = 0;
  int j  = 0;
  int l  = 0;
  double c= 0.0;
  double b= 0.0;
  ::printf(" \n\n* * * Equations to factorize : ");
  for ( n=1 ; n<=pc_skymatrix_rep->square_dim ; n++ )
    {
      printf(" %5d\b\b\b\b\b", pc_skymatrix_rep->square_dim - n);
      kn=*(pc_skymatrix_rep->maxa-1+n);
      kl=kn+1;
      ku=*(pc_skymatrix_rep->maxa-1+n+1)-1;
      kh=ku-kl;         // changes ######## from colsol.c
      if ( kh>0 )       // *(pd_ldl_a. . . ) --> *(pc_skymatrix_rep->pd_nDdata-1. . . )
        {               // *(pi_ldl_maxa. . . ) --> *(pc_skymatrix_rep->p_maxa-1. . .)
          k=n-kh;       // *(pi_ldl_nn) --> pc_skymatrix_rep->square_dim
          ic=0;
          klt=ku;
          for ( j=1 ; j<=kh ; j++ )
            {
              ic=ic+1;
              klt=klt-1;
              ki=*(pc_skymatrix_rep->maxa-1+k);
              nd=*(pc_skymatrix_rep->maxa-1+k+1)-ki-1;
              if ( nd>0 )
                {
                  kk=( (ic<nd) ? ic : nd );
                  c=0.0;
                  for ( l=1 ; l<=kk ; l++ )
                    c=c+(*(pc_skymatrix_rep->data-1+ki+l))*(*(pc_skymatrix_rep->data-1+klt+l));
                  *(pc_skymatrix_rep->data-1+klt)=*(pc_skymatrix_rep->data-1+klt)-c;
                }
              k=k+1;
            }
        }
      if ( kh>=0 )
        {
          k=n;
          b=0.0;
          for ( kk=kl ; kk<=ku ; kk++ )
            {
              k=k-1;
              ki=*(pc_skymatrix_rep->maxa-1+k);
              c=(*(pc_skymatrix_rep->data-1+kk))/(*(pc_skymatrix_rep->data-1+ki));
              b=b+c*(*(pc_skymatrix_rep->data-1+kk));
              *(pc_skymatrix_rep->data-1+kk)=c;
            }
          *(pc_skymatrix_rep->data-1+kn)=*(pc_skymatrix_rep->data-1+kn)-b;
        }
      if ( *(pc_skymatrix_rep->data-1+kn)<=0 )
        {
          printf("\n Colsol Stoped - Stiffness XC::Matrix not positive definite \n");
          printf(" non positive pivot for equation, %d\n ", n);
          printf(" pivot, %.12e \n", *(pc_skymatrix_rep->data-1+kn) );
          exit(1);
        }
//  printf("--------------------------  %d\n",n);
//  for( int i=0 ; i<=11 ; i++ )
//    {
//      printf("pc_skymatrix_rep->pd_nDdata[%d]= %8.4f\n",i, pc_skymatrix_rep->pd_nDdata[i]);
//    }
//  getch();


  }
  printf("\n");
  return(*this);
}

///****************************************************************************
//* COPY-YES  (C):   1990,1991   Jeremic Boris,
//* PROJECT:
//* FILE:
//* FUNCTION:
//* PURPOSE:
//* VERSION
//* LANGUAGE:        Microsoft C 6.0
//* TARGET OS:       DOS
//* PROGRAMMER:      Jeremic Boris
//* DATE:
//* UPDATE HISTORY:
//****************************************************************************/
///*....................................................................*/
///* double function  d_reduce_r_h_s_l_v                                  */
///* will reduce right hand side load vector v                          */
///* arguments are : a    - pointer (double) to vector of name a */
///*                 v    - pointer (double) to vector of name v */
///*                 maxa - pointer (int) to vector maxa         */
///*                 nn   - pointer (int) to number nn           */
///*                                                                    */
///* 10. august.   1990. Bekhme Dam Site      0. revision               */
///* 04. september 1990. Bekhme Dam Site      1. revision               */
///* 15. september 1990. Bekhme Dam Site      2. revision               */
///* 01. february  1991. E.P. Beograd         3. revision               */
///* 12. November  1992. Boulder CU ( home)   4. revision               */
///*....................................................................*/
double * XC::skymatrix::d_reduce_r_h_s_l_v ( double *pd_rhs )
{
  int kl= 0;
  int ku= 0;
  int k = 0;
  int n = 0;
  int kk= 0;
  double c= 0.0;
  printf("\n * * * Right hand side loads to reduce :");
  for ( n=1 ; n<=pc_skymatrix_rep->square_dim ; n++ )
    {
      printf(" %5d\b\b\b\b\b", pc_skymatrix_rep->square_dim - n);
      kl=*(pc_skymatrix_rep->maxa-1+n)+1;
      ku=*(pc_skymatrix_rep->maxa-1+n+1)-1;  // changes ######## from colsol.c
      if ( ku>=kl )             // *(pd_red_a. . . ) --> *(pc_skymatrix_rep->pd_nDdata-1. . . )
        {                       // *(pi_red_maxa. . . ) --> *(pc_skymatrix_rep->p_maxa-1. . .)
          k=n;                  // *(pi_red_nn) --> pc_skymatrix_rep->square_dim
          c=0.0;                // *(pd_rhs. . .) --> *(pd_rhs-1 . . . )
          for ( kk=kl ; kk<=ku ; kk++ )
            {
              k=k-1;
              c=c+(*(pc_skymatrix_rep->data-1+kk))*(*(pd_rhs-1+k));
            }
          *(pd_rhs-1+n)=*(pd_rhs-1+n)-c;   //pd_rhs[-1+n]==*(pd_rhs-1+n)
        }
    }
  printf("\n");
  return( pd_rhs );
}

///****************************************************************************
//* COPY-YES  (C):   1990,1991     Jeremic Boris,
//* PROJECT:
//* FILE:
//* FUNCTION:
//* PURPOSE:
//* VERSION
//* LANGUAGE:        MicroSoft C 6.0
//* TARGET OS:       DOS
//* PROGRAMMER:      Jeremic Boris  (J_P_S_B)
//* DATE:
//* UPDATE HISTORY:
//*
//*
//*
//****************************************************************************/
///*....................................................................*/
///* void function v_back_substitute will                               */
///* back substitute factorized matrix a and solve for unknown vector v */
///* arguments are : pd_bac_a    - pointer (double) to vector of name a */
///*                 pd_bac_v    - pointer (double) to vector of name v */
///*                 pi_bac_maxa - pointer (int) to vector maxa         */
///*                 pi_bac_nn   - pointer (int) to number nn           */
///*                                                                    */
///* 10. august.   1990. Bekhme dam site     0.  revision               */
///* 04. september 1990. Bekhme Dam Site     1.  revision               */
///* 17. septembar 1990. Bekhme Dam Site     2.  revision               */
///* 01. february  1991. E.P. Beograd        3.  revision               */
///* 12. November  1992. Boulder CU ( home)  4. revision                */
///*....................................................................*/
double * XC::skymatrix::d_back_substitute ( double *pd_rhs )
{
  int k = 0;
  int n = 0;
  int kl= 0;
  int ku= 0;
  int l = 0;
  int kk= 0;
  printf(" \n * * * Equations to back substitute :");
  for ( n=1 ; n<=pc_skymatrix_rep->square_dim ; n++ )
    {
      printf(" %5d\b\b\b\b\b", pc_skymatrix_rep->square_dim - n);
      k=*(pc_skymatrix_rep->maxa-1+n);
      *(pd_rhs-1+n)=(*(pd_rhs-1+n))/(*(pc_skymatrix_rep->data-1+k));
    }
  if ( pc_skymatrix_rep->square_dim==1 ) return(pd_rhs);
  n=pc_skymatrix_rep->square_dim ;
  for ( l=2 ; l<=pc_skymatrix_rep->square_dim ; l++ )
    {                            // changes ######## from colsol.c
      kl=*(pc_skymatrix_rep->maxa-1+n)+1;     // *(pd_bac_a. . . ) --> *(pc_skymatrix_rep->pd_nDdata-1. . . )
      ku=*(pc_skymatrix_rep->maxa-1+n+1)-1;   // *(pi_bac_maxa. . . ) --> *(pc_skymatrix_rep->p_maxa-1. . .)
      if( ku>=kl )               // *(pi_bac_nn) --> pc_skymatrix_rep->square_dim
        {                        // *(pd_rhs. . .) --> *(pd_rhs-1 . . . )
          k=n;
          for ( kk=kl ; kk<=ku ; kk++ )
            {
              k=k-1;
              *(pd_rhs-1+k)=*(pd_rhs-1+k)-(*(pc_skymatrix_rep->data-1+kk))*(*(pd_rhs-1+n));
            }
          n=n-1;
        }
    }
  printf("\n");
  return (pd_rhs);
}

///////////////////////////////////////////////////////////////////////////////































































































#endif

