#include "utility/utils/misc_utils/matem.h"
#include <iostream>
#include <cstdio>
#include "utility/utils/misc_utils/colormod.h"
#include <eigen3/Eigen/Dense>

double dicot_zero(double f(double), double a, double b,double &err)
  {
    double epsi= err;
    enum {inf=0,sup=1} modif;
    double c;
    double xx= 0.0;
    double x[2];
    x[0]= a;
    x[1]= b;
    if (f(a) > 0)
      c=1;
    else
      c=-1;
    err= x[sup]-x[inf];
    while ( err > epsi )
      {
	xx = 0.5*(x[inf]+x[sup]);
	if ( (c*f(xx)) > 0)
	  modif= inf;
	else
	  modif= sup;
	x[modif]= xx;
	err= x[sup]-x[inf];
      }
    return xx; 
  }

//! @brief Returns True if a is close in value to b. False otherwise
//! @param a: one of the values to be tested
//! @param b: the other value to be tested
//! @param rel_tol=1e-9: The relative tolerance -- the amount of error
//!                      allowed, relative to the absolute value of the
//!                      larger input value.
//! @param abs_tol=0.0: The minimum absolute tolerance level -- useful
//!                     for comparisons to zero.

// NOTES:

//     -inf, inf and NaN behave similarly to the IEEE 754 Standard. That
//     is, NaN is not close to anything, even itself. inf and -inf are
//     only close to themselves.

//     The function can be used with any type that supports comparison,
//     substratcion and multiplication, including Decimal, Fraction, and
//     Complex

//     Complex values are compared based on their absolute value.

//     See PEP-0485 for a detailed description
bool isclose(const double &a, const double &b, const double rel_tol, const double abs_tol)
  {
    if(a == b)  // short-circuit exact equality
      return true;

    if(rel_tol < 0.0 or abs_tol < 0.0)
      std::cerr << Color::red << __FUNCTION__
		<< "; error tolerances must be non-negative."
		<< Color::def << std::endl;

    if(std::isinf(std::abs(a)) or std::isinf(std::abs(b)))
        // This includes the case of two infinities of opposite sign, or
        // one infinity and one finite number. Two infinities of opposite sign
        // would otherwise have an infinite relative tolerance.
      return false;
    const double diff= std::abs(b - a);

    return (((diff <= std::abs(rel_tol * b)) or
	     (diff <= std::abs(rel_tol * a))) or
            (diff <= abs_tol));
  }

// -----------------------------------------------------------------------------
// compute the eigenvalue decomposition of a symmetric 2x2
// matrix in the form A[2][2], so that
//    A*v1   = v1   *lambda1
//    A*  v2 =   v2 *       lambda2
void eigen_decomposition_2x2(const double A[2][2], double V[2][2], double d[2])
  {
    Eigen::Matrix2d AA;
    AA << A[0][0], A[0][1], A[1][0], A[1][1];
    Eigen::SelfAdjointEigenSolver<Eigen::Matrix2d> eigensolver(AA);
    if(eigensolver.info() != Eigen::Success)
      {
	std::cerr << Color::red << __FUNCTION__
		  << "; the eigensolver crashed."
		  << Color::def << std::endl;
	abort();
      }
    else
      {
	auto eigenvalues= eigensolver.eigenvalues();
	auto eigenvectors= eigensolver.eigenvectors();
	const double lambda1= eigenvalues.col(0)[0];
	const double lambda2= eigenvalues.col(0)[1];
	const double v1x= eigenvectors(0,0);
	const double v1y= eigenvectors(1,0);
	const double v2x= eigenvectors(0,1);
	const double v2y= eigenvectors(1,1);
	
	d[0]= lambda1;
	V[0][0]= v1x; // first column of V
	V[1][0]= v1y;
	d[1]= lambda2;
	V[0][1]= v2x; // second column of V
	V[1][1]= v2y;
      }
  }

// -----------------------------------------------------------------------------
void eigen_decomposition_3x3(const double A[3][3], double V[3][3], double d[3])
  {
    Eigen::Matrix3d AA;
    AA << A[0][0], A[0][1], A[0][2],
          A[1][0], A[1][1], A[1][2],
          A[2][0], A[2][1], A[2][2];
    Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> eigensolver(AA);
    if(eigensolver.info() != Eigen::Success)
      abort();
    else
      {
	auto eigenvalues= eigensolver.eigenvalues();
	auto eigenvectors= eigensolver.eigenvectors();
	const double lambda1= eigenvalues.col(0)[0];
	const double lambda2= eigenvalues.col(0)[1];
	const double lambda3= eigenvalues.col(0)[2];
	
	const double v1x= eigenvectors(0,0);
	const double v1y= eigenvectors(1,0);
	const double v1z= eigenvectors(2,0);
	
	const double v2x= eigenvectors(0,1);
	const double v2y= eigenvectors(1,1);
	const double v2z= eigenvectors(2,1);
	
	const double v3x= eigenvectors(0,2);
	const double v3y= eigenvectors(1,2);
	const double v3z= eigenvectors(2,2);
	
	d[0]= lambda1;
	V[0][0]= v1x; // first column of V
	V[1][0]= v1y;
	V[2][0]= v1z;
	d[1]= lambda2;
	V[0][1]= v2x; // second column of V
	V[1][1]= v2y;
	V[2][1]= v2z;
	d[2]= lambda3;
	V[0][2]= v3x; // third column of V
	V[1][2]= v3y;
	V[2][2]= v3z;
      }
  }

// !@brief Covariance matrix.
std::vector< std::vector<double> > compute_covariance_matrix(const std::vector< std::vector<double> > &data)
  {
    std::vector< std::vector<double> > retval;
    const size_t data_nrows= data.size();
    if(data_nrows>0)
      {
	// Compute the covariance matrix of the point cloud.
	const size_t data_ncols= data[0].size();
	Eigen::MatrixXd eigen_data(data_nrows, data_ncols);
	// Populate eigen_data;
	for(size_t i= 0; i<data_nrows; i++)
	  {
	    const std::vector<double> &row= data[i];
	    for(size_t j= 0; j<data_ncols; j++)
	      { eigen_data(i,j)= row[j]; }
	  }
	// Calculate the mean of each column
	Eigen::RowVectorXd mean= eigen_data.colwise().mean();
	
	// Subtract the mean from each column
	Eigen::MatrixXd centered = eigen_data.rowwise() - mean;
    
	// Compute the covariance matrix
	Eigen::MatrixXd tmp= (centered.adjoint() * centered) / double(eigen_data.rows() - 1);
	// Populate retval;
	const size_t eigen_matrix_nrows= tmp.rows();
	const size_t eigen_matrix_ncols= tmp.cols();
	retval= std::vector<std::vector<double> >(eigen_matrix_nrows, std::vector<double>(eigen_matrix_ncols, 0.0));
	for(size_t i= 0; i<eigen_matrix_nrows; i++)
	  {
	    std::vector<double> &row= retval[i];
	    for(size_t j= 0; j<eigen_matrix_ncols; j++)
	      { row[j]= tmp(i,j); }
	  }
      }
    return retval;
  }

