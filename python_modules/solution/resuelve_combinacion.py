# -*- coding: utf-8 -*-

def resuelveComb(preprocessor,nmbComb,analysis,numSteps):
  preprocessor.resetLoadCase()
  preprocessor.getLoadLoader.addToDomain(nmbComb)
  analOk= analysis.analyze(numSteps)
  preprocessor.getLoadLoader.removeFromDomain(nmbComb)
  # print "Resuelta combinación: ",nmbComb,"\n"

def resuelveCombEstatLin(preprocessor,nmbComb,analysis,numSteps):
  print "DEPRECATED; use resuelveComb"
  resuelveComb(preprocessor,nmbComb,analysis,numSteps)


def resuelveCombEstat2ndOrderLin(preprocessor,nmbComb,analysis,numSteps):
  preprocessor.resetLoadCase()
  preprocessor.getLoadLoader.addToDomain(nmbComb)
  analOk= analysis.analyze(numSteps)
  analOk= analysis.analyze(numSteps)
  preprocessor.getLoadLoader.removeFromDomain(nmbComb)
  # print "Resuelta combinación: ",nmbComb,"\n"

def resuelveCasoEstatNoLin(nmbComb):
  print "DEPRECATED; use use resuelveComb"
  resuelveComb(preprocessor,nmbComb,analysis,numSteps)

# def resuelveCasoEstatNoLinNR(nmbComb):
#     \preprocessor \dom{\nuevo_caso
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
