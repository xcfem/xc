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
                                                                       
                                                                       
#ifndef BeamContact3D_h
#define BeamContact3D_h

// Written: kap
// Created: 06/05
//
// Description: This file contains the class definition for BeamContact3D.

#include "domain/mesh/element/ElemWithMaterial.h"
#include "domain/mesh/element/utils/physical_properties/Contact3D.h"

class CrdTransf;

// number of nodes per element
#define BC3D_NUM_NODE 4
// number of dimensions
#define BC3D_NUM_NDM  3
// degrees of freedom per element
#define BC3D_NUM_DOF  18


namespace XC {
class ContactMaterial3D;


//! @brief 3D beam contact element.
//! The BeamContact3D element is a three-dimensional beam-to-node contact element which defines a frictional contact
//! interface between a beam element and a separate body. The retained nodes (6 DOF) are the endpoints of the beam
//! element, and the constrained node (3 DOF) is a node from a second body. The Lagrange multiplier node (3 DOF) is
//! required to enforce the contact condition. Each contact element should have a unique Lagrange multiplier node.
//! The Lagrange multiplier node should not be fixed, otherwise the contact condition will not work. 
//! @ingroup Elem  
class BeamContact3D : public ElemWithMaterial<BC3D_NUM_NODE, Contact3D>
  {
  private:
    // objects
    CrdTransf3d  *crdTransf;              // pointer to coordinate transformation object
   
    Vector theVector;                   // vector to return the residual
    mutable Matrix mTangentStiffness;           // Tangent Stiffness matrix
    mutable Vector mInternalForces;             // vector of Internal Forces
       
    // input quantities
    double mL;                   // length of Beam Element            
    double mRadius;              // radius of Pile Surface
    double mTolGap;              // gap tolerance
    double mTolForce;            // force tolerance
    int mIniContact;             // initial contact switch (0 = notInContact, 1 = inContact)
                                 // default is set for initially inContact
   
    // boolean variables
    bool inContact;
    bool was_inContact;
    bool to_be_released;
    bool should_be_released;
    bool in_bounds;    
    bool mInitialize;
   
    // calculation variables
    double mxi;                 // centerline projection coordinate: 0 <= xi <= 1
    double mchi;                // twist rotation from end 1 to end 2
    double mGap;                // current value of the gap
    double mLambda;             // current value of Lagrange Multiplier
    double mrho2;               // angular coord relating c2 to radial vector
    double mrho3;               // angular coord relating c3 to radial vector
   
    Matrix meye1;               // Identity Tensor
    Vector mg1;                 // tangent plane basis vector, g_xi
    Vector mg2;                 // tangent plane basis vector, g_psi
    Matrix mg_metric;           // metric tensor
    //  Matrix mG_metric;         // contravariant metric tensor
    Vector mn;                  // normal Vector
    Vector mH;                  // vector of Hermitian Basis Functions
    Vector mIcrd_a;             // initial coordinates of node a
    Vector mIcrd_b;             // initial coordinates of node b
    Vector mIcrd_s;             // initial coordinates of node s
    Vector mDcrd_a;             // current coordinates of node a
    Vector mDcrd_b;             // current coordinates of node b
    Vector mDcrd_s;             // current coordinates of node s
    //Vector mRot_a_n;            // total rotations of node a @ step n
    //Vector mRot_b_n;            // total rotations of node b @ step n
    Vector mDisp_a_n;           // total disps & rotations of node a @ step n
    Vector mDisp_b_n;           // total disps & rotations of node b @ step n
    Vector mDisp_s_n;           // total disps of node s @ step n
    Matrix mQa;                 // coordinate transform for node a
    Matrix mQb;                 // coordinate transform for node b
    Matrix mQc;
    Vector mc1;                 // tangent vector at project point c
    Vector mBn;                 // gap-displacement matrix
    Matrix mBs;                 // slip-displacement matrix
    Matrix mBphi;
    Vector mSlip;
   
    int MyTag;                  // element tag for debugging
    
    // member functions
    void set_transf(const CrdTransf *trf);
    double project(double xi);
    int UpdateBase(double xi);           // method to update base vectors g1 & g2
    void ComputeB(void);                 // method to compute Bn, Bs @ step n
    Matrix ComputeBphi(void);            // method to compute Bphi, used in ComputeB and update
    void UpdateTransforms(void);         // method to update Qa, Qb
    void ComputeQc(double xi);           // method to compute Qc from Qa and Qb
   
    Matrix ExponentialMap(Vector theta); // function returns the exponential map of a vector
    Matrix ComputeSkew(Vector theta);    // function returns skew matrix of given vector
    Vector CrossProduct(Vector &V1, Vector &V2); // cross product (does not exist in Vector Class!)
    Matrix Transpose(int dim1, int dim2, const Matrix &M);   // functions returns the tranpose of Matrix M (does not exist in Matrix Class!)
    
    const Vector &Geta1(void);                 // returns a1 = mQa(:,0)      
    const Vector &Getb1(void);                 // returns b1 = mQb(:,0)
    void Setc1(const Vector &);        // sets member vector c1
    const Vector &Getc1(void);                 // returns member vector c1
    const Vector &Getdx_c(double xi);          // returns dx_c / dxi
    const Vector &Getddx_c(double xi);         // returns d^2(x_c)/ dxi^2
  protected:
    int sendCoordTransf(int posFlag,const int &,const int &,Communicator &);
    CrdTransf3d *recvCoordTransf(int posFlag,const int &,const int &,const Communicator &);
    int sendData(Communicator &);
    int recvData(const Communicator &);
    void copy_members(const BeamContact3D &);
   
  public:
    BeamContact3D(int tag= 0, const ContactMaterial3D *mat= nullptr, const CrdTransf *crdTransf= nullptr);
    BeamContact3D(int tag, int Nd1, int Nd2,
                  int NdS, int NdL, double rad, const CrdTransf *,
                  ContactMaterial3D &, double tolG, double tolF, int cSwitch = 0);
    BeamContact3D(const BeamContact3D &);
    BeamContact3D &operator=(const BeamContact3D &);
    ~BeamContact3D(void);
    Element *getCopy(void) const;

    // public methods to obtain information about dof & connectivity    
    int getNumDOF(void) const;       
    void setDomain(Domain *theDomain);
    inline CrdTransf3d *getCoordTransf(void)
      { return crdTransf; }
    inline const CrdTransf3d *getCoordTransf(void) const
      { return crdTransf; }

    // public methods to set the state of the element    
    int commitState(void);
    int revertToLastCommit(void);        
    int revertToStart(void);        
    int update(void);
   
    // public methods to obtain stiffness, mass, damping and
    // residual information    
    const Matrix &getTangentStiff(void) const;
    const Matrix &getInitialStiff(void) const;    

    void zeroLoad(void);       
    int addLoad(ElementalLoad *theLoad, double loadFactor);
    int addInertiaLoadToUnbalance(const Vector &accel);
    const Vector &getResistingForce(void) const;
    const Vector &getResistingForceIncInertia(void) const;            

    // public methods for element output
    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);
    void Print(std::ostream &s, int flag =0) const;

    // Response *setResponse(const char **argv, int argc, Information &eleInfo);
    Response *setResponse(const std::vector<std::string> &argv, Parameter &param);
    int getResponse(int responseID, Information &eleInformation);

    // public methods for material stage update
    int setParameter(const std::vector<std::string> &argv, Parameter &param);
    int updateParameter(int parameterID, Information &info);
  };
} // end of XC namespace

#endif
