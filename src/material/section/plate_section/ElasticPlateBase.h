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
//ElasticPlateBase.h

#ifndef ElasticPlateBase_h
#define ElasticPlateBase_h

#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>

#include "material/section/SectionForceDeformation.h"


namespace XC {

//! \ingroup MATSCC
//!  @defgroup MATPLAC Materiales para elementos bidimensionales (placas,...).
//
//! @ingroup MATPLAC
//
//! @brief Clase base para para materiales bidimensionales
//! (como los de placas).
class ElasticPlateBase: public SectionForceDeformation
  {
  protected:
    double E;  // elastic modulus
    double nu; // poisson ratio
    double h; // plate thickness

    static const double five6; // =5/6 = shear correction factor

    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);
    bool procesa_comando(CmdStatus &status);
  public : 

    ElasticPlateBase(int tag,int classTag);
    ElasticPlateBase(int classTag);
    ElasticPlateBase(int tag,int classTag, double E, double nu, double h);

    int commitState(void); 
    int revertToLastCommit(void);
    int revertToStart(void);

    inline double getE(void) const
      { return E; }
    void setE(const double &d)
      { E= d; }
    inline double getnu(void) const
      { return nu; }
    void setnu(const double &d)
      { nu= d; }
    inline double getH(void) const
      { return h; }
    void setH(const double &d)
      { h= d; }

    inline double membraneModulus(void) const
      { return (E/(1.0-nu*nu)*h); }
    inline double shearModulus(void) const
      { return (0.5*E/(1.0+nu)*h); }
    inline double bendingModulus(void) const
      { return (E*(h*h*h)/12.0/( 1.0 - nu*nu )); }

    virtual any_const_ptr GetProp(const std::string &cod) const;
  }; //end of ElasticPlateBase declarations

} // fin namespace XC

#endif
