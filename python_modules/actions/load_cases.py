# -*- coding: utf-8 -*-

__author__= "Ana Ortega (AO_O) Luis C. Pérez Tato"
__copyright__= "Copyright 2017, AO_O LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.Ortega@ciccp.es l.pereztato@ciccp.es"

from misc_utils import log_messages as lmsg
from misc.latex import latex_utils
import csv

def get_time_series_type_from_class_name(className):
    ''' Return the type of the time series from the given class name.

    :param className: class name.
    '''
    retval= None
    demangled_class_name= className.removeprefix('XC::')
    if(demangled_class_name=='ConstantSeries'):
        retval= 'constant_ts'    
    elif(demangled_class_name=='RectangularSeries'):
        retval= 'rectangular_ts'
    elif(demangled_class_name=='LinearSeries'):
        retval= 'linear_ts'
    elif(demangled_class_name=='SimulatedRandomProcessSeries'):
        retval= 'simulated_random_process_ts'
    elif(demangled_class_name=='PulseSeries'):
        retval= 'pulse_ts'
    elif(demangled_class_name=='PathSeries'):
        retval= 'path_ts'
    elif(demangled_class_name=='TriangleSeries'):
        retval= 'triangle_ts'
    elif(demangled_class_name=='PathTimeSeries'):
        retval= 'path_time_ts'
    elif(demangled_class_name=='TrigSeries'):
        retval= 'trig_ts'
    else:
        methodName= sys._getframe(0).f_code.co_name
        errMsg= "'; type for class name: '"+str(demangled_class_name)
        errMsg+= "' not found."
        lmsg.error(errMsg)
        exit(1)
    return retval

def get_load_pattern_type_from_class_name(className):
    ''' Return the type of the load pattern from the given class name.

    :param className: class name.
    '''
    retval= None
    demangled_class_name= className.removeprefix('XC::')
    if(demangled_class_name=='LoadPattern'):
        retval= 'default'
    elif(demangled_class_name=='EarthquakePattern'):
        retval= 'earthquake'
    elif(demangled_class_name=='UniformExcitation'):
        retval= 'uniform_excitation'
    elif(demangled_class_name=='MultiSupportPattern'):
        retval= 'multi_support_pattern'
    elif(demangled_class_name=='PBowlLoading'):
        retval= 'pbowl_loading'
    else:
        methodName= sys._getframe(0).f_code.co_name
        errMsg= "'; type for class name: '"+str(demangled_class_name)
        errMsg+= "' not found."
        lmsg.error(errMsg)
        exit(1)
    return retval

class LoadCase(object):
    '''Definition of a load case.

    :ivar name: name given to the load case
    :ivar loadPType: type of load pattern. Available types:
                     "default", "uniform_excitation", "multi_support_pattern", 
                     "pbowl_loading". Defaults to "default"
    :ivar timeSType: type of time series. Available time series:
                     "constant_ts","linear_ts","path_ts","pulse_ts",
                     "rectangular_ts","triangular_ts","trig_ts". 
                     Defaults to "constant_ts"
    :ivar timeSName: name of the time series
    :ivar lstOfLoadDef: list of load definitions added to the load case.
    '''
    def __init__(self, preprocessor, name, loadPType="default", timeSType="constant_ts"):
        ''' Constructor.

        :param preprocessor: preprocessor of the finite element problem.
        :param name: name of the load case.
        :param loadPType: type of the load pattern. Available types are:
                          "default", "uniform_excitation", 
                          "multi_support_pattern", "pbowl_loading". 
                           Defaults to "default"
        :param timeSType: type of time series. Available time series:
                          "constant_ts","linear_ts","path_ts","pulse_ts",
                          "rectangular_ts","triangular_ts","trig_ts". 
                          Defaults to "constant_ts"
        :param timeSName: name of the time series
        '''
        self.preprocessor= preprocessor
        self.name= name
        self.loadPType= loadPType
        self.timeSType= timeSType
        self.lstOfLoadDef= list()

    def getTimeSeriesName(self):
        ''' Return the name of the time series corresponding to this load
            case.
        '''
        return self.name+'TS'

    def getDict(self):
        ''' Return a dictionary with the object values.'''
        return {'name':self.name, 'load_pattern_type':self.loadPType, 'time_series_type':self.timeSType, 'load_list': self.lstOfLoadDef}

    def setFromDict(self, dct):
        ''' Set the fields from the values of the dictionary argument.

        :param dct: dictionary containing the field values.
        '''
        self.name= dct['name']
        self.loadPType= dct['load_pattern_type']
        self.timeSType= dct['time_series_type']
        self.lstOfLoadDef= dct['load_list']

    def newTimeSeries(self):
        ''' Create the time series for the load case.'''
        lPatterns= self.preprocessor.getLoadHandler.getLoadPatterns
        tsName= self.getTimeSeriesName()
        retval= lPatterns.getTimeSeries(tsName)
        if(not retval):
            retval= lPatterns.newTimeSeries(self.timeSType, tsName)
        lPatterns.currentTimeSeries= tsName
        return retval

    def newLoadPattern(self):
        ''' Create the load pattern.'''
        lPatterns= self.preprocessor.getLoadHandler.getLoadPatterns
        retval= lPatterns.getLoadPattern(self.name)
        if(not retval):
            retval= lPatterns.newLoadPattern(self.loadPType, self.name)
        lPatterns.currentLoadPattern= self.name
        return retval

    def create(self):
        '''Create the XC load case and set it as the current one.
        '''
        self.newTimeSeries()
        return self.newLoadPattern()
            
    def addLstLoads(self, lstLoads):
        '''list of loads to be added to the load case.

        :param lstLoads: list of load definitions to be added to this load case.
        '''
        self.lstOfLoadDef+=lstLoads
        for ld in lstLoads:
            ld.appendLoadToCurrentLoadPattern()

