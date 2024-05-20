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

// Written: Kathryn A. Petek
// Created: 06/05
//
// Revisions
//    06/05 created
//    10/07 Peter Mackenzie-Helnwein: adding recorder features
//    11/10 F.Mckenna and C.McGann: changes for incorporation into main source code
//    02/11 C.McGann: added initial contact switch (default is inContact)
//
// Description: This file contains the implementation for the BeamContact3D class.

#include "BeamContact3D.h"
#include "domain/component/Parameter.h"
#include "domain/mesh/node/Node.h"
#include "material/nD/uw_materials/ContactMaterial3D.h"
#include "domain/mesh/element/utils/Information.h"
#include "utility/recorder/response/ElementResponse.h"
#include "domain/mesh/element/utils/coordTransformation/CrdTransf3d.h"

void XC::BeamContact3D::set_transf(const CrdTransf *trf)
  {
    if(crdTransf)
      {
        delete crdTransf;
        crdTransf= nullptr;
      }
    if(trf)
      {
        const CrdTransf3d *tmp= dynamic_cast<const CrdTransf3d *>(trf);
        if(tmp)
            crdTransf = tmp->getCopy();
        else
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
                      << "; failed to get copy of coordinate transformation.\n";
            exit(-1);
          }
      }
    else
        std::cerr << getClassName() << "::" << __FUNCTION__
                  << "; pointer to coordinate transformation is null."
                  << std::endl;
  }

//! @brief copy the values of the members from the other object.
void  XC::BeamContact3D::copy_members(const BeamContact3D &other)
  {
    set_transf(other.crdTransf);
    this->theVector= other.theVector; // vector to return the residual
    this->mTangentStiffness= other.mTangentStiffness; // Tangent Stiffness matrix
    this->mInternalForces= other.mInternalForces; // vector of Internal Forces
       
    // input quantities
    this->mL= other.mL; // length of Beam Element            
    this->mRadius= other.mRadius; // radius of Pile Surface
    this->mTolGap= other.mTolGap; // gap tolerance
    this->mTolForce= other.mTolForce; // force tolerance
    this->mIniContact= other.mIniContact; // initial contact switch (0 = notInContact, 1 = inContact)
                                 // default is set for initially inContact
   
    // boolean variables
    this->inContact= other.inContact; 
    this->was_inContact= other.was_inContact; 
    this->to_be_released= other.to_be_released; 
    this->should_be_released= other.should_be_released; 
    this->in_bounds= other.in_bounds; 
    this->mInitialize= other.mInitialize; 
   
    // calculation variables
    this->mxi= other.mxi; // centerline projection coordinate: 0 <= xi <= 1
    this->mchi= other.mchi; // twist rotation from end 1 to end 2
    this->mGap= other.mGap; // current value of the gap
    this->mLambda= other.mLambda; // current value of Lagrange Multiplier
    this->mrho2= other.mrho2; // angular coord relating c2 to radial vector
    this->mrho3= other.mrho3; // angular coord relating c3 to radial vector
   
    this->meye1= other.meye1; // Identity Tensor
    this->mg1= other.mg1; // tangent plane basis vector, g_xi
    this->mg2= other.mg2; // tangent plane basis vector, g_psi
    this->mg_metric= other.mg_metric; // metric tensor
    //  this->mG_metric= other.mG_metric; // contravariant metric tensor
    this->mn= other.mn; // normal Vector
    this->mH= other.mH; // vector of Hermitian Basis Functions
    this->mIcrd_a= other.mIcrd_a; // initial coordinates of node a
    this->mIcrd_b= other.mIcrd_b; // initial coordinates of node b
    this->mIcrd_s= other.mIcrd_s; // initial coordinates of node s
    this->mDcrd_a= other.mDcrd_a; // current coordinates of node a
    this->mDcrd_b= other.mDcrd_b; // current coordinates of node b
    this->mDcrd_s= other.mDcrd_s; // current coordinates of node s
    //this->mRot_a_n= other.mRot_a_n; // total rotations of node a @ step n
    //this->mRot_b_n= other.mRot_b_n; // total rotations of node b @ step n
    this->mDisp_a_n= other.mDisp_a_n; // total disps & rotations of node a @ step n
    this->mDisp_b_n= other.mDisp_b_n; // total disps & rotations of node b @ step n
    this->mDisp_s_n= other.mDisp_s_n; // total disps of node s @ step n
    this->mQa= other.mQa; // coordinate transform for node a
    this->mQb= other.mQb; // coordinate transform for node b
    this->mQc= other.mQc; 
    this->mc1= other.mc1; // tangent vector at project point c
    this->mBn= other.mBn; // gap-displacement matrix
    this->mBs= other.mBs; // slip-displacement matrix
    this->mBphi= other.mBphi; 
    this->mSlip= other.mSlip; 
   
    this->MyTag= other.MyTag; // element tag for debugging
  }

// constructors:
XC::BeamContact3D::BeamContact3D(int tag, int Nd1, int Nd2,
                                 int NdS, int NdL, double rad, const CrdTransf *coordTransf,
                                 ContactMaterial3D &theMat, double tolG, double tolF, int cSwitch)
    : ElemWithMaterial<BC3D_NUM_NODE, Contact3D>(tag,ELE_TAG_BeamContact3D, Contact3D(1,theMat)),
      crdTransf(nullptr),
      mTangentStiffness(BC3D_NUM_DOF, BC3D_NUM_DOF),
      mInternalForces(BC3D_NUM_DOF),
      mIniContact(cSwitch),
      meye1(BC3D_NUM_NDM,BC3D_NUM_NDM),
      mg1(BC3D_NUM_NDM),
      mg2(BC3D_NUM_NDM),
      mg_metric(2,2),
//   mG_metric(2,2),
      mn(BC3D_NUM_NDM),
      mH(4),
      mIcrd_a(BC3D_NUM_NDM),
      mIcrd_b(BC3D_NUM_NDM),
      mIcrd_s(BC3D_NUM_NDM),
      mDcrd_a(BC3D_NUM_NDM),
      mDcrd_b(BC3D_NUM_NDM),
      mDcrd_s(BC3D_NUM_NDM),
      //mRot_a_n(BC3D_NUM_NDM),
      // mRot_b_n(BC3D_NUM_NDM),
      mDisp_a_n(6),
      mDisp_b_n(6),
      mDisp_s_n(3),
      mQa(BC3D_NUM_NDM,BC3D_NUM_NDM),
      mQb(BC3D_NUM_NDM,BC3D_NUM_NDM),
      mQc(BC3D_NUM_NDM,BC3D_NUM_NDM),
      mc1(BC3D_NUM_NDM),
      mBn(BC3D_NUM_DOF-3),
      mBs(BC3D_NUM_DOF-3,2),
      mBphi(3,12)
  {
#ifdef DEBUG
    std::cerr << "XC::BeamContact3D::BeamContact3D(): " << MyTag << std::endl;
#endif
    this->theNodes.set_id_nodes(Nd1,Nd2,NdS,NdL);

    mRadius = rad;
    mTolGap = tolG;
    mTolForce = tolF;
    mIniContact = cSwitch;

    if (mIniContact == 0)
      {
        inContact          = true;
        was_inContact      = true;
        to_be_released     = false;
        should_be_released = false;
        in_bounds          = true;
      }
    else
      {
        inContact          = false;
        was_inContact      = false;
        to_be_released     = false;
        should_be_released = false;
        in_bounds          = true;
      }

    mGap    = 0.0;
    mLambda = 0.0;

    // set the transformation & material object
    set_transf(coordTransf);
    // element tag for debugging
    MyTag = tag;

    // set initialization to true for setDomain function
    mInitialize= true;
  }

XC::BeamContact3D::BeamContact3D(int tag, const ContactMaterial3D *ptr_mat, const CrdTransf *crdTransf)
  : ElemWithMaterial<BC3D_NUM_NODE, Contact3D>(tag,ELE_TAG_BeamContact3D, Contact3D(1,ptr_mat)),
     crdTransf(nullptr),
     mTangentStiffness(BC3D_NUM_DOF, BC3D_NUM_DOF),
     mInternalForces(BC3D_NUM_DOF),
     meye1(BC3D_NUM_NDM,BC3D_NUM_NDM),
     mg1(BC3D_NUM_NDM),
     mg2(BC3D_NUM_NDM),
     mg_metric(2,2),
