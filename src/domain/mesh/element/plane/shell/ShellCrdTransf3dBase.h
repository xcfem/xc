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
//ShellCrdTransf3dBase.h

#ifndef ShellCrdTransf3dBase_h
#define ShellCrdTransf3dBase_h

#include "xc_utils/src/nucleo/EntCmd.h"
#include "utility/actor/actor/MovableObject.h"
#include "utility/matrix/Vector.h"
#include "domain/mesh/element/ParticlePos3d.h"

class Plano3d;
class Ref2d3d;

namespace XC {

class Matrix;
class NodePtrs;

//! \ingroup ElemPlanos
//
//! @brief Clase base para las transformaciones de coordenadas en 3d.
class ShellCrdTransf3dBase: public EntCmd, public MovableObject
  {
  protected:
    const NodePtrs *theNodes;
    Vector vpos_centroide; //!< Vector de posición del centroide del elemento.
    Vector g1; //!< Vector 1 del triedro de referencia.
    Vector g2; //!< Vector 2 del triedro de referencia.
    Vector g3; //!< Vector 3 del triedro de referencia.
    DbTagData &getDbTagData(void) const;
    int sendData(CommParameters &);
    int recvData(const CommParameters &);

    Vector local_to_global(const Matrix &,const Vector &) const;
    Matrix local_to_global(const Matrix &,const Matrix &) const;

  public:
    ShellCrdTransf3dBase(void);
    inline virtual ~ShellCrdTransf3dBase(void)
      {}
    ShellCrdTransf3dBase(const Vector &,const Vector &,const Vector &);
    virtual ShellCrdTransf3dBase *getCopy(void) const= 0;

    //! @brief Returns the transformation matrix.
    Matrix getTrfMatrix(void) const;
    //! @brief Devuelve el eje 1 local (contenido en el plano del elemento)
    inline const Vector &G1(void) const
      { return g1; }
    //! @brief Devuelve el eje 2 local (contenido en el plano del elemento)
    inline const Vector &G2(void) const
      { return g2; }
    //! @brief Devuelve el eje 3 local (normal al plano del elemento)
    inline const Vector &G3(void) const
      { return g3; }
    //! @brief Devuelve el centroide calculado para el elemento.
    inline const Vector &getVPosCentroide(void) const
      { return vpos_centroide; }
    Plano3d getPlane(void) const;
    Pos3d getProj(const Pos3d &);

    virtual int initialize(const NodePtrs &)= 0;
    virtual int setup_nodal_local_coordinates(double xl[2][4]) const;
    virtual int update(void)= 0;

    virtual int commitState(void) = 0;
    virtual int revertToLastCommit(void) = 0;
    virtual int revertToStart(void) = 0;
    
    virtual Vector getBasicTrialDisp(const int &) const= 0;
    virtual Vector getBasicTrialVel(const int &) const= 0;
    virtual Vector getBasicTrialAccel(const int &) const= 0;

    virtual void getGlobalTangent(Matrix &stiff) const;
    virtual void getGlobalResidAndTangent(Vector &resid,Matrix &stiff) const;
    virtual const Vector &getGlobalResistingForce(const Vector &p0) const= 0;

    const Vector &getVectorGlobalCoordFromLocal(const Vector &localCoords) const;
    const Matrix &getVectorGlobalCoordFromLocal(const Matrix &localCoords) const;
    const Vector &getVectorLocalCoordFromGlobal(const Vector &globalCoords) const;

    virtual Matrix getLocalAxes(bool initialGeometry= true) const;
    Ref2d3d getLocalReference(bool initialGeometry= true) const;
    Pos2d getLocalCoordinates(const Pos3d &) const;
    ParticlePos3d getNaturalCoordinates(const Pos3d &,double xl[2][4]) const;    


  };

} // end of XC namespace

#endif
