# -*- coding: utf-8 -*-
solu= feProblem.getSoluProc
solCtrl= solu.getSoluControl
solModels= solCtrl.getModelWrapperContainer
sm= solModels.newModelWrapper("sm")
cHandler= sm.newConstraintHandler("transformation_constraint_handler")

numberer= sm.newNumberer("default_numberer")
numberer.useAlgorithm("rcm")
analysisAggregations= solCtrl.getAnalysisAggregationContainer
analysisAggregation= analysisAggregations.newAnalysisAggregation("analysisAggregation","sm")

solAlgo= analysisAggregation.newSolutionAlgorithm("frequency_soln_algo")
integ= analysisAggregation.newIntegrator("eigen_integrator",xc.Vector([]))

soe= analysisAggregation.newSystemOfEqn("sym_band_eigen_soe")
solver= soe.newSolver("sym_band_eigen_solver")

analysis= solu.newAnalysis("eigen_analysis","analysisAggregation","")
analOk= analysis.analyze(1)
eig1= analysis.getEigenvalue(1)

