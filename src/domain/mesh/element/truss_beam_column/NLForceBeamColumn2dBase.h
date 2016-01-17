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
//NLForceBeamColumn2dBase.h

#ifndef NLForceBeamColumn2dBase_h
#define NLForceBeamColumn2dBase_h

#include <domain/mesh/element/truss_beam_column/BeamColumnWithSectionFDTrf2d.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include "domain/mesh/element/fvectors/FVectorBeamColumn2d.h"

namespace XC {

//! \ingroup ElemBarra
//
//! @brief Elemento barra no lineal con material de tipo SeccionBarraPrismatica para problemas bidimensionales.
class NLForceBeamColumn2dBase: public BeamColumnWithSectionFDTrf2d
  {
    NLForceBeamColumn2dBase &operator=(const NLForceBeamColumn2dBase &);
  protected:
    static const size_t NDM= 2; //!< dimension of the problem (2d)
    static const int NND= 3; //!< number of nodal dof's
    static const size_t NEGD= 6; //!< number of element global dof's
    static const size_t NEBD= 3; //!< number of element dof's in the basic system

    
    // internal data
    double rho; //!<mass density per unit length
    int maxIters; //!<maximum number of local iterations
    double tol;	 //!<tolerance for relative energy norm for local iterations

    int initialFlag; //!<indicates if the element has been initialized
	
    Matrix kv; //!<stiffness matrix in the basic system 
    Vector Se; //!<element resisting forces in the basic system

    Matrix kvcommit; //!<commited stiffness matrix in the basic system
    Vector Secommit; //!<commited element end forces in the basic system

    std::vector<Matrix> fs; //!<array of section flexibility matrices
    std::vector<Vector> vs; //!<array of section deformation vectors
    std::vector<Vector> Ssr; //!<array of section resisting force vectors
    std::vector<Vector> vscommit; //!<array of commited section deformation vectors

    Matrix sp; //!<Applied section forces due to element loads, 5 x nSections
    FVectorBeamColumn2d p0; // Reactions in the basic system due to element loads

    mutable Matrix Ki;

    static Matrix theMatrix;
    static Vector theVector;
    static double workArea[];

    void resizeMatrices(const size_t &nSections);
    void initializeSectionHistoryVariables(void);

    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);

  public:
    NLForceBeamColumn2dBase(int tag,int classTag,int numSec= 0);
    NLForceBeamColumn2dBase(int tag,int classTag,int numSec,const Material *theSection,const CrdTransf *coordTransf);
    NLForceBeamColumn2dBase(const NLForceBeamColumn2dBase &otro);
    ~NLForceBeamColumn2dBase(void);

    int getNumDOF(void) const;

    double getRho(void) const
      { return rho; }
    void setRho(const double &r)
      { rho= r; }

    const Matrix &getTangentStiff(void) const;

    const Vector &getResistingForce(void) const;
  };
} // end of XC namespace

#endif

