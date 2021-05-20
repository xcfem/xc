# -*- coding: utf-8 -*-
# Quasi-permanent combinations
def_hip_elscp.open("./def_hip_elscp.xci","w")
calc_hip_elscp.open("./calc_hip_elscp.xci","w")
hip_elscp_tex.open("./hipotesis_elscp.tex","w")
\printCabeceraListaHipotesis("hip_elscp_tex"){}
\combinations
  {
    \comb_els_frecuentes
      {
        \expr{conta= 1}
        \for_each
          {
            def_hip_elscp.write("\combination[\"",format(int(conta),"ELSCP%03u"),"\"]{ \descomp{\"",getName,"\"}}\n")
            calc_hip_elscp.write("\\solveStaticLinearComb(\"",format(int(conta),"ELSCP%03u"),"\"){}","\n")
            calc_hip_elscp.write("\\trataResultsComb(\"",format(int(conta),"ELSCP%03u"),"\"){}","\n")
            hip_elscp_tex.write(format(int(conta),"ELSCP%03u")," & ",getName,"\\\\\n")
            \expr{conta= tonum(conta+1)}
          }
      }
  }
hip_elscp_tex.write("\\end{supertabular}\n")
hip_elscp_tex.write("\\end{center}\n")
def_hip_elscp.close()
calc_hip_elscp.close()
hip_elscp_tex.close()
