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
//FVectorShell.h

#ifndef FVectorShell_h
#define FVectorShell_h

#include "domain/mesh/element/fvectors/FVectorData.h"

namespace XC {
class Vector;

//! \ingroup ElemFV
//
//! @brief Esfuerzos en un elemento de tipo lámina.
class FVectorShell: public FVectorData<24>
  {
  public:
    FVectorShell(void);
    FVectorShell(const FVectorShell &otro);
    explicit FVectorShell(const Vector &);
    FVectorShell &operator=(const FVectorShell &otro);
    void zero(void);

    FVectorShell &operator*=(const double &d);
    FVectorShell &operator+=(const FVectorShell &otro);
    FVectorShell &operator-=(const FVectorShell &otro);

    void addForce(const size_t &inod,const double &,const double &,const double &);
    void addMoment(const size_t &inod,const double &,const double &,const double &);

    void Print(std::ostream &os) const;
  };

} // end of XC namespace

#endif


