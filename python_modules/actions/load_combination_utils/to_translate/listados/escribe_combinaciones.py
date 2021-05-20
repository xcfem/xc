# -*- coding: utf-8 -*-
def writeULSCombinations(nmbArchivoDef, nmbArchivoLatex,nmbFormat):
  {
    def_hip_elu.open(nmbArchivoDef,"w")
    \combinations
      {
        \comb_elu_persistentes
          {
            \expr{conta= 1}
            \for_each
              {
                def_hip_elu.write("\combination[\"",format(int(conta),nmbFormat),"\"]{ \descomp{\"",getName,"\"}}\n")
                \expr{conta= tonum(conta+1)}
              }
          }
        \comb_elu_accidentales
          {
            \for_each
              {
                def_hip_elu.write("\combination[\"",format(int(conta),nmbFormat),"\"]{ \descomp{\"",getName,"\"}}\n")
                \expr{conta= tonum(conta+1)}
              }
          }
      }
    def_hip_elu.close()
    hip_elu_tex.open(nmbArchivoLatex,"w")
    \printCabeceraListaHipotesis("hip_elu_tex"){}
    \combinations
      {
        \comb_elu_persistentes
          {
            \expr{conta= 1}
            \for_each
              {
                hip_elu_tex.write(format(int(conta),nmbFormat)," & ",getName,"\\\\\n")
                \expr{conta= tonum(conta+1)}
              }
          }
        \comb_elu_accidentales
          {
            \for_each
              {
                hip_elu_tex.write(format(int(conta),nmbFormat)," & ",getName,"\\\\\n")
                \expr{conta= tonum(conta+1)}
              }
          }
      }
    hip_elu_tex.write("\\end{supertabular}\n")
    hip_elu_tex.write("\\end{center}\n")
    hip_elu_tex.close()
  }

def writeFrequentSLSCombinations(nmbArchivoDef, nmbArchivoLatex,nmbFormat):
  {
    def_hip_els.open(nmbArchivoDef,"w")
    \combinations
      {
        \comb_els_frecuentes
          {
            \expr{conta= 1}
            \for_each
              {
                def_hip_els.write("\combination[\"",format(int(conta),nmbFormat),"\"]{ \descomp{\"",getName,"\"}}\n")
                \expr{conta= tonum(conta+1)}
              }
          }
      }
    def_hip_els.close()
    hip_els_tex.open(nmbArchivoLatex,"w")
    \printCabeceraListaHipotesis("hip_els_tex"){}
    \combinations
      {
        \comb_els_frecuentes
          {
            \expr{conta= 1}
            \for_each
              {
                hip_els_tex.write(format(int(conta),nmbFormat)," & ",getName,"\\\\\n")
                \expr{conta= tonum(conta+1)}
              }
          }
      }
    hip_els_tex.write("\\end{supertabular}\n")
    hip_els_tex.write("\\end{center}\n")
    hip_els_tex.close()
  }
