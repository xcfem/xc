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
//ShellLinearCrdTransf3d.h

#ifndef ShellLinearCrdTransf3d_h
#define ShellLinearCrdTransf3d_h

#include "ShellCrdTransf3dBase.h"

namespace XC {

class Matrix;

//! \ingroup ElemPlanos
//
//! @brief Clase base para las transformaciones de coordenadas en 3d.
class ShellLinearCrdTransf3d: public ShellCrdTransf3dBase
  {
  protected:
    const Vector &local_to_global_resisting_force(const Vector &pl) const;
    const Matrix &local_to_global_stiff_matrix(const Matrix &kl) const;
  public:
    ShellLinearCrdTransf3d(void);
    ShellLinearCrdTransf3d(const Vector &,const Vector &,const Vector &);
    ShellLinearCrdTransf3d(const NodePtrs &t);
    virtual ShellCrdTransf3dBase *getCopy(void) const;

    virtual int initialize(const NodePtrs &);
    virtual int update(void);

    virtual int commitState(void);
    virtual int revertToLastCommit(void);        
    virtual int revertToStart(void);
    
    virtual Vector getBasicTrialDisp(const int &) const;
    virtual Vector getBasicTrialVel(const int &) const;
    virtual Vector getBasicTrialAccel(const int &) const;

    virtual const Vector &getGlobalResistingForce(const Vector &p0) const;

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
  };

} // fin namespace XC

#endif
