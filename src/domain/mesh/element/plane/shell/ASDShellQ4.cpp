/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 1999, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.10 $
// $Date: 2020/05/18 22:51:21 $

// Original implementation: Massimo Petracca (ASDEA)
//
// A 4-node general shell element based on the AGQ formulation 
// for the in-plane behavior, 
// and the MITC4 formulation for the out-of-plane behavior.
// 
// It supports both linear and corotational kinematics. Warped geometries
// can be modelled since this element is not assumed flat.
//

#include "ASDShellQ4.h"
#include "domain/mesh/element/utils/coordTransformation/ASDShellQ4CorotationalTransformation.h"

#include "material/section/SectionForceDeformation.h"
#include "domain/domain/Domain.h"
#include "utility/utils/misc_utils/colormod.h"
#include "domain/mesh/element/utils/damping/Damping.h"
#include <cmath>

// anonymous namespace for utilities
namespace
  {
    // some typedefs
    typedef XC::ASDVector3<double> Vector3Type;

    // calculation options
    constexpr int OPT_NONE = 0;
    constexpr int OPT_UPDATE = (1 << 0);
    constexpr int OPT_LHS = (1 << 1);
    constexpr int OPT_RHS = (1 << 2);
    constexpr int OPT_LHS_IS_INITIAL = (1 << 3);

    // gauss quadrature data
    constexpr double GLOC = 0.577350269189626;
    constexpr std::array<double, 4> XI = { -GLOC, GLOC, GLOC, -GLOC };
    constexpr std::array<double, 4> ETA = { -GLOC, -GLOC, GLOC, GLOC };
    constexpr std::array<double, 4> WTS = { 1.0, 1.0, 1.0, 1.0 };

    // shape functions
    inline void shapeFunctions(double xi, double eta, XC::Vector &N)
      {
        N(0) = 0.25 * (1.0 - xi) * (1.0 - eta);
        N(1) = 0.25 * (1.0 + xi) * (1.0 - eta);
        N(2) = 0.25 * (1.0 + xi) * (1.0 + eta);
        N(3) = 0.25 * (1.0 - xi) * (1.0 + eta);
      }

    // shape function derivatives in iso-parametric space
    inline void shapeFunctionsNaturalDerivatives(double xi, double eta, XC::Matrix &dN)
    {
        dN(0, 0) = -(1.0 - eta) * 0.25;
        dN(1, 0) = (1.0 - eta) * 0.25;
        dN(2, 0) = (1.0 + eta) * 0.25;
        dN(3, 0) = -(1.0 + eta) * 0.25;

        dN(0, 1) = -(1.0 - xi) * 0.25;
        dN(1, 1) = -(1.0 + xi) * 0.25;
        dN(2, 1) = (1.0 + xi) * 0.25;
        dN(3, 1) = (1.0 - xi) * 0.25;
    }

    /** \brief JacobianOperator
     *
     * This class is a utility to compute at a given integration point,
     * the Jacobian, its inverse, its determinant
     * and the derivatives of the shape functions in the local
     * cartesian coordinate system.
     */
    struct JacobianOperator
      {
	// Jacobian matrix
	XC::Matrix J = XC::Matrix(2, 2);
	// Jacobian inverse
	XC::Matrix invJ = XC::Matrix(2, 2);
	// Determinant of the Jacobian matrix
	double detJ = 0.0;

	void calculate(const XC::ASDShellQ4LocalCoordinateSystem& CS, const XC::Matrix &dN)
          {
            // jacobian
            J(0, 0) = dN(0, 0) * CS.X1() + dN(1, 0) * CS.X2() + dN(2, 0) * CS.X3() + dN(3, 0) * CS.X4();
            J(1, 0) = dN(0, 0) * CS.Y1() + dN(1, 0) * CS.Y2() + dN(2, 0) * CS.Y3() + dN(3, 0) * CS.Y4();
            J(0, 1) = dN(0, 1) * CS.X1() + dN(1, 1) * CS.X2() + dN(2, 1) * CS.X3() + dN(3, 1) * CS.X4();
            J(1, 1) = dN(0, 1) * CS.Y1() + dN(1, 1) * CS.Y2() + dN(2, 1) * CS.Y3() + dN(3, 1) * CS.Y4();

            // determinant
            detJ = J(0, 0) * J(1, 1) - J(1, 0) * J(0, 1);
            double mult = 1.0 / detJ;

            // inv(jacobian)
            invJ(0, 0) = J(1, 1) * mult;
            invJ(1, 1) = J(0, 0) * mult;
            invJ(0, 1) = -J(0, 1) * mult;
            invJ(1, 0) = -J(1, 0) * mult;
	  }
    
    };

    /** \brief MITC4Params
     *
     * This class performs some operations and stores some data to compute
     * the transverse shear contribution of the stiffness matrix using the
     * M.I.T.C. formulation.
     *
     */
    struct MITC4Params
      {
        double Ax = 0.0;
        double Ay = 0.0;
        double Bx = 0.0;
        double By = 0.0;
        double Cx = 0.0;
        double Cy = 0.0;
	XC::Matrix transformation = XC::Matrix(2, 2);
	XC::Matrix shearStrains = XC::Matrix(4, 24);

        void compute(const XC::ASDShellQ4LocalCoordinateSystem &LCS)
          {
            const double x21 = LCS.X2() - LCS.X1();
            const double y21 = LCS.Y2() - LCS.Y1();
            const double x34 = LCS.X3() - LCS.X4();
            const double y34 = LCS.Y3() - LCS.Y4();
            const double x41 = LCS.X4() - LCS.X1();
            const double y41 = LCS.Y4() - LCS.Y1();
            const double x32 = LCS.X3() - LCS.X2();
            const double y32 = LCS.Y3() - LCS.Y2();

            Ax = -LCS.X1() + LCS.X2() + LCS.X3() - LCS.X4();
            Bx = LCS.X1() - LCS.X2() + LCS.X3() - LCS.X4();
            Cx = -LCS.X1() - LCS.X2() + LCS.X3() + LCS.X4();
            Ay = -LCS.Y1() + LCS.Y2() + LCS.Y3() - LCS.Y4();
            By = LCS.Y1() - LCS.Y2() + LCS.Y3() - LCS.Y4();
            Cy = -LCS.Y1() - LCS.Y2() + LCS.Y3() + LCS.Y4();

            const double Alpha = std::atan(Ay / Ax);
            const double Beta = M_PI * 0.5 - std::atan(Cx / Cy);

            transformation(0, 0) = std::sin(Beta);
            transformation(0, 1) = -std::sin(Alpha);
            transformation(1, 0) = -std::cos(Beta);
            transformation(1, 1) = std::cos(Alpha);

            shearStrains.Zero();

            shearStrains(0, 2) = -0.5;
            shearStrains(0, 3) = -y41 * 0.25;
            shearStrains(0, 4) = x41 * 0.25;

            shearStrains(0, 20) = 0.5;
            shearStrains(0, 21) = -y41 * 0.25;
            shearStrains(0, 22) = x41 * 0.25;

            shearStrains(1, 2) = -0.5;
            shearStrains(1, 3) = -y21 * 0.25;
            shearStrains(1, 4) = x21 * 0.25;

            shearStrains(1, 8) = 0.5;
            shearStrains(1, 9) = -y21 * 0.25;
            shearStrains(1, 10) = x21 * 0.25;

            shearStrains(2, 8) = -0.5;
            shearStrains(2, 9) = -y32 * 0.25;
            shearStrains(2, 10) = x32 * 0.25;

            shearStrains(2, 14) = 0.5;
            shearStrains(2, 15) = -y32 * 0.25;
            shearStrains(2, 16) = x32 * 0.25;

            shearStrains(3, 14) = 0.5;
            shearStrains(3, 15) = -y34 * 0.25;
            shearStrains(3, 16) = x34 * 0.25;

            shearStrains(3, 20) = -0.5;
            shearStrains(3, 21) = -y34 * 0.25;
            shearStrains(3, 22) = x34 * 0.25;
        }

    };

    /** \brief AGQIParams
     *
     * This class performs some operations and stores some data to compute
     * the AGQI enhancement of the membrane part
     *
     */
    struct AGQIParams
      {
        std::array<double, 4> X = { {0.0, 0.0, 0.0, 0.0} };
        std::array<double, 4> Y = { {0.0, 0.0, 0.0, 0.0} };
        std::array<double, 4> b = { {0.0, 0.0, 0.0, 0.0} };
        std::array<double, 4> c = { {0.0, 0.0, 0.0, 0.0} };
        double A1 = 0.0;
        double A2 = 0.0;
        double A3 = 0.0;
        double A = 0.0;
        std::array<double, 4> g = { {0.0, 0.0, 0.0, 0.0} };

	void compute(const XC::ASDShellQ4LocalCoordinateSystem& LCS)
          {
            // extract the x and y coordinates
            // and compute bi = yj-yk, and ci = xk-xj
            // Eq 3
            for (int i = 0; i < 4; i++)
	      {
                X[i] = LCS.X(i);
                Y[i] = LCS.Y(i);
	      }
            for (int i = 0; i < 4; i++)
	      {
                int j = i + 1; if (j > 3) j = 0;
                int k = j + 1; if (k > 3) k = 0;
                b[i] = Y[j] - Y[k];
                c[i] = X[k] - X[j];
	      }

            // areas of sub-triangles
            A1 = 0.5 * (X[1] * Y[3] + X[0] * Y[1] + X[3] * Y[0] - X[1] * Y[0] - X[3] * Y[1] - X[0] * Y[3]);
            A2 = 0.5 * (X[1] * Y[2] + X[0] * Y[1] + X[2] * Y[0] - X[1] * Y[0] - X[2] * Y[1] - X[0] * Y[2]);
            A3 = 0.5 * (X[2] * Y[3] + X[1] * Y[2] + X[3] * Y[1] - X[2] * Y[1] - X[3] * Y[2] - X[1] * Y[3]);
            A = A1 + A3;
            // characteristic parameters of a quadrilateral
            // Eq 4
            g[0] = A1 / A; g[1] = A2 / A; g[2] = 1.0 - g[0]; g[3] = 1.0 - g[1];
	  }

    };

    /** \brief ASDShellQ4Globals
     *
     * This singleton class stores some data for the shell calculations that
     * can be statically instanciated to avoid useless re-allocations
     *
     */
    class ASDShellQ4Globals
    {
    private:
        ASDShellQ4Globals() = default;

    public:

        JacobianOperator jac; // Jacobian
        MITC4Params mitc; // MITC4 parameters
        AGQIParams agq; // GQ12 parameters

        XC::Vector UG = XC::Vector(24); // global displacements
        XC::Vector UL = XC::Vector(24); // local displacements

        XC::Matrix B = XC::Matrix(8, 24); // strain-displacement matrix
        XC::Matrix B1 = XC::Matrix(8, 24); // strain-displacement matrix with inverted bending terms
        XC::Matrix B1TD = XC::Matrix(24, 8); // holds the B1^T*D terms
        XC::Vector Bd = XC::Vector(24); // strain-displacement matrix for drilling
        XC::Vector Bd0 = XC::Vector(24); // strain-displacement matrix for drilling (reduced integration)
        XC::Vector N = XC::Vector(4); // shape functions
        XC::Matrix dN = XC::Matrix(4, 2); // shape functions derivatives in isoparametric space
        XC::Matrix dNdX = XC::Matrix(4, 2); // shape functions derivatives in cartesian space
        XC::Vector E = XC::Vector(8); // strain vector
        XC::Vector S = XC::Vector(8); // stress vector
        XC::Vector Elocal = XC::Vector(8); // strain vector in local element coordinate system
        XC::Matrix Re = XC::Matrix(8, 8); // rotation matrix for strains
        XC::Matrix Rs = XC::Matrix(8, 8); // rotation matrix for stresses
        XC::Matrix RsT = XC::Matrix(8, 8); // transpose of above
        XC::Matrix Dsection = XC::Matrix(8, 8);
        XC::Matrix D = XC::Matrix(8, 8); // section tangent
        XC::Matrix DRsT = XC::Matrix(8, 8); // holds the product D * Rs^T

        XC::Matrix BQ = XC::Matrix(8, 4); // B matrix for internal DOFs
        XC::Matrix BQ_mean = XC::Matrix(8, 4); // Average B matrix for internal DOFs
        XC::Matrix BQTD = XC::Matrix(4, 8); // holds the BQ^T*D terms
        XC::Matrix DBQ = XC::Matrix(8, 4); // holds the D*BQ^T terms

        XC::Matrix LHS = XC::Matrix(24, 24); // LHS matrix (tangent stiffness)
        XC::Matrix LHS_initial = XC::Matrix(24, 24); // LHS matrix (initial stiffness)
        XC::Matrix LHS_mass = XC::Matrix(24, 24); // LHS matrix (mass matrix)
        XC::Vector RHS = XC::Vector(24); // RHS vector (residual vector)
        XC::Vector RHS_winertia = XC::Vector(24); // RHS vector (residual vector with inertia terms)

    public:
        static ASDShellQ4Globals& instance() {
            static ASDShellQ4Globals _instance;
            return _instance;
        }
    };

    // computes only the drilling B matrix
    void computeBdrilling( const XC::ASDShellQ4LocalCoordinateSystem& LCS,
			   double xi, double eta,
			   const JacobianOperator& Jac,
			   const AGQIParams& agq,
			   const XC::Vector &N,
			   const XC::Matrix &dN,
			   XC::Vector &Bd,
			   bool use_eas)
    {
        // cartesian derivatives of standard shape function
        auto& dNdX = ASDShellQ4Globals::instance().dNdX;
        dNdX.addMatrixProduct(0.0, dN, Jac.invJ, 1.0);

        // initialize
        Bd.Zero();

        // AGQI proc ***********************************************************************************************
        if (use_eas)
	  {

            // area coordinates of the gauss point (Eq 7)
            std::array<double, 4> L;
            L[0] = 0.25 * (1.0 - xi) * (agq.g[1] * (1.0 - eta) + agq.g[2] * (1.0 + eta));
            L[1] = 0.25 * (1.0 - eta) * (agq.g[3] * (1.0 - xi) + agq.g[2] * (1.0 + xi));
            L[2] = 0.25 * (1.0 + xi) * (agq.g[0] * (1.0 - eta) + agq.g[3] * (1.0 + eta));
            L[3] = 0.25 * (1.0 + eta) * (agq.g[0] * (1.0 - xi) + agq.g[1] * (1.0 + xi));

            // computed modified shape function gradients for the strain matrix for external dofs
            // using the area coordinate method as written in the reference paper of the AGQI element
            static constexpr std::array<double, 4> NXI = { -1.0, 1.0, 1.0, -1.0 };
            static constexpr std::array<double, 4> NETA = { -1.0, -1.0, 1.0, 1.0 };
            for (int i = 0; i < 4; i++)
	      {
                int j = i + 1; if (j > 3) j = 0;
                int k = j + 1; if (k > 3) k = 0;
                double SX = 0.0;
                double SY = 0.0;
                for (int ii = 0; ii < 4; ii++)
		  {
                    int jj = ii + 1; if (jj > 3) jj = 0;
                    int kk = jj + 1; if (kk > 3) kk = 0;
                    int mm = kk + 1; if (mm > 3) mm = 0;
                    SX = SX + agq.b[ii] * NXI[ii] * NETA[ii] * (3.0 * (L[jj] - L[mm]) + (agq.g[jj] - agq.g[kk]));
                    SY = SY + agq.c[ii] * NXI[ii] * NETA[ii] * (3.0 * (L[jj] - L[mm]) + (agq.g[jj] - agq.g[kk]));
		  }
                dNdX(i, 0) = (agq.b[i] + agq.b[j]) / agq.A / 2.0 +
                    NXI[i] * NETA[i] * agq.g[k] * SX / 2.0 / agq.A / (1.0 + agq.g[0] * agq.g[2] + agq.g[1] * agq.g[3]);
                dNdX(i, 1) = (agq.c[i] + agq.c[j]) / agq.A / 2.0 +
                    NXI[i] * NETA[i] * agq.g[k] * SY / 2.0 / agq.A / (1.0 + agq.g[0] * agq.g[2] + agq.g[1] * agq.g[3]);
	      }
	  }

        // We use the drilling penalty as defined by hughes and brezzi,
        // where we link the independent rotation to the skew symmetric part
	// of the in-plane displacement field.

        Bd(0) = -0.5 * dNdX(0, 1);
        Bd(1) = 0.5 * dNdX(0, 0);
        Bd(5) = -N(0);

        Bd(6) = -0.5 * dNdX(1, 1);
        Bd(7) = 0.5 * dNdX(1, 0);
        Bd(11) = -N(1);

        Bd(12) = -0.5 * dNdX(2, 1);
        Bd(13) = 0.5 * dNdX(2, 0);
        Bd(17) = -N(2);

        Bd(18) = -0.5 * dNdX(3, 1);
        Bd(19) = 0.5 * dNdX(3, 0);
        Bd(23) = -N(3);
    }

    // computes the complete B matrix
    void computeBMatrix(const XC::ASDShellQ4LocalCoordinateSystem& LCS,
			double xi, double eta,
			const JacobianOperator& Jac, 
			const AGQIParams& agq,
			const MITC4Params& mitc,
			const XC::Vector &N,
			const XC::Matrix &dN,
			const XC::Matrix &BQ_mean,
			XC::Matrix &B,
			XC::Matrix &BQ,
			XC::Vector &Bd,
			bool use_eas)
    {
        // cartesian derivatives of standard shape function
        auto& dNdX = ASDShellQ4Globals::instance().dNdX;
        dNdX.addMatrixProduct(0.0, dN, Jac.invJ, 1.0);

        // initialize
        B.Zero();
        BQ.Zero();
        Bd.Zero();

        // AGQI proc ***********************************************************************************************
        if(use_eas)
	  {
            // area coordinates of the gauss point (Eq 7)
            std::array<double, 4> L;
            L[0] = 0.25 * (1.0 - xi) * (agq.g[1] * (1.0 - eta) + agq.g[2] * (1.0 + eta));
            L[1] = 0.25 * (1.0 - eta) * (agq.g[3] * (1.0 - xi) + agq.g[2] * (1.0 + xi));
            L[2] = 0.25 * (1.0 + xi) * (agq.g[0] * (1.0 - eta) + agq.g[3] * (1.0 + eta));
            L[3] = 0.25 * (1.0 + eta) * (agq.g[0] * (1.0 - xi) + agq.g[1] * (1.0 + xi));

            // computed modified shape function gradients for the strain matrix for external dofs
            // using the area coordinate method as written in the reference paper of the AGQI element
            static constexpr std::array<double, 4> NXI = { -1.0, 1.0, 1.0, -1.0 };
            static constexpr std::array<double, 4> NETA = { -1.0, -1.0, 1.0, 1.0 };
            for (int i = 0; i < 4; i++)
	      {
                int j = i + 1; if (j > 3) j = 0;
                int k = j + 1; if (k > 3) k = 0;
                double SX = 0.0;
                double SY = 0.0;
                for (int ii = 0; ii < 4; ii++)
		  {
                    int jj = ii + 1; if (jj > 3) jj = 0;
                    int kk = jj + 1; if (kk > 3) kk = 0;
                    int mm = kk + 1; if (mm > 3) mm = 0;
                    SX = SX + agq.b[ii] * NXI[ii] * NETA[ii] * (3.0 * (L[jj] - L[mm]) + (agq.g[jj] - agq.g[kk]));
                    SY = SY + agq.c[ii] * NXI[ii] * NETA[ii] * (3.0 * (L[jj] - L[mm]) + (agq.g[jj] - agq.g[kk]));
		  }
                dNdX(i, 0) = (agq.b[i] + agq.b[j]) / agq.A / 2.0 +
                    NXI[i] * NETA[i] * agq.g[k] * SX / 2.0 / agq.A / (1.0 + agq.g[0] * agq.g[2] + agq.g[1] * agq.g[3]);
                dNdX(i, 1) = (agq.c[i] + agq.c[j]) / agq.A / 2.0 +
                    NXI[i] * NETA[i] * agq.g[k] * SY / 2.0 / agq.A / (1.0 + agq.g[0] * agq.g[2] + agq.g[1] * agq.g[3]);
	      }

            // compute the BQ matrix for the internal DOFs
            BQ.Zero();
            for (int i = 0; i < 2; i++)
            {
                unsigned int j = i + 1; if (j > 3) j = 0;
                unsigned int k = j + 1; if (k > 3) k = 0;
                double NQX = (agq.b[i] * L[k] + agq.b[k] * L[i]) / agq.A / 2.0;
                double NQY = (agq.c[i] * L[k] + agq.c[k] * L[i]) / agq.A / 2.0;
                unsigned int index1 = i * 2;
                unsigned int index2 = index1 + 1;
                BQ(0, index1) += NQX;
                BQ(1, index2) += NQY;
                BQ(2, index1) += NQY;
                BQ(2, index2) += NQX;
            }
            BQ.addMatrix(1.0, BQ_mean, -1.0);
	  }

        // membrane ************************************************************************************************
        // this is the memebrane part of the compatible standard in-plane displacement field

        B(0, 0) = dNdX(0, 0);   B(0, 6) = dNdX(1, 0);   B(0, 12) = dNdX(2, 0);   B(0, 18) = dNdX(3, 0);
        B(1, 1) = dNdX(0, 1);   B(1, 7) = dNdX(1, 1);   B(1, 13) = dNdX(2, 1);   B(1, 19) = dNdX(3, 1);
        B(2, 0) = dNdX(0, 1);   B(2, 6) = dNdX(1, 1);   B(2, 12) = dNdX(2, 1);   B(2, 18) = dNdX(3, 1);
        B(2, 1) = dNdX(0, 0);   B(2, 7) = dNdX(1, 0);   B(2, 13) = dNdX(2, 0);   B(2, 19) = dNdX(3, 0);

        // We use the drilling penalty as defined by hughes and brezzi,
        // where we link the independent rotation to the skew symmetric part of the in-plane displacement field.

        Bd(0) = -0.5 * dNdX(0, 1);
        Bd(1) = 0.5 * dNdX(0, 0);
        Bd(5) = -N(0);
        
        Bd(6) = -0.5 * dNdX(1, 1);
        Bd(7) = 0.5 * dNdX(1, 0);
        Bd(11) = -N(1);
        
        Bd(12) = -0.5 * dNdX(2, 1);
        Bd(13) = 0.5 * dNdX(2, 0);
        Bd(17) = -N(2);
        
        Bd(18) = -0.5 * dNdX(3, 1);
        Bd(19) = 0.5 * dNdX(3, 0);
        Bd(23) = -N(3);

        // bending *************************************************************************************************

        B(3, 4) = -dNdX(0, 0);   B(3, 10) = -dNdX(1, 0);   B(3, 16) = -dNdX(2, 0);   B(3, 22) = -dNdX(3, 0);
        B(4, 3) =  dNdX(0, 1);   B(4, 9)  =  dNdX(1, 1);   B(4, 15) =  dNdX(2, 1);   B(4, 21) =  dNdX(3, 1);
        B(5, 3) =  dNdX(0, 0);   B(5, 9)  =  dNdX(1, 0);   B(5, 15) =  dNdX(2, 0);   B(5, 21) =  dNdX(3, 0);
        B(5, 4) = -dNdX(0, 1);   B(5, 10) = -dNdX(1, 1);   B(5, 16) = -dNdX(2, 1);   B(5, 22) = -dNdX(3, 1);

        // shear ***************************************************************************************************

        // MITC modified shape functions
        static XC::Matrix MITCShapeFunctions(2, 4);
        MITCShapeFunctions.Zero();
        MITCShapeFunctions(1, 0) = 1.0 - xi;
        MITCShapeFunctions(0, 1) = 1.0 - eta;
        MITCShapeFunctions(1, 2) = 1.0 + xi;
        MITCShapeFunctions(0, 3) = 1.0 + eta;

        // strain displacement matrix in natural coordinate system.
        // interpolate the shear strains given in MITC4Params
        // using the modified shape function
        static XC::Matrix BN(2, 24);
        BN.addMatrixProduct(0.0, MITCShapeFunctions, mitc.shearStrains, 1.0);

        // modify the shear strain intensity in the tying points
        // to match the values that would be obtained using standard
        // interpolations
        double Temp1, Temp2, Temp3;
        Temp1 = mitc.Cx + xi * mitc.Bx;
        Temp3 = mitc.Cy + xi * mitc.By;
        Temp1 = Temp1 * Temp1 + Temp3 * Temp3;
        Temp1 = std::sqrt(Temp1) / (8.0 * Jac.detJ);
        Temp2 = mitc.Ax + eta * mitc.Bx;
        Temp3 = mitc.Ay + eta * mitc.By;
        Temp2 = Temp2 * Temp2 + Temp3 * Temp3;
        Temp2 = std::sqrt(Temp2) / (8.0 * Jac.detJ);
        for (int i = 0; i < 24; i++)
	  {
            BN(0, i) *= Temp1;
            BN(1, i) *= Temp2;
	  }
        
        // transform the strain-displacement matrix from natural
        // to local coordinate system taking into account the element distorsion
        static XC::Matrix TBN(2, 24);
        TBN.addMatrixProduct(0.0, mitc.transformation, BN, 1.0);
        for (int i = 0; i < 2; i++)
            for (int j = 0; j < 24; j++)
                B(i + 6, j) = TBN(i, j);
    }

    // invert bending terms
    void invertBBendingTerms(const XC::Matrix &B, XC::Matrix &B1) 
      {
        // due to the convention in the shell sections, we need to change the sign of the bending terms
        // for the B^T case.
        B1.addMatrix(0.0, B, 1.0);
        for (int i = 3; i < 6; i++)
	  {
            for (int j = 0; j < 4; j++)
	      {
                B1(i, j * 6 + 3) *= -1.0;
                B1(i, j * 6 + 4) *= -1.0;
	      }
	  }
      }

    // computes the transformation matrix for generalized strains
    inline void getRotationMatrixForGeneralizedStrains(double radians, XC::Matrix &T)
      {
        const double c= std::cos(radians);
        const double s= std::sin(radians);

        T.Zero();

        T(0, 0) = c * c;			T(0, 1) = s * s;			T(0, 2) = -s * c;
        T(1, 0) = s * s;			T(1, 1) = c * c;			T(1, 2) = s * c;
        T(2, 0) = 2.0 * s * c;		T(2, 1) = -2.0 * s * c;		T(2, 2) = c * c - s * s;

        T(3, 3) = c * c;			T(3, 4) = s * s;			T(3, 5) = -s * c;
        T(4, 3) = s * s;			T(4, 4) = c * c;			T(4, 5) = s * c;
        T(5, 3) = 2.0 * s * c;		T(5, 4) = -2.0 * s * c;		T(5, 5) = c * c - s * s;

        T(6, 6) = c;		T(6, 7) = -s;
        T(7, 6) = s;		T(7, 7) = c;
      }

    // computes the transformation matrix for generalized stresses
    inline void getRotationMatrixForGeneralizedStresses(double radians, XC::Matrix &T)
      {
        const double c = std::cos(radians);
        const double s = std::sin(radians);

        T.Zero();

        T(0, 0) = c * c;		T(0, 1) = s * s;		T(0, 2) = -2.0 * s * c;
        T(1, 0) = s * s;		T(1, 1) = c * c;		T(1, 2) = 2.0 * s * c;
        T(2, 0) = s * c;		T(2, 1) = -s * c;		T(2, 2) = c * c - s * s;

        T(3, 3) = c * c;		T(3, 4) = s * s;		T(3, 5) = -2.0 * s * c;
        T(4, 3) = s * s;		T(4, 4) = c * c;		T(4, 5) = 2.0 * s * c;
        T(5, 3) = s * c;		T(5, 4) = -s * c;		T(5, 5) = c * c - s * s;

        T(6, 6)= c;		T(6, 7) = -s;
        T(7, 6)= s;		T(7, 7) = c;
      }

}

