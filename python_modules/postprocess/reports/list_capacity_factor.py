# -*- coding: utf-8 -*-

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es" "ana.Ortega@ciccp.es"

from misc.latex import latex_utils

def printCabeceraListadoCapacityFactor(outputFile, sectionName):
    ''' Print header for capacity factors listing.

    :param outputFile: output file.
    :param sectionName: name of the section being checked.
    '''
    outputFile.write("\\begin{center}\n")
    outputFile.write("\\begin{footnotesize}\n")
    outputFile.write("\\tablefirsthead{%\n")
    outputFile.write("\\hline\n")
    # outputFile.write("\\multicolumn{6}{|c|}{\\textbf{"+nmbArch+"}}\\\\\n")
    outputFile.write("\\multicolumn{6}{|c|}{Comprobación de la sección "+ latex_utils.toLaTeX(sectionName)+"}\\\\\n")
    outputFile.write("\\hline\n")
    outputFile.write("Elem. & Hip. & $N$  & $M_x$  & $M_y$  & FC \\\\\n")
    outputFile.write("      &      & (kN) & (kN m) & (kN m) &    \\\\\n")
    outputFile.write("\\hline}\n")
    outputFile.write("\\tablehead{%\n")
    outputFile.write("\\hline\n")
    outputFile.write("\\multicolumn{6}{|l|}{\\small\\sl \\ldots continúa}\\\\\n")
    outputFile.write("\\hline\n")
    # outputFile.write("\\multicolumn{6}{|c|}{\\textbf{"+nmbArch+"}}\\\\\n")
    outputFile.write("\\multicolumn{6}{|c|}{Comprobación de la sección "+ latex_utils.toLaTeX(sectionName)+"}\\\\\n")
    outputFile.write("\\hline\n")
    outputFile.write("Elem. & Hip. & $N$  & $M_x$  & $M_y$  & FC \\\\\n")
    outputFile.write("      &      & (kN) & (kN m) & (kN m) &    \\\\\n")
    outputFile.write("\\hline}\n")
    outputFile.write("\\tabletail{%\n")
    outputFile.write("\\hline\n")
    outputFile.write("\\multicolumn{6}{|r|}{\\small\\sl continúa\\ldots}\\\\\n")
    outputFile.write("\\hline}\n")
    outputFile.write("\\tablelasttail{\\hline}\n")
    outputFile.write("\\begin{supertabular}{|l|r|r|r|r|r|}\n")

# Imprime el cierre del listado de capacity factors.
def printCierreListadoCapacityFactor(outputFile):
    outputFile.write("\\end{supertabular}\n")
    outputFile.write("\\end{footnotesize}\n")
    outputFile.write("\\end{center}\n")


def listaCapacityFactor(preprocessor, setName, outputFile, title):
    ''' Print results of normal stresses in elastic range checking for an
    element set whose material is a steel shape.

    :param preprocessor: XC preprocessor for the finite element problem.
    :param setName: name of the set whose elements will be checked.
    :param outputFile: output file.
    :param title: listing title.
    '''
    printCabeceraListadoCapacityFactor(outputFile,title)
    xcSet= preprocessor.getSets.getSet(setName)
    elems= xcSet.elements
    tagFCCPCP= 0
    FCCPCP= 0
    HIPCPCP= "" 
    for e in elems:
        outputFile.write(str(e.tag)+" & "+e.getProp("HIPCP")+" & "+'{:5.2f}'.format(e.getProp("NCP")/1e3)+" & "+'{:5.2f}'.format(e.getProp("MyCP")/1e3)+" & "+'{:5.2f}'.format(e.getProp("MzCP")/1e3)+" & "+'{:3.2f}'.format(e.getProp("FCCP"))+"\\\\\n")
        if(e.getProp("FCCP")>FCCPCP):
            FCCPCP= e.getProp("FCCP")
            HIPCPCP= e.getProp("HIPCP")
            tagFCCPCP= e.tag
    outputFile.write("\\hline\n")
    outputFile.write(tagFCCPCP+" & "+HIPCPCP+" &  &  &  & "+'{:3.2f}'.format(FCCPCP)+"\\\\\n")
    printCierreListadoCapacityFactor(outputFile) 

