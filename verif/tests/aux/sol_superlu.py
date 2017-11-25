# -*- coding: utf-8 -*-
solu= prb.getSoluProc
solCtrl= solu.getSoluControl
solModels= solCtrl.getModelWrapperContainer
sm= solModels.newModelWrapper("sm")
cHandler= sm.newConstraintHandler("penalty_constraint_handler")
cHandler.alphaSP= 1.0e15
cHandler.alphaMP= 1.0e15

numberer= sm.newNumberer("default_numberer")
numberer.useAlgorithm("simple")
# Con este solver no es necesario renumerar.
solMethods= solCtrl.getSoluMethodContainer
smt= solMethods.newSoluMethod("smt","sm")

solAlgo= smt.newSolutionAlgorithm("linear_soln_algo")
integ= smt.newIntegrator("load_control_integrator",xc.Vector([]))
soe= smt.newSystemOfEqn("sparse_gen_col_lin_soe")
solver= soe.newSolver("super_lu_solver")

analysis= solu.newAnalysis("static_analysis","smt","")
result= analysis.analyze(1)
