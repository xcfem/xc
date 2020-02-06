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
//FiberSectionBase.h

#ifndef FiberSectionBase_h
#define FiberSectionBase_h

#include <material/section/PrismaticBarCrossSection.h>
#include "material/section/fiber_section/fiber/FiberContainer.h"
#include "material/section/fiber_section/fiber/FiberSets.h"
#include "xc_utils/src/geom/GeomObj.h"
#include <material/section/CrossSectionKR.h>

class Polygon2d;

namespace XC {
class Fiber;
class Response;
class FiberSectionRepr;
class InteractionDiagramData;
class InteractionDiagram;
class InteractionDiagram2d;
class NMPointCloud;
class NMyMzPointCloud;

//! @ingroup MATSCC
//!
//! @defgroup MATSCCFiberModel Fiber model for cross-sections.
//
//! @ingroup MATSCCFiberModel
//
//! @brief Base class for fiber sections.
class FiberSectionBase: public PrismaticBarCrossSection
  {
  public:
    typedef FiberSets::iterator fiber_set_iterator;
    typedef FiberSets::const_iterator fiber_set_const_iterator;
  private:
    Vector eTrial; //!< trial section deformations 
    Vector eInic; //!< initial section deformations 
    Vector eCommit; //!< committed section deformations 
  protected:
    CrossSectionKR kr; //!< Stiffness and internal forces resultant on the section.
    FiberContainer fibers; //!< Pointers to fibers container.
    int fiberTag; //!< Tag for next fiber.
    FiberSets fiber_sets;//!< Fibers sets.
    friend class FiberPtrDeque;
    friend class FiberContainer;
    FiberSectionRepr *section_repres; //! Section representation.

    int sendData(CommParameters &);
    int recvData(const CommParameters &);
    
    void setup_repres(void);
    inline void alloc_fibers(int numOfFibers,const Fiber *sample= nullptr)
      { fibers.allocFibers(numOfFibers,sample); }
    void create_fiber_set(const std::string &nombre);
    fiber_set_iterator get_fiber_set(const std::string &set_name);
    virtual double get_dist_to_neutral_axis(const double &,const double &) const;
    Pos3d Esf2Pos3d(void) const;
    Pos3d getNMyMz(const DeformationPlane &);
    void getInteractionDiagramPointsForTheta(NMyMzPointCloud &lista_esfuerzos,const InteractionDiagramData &,const FiberPtrDeque &,const FiberPtrDeque &,const double &);
    const NMyMzPointCloud &getInteractionDiagramPoints(const InteractionDiagramData &);
    const NMPointCloud &getInteractionDiagramPointsForPlane(const InteractionDiagramData &, const double &);
  public:
    FiberSectionBase(int classTag,int dim,MaterialHandler *mat_ldr= nullptr); 
    FiberSectionBase(int tag, int classTag,int dim,MaterialHandler *mat_ldr= nullptr);
    FiberSectionBase(int tag, int classTag, int numFibers,int dim,MaterialHandler *mat_ldr= nullptr);
    FiberSectionBase(const FiberSectionBase &);
    FiberSectionBase &operator=(const FiberSectionBase &);
    ~FiberSectionBase(void);

    virtual void setupFibers(void) = 0;
    inline size_t getNumFibers(void) const
      { return fibers.getNumFibers(); }
    inline FiberContainer &getFibers(void)
      { return fibers; }
    virtual Fiber *addFiber(Fiber &)= 0;
    virtual Fiber *addFiber(int tag,const MaterialHandler &,const std::string &nmbMat,const double &, const Vector &position)= 0;
    Fiber *addFiber(const std::string &nmbMat,const double &area,const Vector &coo);
    int setInitialSectionDeformation(const Vector &deforms); 
    int setTrialSectionDeformation(const Vector &deforms);
    inline void zeroInitialSectionDeformation(void)
      { eInic.Zero(); }
    inline const Vector &getInitialSectionDeformation(void) const
      { return eInic; }
    const Vector &getSectionDeformation(void) const;

    FiberSectionRepr *getFiberSectionRepr(void);
    GeomSection *getGeomSection(void);
    const GeomSection *getGeomSection(void) const;
    Polygon2d getRegionsContour(void) const;
    double getCompressedZoneDepth(const Line2d &) const;
    double getCompressedZoneDepth(void) const;
    double getTensionedZoneDepth(const Line2d &) const;
    double getTensionedZoneDepth(void) const;
    double getLeverArm(void) const;
    double getNeutralAxisDepth(void) const;
    double getNeutralAxisDist(const double &y,const double &z) const;
    Vector getLeverArmVector(void) const;
    Vector getVectorCantoUtil(void) const;
    Segment2d getLeverArmSegment(void) const;
    Segment2d getEffectiveDepthSegment(void) const;
    double getMechanicLeverArm(void) const;
    double getEffectiveDepth(void) const;
    Line2d getBendingPlaneTrace(void) const;
    Line2d getTensionedPlaneTrace(void) const;
    Line2d getCompressedPlaneTrace(void) const;
    double getAnchoMecanico(void) const;
    double getCompressedStrutWidth(void) const;
    double getCover(const Pos2d &) const;
    Line2d getEffectiveConcreteAreaLimitLine(const double &) const;
    double getGrossEffectiveConcreteArea(const double &) const;
    double getNetEffectiveConcreteArea(const double &,const std::string &,const double &factor= 15) const;
    double computeFibersEffectiveConcreteArea(const double &hEfMax,const std::string &,const double &factor= 15) const;
    std::list<Polygon2d> getGrossEffectiveConcreteAreaContour(const double &) const;
    void computeCovers(const std::string &) const;
    void computeSpacement(const std::string &) const;
    int updateCenterOfMass(void);
    double getHomogenizedI(const double &E0) const;
    double getSPosHomogeneizada(const double &E0) const;

    const Vector &getStressResultant(void) const;
    double getStressResultant(const int &) const;
    const Matrix &getSectionTangent(void) const;

    int commitState(void);
    int revertToLastCommit(void);    
    int revertToStart(void);

    std::string getStrClaseEsfuerzo(const double &tol= 1e-4) const;
    
    inline FiberSets &getFiberSets(void)
      { return fiber_sets; }
    //fiber_set_iterator sel(const std::string &set_name,const std::string &cond);
    fiber_set_iterator sel_mat_tag(const std::string &set_name,const int &matTag);
    //fiber_set_iterator resel(const std::string &set_name,const std::string &set_name_org,const std::string &cond);
    fiber_set_iterator resel_mat_tag(const std::string &set_name,const std::string &set_name_org,const int &matTag);
    inline virtual double getCenterOfMassY(void) const
      { return fibers.getCenterOfMassY(); }
    double getArea(void) const;

    InteractionDiagram GetInteractionDiagram(const InteractionDiagramData &);
    InteractionDiagram2d GetInteractionDiagramForPlane(const InteractionDiagramData &,const double &);
    InteractionDiagram2d GetNMyInteractionDiagram(const InteractionDiagramData &);
    InteractionDiagram2d GetNMzInteractionDiagram(const InteractionDiagramData &);

    // AddingSensitivity:BEGIN //////////////////////////////////////////
    int setParameter(const std::vector<std::string> &argv, Parameter &param);
    // AddingSensitivity:END ///////////////////////////////////////////
  };
} // end of XC namespace

#endif
