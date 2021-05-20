# -*- coding: utf-8 -*-
# Frequent combinations
def_hip_elsf.open("./def_hip_elsf.xci","w")
calc_hip_elsf.open("./calc_hip_elsf.xci","w")
hip_elsf_tex.open("./hipotesis_elsf.tex","w")
\printCabeceraListaHipotesis("hip_elsf_tex"){}
\combinations
  {
    \comb_els_frecuentes
      {
        \expr{conta= 1}
        \for_each
          {
            def_hip_elsf.write("\combination[\"",format(int(conta),"ELSF%03u"),"\"]{ \descomp{\"",getName,"\"}}\n")
            calc_hip_elsf.write("\\solveStaticLinearComb(\"",format(int(conta),"ELSF%03u"),"\"){}","\n")
            calc_hip_elsf.write("\\trataResultsComb(\"",format(int(conta),"ELSF%03u"),"\"){}","\n")
            hip_elsf_tex.write(format(int(conta),"ELSF%03u")," & ",getName,"\\\\\n")
            \expr{conta= tonum(conta+1)}
          }
      }
  }
hip_elsf_tex.write("\\end{supertabular}\n")
hip_elsf_tex.write("\\end{center}\n")
def_hip_elsf.close()
calc_hip_elsf.close()
hip_elsf_tex.close()
