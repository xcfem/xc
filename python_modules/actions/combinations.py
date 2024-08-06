# -*- coding: utf-8 -*-
'''combinations.py: manages the container for load combinations.'''

__author__= "Luis C. Pérez Tato (LCPT) Ana Ortega (A_OO) "
__copyright__= "Copyright 2015,  LCPT A_OO "
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.Ortega.Ort@gmail.com"

import os
import sys
import json
import csv
import math
from import_export import neutral_load_description as nld
from postprocess.reports import graphical_reports
from misc_utils import log_messages as lmsg
from misc.latex import latex_utils
from actions.load_combination_utils import utils


class CombinationRecord(object):
    '''Combination name and expression (i.e. ELS01= 1.0*G+1.0*Q)

    :ivar name: combination name (i.e. "ELS01").
    :ivar expr: expresion of the load combination (i.e. 1.0*G+1.0*Q).
    '''
    def __init__(self,name,expr):
        ''' Constructor.

        :param name: combination name (i.e. "ELS01").
        :param expr: expresion of the load combination (i.e. 1.0*G+1.0*Q).
        '''
        self.name= name
        self.expr= expr

    def getDict(self):
        ''' Return a dictionary with the object values.'''
        return {'name':self.name, 'expr':self.expr}

    def setFromDict(self,dct):
        ''' Set the fields from the values of the dictionary argument.'''
        self.name= dct['name']
        self.expr= dct['expr']
        
    def createCombination(self,xcCombHandler):
        '''Create combination and insert it into the XC combination handler.

        :param xcCombHandler: combination handler.
        '''
        return xcCombHandler.newLoadCombination(self.name,self.expr)

    def getNameExpressionPair(self):
        ''' Return a (combinationName, combinationExpression) tuple.'''
        return (self.name, self.expr)

    def getLaTeXCode(self):
        ''' Return the LaTeX string corresponding to this combination.'''
        retval= self.name.replace('_','\\_')
        retval+= ' & '
        retval+= self.expr.replace('*',' ')
        retval+= '\\\\\n'
        return retval
    
    def exportToLatex(self, outputFile):
        '''Creates LaTeX tables and put the combinations in them.
        
         :param outputFile: file to write into.
         '''
        outputFile.write(self.getLaTeXCode())

    def getLoadCaseDispParameters(self,setsToDispLoads,setsToDispDspRot,setsToDispIntForc, unitsScaleForc= 1e-3, unitsScaleMom= 1e-3, unitsScaleDisp= 1e3, unitsDispl= '[mm]'):
        '''Return a suitable LoadCaseDispParameters for the combination.

        :param setsToDispLoads: ordered list of sets of elements to display 
                                loads.
        :param setsToDispDspRot: ordered list of sets of elements to display 
                                displacements. 
        :param setsToDispIntForc: ordered list of sets of elements to display 
                                 internal forces.
        :param unitsScaleForc: factor to apply to internal forces if we want to 
                               change the units (defaults to 1e-3).
        :param unitsScaleMom: factor to apply to internal moments if we want to
                              change the units (defaults to 1e-3).
        :param unitsScaleDispl: factor to apply to displacements if we want to 
                                change the units (defaults to 1e3).
        :param unitsDispl: text to especify the units in which displacements are 
                           represented (defaults to '[mm]'
        '''
        retval= graphical_reports.LoadCaseDispParameters(self.name,self.expr,self.expr,setsToDispLoads,setsToDispDspRot,setsToDispIntForc)
        retval.unitsScaleForc= unitsScaleForc
        retval.unitsScaleMom= unitsScaleMom
        retval.unitsScaleDispl= unitsScaleDisp
        retval.unitsDispl= unitsDispl    
        return retval
    
    def writePythonScript(self, prefix, os= sys.stdout):
        '''Write a Python script that can be used to re-create this object.

        :param prefix: string defining the container where the combination
                       will be added.
        :param os: output stream.
        '''
        outputStr= prefix+'.add("'+str(self.name)+'","'+str(self.expr)+'")\n'
        os.write(outputStr)

    def splitExpr(self, actions):
        ''' Split the expression in two parts: the first one containing the
            given actions and the second one with the rest of the load
            combination.

        :param actions: names of the actions to be extracted to the first part.
        '''
        part1, part2= utils.splitCombination(self.expr, actions)
        return part1, part2