//   mG_metric(2,2),
     mn(BC3D_NUM_NDM),
     mH(4),
     mIcrd_a(BC3D_NUM_NDM),
     mIcrd_b(BC3D_NUM_NDM),
     mIcrd_s(BC3D_NUM_NDM),
     mDcrd_a(BC3D_NUM_NDM),
     mDcrd_b(BC3D_NUM_NDM),
     mDcrd_s(BC3D_NUM_NDM),
     //mRot_a_n(BC3D_NUM_NDM),
     // mRot_b_n(BC3D_NUM_NDM),
     mDisp_a_n(6),
     mDisp_b_n(6),
     mDisp_s_n(3),
     mQa(BC3D_NUM_NDM,BC3D_NUM_NDM),
     mQb(BC3D_NUM_NDM,BC3D_NUM_NDM),
     mQc(BC3D_NUM_NDM,BC3D_NUM_NDM),
     mc1(BC3D_NUM_NDM),
     mBn(BC3D_NUM_DOF-3),
     mBs(BC3D_NUM_DOF-3,2),
     mBphi(3,12)
  {
#ifdef DEBUG
    std::cerr << "XC::BeamContact3D::BeamContact3D(): " << MyTag << std::endl;
#endif
    // set the transformation & material object
    set_transf(crdTransf);
    mRadius     = 0.0;
    mTolGap     = 0.0;
    mTolForce   = 0.0;
    mIniContact = 0;
    mGap        = 0.0;
    mLambda     = 0.0;

    inContact     = true;
    was_inContact = true;
    in_bounds     = true;
    to_be_released     = false;
    should_be_released = false;

    mInitialize = false;
  }

//! @brief Copy constructor.
XC::BeamContact3D::BeamContact3D(const BeamContact3D &other)
  :ElemWithMaterial<BC3D_NUM_NODE, Contact3D>(other), crdTransf(nullptr)
  {
    this->copy_members(other);
  }

//! @brief Assignment operator.
XC::BeamContact3D &XC::BeamContact3D::operator=(const BeamContact3D &other)
  {
    ElemWithMaterial<BC3D_NUM_NODE, Contact3D>::operator=(other);
    this->copy_members(other);
    return *this;
  }

//! @brief Destructor.
XC::BeamContact3D::~BeamContact3D(void)
  {
    if(crdTransf)
      delete crdTransf;
  }

//! @brief Virtual constructor.
XC::Element *XC::BeamContact3D::getCopy(void) const
  { return new BeamContact3D(*this); }

int XC::BeamContact3D::getNumDOF(void) const
  {
    return BC3D_NUM_DOF;
  }


void XC::BeamContact3D::setDomain(Domain *theDomain)
  {
    meye1.Zero();
    meye1(0,0) = 1.0;
    meye1(1,1) = 1.0;
    meye1(2,2) = 1.0;

    ElemWithMaterial<BC3D_NUM_NODE, Contact3D>::setDomain(theDomain);

    // only perform these steps during initial creation of element
    if(mInitialize)
      {
        //  initialize coordinate vectors and set to initial state
        mIcrd_a = theNodes[0]->getCrds();
        mIcrd_b = theNodes[1]->getCrds();
        mIcrd_s = theNodes[2]->getCrds();
        mDcrd_a = mIcrd_a;
        mDcrd_b = mIcrd_b;
        mDcrd_s = mIcrd_s;
        //mRot_a_n.Zero();
        //mRot_b_n.Zero();
        mDisp_a_n.Zero();
        mDisp_b_n.Zero();
        mDisp_s_n.Zero();

        // Fill the coordinate transformation matrices mQa, mQb
        // initialize the transformation
        if (crdTransf->initialize(theNodes[0], theNodes[1]))
          {
            std::cerr << "XC::BeamContact3D::setDomain(): Error initializing coordinate transformation";
            exit(0);
          }
        // Note: the following initialization is already performed in crdTransf
        //       but it doesn't return the values to beamContact element
        static Vector initXAxis(3);
        static Vector initYAxis(3);
        static Vector initZAxis(3);
        crdTransf->getLocalAxes(initXAxis, initYAxis, initZAxis);
        // fill mQa
        for(int i = 0; i < 3; i++)
	  {
            mQa(i,0) = initXAxis(i);
            mQa(i,1) = initYAxis(i);
            mQa(i,2) = initZAxis(i);
	  }
        // set mQb = mQa : beam column element requires zero initial twist
        // if mQa = mQb -> mchi = 0
        mQb = mQa;
        mchi = 0;

        // length of primary segment L
        mL = (mDcrd_b - mDcrd_a).Norm();

        // perform projection to update local coordinate along centerline
        //  of primary segment.  projection function also sets mn, mc1
        mxi = ((mDcrd_b - mDcrd_s)^(mDcrd_b - mDcrd_a)) / ((mDcrd_b - mDcrd_a)^(mDcrd_b - mDcrd_a)) ;  // initial approx
        // initial basis function values for use in projection
        mxi = project(mxi);

        // initialize contact state based on projection
        // in_bounds      = ( (mxi>=0.000) && (mxi<=1.0000) );
        in_bounds      = ( (mxi>0.000) && (mxi<1.0000) );
        inContact      = ( was_inContact && in_bounds );

        // update base vectors g1, g2 for determination of mg_metric
        UpdateBase(mxi);

        // adjust cohesion force
        //double area = 1.0*sqrt(mg_metric(0,0)*mg_metric(1,1)-mg_metric(0,1)*mg_metric(1,0));
        const double area= 1.0;
        ContactMaterial3D *contactMaterial= physicalProperties.getContactMaterial();
        contactMaterial->ScaleCohesion(area);
        contactMaterial->ScaleTensileStrength(area);
      }

    // compute mBn, mBs
    ComputeB();

    // call the base class method
    this->DomainComponent::setDomain(theDomain);
  }


int XC::BeamContact3D::commitState()
  {
#ifdef DEBUG
    std::cerr << "XC::BeamContact3D::commitState(): " << MyTag << std::endl;
#endif


    // update projection
    // (includes update of Qa, Qb & recalc of mn, mc1)
    mxi = project(mxi);

    // update tangent plane & metric tensor
    // (includes update of Qc, rho2, rho3, g1, g2)
    UpdateBase(mxi);

    // update Bn, Bs for next step
    ComputeB();

    // update Boolean Variables for contact condition
    was_inContact  = ( mGap < mTolGap );
    // in_bounds      = ( (mxi>=-0.0001) && (mxi<=1.0001) );
    in_bounds      = ( (mxi>0.000) && (mxi<1.000) );
    to_be_released = ( should_be_released || !in_bounds );
    inContact      = ( was_inContact && !to_be_released && in_bounds );

    /*              double tol = 1e-10;
                    if ( (mQa(0,0)-mQc(0,0)>tol) || (mQa(0,1)-mQc(0,1)>tol) ||(mQa(0,2)-mQc(0,2)>tol) ||
                             (mQa(1,0)-mQc(1,0)>tol) || (mQa(1,1)-mQc(1,1)>tol) ||(mQa(1,2)-mQc(1,2)>tol) ||
                             (mQa(2,0)-mQc(2,0)>tol) || (mQa(2,1)-mQc(2,1)>tol) ||(mQa(2,2)-mQc(2,2)>tol) ) {
                            std::cerr << "ELE: " << MyTag << std::endl;
                            std::cerr << "*** mQa neq. mQc *** "<< std::endl;
                            std::cerr << "mQa = "  << mQa << std::endl;
                            std::cerr << "mQc = " << mQc << std::endl;
                    }

                    if ( (mQa(0,0)-mQb(0,0)>tol) || (mQa(0,1)-mQb(0,1)>tol) ||(mQa(0,2)-mQb(0,2)>tol) ||
                             (mQa(1,0)-mQb(1,0)>tol) || (mQa(1,1)-mQb(1,1)>tol) ||(mQa(1,2)-mQb(1,2)>tol) ||
                             (mQa(2,0)-mQb(2,0)>tol) || (mQa(2,1)-mQb(2,1)>tol) ||(mQa(2,2)-mQb(2,2)>tol) ) {
                            std::cerr << "ELE: " << MyTag << std::endl;
                            std::cerr << "*** mQa neq. mQb *** "<< std::endl;
                            std::cerr << "mQa = "  << mQa << std::endl;
                            std::cerr << "mQb = " << mQb << std::endl;
                    }
                    */

    /*              Vector a1(3);
                    Vector b1(3);
                    Vector c2(3);
                    Vector c3(3);
                    Vector x_c_G(3);
                    double check1;
                    double check2;

                    a1 = Geta1();
                    b1 = Getb1();
                    for (int i=0; i<3; i++) {
                                    c2(i) = mQc(i,1);
                                    c3(i) = mQc(i,2);
                    }

                    x_c_G = mDcrd_a*mH(0) + a1*mH(1) + mDcrd_b*mH(2) + b1*mH(3) + mrho2*c2 + mrho3*c3;

                    check1 = mg1 ^ (mDcrd_s - x_c_G);
                    check2 = mg2 ^ (mDcrd_s - x_c_G);

                    std::cerr << "ELE = " << MyTag << std::endl;
                    std::cerr << "mDcrd_s = [ " << mDcrd_s << "];" << std::endl;
                    std::cerr << "x_c    = [ " << (mDcrd_a*mH(0) + a1*mH(1) + mDcrd_b*mH(2) + b1*mH(3)) << "];" << std::endl;
                    std::cerr << "r = [ " << (mrho2*c2 + mrho3*c3) << "];" << std::endl;
                    std::cerr << "x_c_G  = [ " << x_c_G << "];" << std::endl;
            std::cerr << "(x_s - x_c_G) = [" << (mDcrd_s - x_c_G) << " ];" << std::endl;
                    std::cerr << "check 1 = "  << check1 << ";      check 2 = " << check2 << ";" << std::endl;
                    std::cerr << std::endl; */


    int retVal = 0;
    // call element commitState to do any base class stuff
    if((retVal = this->ElemWithMaterial<BC3D_NUM_NODE, Contact3D>::commitState()) != 0)
      {
        std::cerr << "XC::BeamContact3D::commitState () - failed in base class";
      }
    retVal = physicalProperties.commitState();

    return retVal;

}


