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
                                                                        
// $Revision: 1.2 $                                                              
// $Date: 2003/02/14 23:01:48 $                                                                  
// $Source: /usr/local/cvs/OpenSees/SRC/nDarray/BJmatrix.cpp,v $                                                                

                                                                        
                                                                        
///*
//################################################################################
//# COPY-YES  (C):     :-))                                                      #
//# PROJECT:           Object Oriented Finite XC::Element Program                    #
//# PURPOSE:                                                                     #
//# CLASS:             BJmatrix                                                  #
//#                                                                              #
//# VERSION:                                                                     #
//# LANGUAGE:          C++.ver >= 2.0 ( Borland C++ ver=3.10, SUN C++ ver=2.1 )  #
//# TARGET OS:         DOS || UNIX || . . .                                      #
//# DESIGNER(S):       Boris Jeremic                                             #
//# PROGRAMMER(S):     Boris Jeremic                                             #
//#                    Bruce Eckel { changed/improved a lot by BJ }              #
//#                                                                              #
//# DATE:              November '92                                              #
//# UPDATE HISTORY:    05 - __ avgust '93.  redefined as derived class from      #
//#                                 nDarray class                                #
//#                    january 06 '93  added BJmatrix2BJtensor_1, BJmatrix2BJtensor_2    #
//#                                   BJmatrix2BJtensor_3                        #
//#                    August 22-29 '94 choped to separate files and worked on   #
//#                                   const and & issues                         #
//#                    August 30-31 '94 added use_def_dim to full the CC         #
//#                                   resolved problem with temoraries for       #
//#                                   operators + and - ( +=, -= )               #
//#                                                                              #
//#                    May 10 2000 Xiaoyan found a bug with 			 #
//#        XC::BJmatrix::BJmatrix(int rows, int columns, double *initvalues):        #
//#           nDarray( 2, rows, columns, initvalues){ } 			 #
//#                                                                              #
//#                                                                              #
//#                                                                              #
//################################################################################
//*/

#include "utility/matrix/nDarray/BJmatrix.h"
#include "utility/matrix/nDarray/BJtensor.h"
#include <iostream>

#define TINY  1e-20
void XC::BJmatrix::error(const std::string &msg1, const std::string &msg2)
  {
    std::cerr << "BJmatrix error: " << msg1 << " " << msg2 << std::endl;
    exit(1);
  }

//##############################################################################
XC::BJmatrix::BJmatrix(int rows, int columns, double initval):
  nDarray( 2, rows, columns, initval){ } // calling the appropriate
                                          // base constructor
//##############################################################################
XC::BJmatrix::BJmatrix(int rows, int columns, double *initvalues):
  nDarray( 2, rows, columns, initvalues){ } // calling the appropriate
                                             // base constructor
//Xiaoyan found a bug it should be initvalue instead of *initvalue
//##############################################################################
//  special for vector
XC::BJmatrix::BJmatrix(int rank, int rows, int columns, double *initvalues):
  nDarray( rank, rows, columns, initvalues){ } // calling the appropriate
                                             // base constructor
//##############################################################################
//  special for vector
XC::BJmatrix::BJmatrix(int rank, int rows, int columns, double initvalues):
  nDarray( rank, rows, columns, initvalues){ } // calling the appropriate
                                             // base constructor
//##############################################################################
XC::BJmatrix::BJmatrix(const std::string &flag, int dimension ): // create an ident XC::BJmatrix
  nDarray("NO")           // with base class constructor cancellation
  {
    if ( flag[0] != 'I' && flag[0] != 'e' )
     {
       std::cerr << "\n To create a 2nd rank Kronecker delta type:"
	         << "nDarray (\"I\",2);\n";
       ::exit(1);
     }
// create the structure:
     pc_nDarray_rep = new nDarray_rep; // this 'new' is overloaded
     rank(2);  //rank_of_nDarray here BJmatrix = 2

// in the case of nDarray_rank=0 add one to get right thing from the
// operator new
     const std::vector<int> pdim({dimension,dimension});
     set_dim(pdim);  // array for dimensions

// allocate memory for the actual XC::nDarray as XC::nDarray
     pc_nDarray_rep->init_data();

     set_reference_count(+1);  // so far, there's one reference

     for(int i2=1 ; i2<=dim(1) ; i2++ )
       for( int j2=1 ; j2<=dim(2) ; j2++ )
         val(i2,j2) = (i2 == j2 ? 1  : 0);
  }

