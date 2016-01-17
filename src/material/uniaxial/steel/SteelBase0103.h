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

#ifndef SteelBase0103_h
#define SteelBase0103_h

#include "material/uniaxial/steel/SteelBase.h"


namespace XC {

// Default values for isotropic hardening parameters a1, a2, a3, and a4
const double STEEL_0103_DEFAULT_A1= 0.0;
const double STEEL_0103_DEFAULT_A2= 55.0;
const double STEEL_0103_DEFAULT_A3= 0.0;
const double STEEL_0103_DEFAULT_A4= 55.0;

//! @ingroup MatUnx
//
//! @brief Clase base para la modelización del acero.
class SteelBase0103: public SteelBase
  {
  protected:
    /*** CONVERGED History Variables ***/
    double CminStrain;  //!< Minimum strain in compression
    double CmaxStrain;  //!< Maximum strain in tension
    double CshiftP;     //!< Shift in hysteresis loop for positive loading
    double CshiftN;     //!< Shift in hysteresis loop for negative loading
    int Cloading;       //!< Flag for loading/unloading
                        // 1 = loading (positive strain increment)
                        // -1 = unloading (negative strain increment)
                        // 0 initially

    /*** CONVERGED State Variables ***/
    double Cstrain;
    double Cstress;
    double Ctangent;

    /*** TRIAL State Variables ***/
    double Tstrain;
    double Tstress;
    double Ttangent; // Not really a state variable, but declared here
                     // for convenience

    /*** TRIAL History Variables ***/
    double TminStrain;
    double TmaxStrain;
    double TshiftP;
    double TshiftN;
    int Tloading;

    virtual void determineTrialState(double dStrain)= 0;

  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);

    virtual int setup_parameters(void);
  public:
    SteelBase0103(int tag, int classTag, double fy, double E0, double b,
       double a1 = STEEL_0103_DEFAULT_A1, double a2 = STEEL_0103_DEFAULT_A2,
       double a3 = STEEL_0103_DEFAULT_A3, double a4 = STEEL_0103_DEFAULT_A4);
    SteelBase0103(int tag, int classTag);
    SteelBase0103(int classTag);

    int setTrialStrain(double strain, double strainRate = 0.0);
    int setTrial(double strain, double &stress, double &tangent, double strainRate = 0.0);
    double getStrain(void) const;
    double getStress(void) const;
    double getTangent(void) const;

    int commitState(void);
    int revertToLastCommit(void);
    int revertToStart(void);

    void Print(std::ostream &s, int flag =0);
  };
} // end of XC namespace

#endif
