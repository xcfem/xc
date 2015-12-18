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
//ProtoTruss.h

#ifndef ProtoTruss_h
#define ProtoTruss_h

#include <domain/mesh/element/Element1D.h>

namespace XC {
class Node;
class Material;

//! \ingroup ElemBarra
//
//! @defgroup ElemBarraArt Elementos de tipo barra articulada.
//!
//! \ingroup ElemBarraArt
//
//! @brief Clase base para los elementos de tipo barra articulada.
class ProtoTruss : public Element1D
  {
  protected:
    int numDOF; //!< number of dof for truss
    int dimSpace; //!< truss in 2 or 3d domain
    Matrix *theMatrix; //!< pointer to objects matrix (a class wide Matrix)
    Vector *theVector; //!< pointer to objects vector (a class wide Vector)

    // static data - single copy for all objects of the class	
    static Matrix trussM2;   // class wide matrix for 2*2
    static Matrix trussM3;   // class wide matrix for 3*3
    static Matrix trussM4;   // class wide matrix for 4*4
    static Matrix trussM6;   // class wide matrix for 6*6
    static Matrix trussM12;  // class wide matrix for 12*12
    static Vector trussV2;   // class wide Vector for size 2
    static Vector trussV3;   // class wide Vector for size 3
    static Vector trussV4;   // class wide Vector for size 44
    static Vector trussV6;   // class wide Vector for size 6
    static Vector trussV12;  // class wide Vector for size 12

    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);
    void setup_matrix_vector_ptrs(int dofNd1);
    bool procesa_comando(CmdStatus &status);
  public:
    ProtoTruss(int tag, int classTag,int Nd1,int Nd2,int ndof,int dimSpace);
    ProtoTruss(const ProtoTruss &);
    ProtoTruss &operator=(const ProtoTruss &);

    virtual const Material *getMaterial(void) const= 0;
    virtual Material *getMaterial(void)= 0;
    Material &getMaterialRef(void);
    virtual double getRho(void) const= 0;

    // public methods to obtain inforrmation about dof & connectivity    
    int getNumDIM(void) const;	
    int getNumDOF(void) const;
    any_const_ptr GetProp(const std::string &cod) const;
  };

} //end of XC namespace
#endif
