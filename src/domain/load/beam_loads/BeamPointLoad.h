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
//BeamPointLoad.h
                                                                        
#ifndef BeamPointLoad_h
#define BeamPointLoad_h

#include "domain/load/beam_loads/BeamMecLoad.h"

namespace XC {
//! @ingroup ElemLoads
//
//! @brief Carga puntual sobre elementos barra.
class BeamPointLoad: public BeamMecLoad
  {
  protected:
    double x; //!< relative distance (x/L) along length from end 1 of element

    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);

  public:
    BeamPointLoad(int tag, int classTag, double Pt, double x,const ID &theElementTags, double Pa = 0.0);
    BeamPointLoad(int tag, int classTag);

    inline double X(void) const
      { return x; }
    inline void setX(const double &X)
      { x= X; } 
    
    std::string Categoria(void) const;


  };
} // end of XC namespace

#endif

