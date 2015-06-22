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
//beam3dBase.h

#ifndef beam3dBase_h
#define beam3dBase_h

#include "domain/mesh/element/truss_beam_column/ProtoBeam3d.h"
#include "utility/matrix/Matrix.h"
#include "utility/matrix/Vector.h"

namespace XC {

//! \ingroup ElemBarra
//
//! @brief Elemento barra Base para problemas tridimensionales.
class beam3dBase : public ProtoBeam3d
  {
  protected:
    double theta;
    mutable double L;
    mutable Vector rForce;
    mutable bool isStiffFormed;

    static Matrix k; // the stiffness matrix
    static Matrix m; // the mass matrix	
    static Matrix d; // the damping matrix

    virtual const Matrix &getStiff(void) const= 0;

    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    beam3dBase(int classTag, int tag= 0);        
    beam3dBase(int tag, int classTag, double A, double E, double G, double Jx, double Iy, double Iz, int Nd1, int Nd2, double theta);

    int revertToLastCommit(void);        
    
    int addLoad(ElementalLoad *theLoad, double loadFactor);
    int addInertiaLoadToUnbalance(const Vector &accel);

    const Matrix &getTangentStiff(void) const;
    const Matrix &getInitialStiff(void) const;

    const Vector &getResistingForce(void) const;
    const Vector &getResistingForceIncInertia(void) const;            
    
    void Print(std::ostream &s, int flag =0);    
  };
} // fin namespace XC

#endif