void XC::ASDShellQ4::free_crd_transf(void)
  {
    if(m_transformation)
      {
        delete m_transformation;
        m_transformation= nullptr;
      }
  }

void XC::ASDShellQ4::free_eas(void)
  {
    if(m_eas)
      {
        delete m_eas;
        m_eas= nullptr;
      }
  }

void XC::ASDShellQ4::free_nldrill(void)
  {
    if(m_nldrill)
      {
        delete m_nldrill;
        m_nldrill= nullptr;
      }
  }

//! @brief Releases memory.
void XC::ASDShellQ4::free_mem(void)
  {
    free_crd_transf();
    free_eas();
    free_nldrill();
  }

//! @brief Allocates memory.
void XC::ASDShellQ4::alloc(bool corotational)
  {
    free_crd_transf();
    if(corotational)
      m_transformation= new ASDShellQ4CorotationalTransformation();
    else
      m_transformation= new ASDShellQ4Transformation();
  }

//! @brief Allocates memory.
void XC::ASDShellQ4::alloc(const ASDShellQ4Transformation *trf)
  {
    free_crd_transf();
    if(trf)
      m_transformation= trf->getCopy();
  }
 
//! @brief Allocates memory.
void XC::ASDShellQ4::alloc_eas(void)
  {
    free_eas();
    m_eas= new EASData();
  }