int XC::BeamContact3D::revertToLastCommit()
  {
    return physicalProperties.revertToLastCommit();
  }

int XC::BeamContact3D::revertToStart()
  {
    if (mIniContact == 0)
      {
        inContact          = true;
        was_inContact      = true;
        to_be_released     = false;
        should_be_released = false;
        in_bounds          = true;
      }
    else
      {
        inContact          = false;
        was_inContact      = false;
        to_be_released     = false;
        should_be_released = false;
        in_bounds          = true;
      }

    // reset applicable member variables
    mDcrd_a = mIcrd_a;
    mDcrd_b = mIcrd_b;
    mDcrd_s = mIcrd_s;
    mDisp_a_n.Zero();
    mDisp_b_n.Zero();
    mDisp_s_n.Zero();

    mL = (mDcrd_b - mDcrd_a).Norm();

    mxi = ((mDcrd_b - mDcrd_s)^(mDcrd_b - mDcrd_a))/((mDcrd_b - mDcrd_a)^(mDcrd_b - mDcrd_a));
    mxi = project(mxi);

    in_bounds      = ( (mxi>0.000) && (mxi<1.0000) );
    inContact      = ( was_inContact && in_bounds );

    UpdateBase(mxi);
    ComputeB();

    return physicalProperties.revertToStart();
  }

int XC::BeamContact3D::update(void)
  {
    if(mInitialize)
      {
        double tensileStrength;
        Vector a1(BC3D_NUM_NDM);
        Vector b1(BC3D_NUM_NDM);
        Vector a1_n(BC3D_NUM_NDM);
        Vector b1_n(BC3D_NUM_NDM);
        Vector disp_a(6);
        Vector disp_b(6);
        Vector disp_s(3);
        Vector disp_L(BC3D_NUM_NDM);
        Vector rot_a(BC3D_NUM_NDM);
        Vector rot_b(BC3D_NUM_NDM);
        Vector x_c(BC3D_NUM_NDM);
        Vector d(BC3D_NUM_NDM);

        // update secondary node coordinate
        mDcrd_s = mIcrd_s + theNodes[2]->getTrialDisp();

        // update Lagrange Multiplier Value
        disp_L = theNodes[3]->getTrialDisp();
        mLambda = disp_L(0);

        // update nodes a, b  coordinates & rotations
        disp_a = theNodes[0]->getTrialDisp();
        disp_b = theNodes[1]->getTrialDisp();
        disp_s = theNodes[2]->getTrialDisp();

        for(int i=0; i<3; i++)
	  {
            // compute updated nodal coordinates a, b
            mDcrd_a(i) = mIcrd_a(i) +  disp_a(i);
            mDcrd_b(i) = mIcrd_b(i) +  disp_b(i);
            // compute incremental rotations from step n to n+1
            //rot_a(i) = disp_a(i+3) - mRot_a_n(i);
            //rot_b(i) = disp_b(i+3) - mRot_b_n(i);
            rot_a(i) = disp_a(i+3) - mDisp_a_n(i+3);
            rot_b(i) = disp_b(i+3) - mDisp_b_n(i+3);
	  }

        // Get tangent vectors from previous converged step
        a1_n = Geta1();
        b1_n = Getb1();

        // Perform linear update of tangent vectors such that
        //  a1_{n+1} = a1_n + theta_{n->n+1}
        //  theta_{n->n+1} = rot_a x a1_n
        a1 = a1_n + CrossProduct(rot_a, a1_n);
        b1 = b1_n + CrossProduct(rot_b, b1_n);

        // update centerline projection coordinate
        // based upon coords & tangents at step n+1,  xi value @ step n
        x_c = mDcrd_a*mH(0) + a1*mH(1) + mDcrd_b*mH(2) + b1*mH(3);

        // update value of the gap
        d = mDcrd_s - x_c;
        mGap = (mn^d) - mRadius;

        // get tensile strength from material
	tensileStrength = physicalProperties.getContactMaterial()->getTensileStrength();

        // set boolean variable for release condition
        //should_be_released = ( mLambda <= -(tensileStrength + mTolForce ) );
        should_be_released = ( mLambda <= - mTolForce );

#ifdef DEBUG
        if (DEBUG_LEVEL > 0) {
            std::cerr << "   CONTACT:            " << inContact << std::endl;
            std::cerr << "   should be released: " << should_be_released << std::endl;
        }
#endif

        if(inContact)
	  {
            Vector strain(4);
            Vector slip(2);
            Vector phi_c(3);
            Vector c2n(3);
            Vector c3n(3);
            Vector c2n1(3);
            Vector c3n1(3);
            Vector incDisp_ab(12);
            Vector incDisp_s(3);
            Vector dstar(3);

            for(int i=0; i<3; i++)
	      {
                c2n(i) = mQc(i,1);
                c3n(i) = mQc(i,2);
                incDisp_ab(i)   = disp_a(i)   - mDisp_a_n(i);
                incDisp_ab(i+3) = rot_a(i);
                incDisp_ab(i+6) = disp_b(i)   - mDisp_b_n(i);
                incDisp_ab(i+9) = rot_b(i);
	      }

            incDisp_s = disp_s - mDisp_s_n;

            phi_c = mBphi * incDisp_ab;

            c2n1 = c2n + CrossProduct(phi_c, c2n);
            c3n1 = c3n + CrossProduct(phi_c, c3n);

            dstar = mDcrd_s - x_c - mrho2*c2n1 - mrho3*c3n1;

            slip(0) = mg1 ^ dstar;
            slip(1) = mg2 ^ dstar;

            // slip is treated in local (curvilinear) coordinates
            strain(0) = mGap;
            strain(1) = slip(0);
            strain(2) = slip(1);
            strain(3) = mLambda;
	    physicalProperties[0]->setTrialStrain(strain);
            mSlip = slip;
	  }
	else if (to_be_released)
	  {
            // prevents sliding & stabilizes behavior in lag step
            Vector strain(4);

            strain(0) = mGap;
            strain(1) = 0.0;
            strain(2) = 0.0;
            strain(3) = mLambda;
	    physicalProperties[0]->setTrialStrain(strain);
            mSlip.Zero();
	  }
      }
    mInitialize = true;
    return 0;
  }

double XC::BeamContact3D::project(double xi)
  {
#ifdef DEBUG
    std::cerr << "XC::BeamContact3D::project(): " << MyTag << std::endl;
#endif

    double xi_P;                                    // local value of xi
    double H1;                                              // local value of Hermitian Function, H1
    double H2;                                              // local value of Hermitian Function, H2
    double H3;                                              // local value of Hermitian Function, H3
    double H4;                                              // local value of Hermitian Function, H4
    double R;
    double DR;
    double dxi;                                             // change in xi
    double xi_P_squared;
    double xi_P_cubed;
    Vector a1(BC3D_NUM_NDM);                // tangent at end a
    Vector b1(BC3D_NUM_NDM);                // tangent at end b
    Vector x_c_P(BC3D_NUM_NDM);             // current centerline projection coordinate
    Vector d(BC3D_NUM_NDM);                 // distance from secondary node to centerline coord
    Vector tc(BC3D_NUM_NDM);                // tangent at projection point = 1st deriv of x_c
    Vector ddx_c(BC3D_NUM_NDM);             // 2nd derivative of x_c

    // initialize xi_P to previous value of xi
    xi_P = xi;

    // Perform exponential update of coordinate transforms Qa, Qb
    UpdateTransforms();
    // set tangent vectors from Qa, Qb for calc of x_c_P
    a1 = Geta1();
    b1 = Getb1();

    // calculate current projection location
    xi_P_squared = xi_P*xi_P;
    xi_P_cubed = xi_P*xi_P_squared;
    H1 = 1.0-3.0*xi_P_squared + 2.0*xi_P_cubed;
    H2 = (xi_P - 2.0*xi_P_squared + xi_P_cubed) * mL;
    H3 = 1.0 - H1;
    H4 = (-xi_P_squared + xi_P_cubed) * mL;
    x_c_P = mDcrd_a*H1 + a1*H2 + mDcrd_b*H3 + b1*H4;

    d = mDcrd_s - x_c_P;
    tc = Getdx_c(xi_P);
    R = (d^tc);

    // Iterate to determine a new value of xi
    //    such that:  normal ^ tangent = d ^ tc = 0
    int Gapcount = 0;
    while (fabs(R/mL) > mTolGap && Gapcount < 50)
      {
        ddx_c = Getddx_c(xi_P);
        DR = (d^ddx_c) - (tc^tc);
        dxi = -R / DR;
        xi_P = xi_P + dxi;

        xi_P_squared = xi_P*xi_P;
        xi_P_cubed = xi_P*xi_P_squared;
        H1 = 1.0-3.0*xi_P_squared + 2.0*xi_P_cubed;
        H2 = (xi_P - 2.0*xi_P_squared + xi_P_cubed) * mL;
        H3 = 1.0 - H1;
        H4 = (-xi_P_squared + xi_P_cubed) * mL;

        x_c_P = mDcrd_a*H1 + a1*H2 + mDcrd_b*H3 + b1*H4;
        d = mDcrd_s - x_c_P;
        tc = Getdx_c(xi_P);
        R = (d^tc);
        Gapcount +=1;
      }

    // update norm, n, for current projection
    mn = (mDcrd_s - x_c_P) / ( (mDcrd_s - x_c_P).Norm() );

    // set value of c1 for current projection for use in ComputeQc
    Setc1( (tc/tc.Norm()) );

    // update Hermitian Basis functions for use in ComputeB
    //    and for use in next time step in function update()
    mH(0) = H1;
    mH(1) = H2;
    mH(2) = H3;
    mH(3) = H4;

    return xi_P;
  }