class SituationCombs(dict):
    '''Dictionary of combinations for a situation (frequent, rare, 
       persistent,...).

    :ivar description: short description of the situation.
    '''
    def __init__(self, desc: str):
        ''' Constructor.

        :param desc: short description of the situation.
        '''
        self.description= desc

    def __eq__(self, other):
        '''Overrides the default implementation'''
        retval= super(SituationCombs, self).__eq__(other)
        if(retval):
            retval= self.description == other.description
        return retval
            
    def add(self, name, expr):
        ''' Add a combination to the dictionary.

        :param name: combination name (i.e. "ELS01").
        :param expr: expresion of the load combination (i.e. 1.0*G+1.0*Q).
        '''
        self[name]= CombinationRecord(name,expr)
        
    def getNames(self):
        '''returns a list of the combination names.'''
        return self.keys()

    def getDict(self):
        ''' Return a dictionary with the object values.'''
        retval= dict()
        retval['description']= self.description
        comb_dict= dict()
        for key in self:
            comb= self[key]
            comb_dict[key]= comb.expr
        retval['combinations']= comb_dict
        return retval
 
    def setFromDict(self,dct):
        ''' Set the fields from the values of the dictionary argument.'''
        self.description= dct['description']
        comb_dict= dct['combinations']
        for key in comb_dict:
            self.add(key, comb_dict[key])        
           
    def find(self, loadCombName:str):
        ''' Return the load combination with the given name.

        :param loadCombName: name of the load combination to find.
        '''
        retval= None
        if(loadCombName in self):
            retval= self[loadCombName]
        return retval
    
    def renumber(self, prefix:str= '', start:int= 0):
        ''' Renumber the combinations of this container.

        :param prefix: prefix to use for the new combination name.
        :param start: first index to use.
        '''
        data= self.getDict()
        comb_dict= data['combinations']
        comb_dict_keys= list(comb_dict.keys())
        numberOfCombinations= len(comb_dict_keys)
        if(numberOfCombinations>0):
            numberOfDigits= int(math.log10(numberOfCombinations))+1
            self.clear() # remove previous values.
            self.description= data['description']
            for i, key in enumerate(comb_dict_keys):
                indexStr= str(i+start).rjust(numberOfDigits, '0')
                newKey= prefix+indexStr
                self.add(newKey, comb_dict[key])
            
    def setFromCSV(self, rows):
        ''' Set the fields from the values of the given rows.'''
        # self.description= dct['description']
        for r in rows:
            key= r[2]
            combExpr= r[3]
            self.add(key, combExpr)        
           
    def getNameExpressionPairs(self):
        ''' Return a list of (combinationName, combinationExpression) tuples.'''
        retval= list()
        for key in self:
            comb= self[key]
            retval.append(comb.getNameExpressionPair())
        return retval
    
    def getNeutralFormat(self, counter, typ, mapLoadCases):
        retval= dict()
        for key in self:
            comb= self[key]
            retval[comb.name]= nld.LoadComb(counter,comb.name,self.description,typ,nld.getComponentsFromStr(comb.expr,mapLoadCases))
            counter+=1
        return retval
      
    def dumpCombinations(self,xcCombHandler):
        '''Introduces the combinations into the XC combination handler.

        :param xcCombHandler: combination handler.
        '''
        for key in self:
            retval= self[key].createCombination(xcCombHandler)
            if(retval is None):
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                lmsg.error(className+'.'+methodName+': couln\'t create combination: \''+key+'\'')

    def getLaTeXCode(self, limitState= None, small= True):
        '''Creates LaTeX tables and put the combinations in them.

        :param outputFile: file to write into.
        :param limitState: description of the limit state to which 
                           this situation corresponds.
        :param small: if true, use small font.
        '''
        retval= str()
        if(len(self)>0):
            if(limitState):
                header= limitState+' '+self.description
            else:
                header= self.description
            retval+= ('\\begin{center}\n')
            if(small):
                retval+= ('\\begin{small}\n')
            retval+= ('\\begin{longtable}{|l|p{10cm}|}\n')
            retval+= ('\\hline\n')
            retval+= ('\\multicolumn{2}{|c|}{'+header+'}\\\\\n')
            retval+= ('\\hline\n')
            retval+= ('\\textbf{Notation} & \\textbf{Combination} \\\\\n')
            retval+= ('\\hline\n')
            retval+= ('\\endfirsthead\n')

            retval+= ('\\hline\n')
            retval+= ('\\multicolumn{2}{|c|}{'+header+'}\\\\\n')
            retval+= ('\\hline\n')
            retval+= ('\\textbf{Notation} & \\textbf{Combination} \\\\\n')
            retval+= ('\\hline\n')
            retval+= ('\\endhead\n')

            retval+= ('\\hline \\multicolumn{2}{|r|}{{../..}} \\\\ \\hline\n')
            retval+= ('\\endfoot\n')

            retval+= ('\\hline\n')
            retval+= ('\\endlastfoot\n')
            for key in sorted(self):
                retval+= self[key].getLaTeXCode()
            retval+= ('\hline\n')
            retval+= ('\end{longtable}\n')
            if(small):
                retval+= ('\\end{small}\n')
            retval+= ('\end{center}\n')
        return retval
            
    def exportToLatex(self, outputFile, limitState= None):
        '''Creates LaTeX tables and put the combinations in them.

        :param outputFile: file to write into.
        :param limitState: description of the limit state to which 
                           this situation corresponds.
        '''
        outputFile.write(self.getLaTeXCode(limitState= limitState))

    def getLoadCaseDispParameters(self,combName,setsToDispLoads,setsToDispDspRot,setsToDispIntForc):
        '''Returns a suitable LoadCaseDispParameters for the combination.

        :param combName: name of the combination to create the record for.
        :param setsToDispLoads: ordered list of sets of elements to display
                                loads.
        :param setsToDispDspRot: ordered list of sets of elements to display 
                                displacements. 
        :param setsToDispIntForc: ordered list of sets of elements to display 
                                 internal forces.
        '''
        comb= self[combName]
        return comb.getLoadCaseDispParameters(setsToDispLoads,setsToDispDspRot,setsToDispIntForc)
    
    def writePythonScript(self, prefix, os= sys.stdout):
        '''Write a Python script that can be used to re-create this object.

        :param prefix: string defining the container where the combination
                       will be added.
        :param os: output stream.
        '''
        if(len(self)>0): # if not empty.
            for key in sorted(self):
                self[key].writePythonScript(prefix= prefix, os= os)

    def writeJSON(self, outputFileName):
        ''' Write the load combinations in a JSON file.

        :param outputFileName: name for the output file.
        '''
        data= self.getDict()
        with open(outputFileName, 'w') as outfile:
            json.dump(data, outfile)           

