# -*- coding: utf-8 -*-

__author__= "Luis C. Pérez Tato (LCPT)"
__cppyright__= "Copyright 2015 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc

def get_analysis(feProblem):
    solu= feProblem.getSoluProc
    solCtrl= solu.getSoluControl
    solModels= solCtrl.getModelWrapperContainer
    sm= solModels.newModelWrapper("sm")
    cHandler= sm.newConstraintHandler("transformation_constraint_handler")

    numberer= sm.newNumberer("default_numberer")
    numberer.useAlgorithm("rcm")
    solutionStrategies= solCtrl.getSolutionStrategyContainer
    solutionStrategy= solutionStrategies.newSolutionStrategy("solutionStrategy","sm")

    solAlgo= solutionStrategy.newSolutionAlgorithm("frequency_soln_algo")
    integ= solutionStrategy.newIntegrator("eigen_integrator",xc.Vector([]))
    soe= solutionStrategy.newSystemOfEqn("band_arpackpp_soe")
    soe.shift= 0.0
    solver= soe.newSolver("band_arpackpp_solver")

    analysis= solu.newAnalysis("eigen_analysis","solutionStrategy","")
    return analysis


