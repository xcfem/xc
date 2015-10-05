//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//FiberSectionBase.h

#ifndef FiberSectionBase_h
#define FiberSectionBase_h

#include <material/section/SeccionBarraPrismatica.h>
#include "material/section/fiber_section/fiber/StoFibras.h"
#include "material/section/fiber_section/fiber/FiberSets.h"
#include "xc_utils/src/geom/GeomObj.h"
#include <material/section/KRSeccion.h>

class Poligono2d;

namespace XC {
class Fiber;
class Response;
class FiberSectionRepr;
class DatosDiagInteraccion;
class DiagInteraccion;
class DiagInteraccion2d;

//! \ingroup MATSCC
//
//! @defgroup MATSCCModeloFibras Modelo de fibras de una sección.
//
//! \ingroup MATSCCModeloFibras
//
//! @brief Clase base para las secciones discretizadas en fibras.
class FiberSectionBase: public SeccionBarraPrismatica
  {
  public:
    typedef FiberSets::iterator set_fibras_iterator;
    typedef FiberSets::const_iterator set_fibras_const_iterator;
  private:
    Vector eTrial; //!< trial section deformations 
    Vector eInic; //!< initial section deformations 
    Vector eCommit; //!< committed section deformations 
  protected:
    KRSeccion kr; //!< Rigidez y resultante en la sección.
    StoFibras fibras; //!< Cola de punteros a fibras.
    int tag_fibra; //!< Tag para la próxima fibra.
    FiberSets sets_fibras;//!< Conjuntos de fibras.
    friend class DqFibras;
    friend class StoFibras;
    FiberSectionRepr *section_repres; //! Representación de la sección.

    void setup_repres(void);
    inline void alloc_fibers(int numFibras,const Fiber *muestra= nullptr)
      { fibras.allocFibers(numFibras,muestra); }
    void crea_set_fibras(const std::string &nombre);
    set_fibras_iterator get_set_fibras(const std::string &nmb_set);
    virtual double get_dist_to_neutral_axis(const double &,const double &) const;
    Pos3d Esf2Pos3d(void) const;
    const Pos3d *InsertaEsfuerzo(const PlanoDeformacion &,GeomObj::list_Pos3d &,const Pos3d *, const double &);
    const Pos3d *get_ptos_diag_interaccion_theta(GeomObj::list_Pos3d &lista_esfuerzos,const DatosDiagInteraccion &datos_diag,const DqFibras &fsC,const DqFibras &fsS,const double &theta);
    const GeomObj::list_Pos3d &get_ptos_diag_interaccion(const DatosDiagInteraccion &);
    const GeomObj::list_Pos2d &get_ptos_diag_interaccionPlano(const DatosDiagInteraccion &, const double &);
    virtual void def_section_repres(CmdStatus &status);
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
    inline const Vector &getInitialSectionDeformation(void) const
      { return eInic; }
    const Vector &getSectionDeformation(void) const;

    FiberSectionRepr *getFiberSectionRepr(void);
    Poligono2d getContornoRegiones(void) const;
    double getCantoMecanicoZonaComprimida(const Recta2d &) const;
    double getCantoMecanicoZonaComprimida(void) const;
    double getCantoMecanicoZonaTraccionada(const Recta2d &) const;
    double getCantoMecanicoZonaTraccionada(void) const;
    double getCantoMecanico(void) const;
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
    std::list<Poligono2d> getContornoAcEficazBruta(const double &) const;
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
    set_fibras_iterator sel(const std::string &nmb_set,const std::string &cond);
    set_fibras_iterator sel_mat_tag(const std::string &nmb_set,const int &matTag);
    set_fibras_iterator resel(const std::string &nmb_set,const std::string &nmb_set_org,const std::string &cond);
    set_fibras_iterator resel_mat_tag(const std::string &nmb_set,const std::string &nmb_set_org,const int &matTag);
    inline virtual double getCdgY(void) const
      { return fibras.getYCdg(); }
    double getArea(void) const;

    DiagInteraccion GetDiagInteraccion(const DatosDiagInteraccion &);
    DiagInteraccion2d GetDiagInteraccionPlano(const DatosDiagInteraccion &,const double &);
    DiagInteraccion2d GetDiagInteraccionNMy(const DatosDiagInteraccion &);
    DiagInteraccion2d GetDiagInteraccionNMz(const DatosDiagInteraccion &);
  };
} // fin namespace XC

#endif
