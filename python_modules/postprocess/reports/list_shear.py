# -*- coding: utf-8 -*-

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es" "ana.Ortega@ciccp.es"

from misc.latex import latex_utils

def printShearVerificationListingHeader(outputFile, sectionName, title):
    ''' Prints the header of the shear verification listing.

    :param outputFile: output file.
    :param sectionName: name of the section being checked.
    :param title: title for the tabular data.
    '''
    outputFile.write("\\begin{center}\n")
    outputFile.write("\\begin{footnotesize}\n")
    outputFile.write("\\tablefirsthead{%\n")
    outputFile.write("\\hline\n")
    outputFile.write("\\multicolumn{12}{|c|}{\\textbf{"+title+"}}\\\\\n")
    outputFile.write("\\multicolumn{12}{|c|}{Comprobación de la sección "+ latex_utils.toLaTeX(sectionName)+"}\\\\\n")
    outputFile.write("\\hline\n")
    outputFile.write("Elem. & Hip. & $N$  & $V_y$ & $V_z$ & $V_{u1}$ & $\\theta$              & $V_{cu}$ & $V_{su}$  & $V_{u2}$ & $V_u$ & FC \\\\\n")
    outputFile.write("      &      & (kN) & (kN)  & (kN)  & (kN)     & (\\textsuperscript{o}) & (kN)     & (kN)      & (kN)     & (kN)  &    \\\\\n")
    outputFile.write("\\hline}\n")
    outputFile.write("\\tablehead{%\n")
    outputFile.write("\\hline\n")
    outputFile.write("\\multicolumn{12}{|l|}{\\small\\sl \\ldots continúa}\\\\\n")
    outputFile.write("\\hline\n")
    outputFile.write("\\multicolumn{12}{|c|}{\\textbf{"+title+"}}\\\\\n")
    outputFile.write("\\multicolumn{12}{|c|}{Comprobación de la sección "+ latex_utils.toLaTeX(sectionName)+"}\\\\\n")
    outputFile.write("\\hline\n")
    outputFile.write("Elem. & Hip. & $N$  & $V_y$ & $V_z$ & $V_{u1}$ & $\\theta$              & $V_{cu}$ & $V_{su}$  & $V_{u2}$ & $V_u$ & FC \\\\\n")
    outputFile.write("      &      & (kN) & (kN)  & (kN)  & (kN)     & (\\textsuperscript{o}) & (kN)     & (kN)      & (kN)     & (kN)  &    \\\\\n")
    outputFile.write("\\hline}\n")
    outputFile.write("\\tabletail{%\n")
    outputFile.write("\\hline\n")
    outputFile.write("\\multicolumn{12}{|r|}{\\small\\sl continúa\\ldots}\\\\\n")
    outputFile.write("\\hline}\n")
    outputFile.write("\\tablelasttail{\\hline}\n")
    outputFile.write("\\begin{supertabular}{|l|r|r|r|r|r|r|r|r|r|r|r|}\n")


def printShearVerificationListingFoot(outputFile):
    '''Print the shear verification listing foot.'''
    outputFile.write("\\end{supertabular}\n")
    outputFile.write("\\end{footnotesize}\n")
    outputFile.write("\\end{center}\n")

