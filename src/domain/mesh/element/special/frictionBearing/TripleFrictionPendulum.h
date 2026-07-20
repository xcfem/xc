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

// $Revision$
// $Date$
// $URL$

#ifndef TripleFrictionPendulum_h
#define TripleFrictionPendulum_h

// Header file for TripleFrictionPendulum element
// Written by Nhan Dao, nhan.unr@gmail.com

#include "domain/mesh/element/Element0D.h"

#include "domain/mesh/element/utils/physical_properties/UniaxialMatPhysicalProperties.h"
#include "frictionModel/FrictionModels.h"

namespace XC {
class UniaxialMaterial;
class Response;


class TripleFrictionPendulum : public Element0D
  {
  private:
    FrictionModels frictionModels; //!< pointer to friction model
    UniaxialMatPhysicalProperties physicalProperties;  //!<array of uniaxial materials
    
    double L1;
    double L2;
    double L3;
    double Ubar1;
    double Ubar2;
    double Ubar3;
    double W;
    double Uy;
    double Kvt;
    double MinFv;
    double TOL;
    int Niter;
    
    Matrix K;
    Matrix Kpr;
    Vector f;
    Vector fpr;
    Matrix k12;
    Matrix k12pr;
    Matrix k34;
    Matrix k34pr;
    Matrix k56;
    Matrix k56pr;
    Vector d1;
    Vector d1pr;
    Vector d3;
    Vector d3pr;
    Vector d5;
    Vector d5pr;
    Vector v1;
    Vector v3;
    Vector v5;
    Vector ep1;
    Vector ep1pr;
    Vector ep3;
    Vector ep3pr;
    Vector ep5;
    Vector ep5pr;
    Vector q1;
    Vector q1pr;
    Vector q3;
    Vector q3pr;
    Vector q5;
    Vector q5pr;
    Vector ep1tmp;
    Vector ep3tmp;
    Vector ep5tmp;
    Vector q1tmp;
    Vector q3tmp;
    Vector q5tmp;
    
    double v1Fact, v3Fact, v5Fact;
    double Gap2, Gap4, Gap6;
    double Vel1Avg, Vel3Avg, Vel5Avg;
    double Fy1pr, Fy3pr, Fy5pr;
    double Wpr, Wcr, Wavg;
    double Fy1, Fy3, Fy5;
    double E1, E3, E5;
    double E2, E4, E6;
    double H1, H3, H5;
    double Fvert, Kvert;
    double TorqX, KrotX;
    double TorqY, KrotY;
    double TorqZ, KrotZ;
    double Hisolator;
    double Dx, Dy, Dz;
    bool Conv;
    
    // private attributes - a copy for each object of the class
    Matrix trans;       // hold the transformation matrix, could use a Vector
                        // if ever bother to change the Vector interface for
                        // x-product.
    
    // static data - single copy for all objects of the class
    static Matrix eleK;      // class wide matrix for returning stiffness
    static Matrix eleKinit;  // class wide matrix for returning initial stiffness
    static Matrix eleD;      // class wide matrix for returning damping
    static Matrix eleM;      // class wide matrix for returning mass
    static Vector eleR;      // class wide vector for returning residual
    
    // private member functions - only available to objects of the class
    void CircularElasticGap(Matrix &kj, Vector &fj, double Ej,double Gapj,Vector di);
    void BidirectionalPlastic(Matrix &ki, Vector &fi, Vector &epitmp, Vector &qitmp, double Fyi, double Ei, double Hi, Vector epi, Vector qi, Vector di);
    void Segment(Vector &epitmp, Vector &qitmp, bool &conv, Matrix &kij, Vector &di, Vector epi, Vector qi, Vector f, Vector df, double Fyi, double Ei, double Hi, double Ej, double Gapj, double Tol, int Niter);
    void TFPElement(bool &Conv, Vector &ep1tmp, Vector &ep3tmp, Vector &ep5tmp, Vector &q1tmp, Vector &q3tmp, Vector &q5tmp, Matrix &K, Vector &f, Matrix &k12, Matrix &k34, Matrix &k56, Vector &d1, Vector &d3, Vector &d5, Vector ep1, Vector ep3, Vector ep5, Vector q1, Vector q3, Vector q5, Vector u, Vector dusub, double Fy1, double Fy3, double Fy5, double E1, double E3, double E5, double H1, double H3, double H5, double E2, double E4, double E6, double Gap2, double Gap4, double Gap6, double Tol, int Niter);
    void StiffnessForm(Matrix &K, Matrix k12, Matrix k34, Matrix k56);
  protected:
    int sendData(Communicator &);
    int recvData(const Communicator &);
    void initialize(void);    
    
  public:
    // constructors
    TripleFrictionPendulum(int tag,
        int Nd1, int Nd2,
        const FrictionModels &,
        const UniaxialMatPhysicalProperties &,
        double L1,
        double L2,
        double L3,
        double Ubar1,
        double Ubar2,
        double Ubar3,
        double W,
        double Uy,
        double Kvt,
        double minFv,
        double tol);
    
    TripleFrictionPendulum();
    
    void setDomain(Domain *theDomain);
    
    // public methods to set the state of the element
    int commitState();
    int revertToLastCommit();
    int revertToStart();
    int update();
    Element *getCopy() const;
    
    // public methods to obtain stiffness, mass, damping and residual information
    const Matrix &getTangentStiff() const;
    const Matrix &getInitialStiff() const;
    const Matrix &getDamp() const;
    const Matrix &getMass() const;
    
    int addLoad(ElementalLoad *theLoad, double loadFactor);
    int addInertiaLoadToUnbalance(const Vector &accel);
    
    const Vector &getResistingForce() const;
    
    // public methods for output
    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);
    void Print(std::ostream &s, int flag = 0) const;    
    
    Response *setResponse(const std::vector<std::string> &argv, Information &eleInformation);
    int getResponse(int responseID, Information &eleInformation);
  };
} // end of XC namespace

#endif