//#############################################################################
// error message when trying to read a 'standard'
// BJmatrix file:
static const std::string nonstandard= "is a 'non-standard'file. A 'standard' BJmatrix file must\n start with the dimensions of the XC::BJmatrix, i.e.:\n \t rows= 12 columns= 14\n or abbreviated: r= 12 c= 14\n Notice rows appear before columns, and chars are lowercase\n comments follow '!' signs to end of line, data follows ::\n";

//#############################################################################
// read from "standard" BJmatrix file:
XC::BJmatrix::BJmatrix(const std::string &initfile)
  : nDarray("NO") // with base class constructor cancellation
  {
  #define BSIZE 120
    FILE *from;

    int rows    = 0;
    int columns = 0;

    if ((from = fopen(initfile.c_str(),"r")) == 0)
      error("cannot open XC::BJmatrix initializer file", initfile);
    char buf[BSIZE], *cp, *cp2;
    int rfound = 0, cfound = 0, colonsfound = 0;
  //        /*Parse file initialization header  */
    while( fgets(buf, BSIZE, from) )         // for each header line
      {
  // Remove comments with ANSI C library function 'strp
	if( ( cp = strpbrk(buf,"!")) != nullptr ) // look for comments
	  *cp = '\0';  // terminate string at comment
	if(( cp = strpbrk(buf,"r")) != nullptr )
	  if (( cp2 = strpbrk(cp, "=")) != nullptr )
	    if (( cp = strpbrk( cp2, "0123456789")) != nullptr )
	      {
		rows = atoi(cp);
		rfound++;  // flag to say rows were found
	      }
	if( ( cp = strpbrk(buf,"c") ) != nullptr )
	   if ( ( cp2 = strpbrk(cp, "=")) != nullptr )
	     if ( ( cp = strpbrk(cp2, "O123456789")) != nullptr )
	       {
		 columns = atoi(cp);
		 cfound++;  // flag to say cols were found
	       }
	if ( strstr(buf,"::") != nullptr )
	  {
	    colonsfound++;
	    break; //... out of "while" loop
	  }
      }
    if ( !rfound || !cfound || !colonsfound )
      {
	std::cerr << initfile << nonstandard << std::endl;
	exit(1);
      }

  // let's construct a BJmatrix

  // create the structure:
    pc_nDarray_rep = new nDarray_rep; // this 'new' is overloaded
    rank(2);  // rank_of_nDarray here BJmatrix = 2

       const std::vector<int> pdim({rows,columns});
       set_dim(pdim);// array for dimensions

  // allocate memory for the actual nDarray as nDarray
       pc_nDarray_rep->init_data();

    set_reference_count(+1);  // so far, there's one reference

    for(int row=1 ; row <= rows ; row++ )
      for(int col=1 ; col <= columns ; col++ )
	{
	  char nb[20];
	  const int tmp= fscanf(from, "%s", nb); // scan for space-delimited string
	  if(tmp==EOF)
	    {
	      std::cerr << "end of file while reading BJmatrix." << std::endl;
	      break;
	    }
	  val(row,col) = (double) atof(nb); // convert it to a double
	  if(ferror(from))
	    error("problem with BJmatrix initializer file ", initfile);
	}
    fclose(from);
  }

//#############################################################################
// read from flat XC::BJmatrix file: and write to test output
XC::BJmatrix::BJmatrix(const std::string &initfile,const std::string &outfile):
  nDarray("NO")           // with base class constructor cancellation
{
//#define BSIZE 120
  FILE *from;
//Kai  FILE *to;

  int rows    = 0;
  int columns = 0;

  if ((from = fopen(initfile.c_str(),"r")) == 0)
    error("cannot open XC::BJmatrix initializer file", initfile);
//  char buf[BSIZE];
  char  cp[32];
  const int tmp= fscanf(from, "%s", cp); // scan for space-delimited string
  if(tmp==EOF)
    std::cerr << "end of file while reading BJmatrix." << std::endl;
//  cp = strpbrk( buf, "0123456789")
  rows = columns = atoi(cp);
// create the structure:
  pc_nDarray_rep = new nDarray_rep; // this 'new' is overloaded
  rank(2);  // rank_of_nDarray here XC::BJmatrix = 2

     const std::vector<int> pdim({rows,columns});
     set_dim(pdim);// array for dimensions

     // allocate memory for the actual nDarray as XC::nDarray
     pc_nDarray_rep->init_data();

     set_reference_count(+1);  // so far, there's one reference

  for ( int row=1 ; row <= rows ; row++ )
    for ( int col=1 ; col <= columns ; col++ )
      {
        char nb[20];
        const int tmp= fscanf(from, "%s", nb); // scan for space-delimited string
        if(tmp==EOF)
	  {
            std::cerr << "end of file while reading BJmatrix." << std::endl;
            break;
          }
        val(row,col) = (double) atof(nb); // convert it to a double
        if(ferror(from))
          error("problem with XC::BJmatrix initializer file ", initfile);
      }
  fclose(from);

//Kai  if ((to = fopen(outfile,"w")) == 0)
//Kai    error("cannot open XC::BJmatrix output file", outfile);
  write_standard(outfile, "this is a test XC::BJmatrix output"); 
}

