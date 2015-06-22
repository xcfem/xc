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
//SectionMatrices.h

#ifndef SectionMatrices_h
#define SectionMatrices_h

#include "utility/actor/actor/MovableObject.h"
#include "utility/matrix/Vector.h"
#include "utility/matrix/Matrix.h"

namespace XC {

//! \ingroup ElemBarra
//
//! @brief Matrices para cada una de las secciones (se usa en BeamColumnWithSectionFD)
class SectionMatrices: public MovableObject
  {
  private:
    std::vector<Vector> vsSubdivide;
    std::vector<Vector> SsrSubdivide;
    std::vector<Matrix> fsSubdivide;

  public:
    static const size_t maxNumSections= 10;
  private:
    void alloc(const size_t &sz);
    void free(void);

  public:
    SectionMatrices(const size_t &sz= 10);

    static const size_t &getMaxNumSections(void)
      { return maxNumSections; }

    const std::vector<Vector> &getVsSubdivide(void) const
      { return vsSubdivide; }
    const std::vector<Vector> &getSsrSubdivide(void) const
      { return SsrSubdivide; }
    const std::vector<Matrix> &getFsSubdivide(void) const
      { return fsSubdivide; }
    std::vector<Vector> &getVsSubdivide(void)
      { return vsSubdivide; }
    std::vector<Vector> &getSsrSubdivide(void)
      { return SsrSubdivide; }
    std::vector<Matrix> &getFsSubdivide(void)
      { return fsSubdivide; }

    void resize(const size_t &sz);

    void initialize(const size_t &i,const int &order);

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
  };

} //fin namespace XC
#endif
