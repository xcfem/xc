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
//PYBase.h

#ifndef PYBASE_H
#define PYBASE_H

#include <material/uniaxial/UniaxialMaterial.h>
#include <material/uniaxial/PY/InternalParamsIn.h>
#include <material/uniaxial/PY/InternalParamsLRIn.h>

namespace XC {
//! @ingroup MatUnx
//
//! @brief Material que representa una curva p-y.
class PYBase: public UniaxialMaterial
  {
  protected:
    InternalParamsA C; //!< Committed history variables for entire Q-z material.
    InternalParamsA T; //!< Trial history variables for entire p-y material.
    double TvRate; // Trial velocity
    InternalParamsA CFar; //!< Committed internal parameters for the Far Field component
    InternalParamsA TFar; //!< Trial internal parameters for the Far Field component

    // Material parameters
    double initialTangent;
    int soilType; //!< Soil type.
    double matCapacity; //!< Material capacity.
    double v50; //!< y (or z) at 50% of matCapacity.
    double vRef; //!< (y or z) reference point for Near Field component
    double np; //!< exponent for hardening shape of Near Field component
    double dashpot; //!< dashpot on the far-field (elastic) component

    void getFarField(const double &y);

    int sendData(CommParameters &);
    int recvData(const CommParameters &);


  public:
    PYBase(int tag, int classtag, int soilType, double mCap, double v50, double dashpot);
    PYBase(int tag, int classtag);
    PYBase(void);

    double getTangent(void) const;
    double getInitialTangent(void) const;
    double getStrain(void) const;
    double getStrainRate(void) const;

    int commitState(void);
    int revertToLastCommit(void);

    void Print(std::ostream &s, int flag =0);

  };
} // end of XC namespace


#endif