int XC::BeamContact3D::UpdateBase(double xi)
// this function calculates g1, g2, mg_metric, and sends value of metric tensor to the material
  {
#ifdef DEBUG
    std::cerr << "XC::BeamContact3D::UpdateBase(): " << MyTag << std::endl;
#endif

    //double rho2;                          // angular coord relating c2 to radial vector
    //double rho3;                          // angular coord relating c3 to radial vector
    Vector c1(BC3D_NUM_NDM);
    Vector c2(BC3D_NUM_NDM);        // c1, c2, c3 are coord transf. vectors at projected point
    Vector c3(BC3D_NUM_NDM);
    Vector tc(BC3D_NUM_NDM);    // tangent at projected point = 1st deriv of x_c
    Vector ddx_c(BC3D_NUM_NDM); // 2nd deriv of x_c w.r.t. xi
    Vector d_c1(BC3D_NUM_NDM);  // deriv of c1 w.r.t xi
    Vector d_c2(BC3D_NUM_NDM);  // deriv of c2 w.r.t xi
    Vector d_c3(BC3D_NUM_NDM);  // deriv of c3 w.r.t xi
    //Vector g1(BC3D_NUM_NDM);      // tangent plane basis vector, g_xi
    //Vector g2(BC3D_NUM_NDM);    // tangent plane basis vector, g_psi
    Matrix Qc(BC3D_NUM_NDM, BC3D_NUM_NDM);  // coord transf at projected point
    Vector g1(BC3D_NUM_NDM);    // temporary vector for g1 (co-variant)
    Vector g2(BC3D_NUM_NDM);    // temporary vector for g2 (co-variant)
    Vector test(3);

    // Update interpolated coordinate transform, Qc
    ComputeQc(xi);  // sets new value of mQc
    Qc = mQc;

    // obtain c1, c2, c3 vectors
    c1 = Getc1();
    for(int i=0; i<3; i++)
      {
        // c1(i) = Qc(i,0);  if //: c1 = tc/norm(tc);
        test(i) = c1(i) - Qc(i,0);
        c2(i) = Qc(i,1);
        c3(i) = Qc(i,2);
      }

//std::cerr << "ELE : " << MyTag << "     test.norm() = " << test.Norm() << ";   test = [" << test << "];" << std::endl;

    // calculate local coordinates rho2, rho3 for radial vector, r_vec
    //  where: r_vec = rho2*c2 + rho3*c3
    mrho2 = mRadius * (mn^c2); //  = cos(psi)*mRadius
    mrho3 = mRadius * (mn^c3); //  = sin(psi)*mRadius

    // calculate all derivatives
    tc = Getdx_c(xi);
    ddx_c = Getddx_c(xi);
    d_c1 = ddx_c - ((c1^ddx_c)*c1);
    d_c1 = d_c1/(tc.Norm());
    d_c2 = -(d_c1 ^ c2)*c1 + mchi*c3;
    d_c3 = -(d_c1 ^ c3)*c1 - mchi*c2;

    // calculate tangent plane vectors, g1 & g2
    // g1 = d(x_c + r_vec)/dxi
    // g2 = d(x_c + r_vec)/dpsi
    g1 = tc + mrho2*d_c2 + mrho3*d_c3;
    g2 = -mrho3 * c2 + mrho2 * c3;

    // fill metric tensor (covariant)
    mg_metric(0,0) = g1^g1;
    mg_metric(0,1) = g1^g2;
    mg_metric(1,0) = mg_metric(0,1);
    mg_metric(1,1) = g2^g2;

    physicalProperties.getContactMaterial()->setMetricTensor(mg_metric);

    Matrix G_metric(2,2);  // contravariant
    double det = (mg_metric(0,0) * mg_metric(1,1)) - (mg_metric(0,1) * mg_metric(1,0)) ;
    G_metric(0,0) =  mg_metric(1,1);
    G_metric(1,0) = -mg_metric(1,0);
    G_metric(0,1) = -mg_metric(0,1);
    G_metric(1,1) =  mg_metric(0,0);
    G_metric = G_metric / det;

    // transform mg1, mg2 to contravariant form
    mg1 = G_metric(0,0)*g1 + G_metric(0,1)*g2;
    mg2 = G_metric(1,0)*g1 + G_metric(1,1)*g2;

    return 0;
  }

