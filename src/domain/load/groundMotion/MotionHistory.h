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
//MotionHistory.h

#ifndef MotionHistory_h
#define MotionHistory_h

#include "xc_utils/src/nucleo/EntCmd.h"

namespace XC {
  class TimeSeries;
  class TimeSeriesIntegrator;
  class Parameter;
  class Information;
  class Vector;

//! @ingroup GMotion
//
//! @brief Datos que definen las aceleraciones velocidades y
//! desplazamientos del terreno producidos por un terremoto.
class MotionHistory: public EntCmd
  {
  private:
    TimeSeries *theAccelSeries; //!< Ground acceleration
    mutable TimeSeries *theVelSeries; //!< Ground velocity
    mutable TimeSeries *theDispSeries; //!< Ground displacement
    mutable TimeSeriesIntegrator *theIntegrator;    
    double delta;

    void calcVel(void) const;
    void calcDisp(void) const;
    void clearSeries(void);
  protected:
    bool procesa_comando(CmdStatus &status);
  public:
    MotionHistory(const double &dt= 0.0);
    MotionHistory(const MotionHistory &otro);
    MotionHistory &operator=(const MotionHistory &otro);
    
    virtual ~MotionHistory(void);

    inline double getDelta(void) const
      { return delta; }
    inline void setDelta(const double &dT)
      { delta= dT; }
    void setAccelHistory(const TimeSeries *);
    TimeSeries *getAccelHistory(void);

    size_t getNumDataPoints(void) const;
    double getDuration(void) const;

    double getPeakAccel(void) const;
    double getPeakVel(void) const;
    double getPeakDisp(void) const;

    double getAccel(double time) const;
    double getVel(double time) const;
    double getDisp(double time) const;
    const XC::Vector &getDispVelAccel(Vector &data,const double &time) const;
    
    void setIntegrator(TimeSeriesIntegrator *integrator);
    TimeSeries *integrate(TimeSeries *theSeries) const; 

    // AddingSensitivity:BEGIN //////////////////////////////////////////
    double getAccelSensitivity(double time);
    int setParameter(const std::vector<std::string> &argv, Parameter &param);
    int updateParameter(int parameterID, Information &info);
    int activateParameter(int parameterID);
    // AddingSensitivity:END ///////////////////////////////////////////
    void loadAccelFile(const std::string &fileNameAccel,const double &timeStep,const double &theFactor);
    void loadAccelTimeFile(const std::string &fileNameAccel, const std::string &fileNameTime,const double &theFactor);
    virtual any_const_ptr GetProp(const std::string &cod) const;
  };
} // fin namespace XC

#endif

