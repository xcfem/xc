# -*- coding: utf-8 -*-
def listaAccionesFamilia(actionFamily, outputFile):
    outputFile.write("\\hline\n")
    outputFile.write("\\multicolumn{6}{|c|}{"+actionFamily.name+"}\\\\\n")
    outputFile.write("\\hline\n")
    outputFile.write("Name & Description & $\\psi_0$ & $\\psi_1$ & $\\psi_2$ & Incompatibles \\\\\n")
    outputFile.write("\\hline\n")
    for action in actionFamily:
        outputFile.write(action.getName)
        outputFile.write(" & ",action.getDescripcion)
        outputFile.write(" & ",action.psi_0)
        outputFile.write(" & ",action.psi_1)
        outputFile.write(" & ",action.psi_2)
        outputFile.write(" & ",action.incompatibles,"\\\\\n")

def listaAcciones(actionFamilies, filePath):
    outAcciones= open(filePath,"w")
    outAcciones.write("\\begin{center}\n")
    outAcciones.write("\\begin{small}\n")
    outAcciones.write("\\begin{longtable}{|l|p{4cm}|c|c|c|l|}\n")
    outAcciones.write("\\multicolumn{6}{|c|}{../..}\\\\\n")
    outAcciones.write("\\hline \\hline\n")
    outAcciones.write("\\endfoot\n")
    outAcciones.write("\\hline \\hline\n")
    outAcciones.write("\\endlastfoot\n")

    for actionFamily in actionFamilies:
        if(len(actionFamily)>0):
            listaAccionesFamilia(actionFamily, outAcciones)

    outAcciones.write("\\end{longtable}\n")
    outAcciones.write("\\end{small}\n")
    outAcciones.write("\\end{center}\n")
    outAcciones.close()

def listaPonderacion(actions, filePath,elimite,accidental):
    outPonderacion= open(filePath,"w")
    outPonderacion.write("\\begin{center}\n")
    outPonderacion.write("\\begin{small}\n")
    outPonderacion.write("\\begin{tabular}{|l|l|l|l|}\n")
    outPonderacion.write("\\hline\n")
    outPonderacion.write("Familia & Acciones & \\multicolumn{2}{|c|}{Efecto} \\\\\n")
    outPonderacion.write("        &          & favorable & desfavorable \\\\\n")
    outPonderacion.write("\\hline\n")
    for action in actions:
        outPonderacion.write(action.name)
        outPonderacion.write(" & ",lista_acciones)
        if(accidental==0):
            outPonderacion.write(" & ",favorable)
            outPonderacion.write(" & ",desfavorable,"\\\\\n")
        else:
            outPonderacion.write(" & ",favorable_accidental)
            outPonderacion.write(" & ",desfavorable_accidental,"\\\\\n")
    outPonderacion.write("\\hline\n")
    outPonderacion.write("\\end{tabular}\n")
    outPonderacion.write("\\end{small}\n")
    outPonderacion.write("\\end{center}\n")
    outPonderacion.close()
