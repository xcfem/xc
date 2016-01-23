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
//BeamMecLoad.h
                                                                        
#ifndef BeamMecLoad_h
#define BeamMecLoad_h


#include <domain/load/beam_loads/BeamLoad.h>

namespace XC {
class Matrix;
class FVector;
class CrossSectionProperties2d;
class CrossSectionProperties3d;

//! @ingroup ElemLoads
//
//! @brief Carga de tipo mecánico (fuerzas) sobre elementos de tipo viga.
class BeamMecLoad : public BeamLoad
  {
  protected:
    double Trans; //!< Transverse load.
    double Axial; //!< Axial load.

    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);
  public:
    BeamMecLoad(int tag, int classTag,const double &Trans,const double &Axial,const ID &theElementTags);
    BeamMecLoad(int tag, int classTag);

    inline double getTransComponent(void) const
      { return Trans; }
    inline double getAxialComponent(void) const
      { return Axial; }
    inline void setTransComponent(const double &t)
      { Trans= t; }
    inline void setAxialComponent(const double &a)
      { Axial= a; }

    virtual const Matrix &getAppliedSectionForces(const double &L,const XC::Matrix &xi,const double &loadFactor);
    virtual void addReactionsInBasicSystem(const double &,const double &,FVector &);
    virtual void addFixedEndForcesInBasicSystem(const double &,const double &,FVector &);
    void addElasticDeformations(const double &L,const CrossSectionProperties3d &ctes_scc,const double &lpI,const double &lpJ,const double &loadFactor,FVector &v0);
    void addElasticDeformations(const double &L,const CrossSectionProperties2d &ctes_scc,const double &lpI,const double &lpJ,const double &loadFactor,FVector &v0);

    virtual size_t getDimVectorFuerza(void) const;
    virtual size_t getDimVectorMomento(void) const;
    virtual const Matrix &getLocalForces(void) const;
    virtual const Matrix &getLocalMoments(void) const;
    virtual const Matrix &getGlobalVectors(const Matrix &) const;
    virtual const Matrix &getGlobalForces(void) const;
    virtual const Matrix &getGlobalMoments(void) const;

    void Print(std::ostream &s, int flag =0) const;

  };
} // end of XC namespace

#endif