void XC::BeamContact3D::ComputeB(void)
  {
#ifdef DEBUG
    std::cerr << "XC::BeamContact3D::ComputeB(): " << MyTag << std::endl;
#endif

    // vector n used below should be of same step

    // initialize Bn, Bs;
    mBn.Zero();
    mBs.Zero();

    Vector a1(3);
    Vector b1(3);
    Vector a1xn(3);
    Vector b1xn(3);
    Vector r(3);
    Vector rxg1(3);
    Vector rxg2(3);

    Matrix Bx(3,12);

    a1 = Geta1();
    b1 = Getb1();
    a1xn = CrossProduct(a1, mn);
    b1xn = CrossProduct(b1, mn);

    r(0) = mrho2*mQc(0,1) + mrho3*mQc(0,2);
    r(1) = mrho2*mQc(1,1) + mrho3*mQc(1,2);
    r(2) = mrho2*mQc(2,1) + mrho3*mQc(2,2);

    rxg1 = CrossProduct(r,mg1);
    rxg2 = CrossProduct(r,mg2);
    //rxg1 = -1*rxg1;
    //rxg2 = -1*rxg2;

    // dgap = Bn' * dq
    mBn(0)  = mn(0)*mH(0);
    mBn(1)  = mn(1)*mH(0);
    mBn(2)  = mn(2)*mH(0);

    mBn(3)  = a1xn(0)*mH(1);
    mBn(4)  = a1xn(1)*mH(1);
    mBn(5)  = a1xn(2)*mH(1);

    mBn(6)  = mn(0)*mH(2);
    mBn(7)  = mn(1)*mH(2);
    mBn(8)  = mn(2)*mH(2);

    mBn(9)  = b1xn(0)*mH(3);
    mBn(10) = b1xn(1)*mH(3);
    mBn(11) = b1xn(2)*mH(3);

    mBn(12) =  -mn(0);
    mBn(13) =  -mn(1);
    mBn(14) =  -mn(2);

    // Derivatives of H
    double dH1 =      - 6.0*mxi + 6.0*mxi*mxi;
    double dH2 =  1.0 - 4.0*mxi + 3.0*mxi*mxi;
    double dH3 =        6.0*mxi - 6.0*mxi*mxi;
    double dH4 =      - 2.0*mxi + 3.0*mxi*mxi;

    Matrix At(3,3);
    Matrix Bt(3,3);
    Matrix Ct(3,3);
    Matrix Dt(3,3);
    Matrix Et(3,3);
    Matrix Ft(3,3);
    Matrix Gt(3,3);
    Matrix Ht(3,3);
    Matrix Qct(3,3);
    Matrix temp1(3,3);
    Matrix temp2(3,3);
    Matrix temp3(3,3);

    At.Zero();
    At(0,0) = mH(0);
    At(1,1) = mH(0);
    At(2,2) = mH(0);

    Bt.Zero();
    temp1.Zero();
    temp1 = ComputeSkew(a1);
    Bt = 1*mH(1) * temp1;  // sign should be +:  -(a1_skew^T) = -(-a1_skew) = + a1_skew

    Ct.Zero();
    Ct(0,0) = mH(2);
    Ct(1,1) = mH(2);
    Ct(2,2) = mH(2);

    Dt.Zero();
    temp1.Zero();
    temp1 = ComputeSkew(b1);
    Dt = 1*mH(3) * temp1;  // sign should be +:  -(b1_skew^T) = -(-b1_skew) = + b1_skew

    Qct = Transpose(3, 3, mQc);

    Et.Zero();
    temp1.Zero();
    temp1(0,1) = -mQa(0,2);
    temp1(0,2) =  mQa(0,1);
    temp1(1,1) = -mQa(1,2);
    temp1(1,2) =  mQa(1,1);
    temp1(2,1) = -mQa(2,2);
    temp1(2,2) =  mQa(2,1);
    Et = (dH1/mL) * temp1 * Qct;

    temp1.Zero();
    temp1(0,0) = (1-mxi)* mQa(0,0);
    temp1(0,1) =    dH2 * mQa(0,1);
    temp1(0,2) =    dH2 * mQa(0,2);
    temp1(1,0) = (1-mxi)* mQa(1,0);
    temp1(1,1) =    dH2 * mQa(1,1);
    temp1(1,2) =    dH2 * mQa(1,2);
    temp1(2,0) = (1-mxi)* mQa(2,0);
    temp1(2,1) =    dH2 * mQa(2,1);
    temp1(2,2) =    dH2 * mQa(2,2);
    Ft = temp1 * Qct;

    Gt.Zero();
    temp1.Zero();
    temp1(0,1) = -mQb(0,2);
    temp1(0,2) =  mQb(0,1);
    temp1(1,1) = -mQb(1,2);
    temp1(1,2) =  mQb(1,1);
    temp1(2,1) = -mQb(2,2);
    temp1(2,2) =  mQb(2,1);
    Gt = (dH3/mL) * temp1 * Qct;

    Ht.Zero();
    temp1(0,0) = mxi * mQb(0,0);
    temp1(0,1) = dH4 * mQb(0,1);
    temp1(0,2) = dH4 * mQb(0,2);
    temp1(1,0) = mxi * mQb(1,0);
    temp1(1,1) = dH4 * mQb(1,1);
    temp1(1,2) = dH4 * mQb(1,2);
    temp1(2,0) = mxi * mQb(2,0);
    temp1(2,1) = dH4 * mQb(2,1);
    temp1(2,2) = dH4 * mQb(2,2);
    Ht = temp1 * Qct;

    // Fill Bs(0:2,0), Bs(0:2,1)
    Vector col1(3);
    Vector col2(3);
    col1.Zero();
    col2.Zero();
    col1 = At*mg1 + Et*rxg1;
    col2 = At*mg2 + Et*rxg2;
    for(int i = 0; i < 3; i++)
      {
        mBs(i,0) = -col1(i);
        mBs(i,1) = -col2(i);
      }

    // Fill Bs(3:5,0), Bs(3:5,1)
    col1 = Bt*mg1 + Ft*rxg1;
    col2 = Bt*mg2 + Ft*rxg2;
    for(int i = 0; i < 3; i++)
      {
        mBs(i+3,0) = -col1(i);
        mBs(i+3,1) = -col2(i);
      }

    // Fill Bs(6:8,0), Bs(6:8,1)
    col1 = Ct*mg1 + Gt*rxg1;
    col2 = Ct*mg2 + Gt*rxg2;
    for(int i = 0; i < 3; i++)
      {
        mBs(i+6,0) = -col1(i);
        mBs(i+6,1) = -col2(i);
      }

    // Fill Bs(9:11,0), Bs(9:11,1)
    col1 = Dt*mg1 + Ht*rxg1;
    col2 = Dt*mg2 + Ht*rxg2;
    for(int i = 0; i < 3; i++)
      {
        mBs(i+9,0) = -col1(i);
        mBs(i+9,1) = -col2(i);
      }

    // Fill Bs(12:14,0), Bs(12:14,1)
    for(int i = 0; i < 3; i++)
      {
        mBs(i+12,0) = mg1(i);
        mBs(i+12,1) = mg2(i);
      }

    mBphi = ComputeBphi();
  }


XC::Matrix XC::BeamContact3D::ComputeBphi(void)
  {
#ifdef DEBUG
    std::cerr << "XC::BeamContact3D::ComputeB(): " << MyTag << std::endl;
#endif
    Matrix dummy1(3,3);
    Matrix dummy2(3,3);
    Matrix dummy3(3,3);
    Matrix Bphi(3,12);

    Bphi.Zero();

    // Derivatives of H
    double dH1 =      - 6.0*mxi + 6.0*mxi*mxi;
    double dH2 =  1.0 - 4.0*mxi + 3.0*mxi*mxi;
    double dH3 =        6.0*mxi - 6.0*mxi*mxi;
    double dH4 =      - 2.0*mxi + 3.0*mxi*mxi;

    // Compute Bphi(0:2, 3:5)
    dummy1.Zero();
    dummy2.Zero();
    dummy3.Zero();
    dummy1(0,0) = (1-mxi)*mQc(0,0);  // N1 * mQc(0:2,0)
    dummy1(1,0) = (1-mxi)*mQc(1,0);
    dummy1(2,0) = (1-mxi)*mQc(2,0);
//              dummy1(0,0) = mxi*mQc(0,0);  // N1 * mQc(0:2,0)
//              dummy1(1,0) = mxi*mQc(1,0);
//              dummy1(2,0) = mxi*mQc(2,0);

    dummy1(0,1) = dH2*mQc(0,1);  // dH2 * mQc(0:2,1:2)
    dummy1(1,1) = dH2*mQc(1,1);
    dummy1(2,1) = dH2*mQc(2,1);
    dummy1(0,2) = dH2*mQc(0,2);
    dummy1(1,2) = dH2*mQc(1,2);
    dummy1(2,2) = dH2*mQc(2,2);
    dummy2 = Transpose(BC3D_NUM_NDM, BC3D_NUM_NDM, mQa);
    dummy3 = dummy1*dummy2;
    for(int i = 0; i < 3; i++)
      {
        for(int j = 0; j < 3; j++)
          {
            Bphi(i,3+j) = dummy3(i,j);
            //Bphi(i,3+j) = dummy3(j,i);
          }
      }

    // Reuse dummy2 and Compute Bphi(0:2, 0:2)
    dummy1.Zero();
    dummy3.Zero();
    dummy1(0,1) =  mQc(0,2);
    dummy1(0,2) = -mQc(0,1);
    dummy1(1,1) =  mQc(1,2);
    dummy1(1,2) = -mQc(1,1);
    dummy1(2,1) =  mQc(2,2);
    dummy1(2,2) = -mQc(2,1);
    dummy3 = dummy1*dummy2;
    for(int i = 0; i < 3; i++)
      {
        for(int j = 0; j < 3; j++)
          {
            Bphi(i,j) = dH1/mL * dummy3(i,j);
            //Bphi(i,j) = dH1/mL * dummy3(j,i);
          }
      }

    // Reuse dummy1 and Compute Bphi(0:2, 6:8)
    dummy2.Zero();
    dummy3.Zero();
    dummy2 = Transpose(BC3D_NUM_NDM, BC3D_NUM_NDM, mQb);
    dummy3 = dummy1*dummy2;
    for(int i = 0; i < 3; i++)
      {
        for(int j = 0; j < 3; j++)
          {
            Bphi(i,6+j) = dH3/mL * dummy3(i,j);
            //Bphi(i,6+j) = dH3/mL * dummy3(j,i);
          }
      }

    // Ruse dummy2 and Compute Bphi(0:2, 9:11)
    dummy1.Zero();
    dummy3.Zero();
    dummy1(0,0) = mxi*mQc(0,0);  // N2 * mQc(0:2,0)
    dummy1(1,0) = mxi*mQc(1,0);
    dummy1(2,0) = mxi*mQc(2,0);
//              dummy1(0,0) = (1-mxi)*mQc(0,0);  // N2 * mQc(0:2,0)
//              dummy1(1,0) = (1-mxi)*mQc(1,0);
//              dummy1(2,0) = (1-mxi)*mQc(2,0);

    dummy1(0,1) = dH4*mQc(0,1);     // dH4 * mQc(0:2,1:2)
    dummy1(1,1) = dH4*mQc(1,1);
    dummy1(2,1) = dH4*mQc(2,1);
    dummy1(0,2) = dH4*mQc(0,2);
    dummy1(1,2) = dH4*mQc(1,2);
    dummy1(2,2) = dH4*mQc(2,2);
    dummy3 = dummy1*dummy2;
    for(int i = 0; i < 3; i++)
      {
        for(int j = 0; j < 3; j++)
          {
            Bphi(i,9+j) = dummy3(i,j);
            //Bphi(i,9+j) = dummy3(i,j);
          }
      }
    return Bphi;
  }