//##############################################################################
XC::BJmatrix::BJmatrix(const BJmatrix &x): // copy-initializer
  nDarray("NO")     // with base class constructor cancellation
    {
      x.pc_nDarray_rep->n++;  // we're adding another reference.
//      x.reference_count(+1); // we're adding another reference.
      pc_nDarray_rep = x.pc_nDarray_rep;  // point to the new BJtensor_rep.
    }



//##############################################################################
XC::BJmatrix::BJmatrix(const XC::nDarray & x):
  nDarray( x ) { }



//#############################################################################
int XC::BJmatrix::rows( ) const  // rows in XC::BJmatrix
  {
    return this->dim(1);
  }



//#############################################################################
int XC::BJmatrix::cols( ) const       // cols in XC::BJmatrix
  {
    return this->dim(2);
  }



//#############################################################################
XC::BJmatrix &XC::BJmatrix::operator=( const XC::BJmatrix & rval)
  {
    if(&rval == this) // if assign an BJvector to itself
      return *this;
    
    assign(rval);
    return *this;
  }



//#############################################################################
void XC::BJmatrix::write_standard(const std::string &filename,const std::string &msg)
  {
    FILE *to;
    if((to = fopen(filename.c_str(),"w")) == nullptr)
      error("cannot open or create BJmatrix output file",filename);
    fprintf(to, "# %s: BJmatrix file written in \"standard\" format\n", filename.c_str());
    time_t clock;
    time(&clock);
    fprintf(to, "# %s", asctime(localtime(&clock)));
    fprintf(to, "# %s", msg.c_str());
    fprintf(to, "rows= %d columns= %d", rows(), cols());
    fprintf(to, "::\n");
    for ( int row=0; row<rows() ; row++ )
      {
        for( int col=0; col<cols() ; col++ )
          {
            fprintf(to, "%6.6g  ",mval(row,col));
            if(ferror(to))
              error("problem with XC::BJmatrix output file ", filename);
          }
        fprintf(to, "\n");
      }
}

//#############################################################################
// OLD good version
XC::BJmatrix XC::BJmatrix::operator*( BJmatrix & arg)
  {
    if( cols() != arg.rows())
      error("# rows of second mat must equal "
               "# cols of first for multiply#");
    BJmatrix result(rows(),arg.cols());
    for( int row=0 ; row<rows() ; row++ )
      for( int col=0 ; col<arg.cols() ; col++ )
        {
          double sum = 0;
          for( int i=0 ; i<cols() ; i++ )
            sum += mval(row,i)*arg.mval(i,col);
          result.mval(row,col) = sum;
        }
    return result; // Returning a local variable?
    // copy-initializer happens before the destructor,
    // so reference count is 2 when destructor is called,
    // thus destructor doesn't free the memory.
  }

//**sky**// this is COUPLING between ordinary XC::BJmatrix and SKYMATRIX
//**sky**// it will be useful in multiplying vector with skyBJmatrix
//**sky**BJmatrix XC::BJmatrix::operator*(const skyBJmatrix & arg)
//**sky**  {
//**sky**    if( cols() != arg.dimension_of_sky_M())
//**sky**      error("# dimension_of_sky_M must equal "
//**sky**               "# cols of first for multiply#");
//**sky**    BJmatrix result(rows(),arg.dimension_of_sky_M());
//**sky**    for( int row=0 ; row<rows() ; row++ )
//**sky**      for( int col=0 ; col<arg.dimension_of_sky_M() ; col++ )
//**sky**        {
//**sky**          double sum = 0;
//**sky**          for( int i=0 ; i<cols() ; i++ )
//**sky**            {
//**sky**//            double prvi = val(row,i);
//**sky**//            double drugi = arg.full_val(i+1,col+1);
//**sky**//            double temp = prvi*drugi; // in skyBJmatrix
//**sky**//            sum += temp;
//**sky**              sum += val(row,i)*arg.full_val(i+1,col+1);
//**sky**            }
//**sky**          result.val(row,col) = sum;                   // start from 1
//**sky**        }
//**sky**    return result; // Returning a local variable?
//**sky**    // copy-initializer happens before the destructor,
//**sky**    // so reference count is 2 when destructor is called,
//**sky**    // thus destructor doesn't free the memory.
//**sky**  }
//**sky**



