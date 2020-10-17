# -*- coding: utf-8 -*-
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

soe= solutionStrategy.newSystemOfEqn("sym_band_eigen_soe")
solver= soe.newSolver("sym_band_eigen_solver")

analysis= solu.newAnalysis("eigen_analysis","solutionStrategy","")
analOk= analysis.analyze(1)
eig1= analysis.getEigenvalue(1)

