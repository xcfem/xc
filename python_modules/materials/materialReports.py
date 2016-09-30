# -*- coding: utf-8 -*-

__author__= "Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega@ciccp.es "

class concreteReport(object):
    '''Constructs LaTex tables for depicting porperties of concrete 
    materials.

    :ivar concrDiag: concrete material (of type concrete01 or concrete02)
    '''
    def __init__(self,concrDiag):
        self.concr=concrDiag

    def tableCompress(self):
        ''':returns: a LaTeX table that depicts the concrete compressive 
        properties.
        '''
        retval='\\begin{tabular}{lr} \n '
        retval+='\\multicolumn{2}{c}{Compressive prop.} \\\\ \n '
        retval+='\\hline \n '
        retval+='$f_{c}$ & ' + str(self.concr.fpc*1e-6) + ' MPa \\\\ \n '
        retval+='$\\epsilon_{c0}$ & ' + str(self.concr.epsc0*1e3) + ' $\\permil$ \\\\ \n '
        retval+='$f_{cu}$ & ' + str(self.concr.fpcu*1e-6) + ' MPa \\\\ \n '
        retval+='$\\epsilon_{cu}$ & ' + str(self.concr.epscu*1e3) + ' $\\permil$ \\\\ \n '
        retval+='$E_{c0}$ & ' + str(self.concr.getInitialTangent()*1e-9) + ' GPa \\\\ \n '
        retval+='\\end{tabular} \n '
        return retval
    
    def tableTens(self):
        ''':returns: a LaTeX table that depicts the concrete tensile 
        properties.
        '''
        retval='\\begin{tabular}{lr} \n '
        retval+='\\multicolumn{2}{c}{Tensile prop.} \\\\ \n '
        retval+='\\hline \n '
        retval+='$f_{ct}$ & ' + str(self.concr.ft*1e-6) + ' MPa \\\\ \n '
        retval+='\\end{tabular} \n '
        return retval

    def tableTensStiff(self,paramTensStiffeness):
        ''':returns: a LaTeX table that depicts the concrete tension 
        stiffening properties (concrete of type concrete02).
        :param paramTensStiffeness: instance of the object 
        concreteBase.paramTensStiffenes that defines the parameters 
        of tension stiffenes
        '''
        retval='\\begin{tabular}{ll} \n '
        retval+='\\multicolumn{2}{c}{Tension-stiffening} \\\\ \n '
        retval+='\\hline \n '
        retval+='$\\nu$ & ' + str(paramTensStiffeness.nu())+'  \\\\ \n '
        retval+='$\\rho_{eff}$ & ' + str(paramTensStiffeness.ro*1e2) +'\%  \\\\ \n '
        retval+='\\multicolumn{2}{l}{exponential curve $\\alpha$=' + str(round(paramTensStiffeness.alfa(),3)) +'} \\\\ \n '
        retval+='\\multicolumn{2}{l}{slope regression line:' + str(round(paramTensStiffeness.regresLine()['slope']*1e-9,2)) +' } \\\\ \n '
        retval+='\\multicolumn{2}{l}{intersection point (onset carcking):} \\\\ \n '
        retval+='\\multicolumn{2}{c}{($\\epsilon$='+str(round(paramTensStiffeness.pointOnsetCracking()['eps_ct']*1e2,2)) +'\\%, $\\sigma$='+ str(round(paramTensStiffeness.pointOnsetCracking()['ft']*1e-6,2)) + 'MPa)} \\\\ \n '
        retval+='\\end{tabular} \n '
        return retval
        