//#############################################################################
XC::BJmatrix XC::BJmatrix::operator*( double arg)
  {
    BJmatrix result(rows(),cols());
    for ( int i=0 ; i<rows() ; i++ )
     for ( int j=0 ; j<cols() ; j++ )
       result.mval(i,j) = mval(i,j) * arg;
    return result;
  }

//....//#############################################################################
//....// I had to overload the operator* from XC::BJmatrix class
//....// because in the case of vectors s*sT and so on
//....vector XC::BJmatrix::operator*( vector & arg)
//....  {
//....    if( cols() != arg.rows())
//....      error("# rows of second mat must equal "
//....               "# cols of first for multiply in XC::BJmatrix::operator*( vector & arg) ");
//....    vector result(arg.rows());
//....      for( int row=0 ; row<arg.rows() ; row++ )
//....        {
//....          double sum = 0;
//....          for( int i=0 ; i<cols() ; i++ )
//....            sum += mval(row,i)*arg.cval(i);
//....          result.val(row) = sum;
//....        }
//....    return result; // Returning a local variable?
//....    // copy-initializer happens before the destructor,
//....    // so reference count is 2 when destructor is called,
//....    // thus destructor doesn't free the memory.
//....  }


//#############################################################################
XC::BJmatrix XC::BJmatrix::transpose()
  {
// This request is relaxed for vectors
//    if(rows() != cols())
//    error("BJmatrix must be square to transpose#\n");
    BJmatrix trans(cols(), rows());
    for ( int row=0 ; row<rows() ; row++ )
      for ( int col=0 ; col<cols() ; col++ )
        trans.mval(col,row) = mval(row,col);
    return trans;
  }

//#############################################################################
double XC::BJmatrix::mmin()
  {
    double temp;
    if ( rows()<=0 || cols()<=0)
      error("bad XC::BJmatrix size for min ()");
    double minimum = mval(0,0);
    for ( int row=0 ; row<rows() ; row++ )
      for ( int col=0 ; col<cols() ; col++ )
        if ( (temp=mval(row,col)) < minimum )
          minimum = temp;
    return minimum;
  }

//#############################################################################
double XC::BJmatrix::mmax()
  {
    double temp;
    if( rows()<=0 || cols()<=0)
      error("bad XC::BJmatrix size for max()");
    double maximum = mval(0,0);
    for ( int row=0 ; row<rows() ; row++ )
      for ( int col=0 ; col<cols() ; col++ )
        {
          if ( (temp=mval(row,col)) > maximum )
          maximum = temp;
        }
    return maximum;
  }


//#############################################################################
double XC::BJmatrix::mean()
  {
    int col = 0;
    int row = 0;
    double sum = 0;
    for ( row=0 ; row<rows() ; row++ )
      for ( col=0 ; col<cols() ; col++ )
        sum += fabs(mval(row,col));
    return sum/(row*col);
  }


//#############################################################################
double XC::BJmatrix::sum()
  {
    int col;
    double sum = 0;
    for ( int row=0 ; row<rows() ; row++ )
      for ( col=0 ; col<cols() ; col++ )
        sum += mval(row,col);
    return sum;
  }



//#############################################################################
double XC::BJmatrix::variance()
  {
    int col = 0;
    int row = 0;
    double s_squared = 0;
    double mn = mean();
    for ( row=0 ; row<rows() ; row++ )
      for ( col=0 ; col<cols() ; col++ )
        {
          double temp = mval(row,col) - mn;
          temp *= temp;
          s_squared += temp;
        }
    s_squared /= row*col-1; // number of elements minus one
    return s_squared;
  }




