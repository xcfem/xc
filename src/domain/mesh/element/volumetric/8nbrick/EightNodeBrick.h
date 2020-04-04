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
///////////////////////////////////////////////////////////////////////////////
//
// COPYLEFT (C):     :-))
//``This  source code is Copyrighted in U.S., by the The Regents of the University
//of California, for an indefinite period, and anybody caught using it without our
//permission,  will  be  mighty  good friends of ourn, cause we don't give a darn.
//Hack  it.  Compile it. Debug it. Run it. Yodel it. Enjoy it. We wrote it, that's
//all we wanted to do.'' bj
// PROJECT:           Object Oriented Finite Element Program
// FILE:              EightNodeBrick.h
// CLASS:             EightNodeBrick
// MEMBER FUNCTIONS:
//
// MEMBER VARIABLES
//
// PURPOSE:           Finite Element Class
// RETURN:
// VERSION:
// LANGUAGE:          C++.ver >= 3.0
// TARGET OS:         DOS || UNIX || . . .
// DESIGNER:          Boris Jeremic, Zhaohui Yang and Xiaoyan Wu
// PROGRAMMER:        Boris Jeremic, Zhaohui Yang  and Xiaoyan Wu
// DATE:              Aug. 2000
// UPDATE HISTORY:			 Modified from Brick3D and FourNodeQuad.hh  07/06/00
//																			 Sept. - Oct 2000 connected to OpenSees by Zhaohui
//																			 Sept 2001 optimized to some extent (static tensors...)
//
///////////////////////////////////////////////////////////////////////////////
//


#ifndef EIGHTNODEBRICK_H
#define EIGHTNODEBRICK_H

// Commented by Xiaoyan. Use  ~/fem/node.hh  08/04/00
// Released Node.h now. Wu use Opensees's Node.09/27/00


#include <domain/mesh/element/ElementBase.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include "domain/mesh/element/utils/body_forces/BodyForces3D.h"

namespace XC {
class Node;
class NDMaterial;
class MatPoint3D;
class stresstensor;
class Information;
class BJtensor;
//class QuadRule1d;

//! @ingroup ElemVol
//!
//! @brief Eight node hexahedral element for three-dimensional problems.
class EightNodeBrick: public ElementBase<8>
  {
  private:
    int numDOF; //!< Number of element DOF

    mutable Matrix *Ki;

    static Matrix K; //!< Element stiffness Matrix
    static Matrix C; //!< Element damping matrix
    static Matrix M; //!< Element mass matrix
    static Vector P; //!< Element resisting force vector
    BodyForces3D bf; //!< Body forces

    //Vector q0; //!< hold element load affects q0 and p0 in one vector

    double rho; //!< Mass per unit volume
    double pressure; //!< Normal surface traction (pressure) over entire element
    int    order; //!< Order of the quadrature rule

    //int dir; //!< Direction of vertical coord.
    //double surflevel; //!< free surface level above or below this element

    //QuadRule1d *theQuadRule; //!< Integration rule

//    Matrix J; //!< Jacobian of transformation
//    Matrix L; //!< Inverse of J
//    Matrix B; //!< Strain interpolation matrix


    //    // static data - single copy for all objects of the class
    //    static G3Matrix N; //!< Displacement interpolation matrix

    //    // private member functions - only objects of this class can call these
    //    void setJacobian (double r, double s, double t); //!< Xiaoyan changed
    //    double formDetJ (double r, double s, double t); //!< xi, eta to r and s
    //    void formBMatrix (double r, double s, double t); //!< and added t
    //    static void formNMatrix (double r, double s, double t); //!< 07/06/00

  private:
    // element number (tag)
    //unsigned int  elem_numb;

    double determinant_of_Jacobian;
    //int  G_N_numbs[8]; //!< Global node numbers for this element  Xiaoyan changed from 20 to 8

    NDMaterial *mmodel; //!< pointer to GLOBAL material models

    int r_integration_order; //!< Gauss-Legendre integration order in r direction
    int s_integration_order; //!< Gauss-Legendre integration order in s direction
    int t_integration_order; //!< Gauss-Legendre integration order in t direction

    // Now I want 3D array of Material points!
    // MatPoint3D[r_integration_order][s_integration_order][t_integration_order]
    // 3D array of Material points
    std::vector<MatPoint3D> matpoint;  //!< pointer to array of Material Points

    // 3D array of material models for each Material points
    // NDMaterial *GPmmodel; //!< pointer to array of material models for Material Points
    // Do we need this one?

    //..NDMaterial  *MatPoint; //!< Zhaohui  10-01-2000