//! @brief Copy EAS data.
void XC::ASDShellQ4::copy_eas(const EASData &other)
  {
    alloc_eas();
    this->m_eas->Q= other.Q;
    this->m_eas->Q_converged= other.Q_converged;
    this->m_eas->U= other.U;
    this->m_eas->U_converged= other.U_converged;
    this->m_eas->Q_residual= other.Q_residual;
    this->m_eas->KQQ_inv= other.KQQ_inv;
    this->m_eas->KQU= other.KQU;
    this->m_eas->KUQ= other.KUQ;
  }

//! @brief Allocates NL drilling data.
void XC::ASDShellQ4::alloc_nldrill(void)
  {
    free_nldrill();
    m_nldrill= new NLDrillingData();
  }

//! @brief Copy EAS data.
void XC::ASDShellQ4::copy_nldrill(const NLDrillingData &other)
  {
    alloc_nldrill();
    this->m_nldrill->strain_comm= other.strain_comm;
    this->m_nldrill->stress_comm= other.stress_comm;
    this->m_nldrill->damage= other.damage;
    this->m_nldrill->damage_comm= other.damage_comm;
  }

//! @brief Constructor.
XC::ASDShellQ4::ASDShellQ4(int tag, bool corotational)
  : QuadBase4N<SectionFDPhysicalProperties>(tag, ELE_TAG_ASDShellQ4, SectionFDPhysicalProperties(4)), m_transformation(nullptr), m_damping(4, nullptr)
  {
    alloc(corotational);
  }