//#############################################################################
double XC::BJmatrix::determinant()
  {
    if( rows() != cols() )
      error("BJmatrix must be square for determinant()");
    BJmatrix indx(cols()); // create the "index vector"
//    BJmatrix B(cols()); // see XC::pp 38. in Numerical Recipes
    int d;
    // perform the decomposition once:
    BJmatrix decomp = lu_decompose(indx,d);
//    decomp.print("decomposed XC::BJmatrix");
    double determinant = d;
      for( int i=0 ; i < cols() ; i++ )
        determinant *= decomp.mval(i,i);
    return determinant;
  }


//#############################################################################
XC::BJmatrix XC::BJmatrix::inverse()
  {
    if( rows()!=cols() )
      error("BJmatrix must be square for inverse()");
    BJmatrix Y("I", rows()); // create an identity XC::BJmatrix
    BJmatrix indx(cols());   // create the "index vector"
    BJmatrix B(cols());      // see Press & Flannery
    int d;
    // perform the decomposition once:
    BJmatrix decomp = lu_decompose(indx,d);
    for(int col=0 ; col<cols() ; col++ )
      {
        B.copy_column(Y,col,0);
        decomp.lu_back_subst(indx, B);
        Y.copy_column(B, 0, col);
      }
    return Y.transpose();
  }




//~~~~//##############################################################################
//~~~~BJtensor XC::BJmatrix::BJmatrix2BJtensor_1()  // convert XC::BJmatrix of to 4th order XC::BJtensor
//~~~~  {                               // I_ijkl scheme
//~~~~
//~~~~// building up a XC::BJtensor back_conv
//~~~~// this is special case just for 4th order with def_dim_4 !!
//~~~~  BJtensor back_conv( 4, def_dim_4, 0.0);
//~~~~
//~~~~  int m41 = 0;
//~~~~  int m42 = 0;
//~~~~
//~~~~// filling back the XC::BJmatrix to XC::BJtensor
//~~~~  for ( int c44=1 ; c44<=back_conv.dim(4) ; c44++ )
//~~~~    for ( int c43=1 ; c43<=back_conv.dim(3) ; c43++ )
//~~~~      for ( int c42=1 ; c42<=back_conv.dim(2) ; c42++ )
//~~~~        for ( int c41=1 ; c41<=back_conv.dim(1) ; c41++ )
//~~~~          {
//~~~~            m41 = back_conv.dim(1)*(c41-1)+c43;
//~~~~            m42 = back_conv.dim(2)*(c42-1)+c44;
//~~~~
//~~~~            back_conv.val(c41,c42,c43,c44) = this->val(m41,m42);
//~~~~          }
//~~~~
//~~~~//    back_conv.print("t","back to XC::BJtensor back_conv");
//~~~~    return back_conv;
//~~~~  }

//##############################################################################
XC::BJtensor XC::BJmatrix::BJmatrix2BJtensor_2()  // convert XC::BJmatrix of to 4th order XC::BJtensor
  {                               // I_ikjl scheme

// building up a XC::BJtensor back_conv
// this is special case just for 4th order with def_dim_4 !!
  BJtensor back_conv( 4, def_dim_4, 0.0);
#ifdef SASA
  BJtensor Back_conv(4,def_dim_4_2,0.0);
 if(cols()==4) back_conv=Back_conv; // dimenzije su po 2
#endif
  int m41 = 0;
  int m42 = 0;

// filling back the XC::BJmatrix to XC::BJtensor
  for ( int c44=1 ; c44<=back_conv.dim(4) ; c44++ )
    for ( int c43=1 ; c43<=back_conv.dim(3) ; c43++ )
      for ( int c42=1 ; c42<=back_conv.dim(2) ; c42++ )
        for ( int c41=1 ; c41<=back_conv.dim(1) ; c41++ )
          {
            m41 = back_conv.dim(1)*(c41-1)+c42;
            m42 = back_conv.dim(3)*(c43-1)+c44;

            back_conv.val(c41,c42,c43,c44) = this->val(m41,m42);
          }

//    back_conv.print("t","back to XC::BJtensor back_conv");
    return back_conv;
  }

