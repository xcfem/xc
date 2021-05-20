# -*- coding: utf-8 -*-
def_hip_elu.open("./def_hip_elu.xci","w")
calc_hip_elu.open("./calc_hip_elu.xci","w")
hip_elu_tex.open("./hipotesis_elu.tex","w")
\printCabeceraListaHipotesis("hip_elu_tex"){}
\combinations
  {
    \comb_elu_persistentes
      {
        \expr{conta= 1}
        \for_each
          {
            def_hip_elu.write("\combination[\"",format(int(conta),"ULS%03u"),"\"]{ \descomp{\"",getName,"\"}}\n")
            calc_hip_elu.write("\\solveStaticLinearComb(\"",format(int(conta),"ULS%03u"),"\"){}","\n")
            calc_hip_elu.write("\\trataResultsComb(\"",format(int(conta),"ULS%03u"),"\"){}","\n")
            hip_elu_tex.write(format(int(conta),"ULS%03u")," & ",getName,"\\\\\n")
            \expr{conta= tonum(conta+1)}
          }
      }
  }
hip_elu_tex.write("\\end{supertabular}\n")
hip_elu_tex.write("\\end{center}\n")
def_hip_elu.close()
calc_hip_elu.close()
hip_elu_tex.close()