    int  LM[24]; //!< for 8noded x 3 = 24
  public:
    EightNodeBrick(int element_number,
                   int node_numb_1, int node_numb_2, int node_numb_3, int node_numb_4,
                   int node_numb_5, int node_numb_6, int node_numb_7, int node_numb_8,
                   NDMaterial * Globalmmodel, const BodyForces3D &bForces,
                  double r, double p);
   // int dir, double surflevel);
   //, EPState *InitEPS);   const std::string &type,

    EightNodeBrick(void);
    Element *getCopy(void) const;
    ~EightNodeBrick(void);

    //Not needed Zhaohui
    //int Initialize(int element_number,
    //               int node_numb_1, int node_numb_2, int node_numb_3, int node_numb_4,
    //               int node_numb_5, int node_numb_6, int node_numb_7, int node_numb_8,
    //               NDMaterial * Globalmmodel,  const BodyForces3D &=,
    //               double p, double r);
    //             //, EPState * InitEPS);const std::string &type,

    // update, Guanzhou added Apr. 2004 to update incremental strain in the domain
    int update(void);

    int getNumDOF(void) const;
    void setDomain(Domain *theDomain);

    // public methods to set the state of the element
    int commitState();
    int revertToLastCommit ();
    int revertToStart();

    // public methods to obtain stiffness, mass, damping and residual information
    // We haven't build the following functions.
    // All the value of K M Dmp and F are nothing. just
    // want to test the program.  Xiaoyan 08/16/00
    const Matrix &getTangentStiff(void) const;
    const Matrix &getInitialStiff(void) const;
    const Matrix &getMass(void) const;

    const Matrix &getConsMass(void);

    int addLoad(ElementalLoad *theLoad, double loadFactor);
    int addInertiaLoadToUnbalance(const Vector &accel);

    const Vector FormEquiBodyForce(void);
    const Vector &getResistingForce(void) const;
    const Vector &getResistingForceIncInertia(void) const;

    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);

    void Print(std::ostream &s, int flag =0) const;
    //    Do nothing with void Print(std::ostream &s, int flag =0);
    //    use Brick3D report.  08/16/00
    Response *setResponse(const std::vector<std::string> &argv, Information &eleInformation);
    int getResponse (int responseID, Information &eleInformation);

    void incremental_Update(void);
    //void iterative_Update(void);


    BJtensor H_3D(double r1, double r2, double r3) const;
    BJtensor interp_poli_at(double r, double s, double t);
    BJtensor dh_drst_at(double r, double s, double t) const;


    //CE Dynamic Allocation for for brick3d s.
    //Finite_Element * new_el( int total );
    EightNodeBrick &operator[](int subscript);
    //Finite_Element & operator[](int subscript);
    //Finite_Element & operator[](int subscript);

    BJtensor getStiffnessTensor(void) const;
    //matrix stiffness_tensor(void);

    void set_strain_stress_tensor(FILE *fp, double * u);
    BJtensor getMassTensor(void) const;
//out19Jan2001     double Potential_Energy(void);

    BJtensor Jacobian_3D(BJtensor dh) const;
    BJtensor Jacobian_3Dinv(BJtensor dh) const;
    BJtensor Nodal_Coordinates(void) const;

    BJtensor incr_disp(void) const;
    BJtensor total_disp(void) const;

    BJtensor total_disp(FILE *fp, double * u);

    BJtensor stiffness_matrix(const BJtensor & K);
    BJtensor mass_matrix(const BJtensor & M);


    int  get_global_number_of_node(int local_node_number);
    int  get_Brick_Number(void);

    int * get_LM(void);
    //void set_LM(Node * node); //!< commented out temporarily 09-27-2000 Zhaohui

    //these two files are originally in fe.h
    double get_Gauss_p_c(short order, short point_numb) const;
    double get_Gauss_p_w(short order, short point_numb) const;

    // returns nodal forces for given stress field in an element
    BJtensor nodal_forces(void) const;
    // returns nodal forces for ITERATIVE stress field in an element
    BJtensor iterative_nodal_forces(void) const;
    // returns nodal forces for given constant stress field in the element
    BJtensor nodal_forces_from_stress(stresstensor &) const;
    // returns nodal forces for given incremental strain field in an element
    // by using the linearized constitutive BJtensor from the beginning of the step !
    BJtensor linearized_nodal_forces(void) const;

    // updates Material point stresses and strains from given displacements
    BJtensor update_stress_strain(BJtensor & disp);

    void report(char *);
    void reportshort(char *);
    void reportPAK(char *);
    void reportpqtheta(int);
    //void reportLM(char *);
    void computeGaussPoint(void);
    void reportCIPIC(char *);
    void reportTensorF(FILE *);

    // Setting initial E according to the initial pressure
    //void setInitE(void);
    //void reportStressTensorF(FILE *);
  };
} // end of XC namespace


#endif