void XC::BeamContact3D::UpdateTransforms(void)
  {
#ifdef DEBUG
    std::cerr << "XC::BeamContact3D::UpdateTransforms(): " << MyTag << std::endl;
#endif

    Vector disp_a(6);           // trial disp/rot vector at a(total disp/rot)
    Vector disp_b(6);           // trial disp/rot vector at a(total disp/rot)
    Vector rot_a(BC3D_NUM_NDM);  // incr. rot vector at a (from n->n+1)
    Vector rot_b(BC3D_NUM_NDM);  // incr. rot vector at a (from n->n+1)
    Matrix Omega(BC3D_NUM_NDM,BC3D_NUM_NDM);  // Matrix used for Exponential Map

    // Recalculate incremental rotations from step n to n+1
    disp_a = theNodes[0]->getTrialDisp();
    disp_b = theNodes[1]->getTrialDisp();
    //for (i=0; i<3; i++) {
    for(int i = 3; i<6; i++)
      {
        //rot_a(i) = disp_a(i+3) - mRot_a_n(i);
        //rot_b(i) = disp_b(i+3) - mRot_b_n(i);
        rot_a(i-3) = disp_a(i) - mDisp_a_n(i);
        rot_b(i-3) = disp_b(i) - mDisp_b_n(i);
      }

    // Perform exponential update of Qa
    //   calculate exponential map of current incremental rotations
    Omega = ExponentialMap(rot_a);
    //   calculate new Qa
    mQa = Omega*mQa;

    // Perform exponential update of Qb
    //   calculate exponential map of current incremental rotations
    Omega = ExponentialMap(rot_b);
    //   calculate new Qb
    mQb = Omega*mQb;

    /* ************************************************************
    // TRY LINEAR UPDATE INSTEAD!
    Omega = ComputeSkew(rot_a);
    Omega = Omega + meye1;
    mQa = Omega*mQa;

    Omega = ComputeSkew(rot_b);
    Omega = Omega + meye1;
    mQb = Omega*mQb;
    ***************************************************************************************** */

    // Reset total disp & rotation vectors for calculation of
    // incremental values in subsequent step
    //for (i=0; i<3; i++) {
    for(int i=0; i<6; i++)
      {
        //mRot_a_n(i) = disp_a(i+3);
        //mRot_b_n(i) = disp_b(i+3);
        mDisp_a_n(i) = disp_a(i);
        mDisp_b_n(i) = disp_b(i);
      }

    mDisp_s_n = theNodes[2]->getTrialDisp();
  }

void XC::BeamContact3D::ComputeQc(double xi)
  {
#ifdef DEBUG
    std::cerr << "XC::BeamContact3D::ComputeQc(): " << MyTag << std::endl;
#endif
    Vector c1(BC3D_NUM_NDM);        // tangent vector at projection point, c
    Vector a1(BC3D_NUM_NDM);        // tangent vector at a
    Vector b1(BC3D_NUM_NDM);        // tangent vector at b
    Vector temp(BC3D_NUM_NDM);          // dummy vector for use in calcs
    Matrix Qc_df(BC3D_NUM_NDM,BC3D_NUM_NDM);   // Drill free transformation matrix for c
    Matrix Qc_chi(BC3D_NUM_NDM,BC3D_NUM_NDM);  // Twist transformation matrix for c
    Matrix Qb_df(BC3D_NUM_NDM,BC3D_NUM_NDM);   // Drill free transf. matrix from a to b
    //Matrix Qc(BC3D_NUM_NDM,BC3D_NUM_NDM);      // Total transformation matrix at projection point, c

    // Fill tangent vectors
    a1 = Geta1();
    b1 = Getb1();
    c1 = Getc1();
    temp.Zero();

    // Calculate the drill free transformation from a to c, Qc_df
    temp = CrossProduct(a1,c1);
    Qc_df = ExponentialMap(temp);

    // Calculate the drill free transformation from a to b, Qb_df
    // for determination of twist angle mchi
    temp = CrossProduct(a1,b1);
    Qb_df = ExponentialMap(temp);
    Qb_df = Qb_df * mQa;

    // mchi = arcsin( b3 dot b2_df) = arcsin(mQb(:,2) dot Qb_df(:,1))
    // WATCH SIGN!!!!
    mchi = mQb(0,2)*Qb_df(0,1) + mQb(1,2)*Qb_df(1,1) + mQb(2,2)*Qb_df(2,1);
    mchi = -asin(mchi);

    // Calculate twist transformation from a to c, Qc_df
    // based upon linear scaling of twist angle: mxi * mchi * c1
    temp = mxi*mchi*c1;
    Qc_chi = ExponentialMap(temp);

    mQc = (Qc_chi * Qc_df) * mQa;
  }


XC::Matrix XC::BeamContact3D::ExponentialMap(Vector th)
  {
//#ifdef DEBUG
//        std::cerr << "XC::BeamContact3D::ExponentialMap(): " << MyTag << std::endl;
//#endif

// This function transforms a vector th to a skew symmetric matrix
// and performs an exponential map to transform the skew symmetric
// matrix into an orthogonal matrix.

//  exp[sk_theta] = cos(theta)*1 + sin(theta)/theta * sk_theta +
//                   (1-cos(theta))/(theta^2) * theta_vec^T*theta_vec
//
//                = sf1*1 + sf2*k_theta + sf3*theta_vec^T*theta_vec
//
//  approximations are used for small theta when necessary

    double theta;                                  // vector norm
    Vector theta_vec(BC3D_NUM_NDM);                // input vector
    Matrix sk_theta(BC3D_NUM_NDM,BC3D_NUM_NDM);    // skew of vector
    Matrix theta_theta(BC3D_NUM_NDM,BC3D_NUM_NDM); // dyadic product of vector
    Matrix Q(BC3D_NUM_NDM,BC3D_NUM_NDM);           // Exonential Map Vector

    double sf1;  // local variables for calculation of exp. map terms
    double sf2;
    double sf3;

    Q.Zero();
    sk_theta.Zero();
    theta_theta.Zero();

    theta_vec = th;
    theta = theta_vec.Norm();
    sk_theta = ComputeSkew(theta_vec);
    for(int i=0; i<3; i++)
      {
        for(int j=0; j<3; j++)
	  {
            theta_theta(i,j) = theta_vec(i) * theta_vec(j);
          }
      }

    sf1 = cos(theta);

    if(theta > 5.0e-3)
      { sf2 = (sin(theta))/theta; }
    else
      {
        // small theta approximation
        sf2 = 1.0 - theta*theta/6.0 + pow(theta,4.0)/120.0;
      }

    if(theta > 0.1)
      { sf3 = (1.0-cos(theta))/(theta*theta); }
    else
      {
        // small theta approximation
        sf3 = 0.5 - theta*theta/24.0 + pow(theta, 4.0)/720.0
              -pow(theta,6.0)/40320.0 + pow(theta, 8.0)/3628800.0;
      }

    Q = sf1*meye1 + sf2*sk_theta + sf3*theta_theta;

    return Q;
  }



XC::Matrix XC::BeamContact3D::ComputeSkew(Vector th)
  {
//#ifdef DEBUG
//        std::cerr << "XC::BeamContact3D::ComputeSkew(): " << MyTag << std::endl;
//#endif

    Matrix skew_th(BC3D_NUM_NDM,BC3D_NUM_NDM);

    skew_th(0,0) =  0.0;
    skew_th(0,1) = -th(2);
    skew_th(0,2) =  th(1);
    skew_th(1,0) =  th(2);
    skew_th(1,1) =  0.0;
    skew_th(1,2) = -th(0);
    skew_th(2,0) = -th(1);
    skew_th(2,1) =  th(0);
    skew_th(2,2) =  0.0;

    return skew_th;
  }


XC::Vector XC::BeamContact3D::CrossProduct(Vector &V1, Vector &V2)
  {
//#ifdef DEBUG
//        std::cerr << "XC::BeamContact3D::CrossProduct(): " << MyTag << std::endl;
//#endif

    Vector V3(3);

    V3(0) = V1(1)*V2(2) - V1(2)*V2(1);
    V3(1) = V1(2)*V2(0) - V1(0)*V2(2);
    V3(2) = V1(0)*V2(1) - V1(1)*V2(0);

    return V3;

  }

XC::Matrix XC::BeamContact3D::Transpose( int dim1, int dim2, const Matrix &M )
  {
// copied from transpose function in Brick.cpp

    Matrix Mtran(dim2,dim1);

    for(int i = 0; i < dim1; i++ )
      {
        for( int j = 0; j < dim2; j++ )
            Mtran(j,i) = M(i,j) ;
      } // end for i
    return Mtran ;
  }



const XC::Vector &XC::BeamContact3D::Geta1(void)
  {
    static Vector a1(BC3D_NUM_NDM);

    for(int i=0; i<3; i++)
      { a1(i) = mQa(i,0); }

    return a1;
  }

const XC::Vector &XC::BeamContact3D::Getb1(void)
  {
    static Vector b1(BC3D_NUM_NDM);
    for(int i=0; i<3; i++)
      {
        b1(i) = mQb(i,0);
      }
    return b1;
  }

void XC::BeamContact3D::Setc1(const Vector &c1_vec)
  {
    mc1 = c1_vec;
  }

const XC::Vector &XC::BeamContact3D::Getc1(void)
  {
    return mc1;
  }