//! @brief Constructor.
XC::ASDShellQ4::ASDShellQ4(int tag,const SectionForceDeformation *ptr_mat)
  : QuadBase4N<SectionFDPhysicalProperties>(tag, ELE_TAG_ASDShellQ4, SectionFDPhysicalProperties(4,ptr_mat)), m_transformation(nullptr), m_damping(4, nullptr)
  {
    alloc(true);
  }

XC::ASDShellQ4::ASDShellQ4(int tag,
			   int node1,
			   int node2,
			   int node3,
			   int node4,
			   SectionForceDeformation* section,
			   const Vector& local_x,
			   bool corotational,
			   bool use_eas,
			   DrillingDOFMode drill_mode,
			   double drilling_stab,
			   Damping *theDamping)
  : QuadBase4N<SectionFDPhysicalProperties>(tag, ELE_TAG_ASDShellQ4, node1, node2, node3, node4, SectionFDPhysicalProperties(4,section)),
    m_transformation(nullptr), m_local_x(local_x), m_damping(4, theDamping),
    m_drill_mode(drill_mode),
    m_drill_stab(drilling_stab)
  {
    alloc(corotational);
    m_local_x.Normalize();
    
    // allocate eas data
    if(use_eas)
      alloc_eas();
    
    // allocate non-linear drilling data
    if(m_drill_mode == DrillingDOF_NonLinear)
      { alloc_nldrill(); }
  }


XC::ASDShellQ4::~ASDShellQ4(void)
  { free_mem(); }

//! @brief Virtual constructor.
XC::Element* XC::ASDShellQ4::getCopy(void) const
  { return new ASDShellQ4(*this); }

//! @brief Copy constructor.
XC::ASDShellQ4::ASDShellQ4(const ASDShellQ4 &other)
  : QuadBase4N<SectionFDPhysicalProperties>(other),
    m_transformation(nullptr),
    m_load(other.m_load), m_drill_strain(other.m_drill_strain),
    m_drill_stiffness(other.m_drill_stiffness),
    m_local_x(other.m_local_x),
    m_angle(other.m_angle),
    m_damping(other.m_damping)
  {
    alloc(other.m_transformation);
    if(other.m_eas)
      copy_eas(*other.m_eas);
  }

//! @brief Assignment operator.
XC::ASDShellQ4 &XC::ASDShellQ4::operator=(const ASDShellQ4 &other)
  {
    QuadBase4N<SectionFDPhysicalProperties>::operator=(other);
    alloc(other.m_transformation);
    m_load= other.m_load;
    m_drill_strain= other.m_drill_strain;
    m_drill_stiffness= other.m_drill_stiffness;
    m_local_x= other.m_local_x;
    m_angle= other.m_angle;
    if(other.m_eas)
      copy_eas(*other.m_eas);
    m_damping= other.m_damping;
    return *this;
  }

//! @brief Reinitialize values that depend on the nodal coordinates (for
//! example after a "manual" change in the nodal coordinates, to impose
//! an imperfect shape or a precamber.
int XC::ASDShellQ4::resetNodalCoordinates(void)
  {
    // compute section orientation angle
    ASDShellQ4LocalCoordinateSystem reference_cs = m_transformation->createReferenceCoordinateSystem();
    Vector3Type e1_local = reference_cs.Vx();
    Vector3Type e1;
    if(!m_local_x.isEmpty())
      {
	// user-defined (already normalized in c-tor)
	e1 = Vector3Type(m_local_x);
	// make sure it's on the reference x-y plane
	const Vector3Type e3 = reference_cs.Vz();
	Vector3Type e2 = e3.cross(e1);
	const double e2_norm = e2.normalize();
	if (e2_norm == 0.0)
	  {
	    std::cerr << getClassName() << "::" << __FUNCTION__
	              << ". Error: The provided local X axis "
	              << "cannot be aligned with the shell normal vector."
	              << std::endl;
	    exit(-1);
	  }
	e1 = e2.cross(e3);
	e1.normalize();
      }
    else
      {
	// default one
	const Vector3Type P1(m_transformation->getNodes()[0]->getCrds());
	const Vector3Type P2(m_transformation->getNodes()[1]->getCrds());
	const Vector3Type P3(m_transformation->getNodes()[2]->getCrds());
	const Vector3Type P4(m_transformation->getNodes()[3]->getCrds());
	e1 = (P2 + P3) / 2.0 - (P1 + P4) / 2.0;
	e1.normalize();
      }
    m_angle = std::acos(std::max(-1.0, std::min(1.0, e1.dot(e1_local))));
    if(m_angle != 0.0)
      {
        // if they are not counter-clock-wise, let's change the sign of
	//! the angle
        const Matrix &R = reference_cs.Orientation();
        if ((e1(0) * R(1, 0) + e1(1) * R(1, 1) + e1(2) * R(1, 2)) < 0.0)
            m_angle = -m_angle;
      }

    // AGQI internal DOFs
    if(m_eas)
      AGQIinitialize();

    return 0;
  }

void XC::ASDShellQ4::setDomain(Domain* theDomain)
  {
    // call base class implementation
    QuadBase4N<SectionFDPhysicalProperties>::setDomain(theDomain);
    if(theDomain)
      {
	// set domain on transformation
	m_transformation->setDomain(theDomain, theNodes.getExternalNodes(), m_initialized);

	// only if not already initialized from recvSelf
	if (!m_initialized)
	  {
	    // compute drilling stiffness
	    m_drill_stiffness = 0.0;
	    for(int i = 0; i < 4; i++)
	      {
		const Matrix &dd= physicalProperties[i]->getInitialTangent();
		m_drill_stiffness+= dd(2, 2);
	      }
	    m_drill_stiffness/= 4.0;
	    m_drill_stiffness*= 0.233; // scale it to get the in-plane roto-distortional modulus

	    // compute section orientation angle
	    this->resetNodalCoordinates();

	    m_damping.setDomain(theDomain, 8);

	    // initialized
	    m_initialized = true;
	  }
      }
  }

int XC::ASDShellQ4::setDamping(Domain *theDomain, Damping *damping)
  {
    if (theDomain && damping)
      {
	m_damping.setDamping(damping);
        m_damping.setDomain(theDomain, 8);
      }
    return 0;
  }

void XC::ASDShellQ4::Print(std::ostream &s, int flag)
  {
    if(flag == -1)
      {
        int eleTag = this->getTag();
        s << "EL_ASDShellQ4\t" << eleTag << "\t";
        s << eleTag << "\t" << 1;
        s << "\t" << theNodes(0) << "\t" << theNodes(1);
        s << "\t" << theNodes(2) << "\t" << theNodes(3) << "\t0.00";
        s << std::endl;
        s << "PROP_3D\t" << eleTag << "\t";
        s << eleTag << "\t" << 1;
        s << "\t" << -1 << "\tSHELL\t1.0\0.0";
        s << std::endl;
     }

    if (flag < -1) {
        int counter = (flag + 1) * -1;
        int eleTag = this->getTag();
        int i, j;
        for (i = 0; i < 4; i++) {
            const Vector &stress = physicalProperties[i]->getStressResultant();
            s << "STRESS\t" << eleTag << "\t" << counter << "\t" << i << "\tTOP";
            for (j = 0; j < 6; j++)
                s << "\t" << stress(j);
            s << std::endl;
        }
    }

    // if (flag == OPS_PRINT_CURRENTSTATE)
    //   {
    //     s << std::endl;
    //     s << "MITC4 Non-Locking Four Node Shell \n";
    //     s << "Element Number: " << this->getTag() << std::endl;
    //     s << "Node 1 : " << theNodes(0) << std::endl;
    //     s << "Node 2 : " << theNodes(1) << std::endl;
    //     s << "Node 3 : " << theNodes(2) << std::endl;
    //     s << "Node 4 : " << theNodes(3) << std::endl;

    //     s << "Material Information : \n ";
    //     physicalProperties[0]->Print(s, flag);

    //     s << std::endl;
    //   }

    // if (flag == OPS_PRINT_PRINTMODEL_JSON)
    //   {
    //     s << "\t\t\t{";
    //     s << "\"name\": " << this->getTag() << ", ";
    //     s << "\"type\": \"ASDShellQ4\", ";
    //     s << "\"nodes\": [" << theNodes(0) << ", " << theNodes(1) << ", ";
    //     s << theNodes(2) << ", " << theNodes(3) << "], ";
    //     s << "\"section\": \"" << physicalProperties[0]->getTag() << "\"}";
    //   }
  }

int XC::ASDShellQ4::getNumDOF(void) const
  { return 24; }


