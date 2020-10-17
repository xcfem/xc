# -*- coding: utf-8 -*-
solu= feProblem.getSoluProc
solCtrl= solu.getSoluControl
solModels= solCtrl.getModelWrapperContainer
sm= solModels.newModelWrapper("sm")
cHandler= sm.newConstraintHandler("penalty_constraint_handler")
cHandler.alphaSP= 1.0e15
cHandler.alphaMP= 1.0e15

numberer= sm.newNumberer("default_numberer")
numberer.useAlgorithm("simple")
# Con este solver no es necesario renumerar.
solutionStrategies= solCtrl.getSolutionStrategyContainer
solutionStrategy= solutionStrategies.newSolutionStrategy("solutionStrategy","sm")

solAlgo= solutionStrategy.newSolutionAlgorithm("linear_soln_algo")
integ= solutionStrategy.newIntegrator("load_control_integrator",xc.Vector([]))
soe= solutionStrategy.newSystemOfEqn("sparse_gen_col_lin_soe")
solver= soe.newSolver("super_lu_solver")

analysis= solu.newAnalysis("static_analysis","solutionStrategy","")
result= analysis.analyze(1)
