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
//SmallDispCrdTransf3d.h

#ifndef SmallDispCrdTransf3d_h
#define SmallDispCrdTransf3d_h

#include "CrdTransf3d.h"

namespace XC {

//! \ingroup ElemCT
//
//! @brief Clase base para las transformaciones de coordenadas en 3d.
class SmallDispCrdTransf3d: public CrdTransf3d
  {
    const Matrix &computeRW(const Vector &nodeOffset) const;
  protected:
    virtual int computeElemtLengthAndOrient(void) const;
    virtual int calculaEjesLocales(void) const;
    Vector &basic_to_local_resisting_force(const Vector &pb, const Vector &p0) const;
    const Vector &local_to_global_resisting_force(const Vector &pl) const;
    Matrix &basic_to_local_stiff_matrix(const Matrix &KB) const;
    const Matrix &local_to_global_stiff_matrix(const Matrix &kl) const;
    DbTagData &getDbTagData(void) const;
  public:
    SmallDispCrdTransf3d(int tag, int classTag);
    SmallDispCrdTransf3d(int tag, int classTag, const Vector &vecInLocXZPlane);

    double getInitialLength(void) const;
    double getDeformedLength(void) const;

    const Vector &getBasicTrialDisp(void) const;
    const Vector &getBasicIncrDisp(void) const;
    const Vector &getBasicIncrDeltaDisp(void) const;
    const Vector &getBasicTrialVel(void) const;
    const Vector &getBasicTrialAccel(void) const;

    const Matrix &getInitialGlobalStiffMatrix(const Matrix &basicStiff) const;

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
  };
} // end of XC namespace

#endif