//! @brie Return a pointer to the i-th section of the element.
const XC::SectionForceDeformation *XC::ASDShellQ4::getSectionPtr(const size_t &i) const
  {
    const SectionForceDeformation *retval= nullptr;
    const size_t sz= this->physicalProperties.size();
    if(sz>i)
      retval= this->physicalProperties[i];
    else
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< "; index " << i << " out of range: (0,"
	        << sz << ")."
		<< Color::def << std::endl;
    return retval;
  }

int XC::ASDShellQ4::commitState(void)
  {
    // transformation
    m_transformation->commit();
    int success= QuadBase4N<SectionFDPhysicalProperties>::commitState();

    // AGQI internal DOFs
    if(m_eas)
      {
        m_eas->U_converged = m_eas->U;
        m_eas->Q_converged = m_eas->Q;
      }
	     
    // damping
    success+= m_damping.commitState();
    
    // non linear drilling.
    if (m_drill_mode == DrillingDOF_NonLinear)
      {
        for (int i = 0; i < 4; i++)
	  {
            const Vector &stress = physicalProperties[i]->getStressResultant();
            m_nldrill->stress_comm[i]= stress;
            const Vector &strain = physicalProperties[i]->getSectionDeformation();
            m_nldrill->strain_comm[i] = strain;
            m_nldrill->damage_comm[i] = m_nldrill->damage[i];
	  }
      }
    
    // done
    return success;
  }

int XC::ASDShellQ4::revertToLastCommit(void)
  {
    // transformation
    m_transformation->revertToLastCommit();
    int success= QuadBase4N<SectionFDPhysicalProperties>::revertToLastCommit();
    
    // AGQI internal DOFs
    if (m_eas)
      {
        m_eas->U = m_eas->U_converged;
        m_eas->Q = m_eas->Q_converged;
      }

    // damping
    success+= m_damping.revertToLastCommit();
    
    // non linear drilling.
    if (m_drill_mode == DrillingDOF_NonLinear)
      {
        for (int i = 0; i < 4; i++)
	  {
            const Vector &stress = physicalProperties[i]->getStressResultant();
            m_nldrill->stress_comm[i]= stress;
            const Vector &strain = physicalProperties[i]->getSectionDeformation();
            m_nldrill->strain_comm[i] = strain;
            m_nldrill->damage_comm[i] = m_nldrill->damage[i];
	  }
      }
    
    // done
    return success;
  }

int XC::ASDShellQ4::revertToStart(void)
  {
    // transformation
    m_transformation->revertToStart();
    int success = QuadBase4N<SectionFDPhysicalProperties>::revertToStart();

    // AGQI internal DOFs
    if(m_eas)
      AGQIinitialize();

    // damping
    success+= m_damping.revertToStart();

    // non linear drilling
    if(m_drill_mode == DrillingDOF_NonLinear)
      {
        for (int i = 0; i < 4; i++)
	  {
            m_nldrill->stress_comm[i].Zero();
            m_nldrill->strain_comm[i].Zero();
            m_nldrill->damage[i] = m_nldrill->damage_comm[i] = 0.0;
	  }
      }
    
    return success;
  }

int XC::ASDShellQ4::update(void)
  {
    // calculate
    auto &LHS = ASDShellQ4Globals::instance().LHS;
    auto &RHS = ASDShellQ4Globals::instance().RHS;
    return calculateAll(LHS, RHS, (OPT_UPDATE));
  }

const XC::Matrix &XC::ASDShellQ4::getTangentStiff(void) const
  {
    // calculate
    auto& LHS = ASDShellQ4Globals::instance().LHS;
    auto& RHS = ASDShellQ4Globals::instance().RHS;
    calculateAll(LHS, RHS, (OPT_LHS));
    return LHS;
 }

const XC::Matrix &XC::ASDShellQ4::getInitialStiff(void) const
  {
    // calculate
    auto &LHS= ASDShellQ4Globals::instance().LHS_initial;
    auto &RHS= ASDShellQ4Globals::instance().RHS;
    calculateAll(LHS, RHS, (OPT_LHS | OPT_LHS_IS_INITIAL));
    return LHS;
  }

const XC::Matrix &XC::ASDShellQ4::getMass(void) const
  {
    // Output matrix
    auto &LHS = ASDShellQ4Globals::instance().LHS_mass;
    LHS.Zero();

    // Compute the reference coordinate system
    ASDShellQ4LocalCoordinateSystem reference_cs =
        m_transformation->createReferenceCoordinateSystem();

    // Jacobian
    auto& jac = ASDShellQ4Globals::instance().jac;

    // Some matrices
    auto& dN = ASDShellQ4Globals::instance().dN;
    auto& N = ASDShellQ4Globals::instance().N;

    // Gauss loop
    for (int i = 0; i < 4; i++)
      {
        // Current integration point data
        const double xi = XI[i];
        const double eta = ETA[i];
        const double w = WTS[i];
        shapeFunctions(xi, eta, N);
        shapeFunctionsNaturalDerivatives(xi, eta, dN);
        jac.calculate(reference_cs, dN);
        const double dA = w * jac.detJ;

        // Section density (already integrated through the thickness)
        const double rho = physicalProperties[i]->getArealRho(); // mass per unit area

        // Add current integration point contribution
        for (int j = 0; j < 4; j++)
          {
            int index = j * 6;

            // Translational mass contribution
            double Tmass = N(j) * rho * dA;
            for (int q = 0; q < 3; q++)
                LHS(index + q, index + q) += Tmass;

            // Rotational mass neglected for the moment ...
          }
      }

    // Done
    return LHS;
  }

//! @brief Reactivates the element.
void XC::ASDShellQ4::alive(void)
  {
    if(isDead())
      {
        std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
  	          << "; not implemented yet."
		  << Color::def << std::endl;
	// // Store the current deformation.
        // this->incrementPersistentInitialDeformationWithCurrentDeformation();
	// this->update();
        QuadBase4N<SectionFDPhysicalProperties>::alive();
      }
  }

void XC::ASDShellQ4::zeroLoad()
  { m_load.Zero(); }

int XC::ASDShellQ4::addLoad(ElementalLoad* theLoad, double loadFactor)
  {
    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	      << "; load type unknown for ele with tag: "
	      << this->getTag()
	      << Color::def << std::endl;
    return -1;
  }

int XC::ASDShellQ4::addInertiaLoadToUnbalance(const Vector &accel)
  {
    // Allocate load vector if necessary
    if(m_load.isEmpty())
      m_load= Vector(24);
    auto &F= m_load;

    // Get mass matrix
    const auto &M= getMass();

    // Add -M*R*acc to unbalance, taking advantage of lumped mass matrix
    for (int i = 0; i < 4; i++)
      {
        const auto& RV = m_transformation->getNodes()[i]->getRV(accel);
        int index = i * 6;
        for (int j = 0; j < 6; j++)
            F(index + j) -= M(index + j, index + j) * RV(j);
      }

    // Done
    return 0;
  }

const XC::Vector &XC::ASDShellQ4::getResistingForce(void) const
  {
    // calculate
    auto& LHS = ASDShellQ4Globals::instance().LHS;
    auto& RHS = ASDShellQ4Globals::instance().RHS;
    calculateAll(LHS, RHS, (OPT_RHS));
    return RHS;
  }

const XC::Vector &XC::ASDShellQ4::getResistingForceIncInertia(void) const
  {
    // calculate
    auto& LHS = ASDShellQ4Globals::instance().LHS;
    auto& RHS = ASDShellQ4Globals::instance().RHS_winertia;
    calculateAll(LHS, RHS, (OPT_RHS));

    // add the damping forces if rayleigh damping
    if(!rayFactors.nullValues())
        RHS.addVector(1.0, this->getRayleighDampingForces(), 1.0);

    // Compute mass
    const auto& M = getMass();

    // Add M*acc to unbalance, taking advantage of lumped mass matrix
    for (int i = 0; i < 4; i++)
    {
        const auto& A = m_transformation->getNodes()[i]->getTrialAccel();
        int index = i * 6;
        for (int j = 0; j < 6; j++)
            RHS(index + j) += M(index + j, index + j) * A(j);
    }

    // Done
    return RHS;
  }

//! @brief Send members through the communicator argument.
int XC::ASDShellQ4::sendData(Communicator &comm)
  {
    int res= QuadBase4N<SectionFDPhysicalProperties>::sendData(comm);
    // Send transformation.
    const bool corotational= !m_transformation->isLinear();
    res+= comm.sendBool(corotational,getDbTagData(),CommMetaData(8));
    const Vector trfInternalData= m_transformation->getInternalData();
    res+= comm.sendVector(trfInternalData,getDbTagData(),CommMetaData(9));
    // Transformation sent.
    
    res+= comm.sendDoubles(m_drill_stiffness, m_angle,getDbTagData(),CommMetaData(10));
    res+= comm.sendVector(m_load,getDbTagData(),CommMetaData(11));
    res+= comm.sendVector(m_drill_strain,getDbTagData(),CommMetaData(12));
    res+= comm.sendVector(m_local_x, getDbTagData(),CommMetaData(13));
    res+= comm.sendBool(m_initialized, getDbTagData(),CommMetaData(14));
    const bool use_eas= (this->m_eas!=nullptr);
    res+= comm.sendBool(use_eas, getDbTagData(),CommMetaData(15));
    if(use_eas)
      {
	res+= comm.sendVector(m_eas->Q,getDbTagData(),CommMetaData(16));
	res+= comm.sendVector(m_eas->Q_converged,getDbTagData(),CommMetaData(17));
	res+= comm.sendVector(m_eas->U,getDbTagData(),CommMetaData(18));
	res+= comm.sendVector(m_eas->U_converged,getDbTagData(),CommMetaData(19));
	res+= comm.sendVector(m_eas->Q_residual,getDbTagData(),CommMetaData(20));
	res+= comm.sendMatrix(m_eas->KQQ_inv,getDbTagData(),CommMetaData(21));
	res+= comm.sendMatrix(m_eas->KQU,getDbTagData(),CommMetaData(22));
	res+= comm.sendMatrix(m_eas->KUQ,getDbTagData(),CommMetaData(23));
      }
    res+= comm.sendMovable(m_damping, getDbTagData(),CommMetaData(24));
    const bool use_nldrill= (this->m_nldrill!=nullptr);
    res+= comm.sendBool(use_nldrill, getDbTagData(),CommMetaData(25));
    if(use_nldrill)
      {
	res+= comm.sendVectors(m_nldrill->strain_comm, getDbTagData(),CommMetaData(26));
	res+= comm.sendVectors(m_nldrill->stress_comm, getDbTagData(),CommMetaData(27));
	res+= comm.sendVector(m_nldrill->damage, getDbTagData(), CommMetaData(28));
	res+= comm.sendVector(m_nldrill->damage_comm, getDbTagData(), CommMetaData(29));
      }
    return res;
  }