class SituationsSet(object):
    '''Set of situations as used in limit states

    :ivar name:        name to identify the situation set
    :ivar situations:  set of situations
    '''
    def __init__(self, name: str):
        '''Constructor.

        :ivar name: name to identify the situation set.
        '''
        self.name= name
        self.situations= None
        
    def getDict(self):
        ''' Return a dictionary with the object values.'''
        return {'name':self.name}

    def setFromDict(self,dct):
        ''' Set the fields from the values of the dictionary argument.'''
        self.name= dct['name']
        
    def getNames(self):
        '''returns a list of the combination names.'''
        retval= list()
        for s in self.situations:
            retval.extend(s.getNames())
        return retval
      
    def getNameExpressionPairs(self):
        ''' Return a list of (combinationName, combinationExpression) tuples.'''
        retval= list()
        for s in self.situations:
            retval.extend(s.getNameExpressionPairs())
        return retval
    
    def dumpCombinations(self,xcCombHandler):
        '''Introduces the combinations into the XC combination handler.

        :param xcCombHandler: combination handler.
        '''
        for s in self.situations:
            s.dumpCombinations(xcCombHandler)

    def getLaTeXCode(self):
        '''Return the LaTeX code corresponding to the combinations in this
           container.'''
        retval= ''
        for s in self.situations:
            retval+= s.getLaTeXCode(limitState= self.name)
        return retval
            
    def exportToLatex(self, outputFile):
        '''Creates LaTeX tables and put the combinations in them.'''
        ouputFile.write(self.getLaTeXCode())
        
    def getLoadCaseDispParameters(self,combName,setsToDispLoads,setsToDispDspRot,setsToDispIntForc):
        '''Returns a suitable LoadCaseDispParameters for the combination.

        :param combName: name of the combination to create the record for.
        :param setsToDispLoads: ordered list of sets of elements to display loads.
        :param setsToDispDspRot: ordered list of sets of elements to display 
                                displacements. 
        :param setsToDispIntForc: ordered list of sets of elements to display 
                                 internal forces.
        '''
        for s in self.situations:
            if combName in s:
                comb= s[combName]
                return comb.getLoadCaseDispParameters(setsToDispLoads,setsToDispDspRot,setsToDispIntForc)
            
    def writeJSON(self, outputFileName):
        ''' Write the load combinations in a JSON file.

        :param outputFileName: name for the output file.
        '''
        data= self.getDict()
        with open(outputFileName, 'w') as outfile:
            json.dump(data, outfile)        

