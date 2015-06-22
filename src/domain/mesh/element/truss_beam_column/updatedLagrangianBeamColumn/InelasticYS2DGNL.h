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
#ifndef INELASTICYS2DGNL_H
#define INELASTICYS2DGNL_H

#include <domain/mesh/element/truss_beam_column/updatedLagrangianBeamColumn/UpdatedLagrangianBeam2D.h>
#include <material/yieldSurface/yieldSurfaceBC/YieldSurface_BC.h>
#include <utility/matrix/Vector.h>

#define DISPLAY_YS 2745

/**Inelastic Element - concentrated hinge model, Fi - Fj interaction
   at each ends using yield surfaces
  *@author rkaul
  */

namespace XC {
//! \ingroup ElemBarra
//
//! @brief ??.
class InelasticYS2DGNL : public UpdatedLagrangianBeam2D
{
 protected:
  static Vector elasticForce;
  static Vector F1, F2, Fs;
  

  YieldSurface_BC *ys1, *ys2;

  mutable bool end1Plastify, end2Plastify;
  bool end1Plastify_hist, end2Plastify_hist;

  Matrix end1G, end2G;
  mutable Matrix Stiff;
  mutable int forceRecoveryAlgo;
  int forceRecoveryAlgo_orig;
  bool end1Damage, end2Damage;
  mutable bool split_step;

  int debug, fdebug, pdebug, ydebug, statusDebug;
  mutable bool init;
  bool updateKt;


  
  const static int INSIDE, OUTSIDE, WITHIN;
  
  static double storage;
 public:
  InelasticYS2DGNL(int tag, 
		   int Nd1, int Nd2,
		   YieldSurface_BC *ysEnd1, YieldSurface_BC *ysEnd2,
		   int rf_algo = -1, // updated
		   bool islinear = false, double rho = 0.0);
  
  ~InelasticYS2DGNL();
  
  virtual const Vector &getResistingForce(void) const;
  virtual const	Matrix &getTangentStiff(void) const;
  virtual int commitState(void);
  virtual int update(void) const;
  void Print(std::ostream &s, int flag =0);
  int sendSelf(CommParameters &);
  int recvSelf(const CommParameters &);

  virtual Response *setResponse(const std::vector<std::string> &argv,Information &eleInformation);
  virtual int getResponse(int responseID, Information &eleInformation);

 protected:
  virtual void getLocalStiff(Matrix &K) const=0;
  virtual void getLocalMass(Matrix &M) const;
  
protected:
  int  computeTrueEleForce(Vector &trial_force) const;
  void checkSpecialCases(void) const;
 
private: 
  void forceBalance(Vector &force, int algo) const;
  void plastifyOneEnd(int end, YieldSurface_BC *ys,  Vector &trial_force, Vector &incrDisp, Matrix &K, Vector &total_force, int algo) const;
  
  void splitStep(int end_shoot, YieldSurface_BC *ys_shoots, YieldSurface_BC *ys_drifts,
		 Vector &trial_force, Matrix &K, Vector &total_force) const;
  
  void driftOneEnd(YieldSurface_BC *ys, Vector &trial_force, Vector &surface_force, Matrix &K, Vector &total_force) const;
  void driftBothEnds(Vector &trial_force, Vector &surface_force,Matrix &K, Vector &total_force) const;
  void plastifyBothEnds(Vector &trial_force, Vector &incrDisp,Matrix &K, Vector &total_force) const;
  void checkEndStatus(bool &end1drifts, bool &end2drifts, Vector &trialForce) const;
  int  plasticPredictor(Vector &trialForce) const;
  int  elasticCorrector(Vector &trialForce, int algo);
};
} // fin namespace XC

#endif