//! @brief Receives members through the communicator argument.
int XC::ASDShellQ4::recvData(const Communicator &comm)
  {
    int res= QuadBase4N<SectionFDPhysicalProperties>::recvData(comm);
    // Receive transformation.
    bool corotational= false;
    res+= comm.receiveBool(corotational,getDbTagData(),CommMetaData(8));
    Vector trfInternalData;
    res+= comm.receiveVector(trfInternalData,getDbTagData(),CommMetaData(9));
    alloc(corotational);
    m_transformation->setInternalData(trfInternalData);
    // Transformation received.
    res+= comm.receiveDoubles(m_drill_stiffness, m_angle, getDbTagData(),CommMetaData(10));
    res+= comm.receiveVector(m_load,getDbTagData(),CommMetaData(11));
    res+= comm.receiveVector(m_drill_strain,getDbTagData(),CommMetaData(12));
    res+= comm.receiveVector(m_local_x,getDbTagData(),CommMetaData(13));
    res+= comm.receiveBool(m_initialized, getDbTagData(), CommMetaData(14));
    bool use_eas= false;
    res+= comm.receiveBool(use_eas, getDbTagData(), CommMetaData(15));
    if(m_eas)
      {
	alloc_eas();
	res+= comm.receiveVector(m_eas->Q,getDbTagData(),CommMetaData(16));
	res+= comm.receiveVector(m_eas->Q_converged,getDbTagData(),CommMetaData(17));
	res+= comm.receiveVector(m_eas->U,getDbTagData(),CommMetaData(18));
	res+= comm.receiveVector(m_eas->U_converged,getDbTagData(),CommMetaData(19));
	res+= comm.receiveVector(m_eas->Q_residual,getDbTagData(),CommMetaData(20));
	res+= comm.receiveMatrix(m_eas->KQQ_inv,getDbTagData(),CommMetaData(21));
	res+= comm.receiveMatrix(m_eas->KQU,getDbTagData(),CommMetaData(22));
	res+= comm.receiveMatrix(m_eas->KUQ,getDbTagData(),CommMetaData(23));
      }
    res+= comm.receiveMovable(m_damping, getDbTagData(),CommMetaData(24));
    bool use_nldrill= false;
    res+= comm.receiveBool(use_nldrill, getDbTagData(), CommMetaData(25));
    if(use_nldrill)
      {
	alloc_nldrill();
	res+= comm.receiveVectors(m_nldrill->strain_comm, getDbTagData(),CommMetaData(26));
	res+= comm.receiveVectors(m_nldrill->stress_comm, getDbTagData(),CommMetaData(27));
	res+= comm.receiveVector(m_nldrill->damage, getDbTagData(), CommMetaData(28));
	res+= comm.receiveVector(m_nldrill->damage_comm, getDbTagData(), CommMetaData(29));
      }
    return res;
  }

int XC::ASDShellQ4::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(30);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
                << "; failed to send data."
		<< Color::def << std::endl;
    return res;
  }

int  XC::ASDShellQ4::recvSelf(const Communicator &comm)
  {
    inicComm(30);

    const int dbTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dbTag);
    if(res<0)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
                << "; failed to receive ids."
		<< Color::def << std::endl;
    else
      {
        res+= recvData(comm);
        if(res<0)
           std::cerr << Color::red<< getClassName() << "::" << __FUNCTION__
                     << "; failed to receive data."
		     << Color::def << std::endl;
      }
    return res;
  }

XC::Response *XC::ASDShellQ4::setResponse(const std::vector<std::string> &argv,  Information &eleInformation)
  {
    Response* theResponse= nullptr;
    const int argc= argv.size();

    // output.tag("ElementOutput");
    // output.attr("eleType", "ASDShellQ4");
    // output.attr("eleTag", this->getTag());
    int numNodes = this->getNumExternalNodes();
    //const ID &nodes = this->theNodes.getExternalNodes();
    static char nodeData[32];

    for (int i = 0; i < numNodes; i++)
      {
        sprintf(nodeData, "node%d", i + 1);
        //output.attr(nodeData, nodes(i));
      }

    if ((argv[0]=="force") || (argv[0]=="forces") ||
        (argv[0]=="globalForce") || (argv[0]=="globalForces"))
      {
        const Vector &force = this->getResistingForce();
        int size = force.Size();
        for(int i = 0; i < size; i++)
	  {
            sprintf(nodeData, "P%d", i + 1);
            //output.tag("ResponseType", nodeData);
          }
        theResponse = new ElementResponse(this, 1, this->getResistingForce());
      }
    else if ((argv[0]=="material") || (argv[0]=="Material"))
      {
        if (argc < 2)
	  {
            std::cerr << Color::red  << getClassName() << "::" << __FUNCTION__
		      << "; need to specify more data."
	              << Color::def << std::endl;
            return 0;
          }
        const int pointNum = atoi(argv[1]);
        if (pointNum > 0 && pointNum <= 4)
	  {

            //output.tag("GaussPoint");
            //output.attr("number", pointNum);
            //output.attr("eta", XI[pointNum - 1]);
            //output.attr("neta", ETA[pointNum - 1]);

	    const std::vector<std::string> tmp(argv.begin()+2, argv.end());
            theResponse = physicalProperties[pointNum - 1]->setResponse(tmp, eleInformation);

            //output.endTag();
	  }
      }
    else if(argv[0]=="stresses")
      {
        for (int i = 0; i < 4; i++)
	  {
            //output.tag("GaussPoint");
            //output.attr("number", i + 1);
            //output.attr("eta", XI[i]);
            //output.attr("neta", ETA[i]);

            //output.tag("SectionForceDeformation");
            //output.attr("classType", physicalProperties[i]->getClassTag());
            //output.attr("tag", physicalProperties[i]->getTag());

            //output.tag("ResponseType", "p11");
            //output.tag("ResponseType", "p22");
            //output.tag("ResponseType", "p1212");
            //output.tag("ResponseType", "m11");
            //output.tag("ResponseType", "m22");
            //output.tag("ResponseType", "m12");
            //output.tag("ResponseType", "q1");
            //output.tag("ResponseType", "q2");

            //output.endTag(); // GaussPoint
            //output.endTag(); // NdMaterialOutput
	  }

        theResponse = new ElementResponse(this, 2, Vector(32));
      }
    else if(argv[0]=="strains")
      {
        for (int i = 0; i < 4; i++)
	  {
            //output.tag("GaussPoint");
            //output.attr("number", i + 1);
            //output.attr("eta", XI[i]);
            //output.attr("neta", ETA[i]);

            //output.tag("SectionForceDeformation");
            //output.attr("classType", physicalProperties[i]->getClassTag());
            //output.attr("tag", physicalProperties[i]->getTag());

            //output.tag("ResponseType", "eps11");
            //output.tag("ResponseType", "eps22");
            //output.tag("ResponseType", "gamma12");
            //output.tag("ResponseType", "theta11");
            //output.tag("ResponseType", "theta22");
            //output.tag("ResponseType", "theta33");
            //output.tag("ResponseType", "gamma13");
            //output.tag("ResponseType", "gamma23");

            //output.endTag(); // GaussPoint
            //output.endTag(); // NdMaterialOutput
	  }

        theResponse = new ElementResponse(this, 3, Vector(32));
      }
    else if(argv[0]=="strains")
      {
	for (int i=0; i<4; i++)
	  {
	    // output.tag("GaussPoint");
	    // output.attr("number",i+1);
	    // output.attr("eta",XI[i]);
	    // output.attr("neta",ETA[i]);
        
	    // output.tag("SectionForceDeformation");
	    // output.attr("classType", m_damping[i]->getClassTag());
	    // output.attr("tag", m_damping[i]->getTag());
        
	    // output.tag("ResponseType","p11");
	    // output.tag("ResponseType","p22");
	    // output.tag("ResponseType","p12");
	    // output.tag("ResponseType","m11");
	    // output.tag("ResponseType","m22");
	    // output.tag("ResponseType","m12");
	    // output.tag("ResponseType","q1");
	    // output.tag("ResponseType","q2");
        
	    // output.endTag(); // GaussPoint
	    // output.endTag(); // NdMaterialOutput
	  }
	theResponse =  new ElementResponse(this, 4, Vector(32));
      }
    //output.endTag();
    return theResponse;
  }

