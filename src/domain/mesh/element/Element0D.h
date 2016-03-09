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
//Element0D.h

#ifndef Element0D_h
#define Element0D_h

#include <domain/mesh/element/ElementBase.h>
#include <utility/matrix/Matrix.h>

namespace XC {
class Node;
class Material;

//! \ingroup Elem
//
//! @brief Elemento de dimensión 0 (los dos nodos ocupan la misma posición en el espacio).
class Element0D : public ElementBase<2>
  {
  protected:
    static const double LenTol; // Tolerance for zero length of element
    class Vxy: public EntCmd
      {
        Vector x,y;
      public:
        Vxy(void);
        Vxy(const Vector &,const Vector &);
        inline const Vector &getX(void)
	  { return x; }
        inline const Vector &getY(void)
	  { return y; }
        bool check(void) const;
      };
    int dimension; // = 2 or 3 dimensions
    int numDOF;	   // number of dof for ZeroLengthSection
    Matrix transformation; // transformation matrix for orientation

    virtual void setUp(int Nd1, int Nd2,const Vector &x,const Vector &y);
    TritrizPtrElem cose(const SetEstruct &f1,const SetEstruct &f2) const;

    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);
  public:
    Element0D(int tag, int classTag,int Nd1,int Nd2);
    Element0D(int tag, int classTag,int Nd1,int Nd2, int dim);
    Element0D(int tag, int classTag,int Nd1,int Nd2, int dim, const Vector &, const Vector &);
    int getNumDOF(void) const;
    size_t getDimension(void) const;
    int getVtkCellType(void) const;
    int getMEDCellType(void) const;
    const Vector &getX(void) const;
    const Vector &getY(void) const;
    const Vector &getZ(void) const;
    inline virtual const Matrix &getTransformation(void) const
      { return transformation; }

    virtual void setUpVectors(const Vector &, const Vector &);
  };


} //end of XC namespace
#endif
