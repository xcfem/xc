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
//BarraSuelta.h

#ifndef BarraSuelta_h 
#define BarraSuelta_h 

#include <material/section/repres/geom_section/reinfLayer/ReinfLayer.h>
#include <utility/matrix/Vector.h>

namespace XC {
class ReinfBar;

//! @ingroup MATSCCArmaduras
//
//! @brief Armadura (barra) suelta (no incluida en una capa).
class BarraSuelta : public ReinfLayer
  {
  private:
    Vector posit; //!< Posición del centro de la barra.
  protected:
    bool procesa_comando(CmdStatus &status);

    friend class ListReinfLayer;
    friend class ReinfLayer;
    BarraSuelta(ListReinfLayer *,Material *mat);
    BarraSuelta(ListReinfLayer *,Material *mat, double  reinfBarArea, const Vector &position);
    BarraSuelta(const ReinfBar &bar);
    ReinfLayer *getCopy(void) const;
    
  public:
    // edition functions
    void setPosition(const Vector &);

    // inquiring functions
    const VectorReinfBar &getReinfBars(void) const;
  
    const Vector &getPosition(void) const;

    double getMaxY(void) const;
    double getMaxZ(void) const;
    double getMinY(void) const;
    double getMinZ(void) const;

    virtual any_const_ptr GetProp(const std::string &cod) const;
    void Print(std::ostream &s, int flag =0) const;   
  };
} // fin namespace XC


#endif

