// -*-c++-*-
//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
//  XC is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
#ifndef INELASTICYS2DGNL_H
#define INELASTICYS2DGNL_H

#include <domain/mesh/element/truss_beam_column/updatedLagrangianBeamColumn/UpdatedLagrangianBeam2D.h>
#include <material/yieldSurface/yieldSurfaceBC/YieldSurface_BC.h>
#include <utility/matrix/Vector.h>

#define DISPLAY_YS 2745

namespace XC {

//! @ingroup UpdatedLagrangianBeams
//
//! @brief Inelastic Element - concentrated hinge model, Fi - Fj interaction
//!  at each ends using yield surfaces
//!  *@author rkaul.
class InelasticYS2DGNL: public UpdatedLagrangianBeam2D
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
  void Print(std::ostream &s, int flag =0) const;
  int sendSelf(Communicator &);
  int recvSelf(const Communicator &);

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
} // end of XC namespace

#endif
