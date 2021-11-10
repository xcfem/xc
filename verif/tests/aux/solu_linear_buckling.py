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

staticAnalysis= solutionStrategies.newSolutionStrategy("staticAnalysis","sm")
#solAlgo= staticAnalysis.newSolutionAlgorithm("newton_raphson_soln_algo")
solAlgo= staticAnalysis.newSolutionAlgorithm("krylov_newton_soln_algo")
ctest= staticAnalysis.newConvergenceTest("norm_disp_incr_conv_test")
ctest.printFlag= 0
ctest.tol= 1e-8
ctest.maxNumIter= 1000
integ= staticAnalysis.newIntegrator("load_control_integrator",xc.Vector([]))
#soe= staticAnalysis.newSystemOfEqn("band_spd_lin_soe")
#solver= soe.newSolver("band_spd_lin_lapack_solver")
soe= staticAnalysis.newSystemOfEqn("band_gen_lin_soe")
solver= soe.newSolver("band_gen_lin_lapack_solver")

bucklingAnalysis= solutionStrategies.newSolutionStrategy("bucklingAnalysis","sm")
buckSolAlgo= bucklingAnalysis.newSolutionAlgorithm("linear_buckling_soln_algo")
buckInteg= bucklingAnalysis.newIntegrator("linear_buckling_integrator",xc.Vector([]))
buckSoe= bucklingAnalysis.newSystemOfEqn("band_arpackpp_soe")
buckSoe.shift= 0.0
buckSolver= buckSoe.newSolver("band_arpackpp_solver")

analysis= solu.newAnalysis("linear_buckling_analysis","staticAnalysis","bucklingAnalysis")
analysis.numModes= 2
analOk= analysis.analyze(2)