//##############################################################################
XC::BJtensor XC::BJmatrix::BJmatrix2BJtensor_22()  // convert XC::BJmatrix of to 2th order XC::BJtensor
  {                               // I_ikjl scheme

// building up a XC::BJtensor back_conv
// this is special case just for 4th order with def_dim_4 !!

//  BJtensor back_conv( 2, def_dim_2, 0.0);
  BJtensor back_conv( 2, def_dim_2, this->data());

//..  //  int m41 = 0;
//..  //  int m42 = 0;
//..  
//..  // filling back the XC::BJmatrix to XC::BJtensor
//..     for ( int c22=1 ; c22<=back_conv.dim(2) ; c22++ )
//..       for ( int c21=1 ; c21<=back_conv.dim(1) ; c21++ )
//..         {
//..  //         m41 = back_conv.dim(1)*(c41-1)+c42;
//..  //         m42 = back_conv.dim(3)*(c43-1)+c44;
//..       
//..           back_conv.val(c21,c22) = this->val(m41,m42);
//..         }
//..  
//..  //    back_conv.print("t","back to XC::BJtensor back_conv");

    return back_conv;
  }
//~~~~//##############################################################################
//~~~~BJtensor XC::BJmatrix::BJmatrix2BJtensor_3()  // convert XC::BJmatrix of to 4th order XC::BJtensor
//~~~~  {                               // I_ilkj scheme
//~~~~
//~~~~// building up a XC::BJtensor back_conv
//~~~~// this is special case just for 4th order with def_dim_4 !!
//~~~~  BJtensor back_conv( 4, def_dim_4, 0.0);
//~~~~
//~~~~  int m41 = 0;
//~~~~  int m42 = 0;
//~~~~
//~~~~// filling back the XC::BJmatrix to XC::BJtensor
//~~~~  for ( int c44=1 ; c44<=back_conv.dim(4) ; c44++ )
//~~~~    for ( int c43=1 ; c43<=back_conv.dim(3) ; c43++ )
//~~~~      for ( int c42=1 ; c42<=back_conv.dim(2) ; c42++ )
//~~~~        for ( int c41=1 ; c41<=back_conv.dim(1) ; c41++ )
//~~~~          {
//~~~~            m41 = back_conv.dim(1)*(c41-1)+c44;
//~~~~            m42 = back_conv.dim(2)*(c42-1)+c43;
//~~~~
//~~~~            back_conv.val(c41,c42,c43,c44) = this->val(m41,m42);
//~~~~          }
//~~~~
//~~~~//    back_conv.print("t","back to XC::BJtensor back_conv");
//~~~~    return back_conv;
//~~~~  }
//~~~~
//~~~~
//#############################################################################
//##BJmatrix XC::BJmatrix::compress_col(int col1, int col2, int to_col)
//##  {
//##    BJmatrix mat_temp( rows(), cols()-1);
//##//    mat_temp.print("mat_temp first appearance\n");
//##    for( int row=1 ; row<=rows() ; row++ )
//##      {
//##        // make temp vector to be put in the to_col :
//##// val STARTS from 0
//##        mat_temp(row,to_col) = val(row-1,col1-1)+val(row-1,col2-1);
//##      }
//##//    mat_temp.print("mat_temp second appearance\n");
//##
//##   int temp_row=1;
//##   int temp_col=1;
//##
//##    for( int col=1 ; col<=cols() ; col++ )
//##      {
//##        if( col!=col1 && col!=col2 )
//##          {
//##            temp_row=1;
//##            if( temp_col==to_col )
//##              {
//##                temp_col++;
//##              }
//##
//##          for( row=1 ; row<=rows() ; row++ )
//##            {
//##              mat_temp(temp_row,temp_col) = val(row-1,col-1);
//##              temp_row++;
//##            }
//##            temp_col++;
//##          }
//##      }
//##//    mat_temp.print("mat_temp third appearance\n");
//##
//##    return mat_temp;
//##  }
//##


//#############################################################################
//##BJmatrix XC::BJmatrix::compress_row(int row1, int row2, int to_row)
//##  {
//##    BJmatrix mat_temp( rows()-1, cols());
//##//    mat_temp.print("mat_temp first appearance\n");
//##    for( int col=1 ; col<=cols() ; col++ )
//##      {
//##        // make temp vector to be put in the to_row :
//##// val STARTS from 0
//##        mat_temp(to_row,col) = val(row1-1,col-1)+val(row2-1,col-1);
//##      }
//##//    mat_temp.print("mat_temp second appearance\n");
//##
//##   int temp_col=1;
//##   int temp_row=1;
//##
//##    for( int row=1 ; row<=rows() ; row++ )
//##      {
//##        if( row!=row1 && row!=row2 )
//##          {
//##            temp_col=1;
//##            if( temp_row==to_row )
//##              {
//##                temp_row++;
//##              }
//##
//##          for( col=1 ; col<=cols() ; col++ )
//##            {
//##              mat_temp(temp_row,temp_col) = val(row-1,col-1);
//##              temp_col++;
//##            }
//##            temp_row++;
//##          }
//##      }
//##//    mat_temp.print("mat_temp third appearance\n");
//##
//##    return mat_temp;
//##  }
//##
//##


