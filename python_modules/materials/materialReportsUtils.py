# -*- coding: utf-8 -*-

__author__= "Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega@ciccp.es "

import materialReports
import matplotlib.pyplot as plt
from materials import materialGraphics
from materials import materialReports

#Utils for generating LaTeX reports concerning materials


def reportConcrete02(concrDiag,paramTensStiffening,grTitle,grFileName,texFileName):
    ''':returns: a LaTeX table that depicts the concrete compressive,
        tensile and tension-stiffening properties as well as its
        stress-strain graphic.
    :param concrDiag: concrete material of type concrete02
    :param paramTensStiffening: instance of the object 
    concrete_base.paramTensStiffenes that defines the parameters 
    of tension stiffenes.
    :param grTitle: title for the graphic
    :param grFileName: name of the graphics file
    :param texFileName: name of the LaTeX file
    '''
    grph= materialGraphics.UniaxialMaterialDiagramGraphic(epsMin=concrDiag.epscu,epsMax=paramTensStiffening.eps_y,title=grTitle)
    grph.setupGraphic(plt,concrDiag)
    grph.savefig(plt,grFileName+'.jpeg')
    grph.savefig(plt,grFileName+'.eps')
    concrRep=materialReports.concreteReport(concrDiag)
    grFileNameNoPath=grFileName.split('/')[grFileName.count('/')]
    f=open(texFileName,'w')
    f.write('\\documentclass{article} \n')
    f.write('\\usepackage{graphicx} \n')
    f.write('\\usepackage{multirow} \n')
    f.write('\\usepackage{wasysym} \n')
    f.write('\\usepackage{gensymb} \n')
    f.write('\\begin{document} \n')
    f.write('\\begin{table} \n')
    f.write('\\begin{center} \n')
    f.write('\\begin{tabular}{ll} \n')
    f.write('\\begin{minipage}{95mm} \n')
    f.write('\\vspace{2mm} \n')
    f.write('\\begin{center} \n')
    f.write('\\includegraphics[width=90mm]{'+grFileNameNoPath+'} \n')
    f.write('\\end{center} \n')
    f.write('\\vspace{1pt} \n')
    f.write('\\end{minipage} &  \n')
    f.write('\\begin{tabular}{lr}  \n') 
    f.write(concrRep.tableCompress())
    f.write(concrRep.tableTens())
    f.write(concrRep.tableTensStiff(paramTensStiffening))
    f.write('\\end{tabular} \n')
    f.write('\\end{tabular} \n')
    f.write('\\end{center} \n')
    f.write('\\end{table} \n')
    f.write('\\end{document} \n')
    f.close()
    return
