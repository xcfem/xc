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
//FiberPtrDeque.h

#ifndef FiberPtrDeque_h
#define FiberPtrDeque_h

#include "xc_utils/src/kernel/CommandEntity.h"
#include "xc_utils/src/geom/GeomObj.h"
#include "utility/actor/actor/MovableObject.h"
#include <deque>

class Ref3d3d;
class Pos2d;
class Line2d;
class Segment2d;
class Polygon2d;
class BND2d;
class HalfPlane2d;

namespace XC {
class Fiber;
class Vector;
class FiberSection2d;
class FiberSectionBase;
class FiberSection3dBase;
class FiberSection3d;
class FiberSectionGJ;
class Matrix;
class Response;
class Parameter;
class Information;
class CrossSectionKR;
class GeomSection;
class DeformationPlane;

enum ClaseEsfuerzo {SIMPLE_TENSION,COMPOSED_TENSION,FLEXION_SIMPLE,FLEXION_COMPUESTA,SIMPLE_COMPRESSION,COMPOSED_COMPRESSION,ERROR};


//! @ingroup MATSCCFibers
//
//! @brief Fiber pointers container.
class FiberPtrDeque: public CommandEntity, public std::deque<Fiber *>, public MovableObject
  {
  public:
    typedef std::deque<Fiber *> fiber_ptrs_dq;

  protected:

    double yCenterOfMass; //!< Y coordinate of the centroid.
    double zCenterOfMass; //!< Z coordinate of the centroid.

    int sendData(Communicator &);  
    int recvData(const Communicator &);

    mutable std::deque<std::list<Polygon2d> > dq_ac_effective; //!< (Where appropriate) effective concrete areas for each fiber.
    mutable std::deque<double> recubs; //! Cover for each fiber.
    mutable std::deque<double> seps; //! Spacing for each fiber.

    inline void resize(const size_t &nf)
      { fiber_ptrs_dq::resize(nf,nullptr); }

    inline reference operator[](const size_t &i)
      { return fiber_ptrs_dq::operator[](i); }

    FiberPtrDeque(const size_t &num= 0);
    FiberPtrDeque(const FiberPtrDeque &);
    FiberPtrDeque &operator=(const FiberPtrDeque &);

  private:
    friend class FiberSectionBase;
    
  public:
    void push_back(Fiber *f);
    inline size_t getNumFibers(void) const
      { return size(); }
    
    const Fiber *findFiber(const int &tag) const;
    Fiber *findFiber(const int &tag);
    bool in(const Fiber *ptr) const;
    inline const_reference operator[](const size_t &i) const
      { return fiber_ptrs_dq::operator[](i); }

    inline const double &getCenterOfMassY(void) const
      { return yCenterOfMass; }
    inline const double &getCenterOfMassZ(void) const
      { return zCenterOfMass; }
    Pos2d getCenterOfMass(void) const;
    inline double getRelativeY(const double &y) const
      { return y-yCenterOfMass; }
    inline double getRelativeZ(const double &z) const
      { return z-zCenterOfMass; }

    GeomObj::list_Pos2d getPositions(void) const;

    bool isSubjectedToBending(const double &tol=1e-4) const;
    double getResultant(void) const;
    double getMz(const double &y0= 0.0) const;
    double getMy(const double &z0= 0.0) const;
    double getExcentricidadMz(const double &y0= 0.0) const;
    double getExcentricidadMy(const double &z0= 0.0) const;
    Vector2d getMomentVector(const double &y0=0.0,const double &z0=0.0) const;
    Pos2d getResultantPosition(const double &y0=0.0,const double &z0=0.0) const;
    Line2d getNeutralAxis(void) const;

    double getCompressionResultant(void) const;
    double getCompressedFibersMz(const double &y0= 0.0) const;
    double getCompressedFibersMy(const double &z0= 0.0) const;
    const Vector &getCompressedFibersCentroid(void) const;
    const Vector &getCentroidFibersWithStrainSmallerThan(const double &epsRef) const;