class SLSCombinations(SituationsSet):
    '''Combinations of actions for serviceability limit states

    :ivar rare:        combination for a rare design situation
    :ivar freq:        combination for a frequent design situation
    :ivar qp:          combination for a quasi-permanent design situation
    :ivar earthquake:  combination for a earthquake design situation
    '''
    def __init__(self):
        ''' Constructor.'''
        super(SLSCombinations,self).__init__("Serviceability limit states.")
        self.rare= SituationCombs('Rare situations.')
        self.freq= SituationCombs('Frequent situations.')
        self.qp= SituationCombs('Quasi-permanent situations.')
        self.earthquake= SituationCombs('Earthquake situations for SLS.')
        self.situations= [self.rare,self.freq,self.qp,self.earthquake]
        
    def getDict(self):
        ''' Return a dictionary with the object values.'''
        retval= super().getDict()
        retval['rare']= self.rare.getDict()
        retval['freq']= self.freq.getDict()
        retval['qp']= self.qp.getDict()
        retval['earthquake']= self.earthquake.getDict()
        return retval

    def setFromDict(self,dct):
        ''' Set the fields from the values of the dictionary argument.'''
        super().setFromDict(dct)
        self.rare.setFromDict(dct['rare'])
        self.freq.setFromDict(dct['freq'])
        self.qp.setFromDict(dct['qp'])
        self.earthquake.setFromDict(dct['earthquake'])

    def find(self, loadCombName:str):
        ''' Return the load combination with the given name.

        :param loadCombName: name of the load combination to find.
        '''
        retval= self.rare.find(loadCombName= loadCombName)
        if(retval is None):
            retval= self.freq.find(loadCombName= loadCombName)
        if(retval is None):
            retval= self.qp.find(loadCombName= loadCombName)
        if(retval is None):
            retval= self.earthquake.find(loadCombName= loadCombName)
        return retval
        
    def renumber(self, start:int= 0):
        ''' Renumber the combinations of this container.

        :param start: first index to use.
        '''
        self.rare.renumber(prefix= 'SLSR', start= start)
        self.freq.renumber(prefix= 'SLSF', start= start)
        self.qp.renumber(prefix= 'SLSQP', start= start)
        self.earthquake.renumber(prefix= 'SLSEQ', start= start)
        
    def setFromCSV(self, rows):
        ''' Set the fields from the values of the given rows.'''
        classifiedRows= dict()
        for r in rows:
            r1= r[1]
            if(r1 not in classifiedRows):
                classifiedRows[r1]= list()
            classifiedRows[r1].append(r)
        if('rare' in classifiedRows):
            self.rare.setFromCSV(classifiedRows['rare'])
        if('freq' in classifiedRows):
            self.freq.setFromCSV(classifiedRows['freq'])
        if('qp' in classifiedRows):
            self.qp.setFromCSV(classifiedRows['qp'])
        if('earthquake' in classifiedRows):
            self.earthquake.setFromCSV(classifiedRows['earthquake'])
            
    def getNeutralFormat(self, counter, mapLoadCases):
        retval= self.rare.getNeutralFormat(counter,'SLSR', mapLoadCases)
        retval.update(self.freq.getNeutralFormat(counter+len(retval),'SLSF', mapLoadCases))
        retval.update(self.qp.getNeutralFormat(counter+len(retval),'SLSQP', mapLoadCases))
        retval.update(self.earthquake.getNeutralFormat(counter+len(retval),'SLSS', mapLoadCases))
        return retval
    
    def writePythonScript(self, prefix, os= sys.stdout):
        '''Write a Python script that can be used to re-create this object.

        :param prefix: string defining the container where the combination
                       will be added.
        :param os: output stream.
        '''
        newPrefix= prefix+'.rare'
        self.rare.writePythonScript(prefix= newPrefix, os= os)
        newPrefix= prefix+'.freq'
        self.freq.writePythonScript(prefix= newPrefix, os= os)
        newPrefix= prefix+'.qp'
        self.qp.writePythonScript(prefix= newPrefix, os= os)
        newPrefix= prefix+'.earthquake'
        self.earthquake.writePythonScript(prefix= newPrefix, os= os)

