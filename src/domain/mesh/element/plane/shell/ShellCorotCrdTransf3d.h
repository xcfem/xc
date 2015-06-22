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
//ShellCorotCrdTransf3d.h

#ifndef ShellCorotCrdTransf3d_h
#define ShellCorotCrdTransf3d_h

#include "ShellCrdTransf3dBase.h"

namespace XC {

//! \ingroup ElemPlanos
//
//! @brief Clase base para las transformaciones de coordenadas en 3d.
class ShellCorotCrdTransf3d: public ShellCrdTransf3dBase
  {
  protected:
    Vector v130;
    Vector v240;

    Vector g1trial; //!< Vector 1 del triedro de referencia.
    Vector g2trial; //!< Vector 2 del triedro de referencia.
    Vector g3trial; //!< Vector 3 del triedro de referencia.

    Vector g1commit; //!< Vector 1 del triedro de referencia.
    Vector g2commit; //!< Vector 2 del triedro de referencia.
    Vector g3commit; //!< Vector 3 del triedro de referencia.

    Vector global_to_local(const Vector &,const Vector &) const;
    Vector global_to_local_disp_nod(const int &) const;
    Vector global_to_local_vel_nod(const int &) const;
    Vector global_to_local_accel_nod(const int &) const;
    Vector local_to_global(const Matrix &,const Matrix &,const Vector &) const;
    Matrix local_to_global(const Matrix &,const Matrix &,const Matrix &) const;
    const Vector &local_to_global_resisting_force(const Vector &pl) const;
  public:
    ShellCorotCrdTransf3d(void);
    ShellCorotCrdTransf3d(const Vector &,const Vector &,const Vector &);
    ShellCorotCrdTransf3d(const NodePtrs &t);
    virtual ShellCrdTransf3dBase *getCopy(void) const;

    //! @brief Devuelve el eje 1 local (contenido en el plano del elemento)
    inline const Vector &G1trial(void) const
      { return g1trial; }
    //! @brief Devuelve el eje 2 local (contenido en el plano del elemento)
    inline const Vector &G2trial(void) const
      { return g2trial; }
    //! @brief Devuelve el eje 3 local (normal al plano del elemento)
    inline const Vector &G3trial(void) const
      { return g3trial; }

    Matrix getR0(void) const;
    Matrix getR(void) const;
    Matrix getR0T(void) const;

    virtual int initialize(const NodePtrs &);
    virtual int update(void);

    virtual int commitState(void);
    virtual int revertToLastCommit(void);        
    virtual int revertToStart(void);
    
    virtual Vector getBasicTrialDisp(const int &) const;
    virtual Vector getBasicTrialVel(const int &) const;
    virtual Vector getBasicTrialAccel(const int &) const;

    void getGlobalTangent(Matrix &stiff) const;
    void getGlobalResidAndTangent(Vector &resid,Matrix &stiff) const;
    virtual const Vector &getGlobalResistingForce(const Vector &p0) const;

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    any_const_ptr GetProp(const std::string &cod) const;
  };

} // fin namespace XC

#endif