    double getTensionResultant(void) const;
    double getTensionedFibersMz(const double &y0= 0.0) const;
    double getTensionedFibersMy(const double &z0= 0.0) const;
    const Vector &getTensionedFibersCentroid(void) const;
    const Vector &getCentroidFibersWithStrainGreaterThan(const double &epsRef) const;

    int commitState(void);

    double getStrainMin(void) const;
    double getStrainMax(void) const;
    double getStrainMed(void) const;
    double getStressMin(void) const;
    double getStressMax(void) const;
    double getStressMed(void) const;
    DeformationPlane getDeformationPlane(void) const;
    const Vector &getDeformation(void) const;
    ClaseEsfuerzo getClaseEsfuerzo(const double &tol= 1e-4) const;
    bool isTensioned(void) const;
    bool isBent(void) const;
    bool isCompressed(void) const;
    std::string getStrClaseEsfuerzo(const double &tol= 1e-4) const;
    double getNeutralAxisDepth(const FiberSectionBase &) const;
    Vector getLeverArmVector(void) const;
    Segment2d getLeverArmSegment(void) const;
    double getMechanicLeverArm(void) const;
    Line2d getBendingPlaneTrace(void) const;
    Line2d getTensionedPlaneTrace(void) const;
    Line2d getCompressedPlaneTrace(void) const;
    double computeFibersEffectiveConcreteArea(const std::list<Polygon2d> &,const double &factor= 15) const;
    const std::list<Polygon2d> &getFiberEffectiveConcretAreaContour(const size_t &i) const;
    double getFiberEffectiveConcreteArea(const size_t &i) const;
    double getFibersEffectiveConcreteArea(void) const;
    void computeCovers(const GeomSection &) const;
    void computeSpacement(void) const;
    const double &getFiberCover(const size_t &i) const;
    const double &getFiberSpacing(const size_t &i) const;
    double getEquivalentDiameterOfFiber(const size_t &i) const;
    double getSigmaSRAtFiber(const size_t &,const double &,const double &,const double &) const;
    double getAverageDistanceBetweenFibers(void) const;
    int updateCenterOfMass(void);
    
    int updateKRCenterOfMass(FiberSection2d &,CrossSectionKR &);
    int setInitialSectionDeformation(const FiberSection2d &);
    int setTrialSectionDeformation(const FiberSection2d &,CrossSectionKR &);
    int revertToLastCommit(FiberSection2d &,CrossSectionKR &);
    int revertToStart(FiberSection2d &,CrossSectionKR &);
    const Matrix &getInitialTangent(const FiberSection2d &) const;
    const Vector &getStressResultantSensitivity(int gradNumber, bool conditional);
    int commitSensitivity(const XC::Vector& defSens, int gradNumber, int numGrads);

    int updateKRCenterOfMass(FiberSection3d &,CrossSectionKR &);
    int setInitialSectionDeformation(const FiberSection3d &);
    int setTrialSectionDeformation(FiberSection3d &,CrossSectionKR &);
    int revertToLastCommit(FiberSection3d &,CrossSectionKR &);
    int revertToStart(FiberSection3d &,CrossSectionKR &);
    const Matrix &getInitialTangent(const FiberSection3d &) const;

    int updateKRCenterOfMass(FiberSectionGJ &,CrossSectionKR &);
    int setInitialSectionDeformation(const FiberSectionGJ &);
    int setTrialSectionDeformation(FiberSectionGJ &,CrossSectionKR &);
    int revertToLastCommit(FiberSectionGJ &,CrossSectionKR &);
    int revertToStart(FiberSectionGJ &,CrossSectionKR &);
    const Matrix &getInitialTangent(const FiberSectionGJ &) const;

    Response *setResponse(const std::vector<std::string> &argv, Information &sectInfo);

