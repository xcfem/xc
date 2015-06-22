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
//FiberSectionShear3d.h

#ifndef FiberSectionShear3d_h
#define FiberSectionShear3d_h

#include "FiberSection3d.h"

namespace XC {
//! \ingroup MATSCC
//
//! @brief FiberSectionShear3d decorates an MP
//! section (couple bending and axial) with an uncoupled shear
//! relation.
class FiberSectionShear3d : public FiberSection3d
  {
  private:
    UniaxialMaterial *respVy;
    UniaxialMaterial *respVz;
    UniaxialMaterial *respT;
    
    static Vector def; //!< Storage for section deformations
    static Vector defzero; //!< Storage for initial section deformations
    static Vector s; //!< Storage for stress resultants
    static Matrix ks;//!< Storage for section stiffness
    static Matrix fs;//!< Storage for section flexibility

    void setRespVy(const UniaxialMaterial *);
    void setRespVz(const UniaxialMaterial *);
    void setRespT(const UniaxialMaterial *);
    void setRespVyVzT(const UniaxialMaterial *,const UniaxialMaterial *,const UniaxialMaterial *);

    void liberaRespVyVzT(void);
    void libera(void);
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
    bool procesa_comando(CmdStatus &status);
  public:
    FiberSectionShear3d(int tag= 0,MaterialLoader *mat_ldr= nullptr);
    FiberSectionShear3d(const FiberSectionShear3d &otro);
    XC::FiberSectionShear3d &operator=(const FiberSectionShear3d &otro);
    ~FiberSectionShear3d(void);

    void setRespVyByName(const std::string &);
    UniaxialMaterial *getRespVy(void);
    void setRespVzByName(const std::string &);
    UniaxialMaterial *getRespVz(void);
    void setRespTByName(const std::string &);
    UniaxialMaterial *getRespT(void);
    void setRespVyVzTByName(const std::string &,const std::string &,const std::string &);

    int setInitialSectionDeformation(const Vector &deforms); 
    int setTrialSectionDeformation(const Vector &deforms); 
    const Vector &getInitialSectionDeformation(void) const;
    const Vector &getSectionDeformation(void) const;
    const Vector &getStressResultant(void) const;
    const Matrix &getSectionTangent(void) const;
    const Matrix &getInitialTangent(void) const;
    const Matrix &getSectionFlexibility(void) const;
    const Matrix &getInitialFlexibility(void) const;

    int commitState(void);
    int revertToLastCommit(void);    
    int revertToStart(void);
 
    SectionForceDeformation *getCopy(void) const;
    const ResponseId &getType(void) const;
    int getOrder(void) const;

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
 
    void Print(std::ostream &s, int flag =0) const;

    int setVariable(const std::string &argv);
    int getVariable(int variableID, double &info);
    virtual any_const_ptr GetProp(const std::string &cod) const;
  };
} // fin namespace XC

#endif