///*************************************************************
//The private support functions for determinant & inverse.
//**************************************************************/
//

//#############################################################################
// copy the from_col of mm to the to  col of "this"
void XC::BJmatrix::copy_column( BJmatrix & mm, int from_col, int to_col)
  {
    if( rows()!=mm.rows() )
      error("number of rows must be equal for copy_column()");
    for( int row=0 ; row<rows() ; row++ )
      mval( row, to_col) = mm.mval(row,from_col);
  }


//#############################################################################
void XC::BJmatrix::switch_columns(int col1, int col2 )
  {
    int row = 0;
    BJmatrix temp( rows() );
    for( row=0 ; row<rows() ; row++ )
      // temporarily store col 1:
      temp.mval(row,0) = mval(row,col1);
    for( row=0 ; row<rows() ; row++ )
      mval(row,col1 ) = mval(row,col2); // move col2 to col1
    for( row=0 ; row<rows() ; row++ )
      mval(row,col2) = temp.mval(row,0); // move temp to col2
  }


//#############################################################################
// make an image of a XC::BJmatrix (used in L-U decomposition)
void XC::BJmatrix::deepcopy(BJmatrix & from, BJmatrix & to)
  {
    if( from.rows()!=to.rows() || from.cols()!=to.cols() )
      error("matrices must be equal dimensions for deepcopy()");
    for( int row=0 ; row<from.rows() ; row++ )
      for( int col=0 ; col<from.cols() ; col++ )
        to.mval(row,col) = from.mval(row,col);
}



//#############################################################################
// scale a XC::BJmatrix (used in L-U decomposition)
XC::BJmatrix XC::BJmatrix::scale( )
  {
//    ::fprintf(stderr,"BJmatrix XC::BJmatrix::scale( )\n"); 
    double double_eps = d_macheps();
    double temp;
    if( rows()<=0 || cols()<=0 )
      error("bad XC::BJmatrix size for scale()");
    if( rows()!=cols() )
      error("BJmatrix must be square for scale()");
    BJmatrix scale_vector(rows());
    for ( int col=0 ; col<cols() ; col++ )
      {
        double maximum = 0.0;
        for( int row=0 ; row<rows() ; row++ )
          if ( (temp=(double)fabs(mval(row,col))) > maximum )
            maximum = temp; // find max column magnitude in this row
	if( fabs(maximum) <= double_eps )
	  error("singular XC::BJmatrix in scale()");
	scale_vector.mval(col,0) = 1/maximum; // save the scaling
      }
    return scale_vector;
  }



