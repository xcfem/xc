# -*- coding: utf-8 -*-
def listaAccionesFamilia(nmbArchivo):
  {
    nmbArchivo.write("\\hline\n")
    nmbArchivo.write("\\multicolumn{6}{|c|}{",getName,"}\\\\\n")
    nmbArchivo.write("\\hline\n")
    nmbArchivo.write("Name & Description & $\\psi_0$ & $\\psi_1$ & $\\psi_2$ & Incompatibles \\\\\n")
    nmbArchivo.write("\\hline\n")
    \for_each_accion
      {
        nmbArchivo.write(getName)
        nmbArchivo.write(" & ",getDescripcion)
        nmbArchivo.write(" & ",psi_0)
        nmbArchivo.write(" & ",psi_1)
        nmbArchivo.write(" & ",psi_2)
        nmbArchivo.write(" & ",incompatibles,"\\\\\n")
      }
  }

def listaAcciones(pathArchivo):
  {
    outAcciones.open(pathArchivo,"w")
    outAcciones.write("\\begin{center}\n")
    outAcciones.write("\\begin{small}\n")
    outAcciones.write("\\begin{longtable}{|l|p{4cm}|c|c|c|l|}\n")
    outAcciones.write("\\multicolumn{6}{|c|}{../..}\\\\\n")
    outAcciones.write("\\hline \\hline\n")
    outAcciones.write("\\endfoot\n")
    outAcciones.write("\\hline \\hline\n")
    outAcciones.write("\\endlastfoot\n")

    \for_each
      {
        if(getSize>0)
          listaAccionesFamilia("outAcciones"){}
      }

    outAcciones.write("\\end{longtable}\n")
    outAcciones.write("\\end{small}\n")
    outAcciones.write("\\end{center}\n")
    outAcciones.close()
  }

def listaPonderacion(pathArchivo,elimite,accidental):
  {
    outPonderacion.open(pathArchivo,"w")
    outPonderacion.write("\\begin{center}\n")
    outPonderacion.write("\\begin{small}\n")
    outPonderacion.write("\\begin{tabular}{|l|l|l|l|}\n")
    outPonderacion.write("\\hline\n")
    outPonderacion.write("Familia & Acciones & \\multicolumn{2}{|c|}{Efecto} \\\\\n")
    outPonderacion.write("        &          & favorable & desfavorable \\\\\n")
    outPonderacion.write("\\hline\n")
    \for_each
      {
        if(num_acciones>0)
            \then
              {
                outPonderacion.write(getName)
                outPonderacion.write(" & ",lista_acciones)
                \gammaf{\elimite
                  {
                    if(accidental==0)
                        \then
                          {
                            outPonderacion.write(" & ",favorable)
                            outPonderacion.write(" & ",desfavorable,"\\\\\n")
                          }
                        else:
                          {
                            outPonderacion.write(" & ",favorable_accidental)
                            outPonderacion.write(" & ",desfavorable_accidental,"\\\\\n")
                          }
                      }
                  }}
              }
          }
      }
    outPonderacion.write("\\hline\n")
    outPonderacion.write("\\end{tabular}\n")
    outPonderacion.write("\\end{small}\n")
    outPonderacion.write("\\end{center}\n")
    outPonderacion.close()
  }