const XC::Vector &XC::BeamContact3D::Getdx_c(double xi)
  {
// ? Is this function necessary?  Calculate directly at place used instead?
    static Vector deriv1(BC3D_NUM_NDM);

    const Vector &a1= Geta1();
    const Vector &b1= Getb1();
    const double xi_squared = xi*xi;

    deriv1 = (    - 6.0*xi + 6.0*xi_squared)*mDcrd_a
             + (1.0 - 4.0*xi + 3.0*xi_squared)*mL*a1
             + (      6.0*xi - 6.0*xi_squared)*mDcrd_b
             + (    - 2.0*xi + 3.0*xi_squared)*mL*b1;

    return deriv1;
  }

const XC::Vector &XC::BeamContact3D::Getddx_c(double xi)
  {
// ? Is this function necessary?  Calculate directly at place used instead?
    static Vector deriv2(BC3D_NUM_NDM);

    const Vector &a1= Geta1();
    const Vector &b1= Getb1();

    deriv2 = ( - 6.0 + 12.0*xi)*mDcrd_a
             + ( - 4.0 +  6.0*xi)*mL*a1
             + (   6.0 - 12.0*xi)*mDcrd_b
             + ( - 2.0 +  6.0*xi)*mL*b1;

    return deriv2;
  }


const XC::Matrix &XC::BeamContact3D::getTangentStiff(void) const
  {
    // initialize Kt
    mTangentStiffness.Zero();

    if(inContact)
      {                // in contact
        Matrix Cmat = physicalProperties[0]->getTangent();

        double Cnl;
        Vector Csl(2);
        Matrix Css(2,2);

        Cnl      = Cmat(0,3);
        Csl(0)   = Cmat(1,3);
        Csl(1)   = Cmat(2,3);
        Css(0,0) = Cmat(1,1);
        Css(0,1) = Cmat(1,2);
        Css(1,0) = Cmat(2,1);
        Css(1,1) = Cmat(2,2);

        // frictionless contact part
        if (Cnl != 0.0)
	  {
#ifdef DEBUG
            if (DEBUG_LEVEL > 0)
	      {
                std::cerr << "   ** tangent: normal" << std::endl;
	      }
#endif
            // assume Cnl == 1 (!)
            for(int i = 0; i < BC3D_NUM_DOF - 3; i++)
	      {
                mTangentStiffness(i,BC3D_NUM_DOF-3) = mBn(i);
                mTangentStiffness(BC3D_NUM_DOF-3,i) = mBn(i);
	      }

            //mTangentStiffness(BC3D_NUM_DOF-3,BC3D_NUM_DOF-3) = -1.0e-16;
            mTangentStiffness(BC3D_NUM_DOF-3,BC3D_NUM_DOF-3) = 0.0;
            mTangentStiffness(BC3D_NUM_DOF-2,BC3D_NUM_DOF-2) =  1.0;
            mTangentStiffness(BC3D_NUM_DOF-1,BC3D_NUM_DOF-1) =  1.0;
	  }

        // contribution from friction

        //  sliding: Csl = mu * r   &    Css != 0
        // sticking: Csl = 0        &    Css != 0

        const bool inSlip= physicalProperties.getContactMaterial()->getContactState();
        if(inSlip)
	  {
            //if (Csl(0) != 0.0 || Csl(1) != 0.0 ) {

            // sliding
#ifdef DEBUG
            if(DEBUG_LEVEL > 0)
	      {
                std::cerr << "   ** tangent: sliding" << std::endl;
	      }
#endif
//std::cerr << "   ** tangent: sliding" << std::endl;

            for(int i = 0; i < BC3D_NUM_DOF-3; i++)
	      {
                // assume first is the row
                mTangentStiffness(i,BC3D_NUM_DOF-3) += mBs(i,0)*Csl(0)
                                                       +  mBs(i,1)*Csl(1);
	      }
            // end sliding
	  }
	else
	  {
            // sticking
#ifdef DEBUG
            if (DEBUG_LEVEL > 0)
	      {
                std::cerr << "   ** tangent: sticking" << std::endl;
	      }
#endif
	  }
        // in 3D: both sticking & sliding cases have Css coefficients
        // in 2D: Css = 0 for sliding
        for(int i = 0; i < BC3D_NUM_DOF-3; i++)
	  {
            for(int j = 0; j < BC3D_NUM_DOF-3; j++)
	      {
                mTangentStiffness(i,j) += mBs(i,0)*mBs(j,0)*Css(0,0)
                                          +  mBs(i,1)*mBs(j,0)*Css(1,0)
                                          +  mBs(i,0)*mBs(j,1)*Css(0,1)
                                          +  mBs(i,1)*mBs(j,1)*Css(1,1);
	      }
	  }

      }
    else
      {
        // not in contact
        mTangentStiffness(BC3D_NUM_DOF-3,BC3D_NUM_DOF-3) = 1.0;
        mTangentStiffness(BC3D_NUM_DOF-2,BC3D_NUM_DOF-2) = 1.0;
        mTangentStiffness(BC3D_NUM_DOF-1,BC3D_NUM_DOF-1) = 1.0;
      }

//              std::cerr << "   K_T: " << mTangentStiffness;

#ifdef DEBUG
    if (DEBUG_LEVEL > 0)
      {
        std::cerr << "   inContact:   " << inContact << std::endl;
        std::cerr << "   K_T: " << mTangentStiffness;
      }
#endif
    return mTangentStiffness;
  }

const XC::Matrix &XC::BeamContact3D::getInitialStiff(void) const
  {
    return getTangentStiff();
  }

void XC::BeamContact3D::zeroLoad(void)
  {}

int XC::BeamContact3D::addLoad(ElementalLoad *theLoad, double loadFactor)
  {
    return 0;
  }

int XC::BeamContact3D::addInertiaLoadToUnbalance(const Vector &accel)
  {
    return 0;
  }

const XC::Vector &XC::BeamContact3D::getResistingForce(void) const
  {
    // initialize F
    mInternalForces.Zero();

    // get contact stresses/forces:  stress(0) = lambda, stress(1) = t_s(0), stress(2) = t_s(1)
    const Vector stress= physicalProperties[0]->getStress();

    if(inContact)
      {
        // in contact
        for(int i=0; i < BC3D_NUM_DOF - 3; i++)
	  {
            mInternalForces(i) = mLambda*mBn(i) + stress(1)*mBs(i,0) + stress(2)*mBs(i,1);
	  }
        mInternalForces(BC3D_NUM_DOF - 3) = -mGap;

      }
    else
      {
        mInternalForces(BC3D_NUM_DOF - 3) = mLambda;
      }

#ifdef DEBUG
    if(DEBUG_LEVEL > 0)
      {
        std::cerr << "inContact:  " << inContact << std::endl;
        std::cerr << "   F_int  = " << mInternalForces;
      }
#endif

    return mInternalForces;
  }


const XC::Vector &XC::BeamContact3D::getResistingForceIncInertia(void) const
  {
    return getResistingForce();
  }

//! @brief Send the coordinate transformation through the communicator argument.
int XC::BeamContact3D::sendCoordTransf(int posFlag,const int &posClassTag,const int &posDbTag,Communicator &comm)
  {
    int retval= 0;
    CrdTransf *ptr= getCoordTransf();
    DbTagData &dt= getDbTagData();
    if(!ptr)
      setDbTagDataPos(posFlag,1);
    else
      {
        setDbTagDataPos(posFlag,0);
        setDbTagDataPos(posClassTag,ptr->getClassTag());
        comm.sendMovable(*ptr,dt,CommMetaData(posDbTag));
      }
    return retval;
  }

//! @brief Receives the coordinate transformation through the communicator argument.
XC::CrdTransf3d *XC::BeamContact3D::recvCoordTransf(int posFlag,const int &posClassTag,const int &posDbTag,const Communicator &comm)
  {
    CrdTransf3d *ptr= getCoordTransf();
    DbTagData &dt= getDbTagData();
    if(getDbTagDataPos(posFlag) == 0)
      {
        // make some room and read in the vector
        if(!ptr)
          {
            ptr= comm.getNewCrdTransf3d(getDbTagDataPos(posClassTag));
            if(!ptr)
              std::cerr << getClassName() << "::" << __FUNCTION__
			<< "; ran out of memory\n";
          }
        if(ptr)
          {
            int res= comm.receiveMovable(*ptr,dt,CommMetaData(posDbTag));
            ptr->revertToLastCommit();// Revert the crdtrasf to its last committed state
            if(res<0)
              std::cerr << getClassName() << "::" << __FUNCTION__
			<< "; failed to receive vector data\n";
          }
      }
    return ptr;
  }

