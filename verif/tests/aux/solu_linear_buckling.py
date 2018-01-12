# -*- coding: utf-8 -*-
solu= feProblem.getSoluProc
solCtrl= solu.getSoluControl
solModels= solCtrl.getModelWrapperContainer
sm= solModels.newModelWrapper("sm")
cHandler= sm.newConstraintHandler("penalty_constraint_handler")
cHandler.alphaSP= 1.0e15
cHandler.alphaMP= 1.0e15

numberer= sm.newNumberer("default_numberer")
numberer.useAlgorithm("rcm")
analysisAggregations= solCtrl.getAnalysisAggregationContainer

analysisAggregation= analysisAggregations.newAnalysisAggregation("analysisAggregation","sm")
solAlgo= analysisAggregation.newSolutionAlgorithm("newton_raphson_soln_algo")
ctest= analysisAggregation.newConvergenceTest("norm_disp_incr_conv_test")
ctest.printFlag= 0
ctest.tol= 1e-8
ctest.maxNumIter= 10
integ= analysisAggregation.newIntegrator("load_control_integrator",xc.Vector([1.0,1,1.0,1.0]))
soe= analysisAggregation.newSystemOfEqn("band_spd_lin_soe")
solver= soe.newSolver("band_spd_lin_lapack_solver")

buck= analysisAggregations.newAnalysisAggregation("buck","sm")
buckSolAlgo= buck.newSolutionAlgorithm("linear_buckling_soln_algo")
buckInteg= buck.newIntegrator("linear_buckling_integrator",xc.Vector([]))
buckSoe= buck.newSystemOfEqn("band_arpackpp_soe")
buckSoe.shift= 0.0
buckSolver= buckSoe.newSolver("band_arpackpp_solver")

analysis= solu.newAnalysis("linear_buckling_analysis","analysisAggregation","buck")
analysis.numModes= 2
analOk= analysis.analyze(2)
