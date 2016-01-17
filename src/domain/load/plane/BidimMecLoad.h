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
//BidimMecLoad.h
                                                                        
#ifndef BidimMecLoad_h
#define BidimMecLoad_h


#include "BidimLoad.h"

namespace XC {
class Matrix;

//! @ingroup ElemLoads
//
//! @brief Carga de tipo mecánico (fuerzas) sobre elementos bidimensionales.
class BidimMecLoad : public BidimLoad
  {
  protected:
    double Trans; //!< Transverse load.
    double Axial1; //!< Axial según el eje 1.
    double Axial2; //!< Axial según el eje 2.

    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);

  public:
    BidimMecLoad(int tag, int classTag,const double &Trans,const double &,const double &,const ID &theElementTags);
    BidimMecLoad(int tag, int classTag);

    inline double getAxial1Component(void)
      { return Axial1; }
    inline void setAxial1Component(const double &d)
      { Axial1= d; }
    inline double getAxial2Component(void)
      { return Axial2; }
    inline void setAxial2Component(const double &d)
      { Axial2= d; }
    inline double getTransComponent(void)
      { return Trans; }
    inline void setTransComponent(const double &d)
      { Trans= d; }


    void Print(std::ostream &s, int flag =0) const;

  };
} // end of XC namespace

#endif

