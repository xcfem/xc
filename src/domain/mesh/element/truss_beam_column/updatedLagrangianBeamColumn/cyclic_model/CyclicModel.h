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
#ifndef CyclicModel_H
#define CyclicModel_H

#include <utility/tagged/TaggedObject.h>
#include <utility/actor/actor/MovableObject.h>

namespace XC {
//! \ingroup ElemBarra
//
//! @brief ??.
class CyclicModel : public TaggedObject, public MovableObject
  {
  private:
    int initNewTask();
  protected:
    double resFactor;
    double cycFactor, cycFactor_hist;
    double f_hist, d_hist;
    double f_curr, d_curr;
    double delT_curr, delT_hist;
    double f_bgn, d_bgn;
    double f_end, d_end;

    bool initYieldPos, initYieldNeg;
    bool   initCyc;
    bool   yielding, yielding_hist;
    double initFpos,initDpos;
    double initFneg,initDneg;
    double initFMag,initDMag;
    double k_init;
    double k_hist, k_curr;
    double fpeakPos, fpeakNeg;
    double dpeakPos, dpeakNeg;
    int state_hist, state_curr;

    const static int Loading,Unloading, Crossover;
    const static double Tol, delK;
  public:
    CyclicModel(int tag, int classTag);
	
    int commitState(double newResidual);
    void update(double f, double d, bool yield);
    virtual CyclicModel *getCopy()=0;
	
    virtual double getFactor();
    // for now ignore
    virtual int sendSelf(CommParameters &)
      {return -1;}
    virtual int recvSelf(const CommParameters &)
      {return -1;}
    virtual void Print (std::ostream &s, int flag=0);
  protected:
// virtual methods
    virtual int createFullCycleTask();
    virtual int createHalfCycleTask();
    virtual double getTaskFactor()=0;

// protected:
    int setCurrent(double f, double d);
    int dir(double x);

    int taskStatus(void);
    double rationalize(double x1, double y1, double x2, double y2);
    bool contains(double x1, double x2, double x);
  };
} // fin namespace XC

#endif

