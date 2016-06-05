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
//ShellMITC4Base.h

#ifndef ShellMITC4Base_h
#define ShellMITC4Base_h

#include "domain/mesh/element/plane/QuadBase4N.h"
#include "domain/mesh/element/physical_properties/SectionFDPhysicalProperties.h"
#include "ShellCrdTransf3dBase.h"
#include "ShellBData.h"
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>
#include "domain/mesh/element/fvectors/FVectorShell.h"

class Poligono3d;

namespace XC {

class ShellUniformLoad;

//! \ingroup ElemPlanos
//
//! @brief Clase base para los elementos shell MIT C4.
class ShellMITC4Base : public QuadBase4N<SectionFDPhysicalProperties>
  {
  protected:
    double Ktt; //!<drilling stiffness
    mutable double xl[2][4]; //!< local nodal coordinates, two coordinates for each of four nodes

    ShellCrdTransf3dBase *theCoordTransf; //!< Transformación de coordenadas.

    FVectorShell p0; //!< Reactions in the basic system due to element loads

    mutable Matrix Ki;

    std::vector<Vector> inicDisp; //!< Desplazamientos iniciales.

    //static data
    static Matrix stiff;
    static Vector resid;
    static Matrix mass;
    static Matrix damping;

    static ShellBData BData; //!< B-bar data

    void libera(void);
    void alloc(const ShellCrdTransf3dBase *);

    void setupInicDisp(void);
    void capturaInicDisp(void);
    void zeroInicDisp(void);

    void formInertiaTerms(int tangFlag) const;
    void formResidAndTangent(int tang_flag) const;
    const Matrix calculateG(void) const;
    double *computeBdrill(int node, const double shp[3][4]) const;
    const Matrix& assembleB(const Matrix &Bmembrane, const Matrix &Bbend, const Matrix &Bshear) const;
    const Matrix& computeBmembrane(int node, const double shp[3][4] ) const;
    const Matrix& computeBbend(int node, const double shp[3][4] ) const;
    static void shape2d(const double &,const double &, const double x[2][4], double shp[3][4], double &xsj);
    int sendCoordTransf(int posFlag,const int &,const int &,CommParameters &);
    int recvCoordTransf(int posFlag,const int &posClassTag,const int &posDbTag,const CommParameters &);
    int sendData(CommParameters &);
    int recvData(const CommParameters &);

  public:
    //null constructor
    ShellMITC4Base(int classTag,const ShellCrdTransf3dBase *);
    ShellMITC4Base(int tag,int classTag,const SectionForceDeformation *ptr_mat,const ShellCrdTransf3dBase *); 
    //full constructor
    ShellMITC4Base(int tag,int classTag, int node1, int node2, int node3, int node4, const SectionFDPhysicalProperties &,const ShellCrdTransf3dBase *);
    ShellMITC4Base(const ShellMITC4Base &otro);
    ShellMITC4Base &operator=(const ShellMITC4Base &otro);
    ~ShellMITC4Base(void);

    //set domain because frank is a dumb ass 
    void setDomain(Domain *theDomain);
  
    //return number of dofs
    int getNumDOF(void) const;
	
    int update(void);

    //return stiffness matrix 
    const Matrix &getTangentStiff(void) const;
    const Matrix &getInitialStiff(void) const;
    const Matrix &getMass(void) const;

    const GaussModel &getGaussModel(void) const;

    Vector getInterpolationFactors(const ParticlePos3d &) const;
    Vector getInterpolationFactors(const Pos3d &) const;
    Vector getInterpolatedDisplacements(const ParticlePos3d &) const;
    Vector getInterpolatedDisplacements(const Pos3d &) const;

    //Métodos para definir cargas.
    const ShellUniformLoad *vector3dUniformLoadLocal(const Vector &);
    const ShellUniformLoad *vector3dUniformLoadGlobal(const Vector &);
    void strainLoad(const Matrix &);

    // methods for applying loads
    void zeroLoad(void);
    int addLoad(ElementalLoad *theLoad, double loadFactor);
    int addInertiaLoadToUnbalance(const Vector &accel);

    int commitState(void);
    int revertToLastCommit(void);
    int revertToStart(void);
    void alive(void);

    const Vector &getResistingForce(void) const;
    const Vector &getResistingForceIncInertia(void) const;
    double getMeanInternalForce(const std::string &) const;
    double getMeanInternalDeformation(const std::string &) const;

    virtual Matrix getLocalAxes(bool initialGeometry= true) const;
    virtual ShellCrdTransf3dBase *getCoordTransf(void);
    virtual const ShellCrdTransf3dBase *getCoordTransf(void) const;

    void computeBasis(void);
    ParticlePos3d getLocalCoordinatesOfNode(const int &) const;
    ParticlePos3d getNaturalCoordinates(const Pos3d &) const;

    void Print(std::ostream &s, int flag );
  }; 

} // end of XC namespace
#endif