class LoadCaseManager(object):
    '''Manager for XC load cases.

    :ivar prep: preprocessor of the finite element problem.
    :ivar timeSeries: dictionary containing the time series defined by this
                      object. 
    :ivar loadCases: dictionary containing the load patterns defined by this
                     object. 
    '''
    def __init__(self, preprocessor):
        ''' Constructor.

        :param preprocessor: preprocessor of the finite element problem.
        ''' 
        self.prep= preprocessor
        self.timeSeries= dict()
        self.loadCases= dict()

    def getDict(self):
        ''' Return a dictionary with the values for the object fields.'''
        retval= dict()
        # Create time series dictionary.
        ts_dict= dict()
        for key in self.timeSeries:
            ts= self.timeSeries[key]
            ts_dict[key]= ts.getDict()
        retval['time_series']= ts_dict
        # Create load cases dictionary.
        lp_dict= dict()
        for key in self.loadCases:
            lc= self.loadCases[key]
            lp_dict[key]= lc.getDict()
        retval['load_patterns']= lp_dict
        return retval
            
    def setFromDict(self, dct):
        ''' Set the fields from the values of the dictionary argument.

        :param dct: dictionary containing the field values.
        '''
        # Read time series.
        ts_dict= dct['time_series']
        for key in ts_dict:
            value= ts_dict[key]
            tsType= get_time_series_type_from_class_name(value['className'])
            ts= self.newTimeSeries(tsName= key, tsType= tsType)
            self.timeSeries[key]= ts
        # Read load cases.
        lp_dict= dct['load_patterns']
        for key in lp_dict:
            value= lp_dict[key]
            lpType= get_load_pattern_type_from_class_name(value['className'])
            lp= self.newLoadPattern(name= key, lpType= lpType)
            lp.setFromDict(value)
            self.loadCases[key]= lp
        
    def getLoadHandler(self):
        ''' Return the preprocessor's load handler.'''
        return self.prep.getLoadHandler
        
    def getLoadPatterns(self):
        ''' Return the preprocessor's loads handler container.'''
        return self.getLoadHandler().getLoadPatterns

    def newTimeSeries(self, tsName, tsType):
        ''' Create the time series for the load case.

        :param tsName: name for the new time series.
        :param tsType: type for the new time series.
        '''
        retval= None
        lPatterns= self.getLoadPatterns()
        if(tsName not in self.timeSeries):
            retval= lPatterns.getTimeSeries(tsName)
            if(not retval):
                retval= lPatterns.newTimeSeries(tsType, tsName)
            
            self.timeSeries[tsName]= retval
        else:
            retval= self.timeSeries[tsName]
        if(not retval):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            errMsg= "; creation of time series: '"+str(tsName)+"' of type: '"
            errMsg+= str(tsType)+"' failed."
            lmsg.error(className+'.'+methodName+errMsg)
            exit(1)
        else:
            lPatterns.currentTimeSeries= tsName
        return retval
    
    def defineDefaultTimeSeries(self):
        ''' Define the time series to use by default for new load patterns.'''
        retval= None
        tsName= 'ts'
        return self.newTimeSeries(tsName= 'ts', tsType= 'constant_ts')
    
    def defineSimpleLoadCases(self, names):
       '''Define load patterns with constant time series.

       :param names: list of names for the load patterns to define.
       '''
       ts= self.defineDefaultTimeSeries()
       for name in names:
           self.newSimpleLoadPattern(name= name)

    def newLoadPattern(self, name, lpType):
        '''Define a load pattern with constant time series.

        :param name: name for the new load pattern.
        :param lpType: type of the new load pattern.
        '''
        retval= None
        lPatterns= self.getLoadPatterns()
        if(name not in self.loadCases): # Load pattern not already defined.
            retval=  lPatterns.getLoadPattern(name)
            if(not retval):
                retval= lPatterns.newLoadPattern(lpType, name)
            self.appendLoadPattern(retval)
        else:
            retval= self.loadCases[name]
        if(not retval):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            errMsg= "; creation of time series: '"+str(tsName)+"' of type: '"
            errMsg+= str(tsType)+"' failed."
            lmsg.error(className+'.'+methodName+errMsg)
            exit(1)
        return retval
           
    def newSimpleLoadPattern(self, name):
       '''Define a load pattern with constant time series.

       :param name: name for the new load pattern.
       '''
       self.newLoadPattern(name= name, lpType= 'default')

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
        self.getLoadPatterns().currentLoadPattern= name
        if(name not in self.loadCases):
            if(newLoadCase):
                self.defineSimpleLoadCases([name])
            else: 
                lmsg.warning('Load case: \''+ name+ '\' doesn\'t exists.')
        return self.getLoadCase(name)
    
    def getCurrentLoadCase(self):
        '''Returns current load case.'''
        name= self.getLoadPatterns().currentLoadPattern
        return self.getLoadCase(name)

    def getLoadCase(self, name):
        ''' Return the load case with the give name.

        :param name: name of the load case to retrieve.
        '''
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
            
    def getLaTeXCode(self, small= True, superTabular= False):
        ''' Return the LaTeX string corresponding to load cases in the
            container.

        :param small: if true, use small font.
        :param superTabular: if true, use supertabular instead longtable.
        '''
        retval= str()
        if(len(self.loadCases)>0):
            header= 'Actions'
            retval+= ('\\begin{center}\n')
            if(small):
                retval+= ('\\begin{small}\n')
            tableStr= '|l|p{10cm}|'
            if(superTabular):
                # Create LaTeX supertabular.
                firstHeadStr= '\\hline%\n'
                firstHeadStr+= '\\multicolumn{2}{|c|}{'+header+'}\\\\%\n'
                firstHeadStr+= '\\hline%\n'
                firstHeadStr+= '\\textbf{Code} & \\textbf{Description} \\\\%\n'
                firstHeadStr+= '\\hline%\n'
                retval+= '\\tablefirsthead{'+firstHeadStr+'}%\n'

                headStr= firstHeadStr
                retval+= '\\tablehead{'+headStr+'}%\n'

                tailStr= '\\hline%\n'
                tailStr+= '\\multicolumn{2}{|r|}{{../..}} \\\\%\n'
                tailStr+= '\\hline%\n'
                retval+= '\\tabletail{'+tailStr+'}%\n'

                lastTailStr= '\\hline%\n'
                retval+= '\\tablelasttail{'+lastTailStr+'}%\n'
                retval+= '\\begin{supertabular}{'+tableStr+'}\n'
            else:
                # Create LaTeX longtable.
                retval+= '\\begin{longtable}{'+tableStr+'}\n'
                retval+= '\\hline\n'
                retval+= '\\multicolumn{2}{|c|}{'+header+'}\\\\\n'
                retval+= '\\hline\n'
                retval+= '\\textbf{Code} & \\textbf{Description} \\\\\n'
                retval+= '\\hline\n'
                retval+= '\\endfirsthead\n'

                retval+= '\\hline\n'
                retval+= '\\multicolumn{2}{|c|}{'+header+'}\\\\\n'
                retval+= '\\hline\n'
                retval+= '\\textbf{Code} & \\textbf{Description} \\\\\n'
                retval+= '\\hline\n'
                retval+= '\\endhead\n'

                retval+= ('\\hline \\multicolumn{2}{|r|}{{../..}} \\\\ \\hline\n')
                retval+= ('\\endfoot\n')

                retval+= ('\\hline\n')
                retval+= ('\\endlastfoot\n')
            for key in self.loadCases:
                loadCase= self.loadCases[key]
                retval+= key + ' & '+ loadCase.description +'\\\\\n'
            retval+= ('\\hline\n')
            if(superTabular):
                retval+= ('\\end{supertabular}\n')
            else:
                retval+= ('\\end{longtable}\n')
            if(small):
                retval+= ('\\end{small}\n')
            retval+= ('\\end{center}\n')
        return retval
    
    def exportToLatex(self, fileName, superTabular= False):
        '''Creates LaTeX tables and put the combinations in them.

        :param fileName: output file name.
        :param superTabular: if true, use supertabular instead longtable.
        '''
        with open(fileName, "w") as f:
            f.write(self.getLaTeXCode())
            
    def exportToPDF(self, fileName, superTabular= False):
        ''' Creates a PDF file and write the combinations to it.

        :param fileName: output file name.
        :param superTabular: if true, use supertabular instead longtable.
        '''
        latexCode= self.getLaTeXCode()
        latex_utils.latex_to_pdf(latexCode= latexCode, pdfFileName= fileName, superTabular= superTabular)

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