//#############################################################################
XC::BJmatrix XC::BJmatrix::lu_decompose(BJmatrix & indx, int & d )
  {
///*
// Returns the L-U decomposition of a XC::BJmatrix. indx is an output
// vector that records the row permutation effected by the
// partial pivoting, d is output as +-1 depending on whether the
// number of row interchanges was even or odd, respectively.
// This routine is used in combination with lu_back_subst to
// solve linear equations or invert a XC::BJmatrix.
//*/
    if( rows()!=cols() )
      error("Matrix must be square to L-U decompose#\n");
    d = 1; // parity check
    int row=0, col=0, k=0, col_max=0; // counters
    double dum=0.0; // from the book - I don't know significance (Authors remark##)
    double sum=0.0;
    double maximum=0.0;
    BJmatrix lu_decomp(rows(),cols());
 // make a direct copy of the original XC::BJmatrix.
    deepcopy( *this, lu_decomp );
    BJmatrix scale_vector = lu_decomp.scale(); // scale the XC::BJmatrix

 // The loop over columns of Crout's method:
    for( row=0 ; row<rows() ; row++ )
      {
        if ( row>0 )        // eqn ±.3. 1 ± except for row=col:
          {
            for ( col=0 ; col <= row-1 ; col++ )
              {
                sum = lu_decomp.mval(row,col);
                if( col > 0 )
                  {
                    for( k = 0 ; k <= col-1 ; k++ )
                      sum -= lu_decomp.mval(row,k)*lu_decomp.mval(k, col);
                    lu_decomp.mval(row,col) = sum;
                  }
              }
          }

  // Initialize for the search for the largest pivot element
        maximum = 0;
  // i=j of eq 2.3.12 & i=j+ 1.. N of 2.3.13.
        for( col=row ; col <= cols()-1 ; col++ )
          {
            sum = lu_decomp.mval(row,col);
            if( row > 0 )
              {
                for( k=0 ; k <= row-1 ; k++ )
                  sum -=  lu_decomp.mval(k,col)*lu_decomp.mval(row,k);
                lu_decomp.mval(row,col) = sum;
              }
   // figure of merit for pivot:
            dum = scale_vector.mval(col,0)*fabs(sum);
            if ( dum >= maximum )   // is it better than the best so far?
              {
                col_max = col;
                maximum = dum;
              }
          }

  // Do we need to interchange rows?
        if( row != col_max)
          {
            lu_decomp.switch_columns(col_max,row); // Yes, do so ...
            d *= -1; //... and change the parity of d
  // also interchange the scale factor:
            dum = scale_vector.mval(col_max,0);
	    // scale_vector.mval(col_max,0) = scale_vector.mval(col,0); is wrong
	    // Bug! busted by Frank McKenna (fmckenna@ce.berkeley.edu)  

	    scale_vector.mval(col_max,0) = scale_vector.mval(row,0); // this is OK
            scale_vector.mval(row,0) = dum;
          }

        indx.mval(row,0) = col_max;
  // Now, finally, divide by the pivot element:
        if( row != rows() - 1 )
          {
            if( lu_decomp.mval(row,row) == 0 )
              lu_decomp.mval(row,row) = TINY;
       // If the pivot element is zero the BJmatrix is
       // singular (at least to the precision of the
       // algorithm).  For some applications on singular
       // matrices, it is desirable to substitute TINY for zero

            dum = 1/lu_decomp.mval(row,row);

            for( col=row+1 ; col <= cols()-1 ; col++ )
              lu_decomp.mval(row,col)*= dum;

          }

      }
    
    if( lu_decomp.mval(rows()-1, cols()-1 ) == 0 )
      lu_decomp.mval(rows()-1, cols()-1 ) = TINY;

    return lu_decomp;
  }




//#############################################################################
void XC::BJmatrix::lu_back_subst(BJmatrix & indx, BJmatrix & b)
  {
///*
// Solves the set of N linear equations A*X = B.  Here "this"
// is the LU-decomposition of the XC::BJmatrix A, determined by the
// routine lu_decompose(). Indx is input as the permutation
// vector returned  by lu_decompose().  B is input as the
// right-hand side vector B,  and returns with the solution
// vector X.  This routine takes into  account the possibility
// that B will begin with many zero elements,  so it is efficient
// for use in XC::BJmatrix inversion.  See XC::pp 36-37 in
// Press & Flannery.
//*/
    if( rows() != cols() )
      error ("non-square lu_decomp XC::BJmatrix in lu_back_subst()");
    if( rows() != b.rows() )
      error("wrong size B vector passed to lu_back_subst()");
    if( rows() != indx.rows() )
      error("wrong size indx vector passed to lu_back_subst()");
    int row, col, ll;
    int ii = 0;
    double sum;
    for( col=0 ; col < cols() ; col++ )
      {
        ll = (int)indx.mval(col,0);
        sum = b.mval(ll,0);
        b.mval(ll,0) = b.mval(col,0);
        if ( ii >= 0 )
          for( row = ii ; row <= col-1 ; row++ )
            sum -= mval(row,col)*b.mval(row,0);
        else if( sum != 0 )
        ii = col;
        b.mval(col,0) = sum;
      }
    for( col = cols() -1 ; col >= 0 ; col-- )
      {
        sum = b.mval(col,0);
        if ( col < cols() - 1 )
          for ( row = col + 1 ; row <= rows()-1 ; row++ )
        sum -= mval(row,col)*b.mval(row,0);
  // store a component of the soln vector X:
        b.mval(col,0) = sum/mval(col,col);
      }
  }



//#############################################################################
double & XC::BJmatrix::mval (int row, int col) // I am still keeping mval
  {                                      // operator for compatibility
     return( this->val(row+1,col+1) );   // with old XC::BJmatrix class members
  }                                      // and they start from 0 !



//#############################################################################
double *XC::BJmatrix::BJmatrixtoarray(int &num)
  {
    num=pc_nDarray_rep->total_numb;
    return pc_nDarray_rep->get_data_ptr();
  }