int XC::ASDShellQ4::getResponse(int responseID, Information &eleInfo)
  {
    static Vector stresses(32);
    static Vector strains(32);

    switch (responseID) {
    case 1: // global forces
        return eleInfo.setVector(this->getResistingForce());
        break;

    case 2: // stresses
        for (int i = 0; i < 4; i++)
	  {

            // Get material stress response
            const Vector &sigma = physicalProperties[i]->getStressResultant();
	    const int offset = i*8;
            stresses(0+offset)= sigma(0);
            stresses(1+offset)= sigma(1);
            stresses(2+offset)= sigma(2);
            stresses(3+offset)= sigma(3);
            stresses(4+offset)= sigma(4);
            stresses(5+offset)= sigma(5);
            stresses(6+offset)= sigma(6);
            stresses(7+offset)= sigma(7);
          }
        return eleInfo.setVector(stresses);
        break;
    case 3: //strain
        for (int i = 0; i < 4; i++)
	  {

            // Get section deformation
            const Vector &deformation = physicalProperties[i]->getSectionDeformation();
	    const int offset = i*8;
            strains(0+offset)= deformation(0);
            strains(1+offset)= deformation(1);
            strains(2+offset)= deformation(2);
            strains(3+offset)= deformation(3);
            strains(4+offset)= deformation(4);
            strains(5+offset)= deformation(5);
            strains(6+offset)= deformation(6);
            strains(7+offset)= deformation(7);
	  }
        return eleInfo.setVector(strains);
        break;
    case 4: // damping stresses
	for (int i = 0; i < 4; i++)
	  {
	    // Get material stress response
	    const Vector &sigma = m_damping[i]->getDampingForce();
	    stresses(0) = sigma(0);
	    stresses(1) = sigma(1);
	    stresses(2) = sigma(2);
	    stresses(3) = sigma(3);
	    stresses(4) = sigma(4);
	    stresses(5) = sigma(5);
	    stresses(6) = sigma(6);
	    stresses(7) = sigma(7);
	  }
	return eleInfo.setVector(stresses);
	break;
    default:
        return -1;
    }
}

int XC::ASDShellQ4::setParameter(const std::vector<std::string> &argv, Parameter& param)
  {
    int res = -1;
    const int argc= argv.size();
    // damping
    if(argv[0]=="damp")
      {
        if(argc < 2 || m_damping.empty())
	  return -1;

        for (int i=0; i<4; i++)
	  {
            int dmpRes =  m_damping[i]->setParameter(argv, param);
            if (dmpRes != -1)
                res = dmpRes;
	  }
        return res;
      }
    
    int matRes = res;
    for (int i = 0; i < 4; i++)
      {
        matRes = physicalProperties[i]->setParameter(argv, param);
        if (matRes != -1)
            res = matRes;
      }
    return res;
  }
double XC::ASDShellQ4::getCharacteristicLength(void) const
  {
    // by default we return the min distance between 2 nodes, which should coincide
    // with lch = sqrt(A) in non-distorted elements.
    // however, when the EAS is used, the element localizes only in a row of gauss points
    // so the lch should be half the original one
    double lch = Element::getCharacteristicLength();
    if (m_eas)
        lch/= 2.0;
    return lch;
  }

int XC::ASDShellQ4::calculateAll(Matrix &LHS, Vector &RHS, int options) const
  {
    // Check options
    if (!m_transformation->isLinear())
      {
        // corotational calculation of the tangent LHS requires the RHS!
        if (options & OPT_LHS)
            options |= OPT_RHS;
      }

    // Zero output
    int result = 0;
    if(options & OPT_RHS)
        RHS.Zero();
    if(options & OPT_LHS)
        LHS.Zero();

    // Global displacements
    auto& UG = ASDShellQ4Globals::instance().UG;
    m_transformation->computeGlobalDisplacements(UG);

    // update transformation
    if(options & OPT_UPDATE)
        m_transformation->update(UG);

    // Compute the reference coordinate system
    ASDShellQ4LocalCoordinateSystem reference_cs =
        m_transformation->createReferenceCoordinateSystem();

    // Compute the local coordinate system.
    ASDShellQ4LocalCoordinateSystem local_cs =
        m_transformation->createLocalCoordinateSystem(UG);

    // Prepare all the parameters needed for the MITC4
    // and AGQI formulations.
    // This is to be done here outside the Gauss Loop.
    auto& mitc = ASDShellQ4Globals::instance().mitc;
    auto& agq = ASDShellQ4Globals::instance().agq;
    mitc.compute(reference_cs);
    if(m_eas)
        agq.compute(reference_cs);

    // Jacobian
    auto& jac = ASDShellQ4Globals::instance().jac;

    // Some matrices/vectors
    auto& B = ASDShellQ4Globals::instance().B;
    auto& Bd = ASDShellQ4Globals::instance().Bd;
    auto& Bd0 = ASDShellQ4Globals::instance().Bd0;
    auto& BQ = ASDShellQ4Globals::instance().BQ;
    auto& BQ_mean = ASDShellQ4Globals::instance().BQ_mean;
    auto& BQTD = ASDShellQ4Globals::instance().BQTD;
    auto& DBQ = ASDShellQ4Globals::instance().DBQ;
    auto& B1 = ASDShellQ4Globals::instance().B1;
    auto& B1TD = ASDShellQ4Globals::instance().B1TD;
    auto& N = ASDShellQ4Globals::instance().N;
    auto& dN = ASDShellQ4Globals::instance().dN;
    auto& E = ASDShellQ4Globals::instance().E;
    auto& S = ASDShellQ4Globals::instance().S;
    auto& D = ASDShellQ4Globals::instance().D;
    auto& Dsection = ASDShellQ4Globals::instance().Dsection;

    // matrices for orienting strains in section coordinate system
    auto& Re = ASDShellQ4Globals::instance().Re;
    auto& Rs = ASDShellQ4Globals::instance().Rs;
    if (m_angle != 0.0)
      {
        if (options & OPT_UPDATE)
            getRotationMatrixForGeneralizedStrains(-m_angle, Re);
        if ((options & OPT_RHS) || (options & OPT_LHS))
            getRotationMatrixForGeneralizedStresses(m_angle, Rs);
      }

    // Local displacements
    auto& UL = ASDShellQ4Globals::instance().UL;
    m_transformation->calculateLocalDisplacements(local_cs, UG, UL);

    // Update AGQI internal DOFs
    if (options & OPT_UPDATE)
        if(m_eas)
            AGQIupdate(UL);

    // AGQI begin gauss loop (computes the BQ_mean correction matrix)
    if(m_eas)
        AGQIbeginGaussLoop(reference_cs);

    // Drilling strain-displacement matrix at center for reduced
    // integration
    shapeFunctions(0.0, 0.0, N);
    shapeFunctionsNaturalDerivatives(0.0, 0.0, dN);
    computeBdrilling(reference_cs, 0.0, 0.0, jac, agq, N, dN, Bd0, m_eas);
    static Vector drill_dstrain(8);
    static Vector drill_dstress(8);
    static Vector drill_dstress_el(8);

    // Gauss loop
    for (int igauss = 0; igauss < 4; igauss++)
      {
        // Current integration point data
        const double xi = XI[igauss];
        const double eta = ETA[igauss];
        const double w = WTS[igauss];
        shapeFunctions(xi, eta, N);
        shapeFunctionsNaturalDerivatives(xi, eta, dN);
        jac.calculate(reference_cs, dN);
        const double dA = w * jac.detJ;

        // Strain-displacement matrix
        computeBMatrix(reference_cs, xi, eta, jac, agq, mitc, N, dN, BQ_mean, B, BQ, Bd, m_eas);

        // The drilling according to Hughes-Brezzi plays and important role in
	// warped shells and the stiffness should be equal to the initial
	// (elastic) in-plane shear modulus.
        // If fully integrated however, locks the membrane response. If
	// under-integrated the element has spurious zero-energy modes. Thus
	// here we use the compute the drilling B matrix as a weighted sum
	// of the reduced integrated one (Bd0) and a scaled  version of the
	// fully integrated one, just to suppress spurious zero energy modes.
        // Kd0 = Bd0'*Bd0*kdrill*V
        // Kd = int(Bd'*Bd*kdrill*dV)
        // K = stab*Kd + (1-stab)*Kd0
        const double drill_alpha = std::sqrt(m_drill_stab);
        const double drill_beta = std::sqrt(1.0 - m_drill_stab);
        Bd.addVector(drill_alpha, Bd0, drill_beta);

        // Update strain strain
        if (options & OPT_UPDATE)
	  {
            // Section deformation
            if (m_angle != 0.0)
	      {
                auto& Elocal = ASDShellQ4Globals::instance().Elocal;
                Elocal.addMatrixVector(0.0, B, UL, 1.0);
                if (m_eas)
                    Elocal.addMatrixVector(1.0, BQ, m_eas->Q, 1.0);
                E.addMatrixVector(0.0, Re, Elocal, 1.0);
	      }
            else
	      {
                E.addMatrixVector(0.0, B, UL, 1.0);
                if (m_eas)
                    E.addMatrixVector(1.0, BQ, m_eas->Q, 1.0);
	      }

            // Update section
	    ASDShellQ4 *this_no_const= const_cast<ASDShellQ4 *>(this);
            result+= this_no_const->physicalProperties[igauss]->setTrialSectionDeformation(E);

            // Drilling strain Ed = Bd*UL
            double &Ed = m_drill_strain[igauss];
            Ed = 0.0;
            for (int i = 0; i < 24; i++)
                Ed += Bd(i) * UL(i);
        }

        // Invert bending terms for correct statement of equilibrium
        if((options & OPT_RHS) || (options & OPT_LHS))
            invertBBendingTerms(B, B1);

        // Integrate RHS
        if (options & OPT_RHS)
	  {
            // Section force
            if (m_angle != 0.0)
	      {
                auto& Ssection = physicalProperties[igauss]->getStressResultant();
                S.addMatrixVector(0.0, Rs, Ssection, 1.0);
                if (m_damping[igauss])
		  {
                    m_damping[igauss]->update(Ssection);
                    auto &Sdsection= m_damping[igauss]->getDampingForce();
                    S.addMatrixVector(1.0, Rs, Sdsection, 1.0);
		  }
	      }
            else
	      {
                S = physicalProperties[igauss]->getStressResultant();
                if (m_damping[igauss])
		  {
                    m_damping[igauss]->update(S);
                    S+= m_damping[igauss]->getDampingForce();
		  }
	      }

            // Add current integration point contribution (RHS)
            RHS.addMatrixTransposeVector(1.0, B1, S, dA);

            // Compute drilling damages
            if (m_drill_mode == DrillingDOF_NonLinear)
	      {
                drill_dstrain = physicalProperties[igauss]->getSectionDeformation();
                drill_dstrain.addVector(1.0, m_nldrill->strain_comm[igauss], -1.0);
                if (drill_dstrain.Norm() > 1.0e-10)
		  {
                    drill_dstress = physicalProperties[igauss]->getStressResultant();
                    drill_dstress.addVector(1.0, m_nldrill->stress_comm[igauss], -1.0);
                    const Matrix& C0 = physicalProperties[igauss]->getInitialTangent();
                    drill_dstress_el.addMatrixVector(0.0, C0, drill_dstrain, 1.0);
                    double drill_damage = m_nldrill->damage_comm[igauss];
                    for (int j = 0; j < 8; ++j)
		      {
                        const double dx = drill_dstrain(j);
                        if (dx > 0.0)
			  {
                            const double &dy= drill_dstress(j);
                            const double &dy0= drill_dstress_el(j);
                            drill_damage = std::max(drill_damage, 1.0 - dy / dy0);
			  }
		      }
                    drill_damage = std::max(0.0, std::min(0.999, drill_damage));
                    m_nldrill->damage[igauss] = drill_damage;
		  }
	      }
	    
            // Add drilling stress = Bd'*Sd * dA (RHS)
            double Sd = m_drill_stiffness * m_drill_strain[igauss];
            if (m_drill_mode == DrillingDOF_NonLinear)
                Sd *= (1.0 - m_nldrill->damage_comm[igauss]);
            for (int i = 0; i < 24; i++)
                RHS(i) += Bd(i) * Sd * dA;

            // AGQI: update residual for internal DOFs
            if(m_eas)
                m_eas->Q_residual.addMatrixTransposeVector(1.0, BQ, S, -dA);
	  }

        // AGQI: due to the static condensation, the following items
        // must be computed if we need either the RHS or the LHS, or
	// both of them
        if (((options & OPT_RHS) && m_eas) || (options & OPT_LHS))
	  {
            // Section tangent
            if (m_angle != 0.0)
	      {
		Dsection= (options & OPT_LHS_IS_INITIAL) ?
		  physicalProperties[igauss]->getInitialTangent() :
		  physicalProperties[igauss]->getSectionTangent();
                if(m_damping[igauss])
		  {
		    Matrix &Dsection_no_const= const_cast<Matrix &>(Dsection);
		    
		    Dsection_no_const*= m_damping[igauss]->getStiffnessMultiplier();
		  }
                auto& RsT = ASDShellQ4Globals::instance().RsT;
                RsT.addMatrixTranspose(0.0, Rs, 1.0);
                auto& DRsT = ASDShellQ4Globals::instance().DRsT;
                DRsT.addMatrixProduct(0.0, Dsection, RsT, 1.0);
                D.addMatrixProduct(0.0, Rs, DRsT, 1.0);
	      }
            else
	      {
                D = (options & OPT_LHS_IS_INITIAL) ?
                    physicalProperties[igauss]->getInitialTangent() :
                    physicalProperties[igauss]->getSectionTangent();
                if(m_damping[igauss])
		  D*= m_damping[igauss]->getStiffnessMultiplier();
	      }

            // Matrices for AGQI static condensation of internal DOFs
            if (m_eas)
	      {
                BQTD.addMatrixTransposeProduct(0.0, BQ, D, dA);
                DBQ.addMatrixProduct(0.0, D, BQ, dA);
                m_eas->KQQ_inv.addMatrixProduct(1.0, BQTD, BQ, 1.0);
                m_eas->KQU.addMatrixProduct(1.0, BQTD, B, 1.0);
                m_eas->KUQ.addMatrixTransposeProduct(1.0, B1, DBQ, 1.0);
	      }
        }

        // Integrate LHS
        if (options & OPT_LHS) 
        {
            // Add current integration point contribution (LHS)
            B1TD.addMatrixTransposeProduct(0.0, B1, D, dA);
            LHS.addMatrixProduct(1.0, B1TD, B, 1.0);

            // Add drilling stiffness = Bd'*Kd*Bd * dA (LHS)
            double drill_tang = m_drill_stiffness;
            if (m_drill_mode == DrillingDOF_NonLinear) 
                drill_tang *= (1.0 - m_nldrill->damage_comm[igauss]);
            for (int i = 0; i < 24; i++)
                for (int j = 0; j < 24; j++)
                    LHS(i, j) += Bd(i) * drill_tang * Bd(j) * dA;
        }

    } // End of Gauss Loop

    // AGQI: static condensation
    if (((options & OPT_RHS) || (options & OPT_LHS)) && m_eas)
      {
        static Matrix KQQ = Matrix(4, 4);
        static Matrix KUQ_KQQ_inv = Matrix(24, 4);
        KQQ = m_eas->KQQ_inv;
        int inv_res = KQQ.Invert(m_eas->KQQ_inv);
        KUQ_KQQ_inv.addMatrixProduct(0.0, m_eas->KUQ, m_eas->KQQ_inv, 1.0);
        if (options & OPT_RHS)
            RHS.addMatrixVector(1.0, KUQ_KQQ_inv, m_eas->Q_residual, 1.0);
        if (options & OPT_LHS)
            LHS.addMatrixProduct(1.0, KUQ_KQQ_inv, m_eas->KQU, -1.0);
      }

    // Tranform LHS to global coordinate system
    m_transformation->transformToGlobal(local_cs, UG, UL, LHS, RHS, (options & OPT_LHS));

    // Subtract external loads if any
    if ((options & OPT_RHS) && (not m_load.isEmpty()))
        RHS.addVector(1.0, m_load, -1.0);

    // Done
    return result;
}

