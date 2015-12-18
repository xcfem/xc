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
//StoFibras.h

#ifndef StoFibras_h
#define StoFibras_h

#include "DqFibras.h"
#include <material/section/repres/section/contenedor_fibras.h>

namespace XC {

//! \ingroup MATSCCFibras
//
//! @brief Contenedor de fibras.
class StoFibras : public DqFibras
  {
    void libera(void);
    void copia_fibras(const StoFibras &otro);
    void copia_fibras(const contenedor_fibras &);

  public:
    StoFibras(const size_t &num= 0); 
    StoFibras(const StoFibras &otro);
    StoFibras &operator=(const StoFibras &otro);
    void allocFibers(int numFibras,const Fiber *muestra= nullptr);
    void setup(FiberSection2d &,const contenedor_fibras &,KRSeccion &);
    void setup(FiberSection3d &,const contenedor_fibras &,KRSeccion &);
    void setup(FiberSectionGJ &,const contenedor_fibras &,KRSeccion &);
    ~StoFibras(void);
  };
} // end of XC namespace

#endif
