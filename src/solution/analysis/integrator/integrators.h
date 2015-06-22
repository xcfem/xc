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
//integrators.h

//Archivos de cabecera de todos los integradores.
//Integrador.
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
