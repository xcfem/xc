# -*- coding: utf-8 -*-
solu= feProblem.getSoluProc
solCtrl= solu.getSoluControl
solModels= solCtrl.getModelWrapperContainer
sm= solModels.newModelWrapper("sm")
# cHandler= sm.newConstraintHandler("penalty_constraint_handler")
# cHandler.alphaSP= 1.0e15
# cHandler.alphaMP= 1.0e15
cHandler= sm.newConstraintHandler("transformation_constraint_handler")

numberer= sm.newNumberer("default_numberer")
numberer.useAlgorithm("rcm")
solutionStrategies= solCtrl.getSolutionStrategyContainer

solutionStrategy= solutionStrategies.newSolutionStrategy("solutionStrategy","sm")
#solAlgo= solutionStrategy.newSolutionAlgorithm("newton_raphson_soln_algo")
solAlgo= solutionStrategy.newSolutionAlgorithm("krylov_newton_soln_algo")
ctest= solutionStrategy.newConvergenceTest("norm_disp_incr_conv_test")
ctest.printFlag= 0
ctest.tol= 1e-8
ctest.maxNumIter= 1000
integ= solutionStrategy.newIntegrator("load_control_integrator",xc.Vector([]))
#soe= solutionStrategy.newSystemOfEqn("band_spd_lin_soe")
#solver= soe.newSolver("band_spd_lin_lapack_solver")
soe= solutionStrategy.newSystemOfEqn("band_gen_lin_soe")
solver= soe.newSolver("band_gen_lin_lapack_solver")

buck= solutionStrategies.newSolutionStrategy("buck","sm")
buckSolAlgo= buck.newSolutionAlgorithm("linear_buckling_soln_algo")
buckInteg= buck.newIntegrator("linear_buckling_integrator",xc.Vector([]))
buckSoe= buck.newSystemOfEqn("band_arpackpp_soe")
buckSoe.shift= 0.0
buckSolver= buckSoe.newSolver("band_arpackpp_solver")

analysis= solu.newAnalysis("linear_buckling_analysis","solutionStrategy","buck")
analysis.numModes= 2
analOk= analysis.analyze(2)
