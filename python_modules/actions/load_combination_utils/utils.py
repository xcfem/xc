# -*- coding: utf-8 -*-
''' Functions that are useful when dealing with load combinations.'''

from __future__ import print_function

import math
import sys

def getCombinationDict(loadCombination:str):
    ''' Return a Python dictionary whose keys are the names of the actions
        in the combination and whose values are the factor that multiply the
        action.

    :param loadCombination: string of the form "1.00*G1 + 1.00*G2 + 1.35*Qwind"
    '''
    retval= dict()
    if(len(loadCombination)>0):
        loads= loadCombination.split('+')
        for l in loads:
            l= l.strip()
            factor, action= l.split('*')
            retval[action]= float(factor)
    return retval

def getCombinationExpr(combDict:dict):
    ''' Return the expression corresponding to the load combination argument
        in dictionary form (i. e.: {'G1':1.0, 'G2':1.00, 'Qwind':1.35}).

    :param combDict: combination expressed in the form of a dictionary.
    '''
    tmp= list()
    for key in combDict:
        factor= combDict[key]
        tmp.append(str(factor)+'*'+key)
    retval= ''
    if(len(tmp)>0):
        retval= tmp[0]
        for s in tmp[1:]:
            retval+='+'+s
    return retval
    
def splitCombination(loadCombination:str, loads):
    ''' Return the part of a combination that concerns the actions passed as
        argument and then the rest of the original combination.

    :param loads: names of the desired loads.
    '''
    combDict= getCombinationDict(loadCombination)
    tmp1= list(); tmp2= list()
    for key in combDict:
        factor= combDict[key]
        addend= str(factor)+'*'+key
        if(key in loads):
            tmp1.append(addend)
        else:
            tmp2.append(addend)
    retval= '+'.join(tmp1), '+'.join(tmp2)
    return retval

def getFileNameFromCombinationExpresion(loadCombination:str):
    ''' Return a valid filename from the combination expression.'''
    return ''.join(x for x in loadCombination if x.isalnum())

def listActionGroup(actionGroup):
    '''List the defined actions in a group (permanent, variable, accidental).'''
    for familyName in actionGroup.getKeys():
        print('    actions family: ', familyName)
        actionFamily= actionGroup[familyName]
        print('      partial safety factors: ', actionFamily.partial_safety_factors)
        for a in actionFamily.actions:
          print('     ', a)

def listActionFamily(actionFamily):
    '''List the defined actions in a family.

    :param actionFamily: family of actions. 
    '''
    print('    partial safety factors: ', actionFamily.partial_safety_factors)
    actions= actionFamily.actions
    for a in actions:
        print('     ', a)

def listActionWeighting(actionWeighting):
    '''List the defined actions and the weighting for each one.'''
    for awKey in actionWeighting.getKeys():
        print(awKey)
        aw= actionWeighting[awKey]
        print('  Permanent actions: ')
        listActionGroup(aw.permanentActions)
        print('  Non-constant permanent actions: ')
        listActionGroup(aw.ncPermanentActions)
        print('  Variable actions: ')
        listActionGroup(aw.variableActions)
        print('  Accidental actions: ')
        listActionFamily(aw.accidentalActions)
        print('  Seismic actions: ')
        listActionFamily(aw.seismicActions)

def getNamedCombinations(loadCombinations, prefix):
    '''Return a dictionary containing the load combinations in the argument
       with an arbitrary name as key. The name is formed by concatenation of
       a prefix (such as ULS, SLS or somethink like that) and a number.

    :param loadCombinations: load combinations to be named.
    :param prefix: prefix to form the name (such as ULS, SLS or somethink like that).
    '''
    sz= len(loadCombinations) # number of combinations to name.
    szLength= int(math.log(sz,10))+1 # number of leading zeros
    retval= dict()
    for count, comb in enumerate(loadCombinations):
        key= prefix+str(count).zfill(szLength)
        retval[key]= comb
    return retval

def writeXCLoadCombinations(prefix, loadCombinations, outputFileName= None):
    ''' Write the load combinations in a XC format.

    :param loadCombinations: load combinations to be named.
    :param prefix: prefix to form the name (such as ULS, SLS or somethink like that).
    :param outputFileName: name of the output file (if None use standard output).
    '''
    if(outputFileName is None):
        f= sys.stdout
    else:
        f= open(outputFileName,'w')
    f.write("combs= loadLoader.getLoadCombinations\n")
    # Assign a name to each combination.
    namedCombinations= getNamedCombinations(loadCombinations, prefix)
    for key in namedCombinations:
        comb= namedCombinations[key]
        output= 'comb= combs.newLoadCombination('
        output+= '"'+key+'","'+comb.name+'")\n'
        f.write(output)
    if(outputFileName is None):
        f.flush()
    else:
        f.close()
