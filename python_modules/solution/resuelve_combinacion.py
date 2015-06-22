# -*- coding: utf-8 -*-

def resuelveComb(mdlr,nmbComb,analysis,numSteps):
  mdlr.resetLoadCase()
  mdlr.getLoadLoader.addToDomain(nmbComb)
  analOk= analysis.analyze(numSteps)
  mdlr.getLoadLoader.removeFromDomain(nmbComb)
  # print "Resuelta combinación: ",nmbComb,"\n"

def resuelveCombEstatLin(mdlr,nmbComb,analysis,numSteps):
  print "DEPRECATED; use resuelveComb"
  resuelveComb(mdlr,nmbComb,analysis,numSteps)


def resuelveCombEstat2ndOrderLin(mdlr,nmbComb,analysis,numSteps):
  mdlr.resetLoadCase()
  mdlr.getLoadLoader.addToDomain(nmbComb)
  analOk= analysis.analyze(numSteps)
  analOk= analysis.analyze(numSteps)
  mdlr.getLoadLoader.removeFromDomain(nmbComb)
  # print "Resuelta combinación: ",nmbComb,"\n"

def resuelveCasoEstatNoLin(nmbComb):
  print "DEPRECATED; use use resuelveComb"
  resuelveComb(mdlr,nmbComb,analysis,numSteps)

# def resuelveCasoEstatNoLinNR(nmbComb):
#     \mdlr \dom{\nuevo_caso
#     cargas.addToDomain(nmbComb)

#     \sol_proc
#       {
#         \control
#           {
#             \solu_method["smt","sm"]
#               {
#                 \norm_unbalance_conv_test \tol{1.0e-2} \print_flag{0} \max_num_iter{10}
#               }
#           }
#         \static_analysis["smt"]  \analyze{1} analOk= result 
#       }
#     cargas.removeFromDomain(nmbComb)
#     # print "Resuelto caso: ",nmbComb,"\n"
#   }