class ULSCombinations(SituationsSet):
    '''Combinations of actions for ultimate limit states

    :ivar name:        name to identify the limit state (ultimate,...)
    :ivar perm:        combination for a persistent or transient design situation
    :ivar acc:         combination for a accidental design situation
    :ivar fatigue:     combination for a fatigue design situation
    :ivar earthquake:  combination for a seismic design situation
    '''
    def __init__(self):
        ''' Constructor.'''
        super(ULSCombinations,self).__init__("Ultimate limit states.")
        self.perm= SituationCombs('Persistent or transient situations.')
        self.acc= SituationCombs('Exceptional (accidental) situations.')
        self.fatigue= SituationCombs('Fatigue situations.')
        self.earthquake= SituationCombs('Earthquake situations for ULS.')
        self.situations= [self.perm,self.acc,self.fatigue,self.earthquake]
        
    def getDict(self):
        ''' Return a dictionary with the object values.'''
        retval= super().getDict()
        retval['perm']= self.perm.getDict()
        retval['acc']= self.acc.getDict()
        retval['fatigue']= self.fatigue.getDict()
        retval['earthquake']= self.earthquake.getDict()
        return retval

    def setFromDict(self,dct):
        ''' Set the fields from the values of the dictionary argument.'''
        super().setFromDict(dct)
        self.perm.setFromDict(dct['perm'])
        self.acc.setFromDict(dct['acc'])
        self.fatigue.setFromDict(dct['fatigue'])
        self.earthquake.setFromDict(dct['earthquake'])

    def find(self, loadCombName:str):
        ''' Return the load combination with the given name.

        :param loadCombName: name of the load combination to find.
        '''
        retval= self.perm.find(loadCombName= loadCombName)
        if(retval is None):
            retval= self.acc.find(loadCombName= loadCombName)
        if(retval is None):
            retval= self.fatigue.find(loadCombName= loadCombName)
        if(retval is None):
            retval= self.earthquake.find(loadCombName= loadCombName)
        return retval
            
    def renumber(self, start:int= 0):
        ''' Renumber the combinations of this container.

        :param start: first index to use.
        '''
        self.perm.renumber(prefix= 'ULS', start= start)
        self.acc.renumber(prefix= 'ULSA', start= start)
        self.fatigue.renumber(prefix= 'ULSF', start= start)
        self.earthquake.renumber(prefix= 'ULSEQ', start= start)
        
    def setFromCSV(self, rows):
        ''' Set the fields from the values of the given rows.'''
        classifiedRows= dict()
        for r in rows:
            r1= r[1]
            if(r1 not in classifiedRows):
                classifiedRows[r1]= list()
            classifiedRows[r1].append(r)
        if('perm' in classifiedRows):
            self.perm.setFromCSV(classifiedRows['perm'])
        if('acc' in classifiedRows):
            self.acc.setFromCSV(classifiedRows['acc'])
        if('fatigue' in classifiedRows):
            self.fatigue.setFromCSV(classifiedRows['fatigue'])
        if('earthquake' in classifiedRows):
            self.earthquake.setFromCSV(classifiedRows['earthquake'])
        
    def getNeutralFormat(self, counter, mapLoadCases):
        retval= self.perm.getNeutralFormat(counter,'ULST2', mapLoadCases)
        retval.update(self.acc.getNeutralFormat(counter+len(retval),'ULST2A', mapLoadCases))
        retval.update(self.fatigue.getNeutralFormat(counter+len(retval),'ULSF', mapLoadCases))
        retval.update(self.earthquake.getNeutralFormat(counter+len(retval),'ULSS', mapLoadCases))
        return retval
    
    def writePythonScript(self, prefix, os= sys.stdout):
        '''Write a Python script that can be used to re-create this object.

        :param prefix: string defining the container where the combination
                       will be added.
        :param os: output stream.
        '''
        newPrefix= prefix+'.perm'
        self.perm.writePythonScript(prefix= newPrefix, os= os)
        newPrefix= prefix+'.acc'
        self.acc.writePythonScript(prefix= newPrefix, os= os)
        newPrefix= prefix+'.fatigue'
        self.fatigue.writePythonScript(prefix= newPrefix, os= os)
        newPrefix= prefix+'.earthquake'
        self.earthquake.writePythonScript(prefix= newPrefix, os= os)

