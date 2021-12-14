//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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
//integrators.h

//File headers for all integrators.
//Integrator.
#include <solution/analysis/integrator/EigenIntegrator.h>
#include <solution/analysis/integrator/StaticIntegrator.h>
#include <solution/analysis/integrator/TransientIntegrator.h>

//static
#include <solution/analysis/integrator/static/ArcLength.h>
#include <solution/analysis/integrator/static/ArcLength1.h>
#include <solution/analysis/integrator/static/DisplacementControl.h>
#include <solution/analysis/integrator/static/DistributedDisplacementControl.h>
#include <solution/analysis/integrator/static/HSConstraint.h>
#include <solution/analysis/integrator/static/LoadControl.h>
#include <solution/analysis/integrator/static/LoadPath.h>
#include <solution/analysis/integrator/static/MinUnbalDispNorm.h>
//#include "solution/analysis/integrator/TRBDF2.h"
#include "solution/analysis/integrator/transient/rayleigh/CentralDifference.h"

//eigen
#include <solution/analysis/integrator/EigenIntegrator.h>
#include <solution/analysis/integrator/eigen/KEigenIntegrator.h>
#include <solution/analysis/integrator/eigen/LinearBucklingIntegrator.h>

//transient
#include <solution/analysis/integrator/transient/CentralDifferenceAlternative.h>
#include <solution/analysis/integrator/transient/CentralDifferenceNoDamping.h>
#include <solution/analysis/integrator/transient/HHT1.h>
#include <solution/analysis/integrator/transient/newmark/Newmark.h>
#include <solution/analysis/integrator/transient/newmark/Newmark1.h>
#include <solution/analysis/integrator/transient/newmark/NewmarkExplicit.h>
#include <solution/analysis/integrator/transient/newmark/NewmarkHybridSimulation.h>

#include <solution/analysis/integrator/transient/rayleigh/AlphaOS.h>
#include <solution/analysis/integrator/transient/rayleigh/AlphaOSGeneralized.h>
#include <solution/analysis/integrator/transient/rayleigh/CentralDifference.h>
#include <solution/analysis/integrator/transient/rayleigh/Collocation.h>
#include <solution/analysis/integrator/transient/rayleigh/CollocationHybridSimulation.h>
#include <solution/analysis/integrator/transient/rayleigh/HHT.h>
#include <solution/analysis/integrator/transient/rayleigh/HHTExplicit.h>
#include <solution/analysis/integrator/transient/rayleigh/HHTGeneralized.h>
#include <solution/analysis/integrator/transient/rayleigh/HHTGeneralizedExplicit.h>
#include <solution/analysis/integrator/transient/rayleigh/HHTHybridSimulation.h>
#include <solution/analysis/integrator/transient/rayleigh/WilsonTheta.h>