void XC::ASDShellQ4::AGQIinitialize(void)
  {
    // Global displacements
    auto& UG = ASDShellQ4Globals::instance().UG;
    m_transformation->computeGlobalDisplacements(UG);

    ASDShellQ4LocalCoordinateSystem local_cs = m_transformation->createLocalCoordinateSystem(UG);

    // Local displacements
    auto& UL = ASDShellQ4Globals::instance().UL;
    m_transformation->calculateLocalDisplacements(local_cs, UG, UL);

    // Initialize internal DOFs members
    m_eas->Q.Zero();
    m_eas->Q_converged.Zero();
    m_eas->U = UL;
    m_eas->U_converged = UL;
  }

void XC::ASDShellQ4::AGQIupdate(const Vector &UL) const
  {
    // Compute incremental displacements
    static Vector dUL(24);
    dUL = UL;
    dUL.addVector(1.0, m_eas->U, -1.0);

    // Save current trial displacements
    m_eas->U= UL;

    // Update internal DOFs
    static Vector temp(4);
    temp.addMatrixVector(0.0, m_eas->KQU, dUL, 1.0);
    temp.addVector(1.0, m_eas->Q_residual, -1.0);
    m_eas->Q.addMatrixVector(1.0, m_eas->KQQ_inv, temp, -1.0);
  }

void XC::ASDShellQ4::AGQIbeginGaussLoop(const ASDShellQ4LocalCoordinateSystem& reference_cs) const
  {
    // set to zero vectors and matrices for the static condensation
    // of internal DOFs before proceding with gauss integration
    m_eas->KQU.Zero();
    m_eas->KUQ.Zero();
    m_eas->KQQ_inv.Zero();
    m_eas->Q_residual.Zero();

    // Some matrices/vectors
    auto& N = ASDShellQ4Globals::instance().N;
    auto& dN = ASDShellQ4Globals::instance().dN;

    // Jacobian
    auto& jac = ASDShellQ4Globals::instance().jac;

    // this one is already computed in the caller method
    auto& agq = ASDShellQ4Globals::instance().agq;

    // The AGQI uses incompatible modes and only the weak patch test is passed.
    // Here we computed the mean Bq matrix for the enhanced strains. It will be subtracted
    // from the gauss-wise Bq matrices to make this element pass the strict patch test:
    // BQ = BQ - 1/A*int{BQ*dV}
    auto& BQ_mean = ASDShellQ4Globals::instance().BQ_mean;
    BQ_mean.Zero();
    double Atot = 0.0;

    // Gauss loop
    std::array<double, 4> L;
    for (int igauss = 0; igauss < 4; igauss++)
      {
        // Current integration point data
        const double xi = XI[igauss];
        const double eta = ETA[igauss];
        const double w = WTS[igauss];
        shapeFunctions(xi, eta, N);
        shapeFunctionsNaturalDerivatives(xi, eta, dN);
        jac.calculate(reference_cs, dN);
        double dA = w * jac.detJ;
        Atot += dA;

        // area coordinates of the gauss point (Eq 7)
        L[0] = 0.25 * (1.0 - xi) * (agq.g[1] * (1.0 - eta) + agq.g[2] * (1.0 + eta));
        L[1] = 0.25 * (1.0 - eta) * (agq.g[3] * (1.0 - xi) + agq.g[2] * (1.0 + xi));
        L[2] = 0.25 * (1.0 + xi) * (agq.g[0] * (1.0 - eta) + agq.g[3] * (1.0 + eta));
        L[3] = 0.25 * (1.0 + eta) * (agq.g[0] * (1.0 - xi) + agq.g[1] * (1.0 + xi));

        // strain matrix for internal dofs
        for (int i = 0; i < 2; i++)
	  { 
            int j = i + 1; if (j > 3) j = 0;
            int k = j + 1; if (k > 3) k = 0;
            const double NQX = (agq.b[i] * L[k] + agq.b[k] * L[i]) / agq.A / 2.0;
            const double NQY = (agq.c[i] * L[k] + agq.c[k] * L[i]) / agq.A / 2.0;
            const int index1 = i * 2;
            const int index2 = index1 + 1;
            BQ_mean(0, index1) += NQX * dA;
            BQ_mean(1, index2) += NQY * dA;
            BQ_mean(2, index1) += NQY * dA;
            BQ_mean(2, index2) += NQX * dA;
        }
    }

    // Average
    BQ_mean /= Atot;
}

