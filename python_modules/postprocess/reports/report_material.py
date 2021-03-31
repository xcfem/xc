# -*- coding: utf-8 -*-

__author__= "Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega@ciccp.es "

import matplotlib.pyplot as plt
from postprocess.reports import graph_material

class ConcreteReport(object):
    '''Constructs LaTex tables for depicting porperties of concrete 
    materials.

    :ivar concrDiag: concrete material (of type concrete01 or concrete02)
    '''
    def __init__(self,concrDiag):
        self.concr=concrDiag

    def tableCompress(self):
        ''' returns a LaTeX table that depicts the concrete compressive 
         properties.
        '''
        retval='\\multicolumn{2}{c}{Compressive properties} \\\\ \n '
        retval+='\\hline \n '
        retval+='$f_{c}$ & ' + str(self.concr.fpc*1e-6) + ' MPa \\\\ \n '
        retval+='$\\epsilon_{c0}$ & ' + str(self.concr.epsc0*1e3) + ' $\\permil$ \\\\ \n '
        retval+='$f_{cu}$ & ' + str(self.concr.fpcu*1e-6) + ' MPa \\\\ \n '
        retval+='$\\epsilon_{cu}$ & ' + str(self.concr.epscu*1e3) + ' $\\permil$ \\\\ \n '
        retval+='$E_{c0}$ & ' + str(self.concr.getInitialTangent()*1e-9) + ' GPa \\\\ \n '
        return retval
    
    def tableTens(self):
        ''' returns a LaTeX table that depicts the concrete tensile 
         properties.
        '''
        retval='\\multicolumn{2}{c}{Tensile properties} \\\\ \n '
        retval+='\\hline \n '
        retval+='$f_{ct}$ & ' + str(round(self.concr.ft*1e-6,2)) + ' MPa \\\\ \n '
        return retval

    def tableTensStiff(self,paramTensStiffness):
        '''returns a LaTeX table that depicts the concrete tension 
         stiffening properties (concrete of type concrete02).

        :param paramTensStiffness: instance of the object 
         concrete_base.paramTensStiffness that defines the parameters 
         of tension stiffness
        '''
        retval='\\multicolumn{2}{c}{Tension-stiffening} \\\\ \n '
        retval+='\\hline \n '
        retval+='$\\nu$ & ' + str(paramTensStiffness.nu())+'  \\\\ \n '
        retval+='$\\rho_{eff}$ & ' + str(paramTensStiffness.ro*1e2) +'\%  \\\\ \n '
        retval+='\\multicolumn{2}{l}{exponential curve $\\alpha$=' + str(round(paramTensStiffness.alfa(),3)) +'} \\\\ \n '
        retval+='\\multicolumn{2}{l}{slope regression line:' + str(round(paramTensStiffness.regresLine()['slope']*1e-9,2)) +' } \\\\ \n '
        retval+='\\multicolumn{2}{l}{inters. point (onset carcking):} \\\\ \n '
        retval+='\\multicolumn{2}{c}{($\\epsilon$='+str(round(paramTensStiffness.pointOnsetCracking()['eps_ct']*1e2,2)) +'\\%, $\\sigma$='+ str(round(paramTensStiffness.pointOnsetCracking()['ft']*1e-6,2)) + 'MPa)} \\\\ \n '
        return retval
        

def report_concrete02(concrDiag,paramTensStiffening,grTitle,grFileName,texFileName):
    '''return a LaTeX table that depicts the concrete compressive,
    tensile and tension-stiffening properties as well as its
    stress-strain graphic.

    :param concrDiag: concrete material of type concrete02
    :param paramTensStiffening: instance of the object 
     concrete_base.paramTensStiffness that defines the parameters 
     of tension stiffness.
    :param grTitle: title for the graphic
    :param grFileName: name of the graphics file
    :param texFileName: name of the LaTeX file
    '''
    grph= graph_material.UniaxialMaterialDiagramGraphic(epsMin=concrDiag.epscu,epsMax=paramTensStiffening.eps_y,title=grTitle)
    grph.setupGraphic(plt,concrDiag)
    grph.savefig(plt,grFileName+'.png')
    grph.savefig(plt,grFileName+'.eps')
    concrRep= ConcreteReport(concrDiag)
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
   