    void SelMatTag(const int &matTag,FiberPtrDeque &,bool clear= true);

    //size_t IMaxProp(const std::string &prop_name) const;
    //size_t IMinProp(const std::string &prop_name) const;
    size_t getFiberWithMaxCoord(const Ref3d3d &r,const size_t &iCoo) const;
    size_t getFiberWithMinCoord(const Ref3d3d &r,const size_t &iCoo) const;

    int setParameter(const std::vector<std::string> &, Parameter &);
    int setParameter(const int &,const std::vector<std::string> &, Parameter &);
    int updateParameter(const int &,int parameterID, Information &info);
    int activateParameter(int passedParameterID);

    double GetYMin(void) const;
    double GetZMin(void) const;
    double GetYMax(void) const;
    double GetZMax(void) const;
    Pos2d GetPMax(void) const;
    Pos2d GetPMin(void) const;
    BND2d Bnd(void) const;
    Fiber *getClosestFiber(const int matTag, const double &yCoord);
    double getArea(const double &factor= 1.0) const;
    double getAreaHomogenizedSection(const double &E0) const;
    const Vector &getCenterOfMassHomogenizedSection(const double &E0) const;
    //Moments of inertia.
    double getIz(const double &factor= 1.0,const double &y0= 0.0) const;
    double getIy(const double &factor= 1.0,const double &z0= 0.0) const;
    double getPyz(const double &factor= 1.0,const double &y0= 0.0,const double &z0= 0.0) const;
    double getI1(const double &factor= 1.0,const double &y0= 0.0,const double &z0= 0.0) const;
    double getI2(const double &factor= 1.0,const double &y0= 0.0,const double &z0= 0.0) const;
    double getTh1(const double &y0= 0.0,const double &z0= 0.0) const;
    Vector getAxis1(const double &y0= 0.0,const double &z0= 0.0) const;
    Vector getAxis2(const double &y0= 0.0,const double &z0= 0.0) const;
    double getIyHomogenizedSection(const double &) const;
    double getIzHomogenizedSection(const double &) const;
    double getPyzHomogenizedSection(const double &) const;
    double getIHomogenizedSection(const double &,const unsigned short int &,const unsigned short int &) const;
    Matrix &getIHomogenizedSection(const double &) const;
    Matrix &getIHomogenizedSection(const double &,const Pos2d &o) const;
    double getIHomogenizedSection(const double &,const Pos2d &O,const Vector &e) const;
    double getIHomogenizedSection(const double &,const Line2d &r) const;
    //Static moments.
    double getSzPos(const double &yf,const double &y0,const double &factor= 1.0) const;
    double getSzNeg(const double &yf,const double &y0,const double &factor= 1.0) const;
    double getSyPos(const double &zf,const double &z0,const double &factor= 1.0) const;
    double getSyNeg(const double &zf,const double &z0,const double &factor= 1.0) const;
    double getSPosHomogenizedSection(const double &E0,const HalfPlane2d &sp) const;
    double getSNegHomogenizedSection(const double &E0,const HalfPlane2d &sp) const;
    
    //! @brief Returns the polar moment of inertia with respect to G.
    inline double getIpolar(const double &factor= 1.0) const
      { return getIz(factor,yCenterOfMass)+getIy(factor,zCenterOfMass); }
    //! @brief Returns the radius of gyration with respect to the axis
    //! parallel to z through the centroid.
    inline double getiz(const double factor= 1.0) const
      { return getIz(factor,yCenterOfMass)/getArea(factor); }
    //! @brief Returns the radius of gyration with respect to the axis
    //! parallel to y through the centroid.
    inline double getiy(const double factor= 1.0) const
      { return getIy(factor,zCenterOfMass)/getArea(factor); }
    size_t nearest_fiber(const double &y,const double &z) const;

    void Print(std::ostream &s,const int &flag) const;
    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);
  };
} // end of XC namespace

#endif
