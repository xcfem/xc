# -*- coding: utf-8 -*-

__author__= "Ana Ortega (AO_O) Luis C. Pérez Tato"
__copyright__= "Copyright 2017, AO_O LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.Ortega@ciccp.es l.pereztato@ciccp.es"

from misc_utils import log_messages as lmsg
from misc.latex import latex_utils
import csv

class LoadCase(object):
    '''Definition of a load case.

    :ivar name: name given to the load case
    :ivar loadPType: type of load pattern. Available types:
          "default", "uniform_excitation", "multi_support_pattern", 
          "pbowl_loading". Defaults to "default"
    :ivar timeSType: type of Time Series. Available time series:
          "constant_ts","linear_ts","path_ts","pulse_ts","rectangular_ts","triangular_ts","trig_ts". Defaults to "constant_ts"
    :ivar timeSName: name of the Time Serie
    :ivar lstOfLoadDef: list of load definitions added to the load case
    '''
    def __init__(self, preprocessor, name, loadPType="default", timeSType="constant_ts"):
        self.preprocessor=preprocessor
        self.name= name
        self.loadPType=loadPType
        self.timeSType=timeSType
        self.timeSName=self.name+'TS'
        self.lstOfLoadDef=[]

    def create(self):
        '''Create the load case and set it as current
        '''
        lPatterns= self.preprocessor.getLoadHandler.getLoadPatterns
        lPatterns.newTimeSeries(self.timeSType,self.timeSName)
        lPatterns.currentTimeSeries=self.timeSName
        lPatterns.newLoadPattern(self.loadPType,self.name)
#        lPatterns.addToDomain(self.name) #10/06/2019
        lPatterns.currentLoadPattern=self.name
            
    def addLstLoads(self,lstLoads):
        '''list of loads to be added to the load case.
        '''
        self.lstOfLoadDef+=lstLoads
        for ld in lstLoads:
            ld.appendLoadToCurrentLoadPattern()

class LoadCaseManager(object):
    '''Manager for XC load cases.'''
    def __init__(self,preprocessor):
       self.prep= preprocessor
       self.loadHandler= preprocessor.getLoadHandler
       self.loadPatterns= self.loadHandler.getLoadPatterns
       self.timeSeries= dict()
       self.loadCases= dict()

    def defineSimpleLoadCases(self, names):
       '''Define load patterns with constant time series.

       :param names: list of names for the load patterns to define.
       '''
       tsName= 'ts'
       if(tsName not in self.timeSeries): # Time series not already defined.
           ts= self.loadPatterns.newTimeSeries('constant_ts',tsName)
           self.timeSeries[tsName]= ts
       self.loadPatterns.currentTimeSeries= tsName
       for name in names:
           self.newSimpleLoadPattern(name= name)

    def newSimpleLoadPattern(self, name):
       '''Define a load pattern with constant time series.

       :param name: name for the new load pattern.
       '''
       if(name not in self.loadCases): # Load pattern not already defined.
           self.appendLoadPattern(self.loadPatterns.newLoadPattern('default',name))

    def appendLoadPattern(self, lp):
        ''' Appends an already defined load pattern.

        :param lp: load pattern to append.
        '''
        if(lp.name not in self.loadCases): # not already in the container.
            self.loadCases[lp.name]= lp
        
    def setCurrentLoadCase(self, name, newLoadCase= False):
       '''Sets current load case.

       :param name: name for the load case to set as current.
       :param newLoadCase: if true, create the load case if doesn't exists.
       '''
       self.loadPatterns.currentLoadPattern= name
       if(name not in self.loadCases):
           if(newLoadCase):
               self.defineSimpleLoadCases([name])
           else: 
               lmsg.warning('Load case: \''+ name+ '\' doesn\'t exists.')
       return self.getLoadCase(name)
    
    def getCurrentLoadCase(self):
       '''Returns current load case.'''
       name= self.loadPatterns.currentLoadPattern
       return self.getLoadCase(name)

    def getLoadCase(self,name):
       return self.loadCases[name]

    def getLoadCasesNames(self):
        ''' Return the names of the load cases in the container.'''
        return list(self.loadCases.keys())

    def getCodesAndDescriptions(self, header= False):
        ''' Return a list with the defined load cases and their descriptions.

        :param header: if true put a header in the first row.
        '''
        retval= list()
        if(header):
            retval.append(('Code', 'description'))
        for key in self.loadCases:
            loadCase= self.loadCases[key]
            retval.append((key, loadCase.description))
        return retval

    def exportToCSV(self, fileName, header= False):
        ''' Creates a CSV file with the defined load cases and their
            descriptions.

        :param fileName: output file name.
        '''
        rows= self.getCodesAndDescriptions()
        with open(fileName, 'w') as file:
            writer = csv.writer(file)
            for r in rows:
                writer.writerow(r)
            
    def getLaTeXCode(self, small= True):
        ''' Return the LaTeX string corresponding to load cases in the
            container.

        :param small: if true, use small font.
        '''
        retval= str()
        if(len(self.loadCases)>0):
            header= 'Actions'
            retval+= ('\\begin{center}\n')
            if(small):
                retval+= ('\\begin{small}\n')
            retval+= ('\\begin{longtable}{|l|p{10cm}|}\n')
            retval+= ('\\hline\n')
            retval+= ('\\multicolumn{2}{|c|}{'+header+'}\\\\\n')
            retval+= ('\\hline\n')
            retval+= ('\\textbf{Code} & \\textbf{Description} \\\\\n')
            retval+= ('\\hline\n')
            retval+= ('\\endfirsthead\n')

            retval+= ('\\hline\n')
            retval+= ('\\multicolumn{2}{|c|}{'+header+'}\\\\\n')
            retval+= ('\\hline\n')
            retval+= ('\\textbf{Code} & \\textbf{Description} \\\\\n')
            retval+= ('\\hline\n')
            retval+= ('\\endhead\n')

            retval+= ('\\hline \\multicolumn{2}{|r|}{{../..}} \\\\ \\hline\n')
            retval+= ('\\endfoot\n')

            retval+= ('\\hline\n')
            retval+= ('\\endlastfoot\n')
            for key in self.loadCases:
                loadCase= self.loadCases[key]
                retval+= key + ' & '+ loadCase.description +'\\\\\n'
            retval+= ('\\hline\n')
            retval+= ('\\end{longtable}\n')
            if(small):
                retval+= ('\\end{small}\n')
            retval+= ('\\end{center}\n')
        return retval
    
    def exportToLatex(self, fileName):
        '''Creates LaTeX tables and put the combinations in them.

        :param fileName: output file name.
        '''
        with open(fileName, "w") as f:
            f.write(self.getLaTeXCode())
            
    def exportToPDF(self, fileName):
        ''' Creates a PDF file and write the combinations to it.

        :param fileName: output file name.
        '''
        latexCode= self.getLaTeXCode()
        latex_utils.latex_to_pdf(latexCode= latexCode, pdfFileName= fileName)

def resetAccionesConstantTS(preprocessor,tipoTimeSeries, nmbTimeSeries, fct):
    '''Clear all load patterns in the model and create a new TimeSeries

    :param tipoTimeSeries: time series type
    :param  nmbTimeSeries: time series name.
    :param  fct: Factor
    '''
    constraints= preprocessor.getBoundaryCondHandler
    lps= constraints.getActiveLoadPatterns
    lps.clearrAll()
    loads= preprocessor.getLoadHandler
    casos= loads.getLoadPatterns
    loads.clearAll()
    ts= casos.newTimeSeries(tipoTimeSeries,nmbTimeSeries)
    casos.currentTimeSeries= ts.name