//! @brief Send object members through the communicator argument.
int XC::BeamContact3D::sendData(Communicator &comm)
  {
    int res= ElemWithMaterial<BC3D_NUM_NODE, Contact3D>::sendData(comm);
    res+= comm.sendDoubles(mRadius, mTolGap, mTolForce, getDbTagData(), CommMetaData(8));
    res+= sendCoordTransf(9,10,11,comm);
    res+= comm.sendInt(mIniContact, getDbTagData(),CommMetaData(12));
    res+= comm.sendBools(inContact, was_inContact, to_be_released, should_be_released, in_bounds, getDbTagData(),CommMetaData(13));
    res+= comm.sendDoubles(mGap, mLambda, mxi, getDbTagData(), CommMetaData(14));
    res+= comm.sendDoubles(mchi, mL, mrho2, mrho3, getDbTagData(), CommMetaData(15));
    res+= comm.sendVector(mH, getDbTagData(), CommMetaData(16));
    res+= comm.sendVector(mn, getDbTagData(), CommMetaData(17));
    res+= comm.sendVector(mg1, getDbTagData(), CommMetaData(18));
    res+= comm.sendVector(mg2, getDbTagData(), CommMetaData(19));
    res+= comm.sendMatrix(mQa, getDbTagData(), CommMetaData(20));
    res+= comm.sendMatrix(mQb, getDbTagData(), CommMetaData(21));
    res+= comm.sendMatrix(mQc, getDbTagData(), CommMetaData(22));
    res+= comm.sendVector(mIcrd_a, getDbTagData(), CommMetaData(23));
    res+= comm.sendVector(mDcrd_a, getDbTagData(), CommMetaData(24));
    res+= comm.sendVector(mDisp_a_n, getDbTagData(), CommMetaData(25));
    res+= comm.sendVector(mDisp_b_n, getDbTagData(), CommMetaData(26));
    res+= comm.sendVector(mDisp_s_n, getDbTagData(), CommMetaData(27));
    return res;
  }

//! @brief Send object members through the communicator argument.
int XC::BeamContact3D::recvData(const Communicator &comm)
  {
    int res= ElemWithMaterial<BC3D_NUM_NODE, Contact3D>::recvData(comm);
    res+= comm.receiveDoubles(mRadius, mTolGap, mTolForce,getDbTagData(),CommMetaData(8));
    crdTransf= recvCoordTransf(9,10,11,comm);
    res+= comm.receiveInt(mIniContact, getDbTagData(),CommMetaData(12));
    res+= comm.receiveBools(inContact, was_inContact, to_be_released, should_be_released, in_bounds, getDbTagData(),CommMetaData(13));
    res+= comm.receiveDoubles(mGap, mLambda, mxi, getDbTagData(), CommMetaData(14));
    res+= comm.receiveDoubles(mchi, mL, mrho2, mrho3, getDbTagData(), CommMetaData(15));
    res+= comm.receiveVector(mH, getDbTagData(), CommMetaData(16));
    res+= comm.receiveVector(mn, getDbTagData(), CommMetaData(17));
    res+= comm.receiveVector(mg1, getDbTagData(), CommMetaData(18));
    res+= comm.receiveVector(mg2, getDbTagData(), CommMetaData(19));
    res+= comm.receiveMatrix(mQa, getDbTagData(), CommMetaData(20));
    res+= comm.receiveMatrix(mQb, getDbTagData(), CommMetaData(21));
    res+= comm.receiveMatrix(mQc, getDbTagData(), CommMetaData(22));
    res+= comm.receiveVector(mIcrd_a, getDbTagData(), CommMetaData(23));
    res+= comm.receiveVector(mDcrd_a, getDbTagData(), CommMetaData(24));
    res+= comm.receiveVector(mDisp_a_n, getDbTagData(), CommMetaData(25));
    res+= comm.receiveVector(mDisp_b_n, getDbTagData(), CommMetaData(26));
    res+= comm.receiveVector(mDisp_s_n, getDbTagData(), CommMetaData(27));
    return res;
  }

//! @brief Sends object through the communicator argument.
int XC::BeamContact3D::sendSelf(Communicator &comm)
  {
    inicComm(28);
    int res= sendData(comm);

    const int dataTag= getDbTag();
    res= comm.sendIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; could not send data Vector\n";
    return res;
  }

int XC::BeamContact3D::recvSelf(const Communicator &comm)
  {
    inicComm(28);
    const int dataTag= getDbTag();
    int res = comm.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to send ID data\n";
    else
      res+= recvData(comm);
    return res;
  }

void XC::BeamContact3D::Print(std::ostream &s, int flag) const
  {
    s << "BeamContact3D, element id:  " << this->getTag() << std::endl;
    s << "   Connected external nodes:  " ;
    for(int i = 0; i<BC3D_NUM_NODE; i++)
      {
        s << theNodes[i]->getTag() << " ";
      }
    return;
  }


XC::Response *XC::BeamContact3D::setResponse(const std::vector<std::string> &argv, Parameter &param)
  {
    if ((argv[0]=="force") || (argv[0]=="forces"))
        return new ElementResponse(this, 1, Vector(3));

    else if ((argv[0]=="frictionforce") || (argv[0]=="frictionforces"))
        return new ElementResponse(this, 2, Vector(3));

    else if ((argv[0]=="forcescalar") || (argv[0]=="forcescalars"))
        return new ElementResponse(this, 3, Vector(3));

    else if ((argv[0]=="masterforce") || (argv[0]=="masterforces") ||
             (argv[0]=="primaryforce") || (argv[0]=="primaryforces"))
        return new ElementResponse(this, 4, Vector(6));

    else if ((argv[0]=="mastermoment") || (argv[0]=="mastermoments") ||
             (argv[0]=="primarymoment") || (argv[0]=="primarymoments"))
        return new ElementResponse(this, 5, Vector(6));

    else if ((argv[0]=="masterreaction") || (argv[0]=="masterreactions") ||
             (argv[0]=="primaryreaction") || (argv[0]=="primaryreactions"))
        return new ElementResponse(this, 6, Vector(12));

    else if ((argv[0]=="slip"))
        return new ElementResponse(this, 7, Vector(2));

    // otherwise response quantity is unknown for the BeamContact3D class
    else
        std::cerr << "XC::BeamContact3D::setResponse(const char **argv, int argc, OPS_Stream &eleInfo): " << argv[0] << " unknown request" << std::endl;
    return 0;
}


int XC::BeamContact3D::getResponse(int responseID, Information &eleInfo)
{
#ifdef DEBUG
    std::cerr << "XC::BeamContact3D::getResponse(int responseID =" << responseID << ", Information &eleInfo): " << MyTag << std::endl;
#endif
    Vector force(3);
    Vector sForce(3);
    Vector mForces(6);
    Vector mMoments(6);
    Vector mReactions(12);
    Vector theSlip(2);

    // get contact stresse/forces
    Vector stress = physicalProperties[0]->getStress();

    if (responseID == 1) {

        // force = stress(0)*mn + stress(1)*mg1 + stress(2)*mg2;

        // forces on secondary node
        for (int ii=0; ii<3; ii++) {
            sForce(ii)   = -mInternalForces(BC3D_NUM_DOF - 6 + ii);
            /*
                              if ( sForce(ii) != force(ii) ) {
                                    std::cerr << "** problem in recorder: " << sForce(ii) << " != " << force(ii) << std::endl ;
                                    std::cerr << "   stress = " << stress;
                        std::cerr << "   Fn     = " << stress(0)*mn;
                        std::cerr << "   Fs1    = " << stress(1)*mg1;
                        std::cerr << "   Fs2    = " << stress(2)*mg2;
                        std::cerr << "   force  = " << force;
                        std::cerr << "   F_int  = " << mInternalForces;
                                    }
            */
        }

        return eleInfo.setVector(sForce);

    } else if (responseID == 2) {

        force = stress(1)*mg1 + stress(2)*mg2;

        return eleInfo.setVector(force);

    } else if (responseID == 3) {

        force(0) = stress(0);
        force(1) = stress(1);
        force(2) = stress(2);

        return eleInfo.setVector(force);

    } else if (responseID == 4) {

        // forces on primary nodes
        for (int ii=0; ii<3; ii++) {
            mForces(ii)   = -mInternalForces(ii);
            mForces(ii+3) = -mInternalForces(ii+6);
        }

        return eleInfo.setVector(mForces);

    } else if (responseID == 5) {

        // moments on primary nodes
        for (int ii=0; ii<3; ii++) {
            mMoments(ii)   = -mInternalForces(ii+3);
            mMoments(ii+3) = -mInternalForces(ii+9);
        }

        return eleInfo.setVector(mMoments);

    } else if (responseID == 6) {

        // full reactions on primary nodes
        for (int ii=0; ii<6; ii++) {
            mReactions(ii)   = -mInternalForces(ii);
            mReactions(ii+6) = -mInternalForces(ii+6);
        }

        return eleInfo.setVector(mReactions);

    } else if (responseID == 7) {

        // slip vector on tangent plane
        return eleInfo.setVector(mSlip);

    } else

        std::cerr << "XC::BeamContact3D::getResponse(int responseID=" << responseID << ", Information &eleInfo): " << " unknown request" << std::endl;
    return -1;
}

int XC::BeamContact3D::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
    int retval= -1;
    if(size(argv)>1)
      {
        if(argv[0]=="friction")
	  {
	    retval= param.addObject(1, this);
	  }
      }
    return retval;
  }

int XC::BeamContact3D::updateParameter(int parameterID, Information &info)
  {
    int res= -1;
    const int matRes=  physicalProperties[0]->updateParameter(parameterID, info);
    if(matRes != -1)
      { res = matRes; }
    return res;
  }
