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
//FiberDeque.h

#ifndef FiberDeque_h
#define FiberDeque_h

#include "xc_utils/src/nucleo/EntCmd.h"
#include "xc_utils/src/geom/GeomObj.h"
#include <deque>

class Ref3d3d;
class Pos2d;
class Recta2d;
class Segmento2d;
class Poligono2d;
class BND2d;
class Semiplano2d;

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

enum ClaseEsfuerzo {TRACCION_SIMPLE,TRACCION_COMPUESTA,FLEXION_SIMPLE,FLEXION_COMPUESTA,COMPRESION_SIMPLE,COMPRESION_COMPUESTA,ERROR};


//! \ingroup MATSCCFibers
//
//! @brief Fiber double ended queue.
class FiberDeque : public EntCmd, public std::deque<Fiber *>
  {
  public:
    typedef std::deque<Fiber *> fiber_ptrs_dq;
  protected:

    double yCDG; //!< Y coordinate of the centroid.
    double zCDG; //!< Z coordinate of the centroid.

    mutable std::deque<std::list<Poligono2d> > dq_ac_effective; //!< (Where appropriate) effective concrete areas for each fiber.
    mutable std::deque<double> recubs; //! Cover for each fiber.
    mutable std::deque<double> seps; //! Spacing for each fiber.

    Fiber *insert(const Fiber &f);
    inline void resize(const size_t &nf)
      { fiber_ptrs_dq::resize(nf,nullptr); }



  private:
    friend class FiberSectionBase;
    
  public:
    FiberDeque(const size_t &num= 0);
    FiberDeque(const FiberDeque &otro);
    FiberDeque &operator=(const FiberDeque &otro);

    void push_back(Fiber *f);
     inline size_t getNumFibers(void) const
      { return size(); }

    const Fiber *findFiber(const int &tag) const;
    Fiber *findFiber(const int &tag);

    inline const double &getYCdg(void) const
      { return yCDG; }
    inline const double &getZCdg(void) const
      { return zCDG; }
    Pos2d getCdg(void) const;
    inline double getYRelativa(const double &y) const
      { return y-yCDG; }
    inline double getZRelativa(const double &z) const
      { return z-zCDG; }

    GeomObj::list_Pos2d getPositions(void) const;

    bool hayMomento(const double &tol=1e-4) const;
    double Resultante(void) const;
    double getMz(const double &y0= 0.0) const;
    double getMy(const double &z0= 0.0) const;
    double getExcentricidadMz(const double &y0= 0.0) const;
    double getExcentricidadMy(const double &z0= 0.0) const;
    Vector2d getMomentVector(const double &y0=0.0,const double &z0=0.0) const;
    Pos2d getPosResultante(const double &y0=0.0,const double &z0=0.0) const;
    Recta2d getNeutralAxis(void) const;

    double ResultanteComp(void) const;
    double getMzComp(const double &y0= 0.0) const;
    double getMyComp(const double &z0= 0.0) const;
    const Vector &baricentroCompresiones(void) const;
    const Vector &baricentroDefMenores(const double &epsRef) const;

    double ResultanteTracc(void) const;
    double getMzTracc(const double &y0= 0.0) const;
    double getMyTracc(const double &z0= 0.0) const;
    const Vector &baricentroTracciones(void) const;
    const Vector &baricentroDefMayores(const double &epsRef) const;

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
    bool enTraccion(void) const;
    bool enFlexion(void) const;
    bool enCompresion(void) const;
    std::string getStrClaseEsfuerzo(const double &tol= 1e-4) const;
    double getNeutralAxisDepth(const FiberSectionBase &) const;
    Vector getVectorBrazoMecanico(void) const;
    Segmento2d getSegmentoBrazoMecanico(void) const;
    double getMechanicLeverArm(void) const;
    Recta2d getBendingPlaneTrace(void) const;
    Recta2d getTensionedPlaneTrace(void) const;
    Recta2d getCompressedPlaneTrace(void) const;
    double computeFibersEffectiveConcreteArea(const std::list<Poligono2d> &,const double &factor= 15) const;
    const std::list<Poligono2d> &getFiberEffectiveConcretAreaContour(const size_t &i) const;
    double getFiberEffectiveConcreteArea(const size_t &i) const;
    double getFibersEffectiveConcreteArea(void) const;
    void computeCovers(const GeomSection &) const;
    void computeSpacement(void) const;
    const double &getFiberCover(const size_t &i) const;
    const double &getFiberSpacing(const size_t &i) const;
    double getEquivalentDiameterOfFiber(const size_t &i) const;
    double getSigmaSRAtFiber(const size_t &,const double &,const double &,const double &) const;
    double getAverageDistanceBetweenFibers(void) const;
    int updateCDG(void);
    
    int updateKRCDG(FiberSection2d &,CrossSectionKR &);
    Fiber *addFiber(FiberSection2d &,Fiber &,CrossSectionKR &);
    int setInitialSectionDeformation(const FiberSection2d &);
    int setTrialSectionDeformation(const FiberSection2d &,CrossSectionKR &);
    int revertToLastCommit(FiberSection2d &,CrossSectionKR &);
    int revertToStart(FiberSection2d &,CrossSectionKR &);
    const Matrix &getInitialTangent(const FiberSection2d &) const;
    const Vector &getStressResultantSensitivity(int gradNumber, bool conditional);
    int commitSensitivity(const XC::Vector& defSens, int gradNumber, int numGrads);

    int updateKRCDG(FiberSection3d &,CrossSectionKR &);
    Fiber *addFiber(FiberSection3d &,Fiber &,CrossSectionKR &);
    int setInitialSectionDeformation(const FiberSection3d &);
    int setTrialSectionDeformation(FiberSection3d &,CrossSectionKR &);
    int revertToLastCommit(FiberSection3d &,CrossSectionKR &);
    int revertToStart(FiberSection3d &,CrossSectionKR &);
    const Matrix &getInitialTangent(const FiberSection3d &) const;

    int updateKRCDG(FiberSectionGJ &,CrossSectionKR &);
    Fiber *addFiber(FiberSectionGJ &,Fiber &,CrossSectionKR &);
    int setInitialSectionDeformation(const FiberSectionGJ &);
    int setTrialSectionDeformation(FiberSectionGJ &,CrossSectionKR &);
    int revertToLastCommit(FiberSectionGJ &,CrossSectionKR &);
    int revertToStart(FiberSectionGJ &,CrossSectionKR &);
    const Matrix &getInitialTangent(const FiberSectionGJ &) const;

    Response *setResponse(const std::vector<std::string> &argv, Information &sectInfo);

    //void Cumplen(const std::string &cond,FiberDeque &,bool clear= true);
    void SelMatTag(const int &matTag,FiberDeque &,bool clear= true);

    //size_t IMaxProp(const std::string &nmb_prop) const;
    //size_t IMinProp(const std::string &nmb_prop) const;
    size_t getFiberWithMaxCoord(const Ref3d3d &r,const size_t &iCoo) const;
    size_t getFiberWithMinCoord(const Ref3d3d &r,const size_t &iCoo) const;

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
    double getArea(const double &factor= 1.0) const;
    double getAreaHomogenizedSection(const double &E0) const;
    const Vector &getCdgHomogenizedSection(const double &E0) const;
    //Moments of inertia.
    double getIz(const double &factor= 1.0,const double &y0= 0.0) const;
    double getIy(const double &factor= 1.0,const double &z0= 0.0) const;
    double getPyz(const double &factor= 1.0,const double &y0= 0.0,const double &z0= 0.0) const;
    double getI1(const double &factor= 1.0,const double &y0= 0.0,const double &z0= 0.0) const;
    double getI2(const double &factor= 1.0,const double &y0= 0.0,const double &z0= 0.0) const;
    double getTh1(const double &y0= 0.0,const double &z0= 0.0) const;
    Vector getEje1(const double &y0= 0.0,const double &z0= 0.0) const;
    Vector getEje2(const double &y0= 0.0,const double &z0= 0.0) const;
    double getIyHomogenizedSection(const double &) const;
    double getIzHomogenizedSection(const double &) const;
    double getPyzHomogenizedSection(const double &) const;
    double getIHomogenizedSection(const double &,const unsigned short int &,const unsigned short int &) const;
    Matrix &getIHomogenizedSection(const double &E0) const;
    Matrix &getIHomogenizedSection(const double &E0,const Pos2d &o) const;
    double getIHomogenizedSection(const double &E0,const Pos2d &O,const Vector &e) const;
    double getIHomogenizedSection(const double &E0,const Recta2d &r) const;
    //Static moments.
    double getSzPos(const double &yf,const double &y0,const double &factor= 1.0) const;
    double getSzNeg(const double &yf,const double &y0,const double &factor= 1.0) const;
    double getSyPos(const double &zf,const double &z0,const double &factor= 1.0) const;
    double getSyNeg(const double &zf,const double &z0,const double &factor= 1.0) const;
    double getSPosHomogenizedSection(const double &E0,const Semiplano2d &sp) const;
    double getSNegHomogenizedSection(const double &E0,const Semiplano2d &sp) const;
    
    //! @brief Returns the polar moment of inertia with respecto to G.
    inline double getIpolar(const double &factor= 1.0) const
      { return getIz(factor,yCDG)+getIy(factor,zCDG); }
    //! @brief Returns the radio de giro respecto al eje paralelo al z que pasa por el CDG.
    inline double getiz(const double factor= 1.0) const
      { return getIz(factor,yCDG)/getArea(factor); }
    //! @brief Returns the radio de giro respecto al eje paralelo al y que pasa por el CDG.
    inline double getiy(const double factor= 1.0) const
      { return getIy(factor,zCDG)/getArea(factor); }
    size_t nearest_fiber(const double &y,const double &z) const;

    void Print(std::ostream &s,const int &flag);
  };
} // end of XC namespace

#endif
