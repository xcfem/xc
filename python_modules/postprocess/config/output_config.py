# -*- coding: utf-8 -*-

'''Configuration of variables that control output
'''
__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

class verifOutVars(object):
    '''Variables that control the output of limit state verifications.

    :param setCalc: set of elements to be analyzed (defaults to 'None' which 
           means that all the elements in the file of internal forces
           results are analyzed) 
    :param appendToResFile:  'Yes','Y','y',.., if results are appended to 
           existing file of results (defaults to 'N')
    :param listFile: 'Yes','Y','y',.., if latex listing file of results 
           is desired to be generated (defaults to 'N')
    :param calcMeanCF: 'Yes','Y','y',.., if mean capacity factor is desired
           to be calculated (defaults to 'N')
    '''
    def __init__(self,setCalc=None,appendToResFile='N',listFile='N',calcMeanCF='N'):
        self.setCalc=setCalc
        self.appendToResFile=appendToResFile
        self.listFile=listFile
        self.calcMeanCF=calcMeanCF
