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
#include "material/section/fiber_section/fiber/StoFibras.h"
#include "material/section/fiber_section/fiber/FiberSets.h"
#include "xc_utils/src/geom/GeomObj.h"
#include <material/section/CrossSectionKR.h>

class Poligono2d;

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
//
//! @defgroup MATSCCModeloFibras Fiber model for cross-sections.
//
//! @ingroup MATSCCModeloFibras
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
    StoFibras fibras; //!< Pointers to fibers container.
    int tag_fibra; //!< Tag for next fiber.
    FiberSets sets_fibras;//!< Fibers sets.
    friend class DqFibras;
    friend class StoFibras;
    FiberSectionRepr *section_repres; //! Section representation.

    void setup_repres(void);
    inline void alloc_fibers(int numFibras,const Fiber *muestra= nullptr)
      { fibras.allocFibers(numFibras,muestra); }
    void create_fiber_set(const std::string &nombre);
    fiber_set_iterator get_fiber_set(const std::string &nmb_set);
    virtual double get_dist_to_neutral_axis(const double &,const double &) const;
    Pos3d Esf2Pos3d(void) const;
    Pos3d getNMyMz(const DeformationPlane &);
    void getInteractionDiagramPointsForTheta(NMyMzPointCloud &lista_esfuerzos,const InteractionDiagramData &,const DqFibras &,const DqFibras &,const double &);
    const NMyMzPointCloud &getInteractionDiagramPoints(const InteractionDiagramData &);
    const NMPointCloud &getInteractionDiagramPointsForPlane(const InteractionDiagramData &, const double &);
  public:
    FiberSectionBase(int classTag,int dim,MaterialLoader *mat_ldr= nullptr); 
    FiberSectionBase(int tag, int classTag,int dim,MaterialLoader *mat_ldr= nullptr);
    FiberSectionBase(int tag, int classTag, int numFibers,int dim,MaterialLoader *mat_ldr= nullptr);
    FiberSectionBase(const FiberSectionBase &otro);
    FiberSectionBase &operator=(const FiberSectionBase &otro);
    ~FiberSectionBase(void);

    virtual void setupFibers(void) = 0;
    inline size_t getNumFibers(void) const
      { return fibras.getNumFibers(); }
    inline StoFibras &getFibers(void)
      { return fibras; }
    virtual Fiber *addFiber(Fiber &)= 0;
    virtual Fiber *addFiber(int tag,const MaterialLoader &,const std::string &nmbMat,const double &, const Vector &position)= 0;
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
    Poligono2d getRegionsContour(void) const;
    double getCompressedZoneLeverArm(const Recta2d &) const;
    double getCompressedZoneLeverArm(void) const;
    double getTensionedZoneLeverArm(const Recta2d &) const;
    double getTensionedZoneLeverArm(void) const;
    double getLeverArm(void) const;
    double getNeutralAxisDepth(void) const;
    double getDistFibraNeutra(const double &y,const double &z) const;
    Vector getVectorBrazoMecanico(void) const;
    Vector getVectorCantoUtil(void) const;
    Segmento2d getSegmentoBrazoMecanico(void) const;
    Segmento2d getSegmentoCantoUtil(void) const;
    double getBrazoMecanico(void) const;
    double getCantoUtil(void) const;
    Recta2d getTrazaPlanoFlexion(void) const;
    Recta2d getTrazaPlanoTraccion(void) const;
    Recta2d getTrazaPlanoCompresion(void) const;
    double getAnchoMecanico(void) const;
    double getAnchoBielaComprimida(void) const;
    double getRecubrimiento(const Pos2d &) const;
    Recta2d getRectaLimiteAcEficaz(const double &) const;
    double getAcEficazBruta(const double &) const;
    double getAcEficazNeta(const double &,const std::string &,const double &factor= 15) const;
    double calcAcEficazFibras(const double &hEfMax,const std::string &,const double &factor= 15) const;
    std::list<Poligono2d> getContourAcEficazBruta(const double &) const;
    void calcRecubrimientos(const std::string &) const;
    void calcSeparaciones(const std::string &) const;
    int updateCDG(void);
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
      { return sets_fibras; }
    //fiber_set_iterator sel(const std::string &nmb_set,const std::string &cond);
    fiber_set_iterator sel_mat_tag(const std::string &nmb_set,const int &matTag);
    //fiber_set_iterator resel(const std::string &nmb_set,const std::string &nmb_set_org,const std::string &cond);
    fiber_set_iterator resel_mat_tag(const std::string &nmb_set,const std::string &nmb_set_org,const int &matTag);
    inline virtual double getCdgY(void) const
      { return fibras.getYCdg(); }
    double getArea(void) const;

    InteractionDiagram GetInteractionDiagram(const InteractionDiagramData &);
    InteractionDiagram2d GetInteractionDiagramForPlane(const InteractionDiagramData &,const double &);
    InteractionDiagram2d GetNMyInteractionDiagram(const InteractionDiagramData &);
    InteractionDiagram2d GetNMzInteractionDiagram(const InteractionDiagramData &);
  };
} // end of XC namespace

#endif