class CombContainer(object):
    '''Container of load combinations.

    :ivar SLS: serviceability limit state combinations.
    :ivar ULS: ultimate limit state combinations.
    '''
    def __init__(self):
        ''' Constructor.'''
        self.SLS= SLSCombinations()
        self.ULS= ULSCombinations()
        self.limitStates= [self.SLS, self.ULS]
        
    def getDict(self):
        ''' Return a dictionary with the object values.'''
        retval= dict()
        retval['SLS']= self.SLS.getDict()
        retval['ULS']= self.ULS.getDict()
        return retval

    def setFromDict(self,dct):
        ''' Set the fields from the values of the given dictionary.'''
        self.SLS.setFromDict(dct['SLS'])
        self.ULS.setFromDict(dct['ULS'])

    def find(self, loadCombName:str):
        ''' Return the load combination with the given name.

        :param loadCombName: name of the load combination to find.
        '''
        retval= self.SLS.find(loadCombName= loadCombName)
        if(retval is None):
            retval= self.ULS.find(loadCombName= loadCombName)
        return retval
    
    def renumber(self, start:int= 0):
        ''' Renumber the combinations of this container.

        :param start: first index to use.
        '''
        self.SLS.renumber(start= start)
        self.ULS.renumber(start= start)
        
    def setFromCSV(self, rows):
        ''' Set the fields from the values of the given rows.'''
        slsRows= list()
        ulsRows= list()
        for r in rows:
            r0= r[0]
            if(r0=='ULS'):
                ulsRows.append(r)
            elif(r0=='SLS'):
                slsRows.append(r)
            else:
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                lmsg.error(className+'.'+methodName+': limit state: \''+str(r0)+'\' unknown.')
                exit(1)
        self.ULS.setFromCSV(ulsRows)
        self.SLS.setFromCSV(slsRows)
        
    def getNames(self):
        '''returns a list of the combination names.'''
        retval= list()
        for ls in self.limitStates:
            retval.extend(ls.getNames())
        return retval

    def getNameExpressionPairs(self):
        ''' Return a list of (combinationName, combinationExpression) tuples.'''
        retval= list()
        for ls in self.limitStates:
            retval.extend(ls.getNameExpressionPairs())
        return retval
     
    def getNeutralFormat(self, mapLoadCases):
        counter= 1
        retval= self.SLS.getNeutralFormat(counter, mapLoadCases)
        retval.update(self.ULS.getNeutralFormat(counter+len(retval), mapLoadCases))
        return retval
     
    def dumpCombinations(self, preprocessor):
        '''Introduces the combinations into the XC combination handler.

        :param preprocessor: pre-processor for the finite element problem.
        '''
        xcCombHandler= preprocessor.getLoadHandler.getLoadCombinations
        for ls in self.limitStates:
            ls.dumpCombinations(xcCombHandler)

    def getLaTeXCode(self):
        ''' Return the LaTeX code correspoding to the combinations in this
            container.'''
        retval= ''
        for ls in self.limitStates:
            retval+= ls.getLaTeXCode()
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

    def getList(self):
        ''' Return a list populated with the combinations.'''
        retval= list()
        combDict= self.getDict()
        for lsKey in combDict: # limit state.
            lsCombinations= combDict[lsKey]
            for psKey in lsCombinations: # project situation.
                psCombinations= lsCombinations[psKey]
                if(psKey!='name'):
                    description= psCombinations['description']
                    combinations= psCombinations['combinations']
                    for combKey in combinations: # combination.
                        combExpr= combinations[combKey]
                        retval.append([lsKey, psKey, combKey, combExpr])
        return retval
    
    def exportToCSV(self, os= sys.stdout):
        '''Write the load combinations as comma separated values.

        :param os: output stream.
        '''
        combList= self.getList()
        writer= csv.writer(os, quoting= csv.QUOTE_NONNUMERIC)
        for row in combList:
            writer.writerow(row)

    def writePythonScript(self, containerName= 'combContainer', os= sys.stdout):
        '''Write a Python script that can be used to re-create this object.

        :param prefix: string defining the container where the combination
                       will be added.
        :param os: output stream.
        '''
        newPrefix= containerName+'.SLS'
        self.SLS.writePythonScript(prefix= newPrefix, os= os)
        newPrefix= containerName+'.ULS'
        self.ULS.writePythonScript(prefix= newPrefix, os= os)

    def writeJSON(self, outputFileName):
        ''' Write the load combinations in a JSON file.

        :param outputFileName: name for the output file.
        '''
        data= self.getDict()
        with open(outputFileName, 'w') as outfile:
            json.dump(data, outfile)

    def readFromJSON(self, inputFileName):
        ''' Read the load combinations from a JSON file.

        :param inputFileName: name of the input file.
        '''
        with open(inputFileName, 'r') as f:
            combsDict= json.load(f)
        self.setFromDict(combsDict)

    def readFromCSV(self, inputFileName):
        ''' Read the load combinations from a CSV file.

        :param inputFileName: name of the input file.
        '''
        rows= list()
        with open(inputFileName, 'r') as f:
            reader= csv.reader(f)
            for row in reader:
                rows.append(row)
        self.setFromCSV(rows)

                  
    def getLoadCaseDispParameters(self,combName,setsToDispLoads,setsToDispDspRot,setsToDispIntForc):
        '''Returns a suitable LoadCaseDispParameters for the combination.

        :param combName: name of the combination to create the record for.
        :param setsToDispLoads: ordered list of sets of elements to display loads.
        :param setsToDispDspRot: ordered list of sets of elements to display 
                                displacements. 
        :param setsToDispIntForc: ordered list of sets of elements to display 
                                 internal forces.
        '''
        retval= self.SLS.getLoadCaseDispParameters(combName,setsToDispLoads,setsToDispDspRot,setsToDispIntForc)
        if not retval:
            retval= self.ULS.getLoadCaseDispParameters(combName,setsToDispLoads,setsToDispDspRot,setsToDispIntForc)
        return retval

    def getCorrespondingLoadCombinations(self, designSituations):
        '''Return the load combinations needed for the design situation
           argument.

        :param designSituations: design situations that will be checked; 
                                 i. e. uls_permanent, sls_quasi-permanent,
                                 sls_frequent, sls_rare, uls_earthquake, etc. 
        '''
        retval= list()
        if('uls_permanent' in designSituations):
            retval.append(self.ULS.perm)
        if('uls_fatigue' in designSituations):
            retval.append(self.ULS.fatigue)
        if('uls_accidental' in designSituations):
            retval.append(self.ULS.acc)
        if('uls_earthquake' in designSituations):
            retval.append(self.ULS.earthquake)
        if('sls_quasi-permanent' in designSituations):
            retval.append(self.SLS.qp)
        if('sls_frequent' in designSituations):
            retval.append(self.SLS.freq)
        if('sls_rare' in designSituations):
            retval.append(self.SLS.rare)
        if('sls_earthquake' in designSituations):
            retval.append(self.SLS.earthquake)
        if(len(retval)==0):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.warning(className+'.'+methodName+"; design situations: '"+str(designSituations)+"' have no defined combinations.")
        return retval
