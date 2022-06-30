# -*- coding: utf-8 -*-
''' Functions that are useful when dealing with load combinations.'''

from __future__ import print_function

import re
import math
import loadCombinations

def getCombinationDict(loadCombination:str):
    ''' Return a Python dictionary whose keys are the names of the actions
        in the combination and whose values are the factor that multiply the
        action.

    :param loadCombination: string of the form "1.00*G1 + 1.00*G2 + 1.35*Qwind"
    '''
    loads= loadCombination.split('+')
    retval= dict()
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
    '''List the defined actions in a family.'''
    print('    partial safety factors: ', actionFamily.partial_safety_factors)
    actions= actionFamily.actions
    for a in actions:
        print('     ', a)

def listActionWeighting(actionWeighting):
    '''List the defined actions and the weighting for each one.'''
    for awKey in actionWeighting.getKeys():
        print(awKey)
        aw= actionWeighting[awKey]
        permanentActions= aw.permanentActions
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
  

def writeLoadCombinations(outputFileName, prefix, loadCombs):
    f= open(outputFileName,'w')
    f.write("combs= loadLoader.getLoadCombinations\n")
    conta= 1
    sz= len(loadCombs)
    szLength= int(math.log(sz,10))+1
    for comb in loadCombs:
        cod= prefix+str(conta).zfill(szLength)
        output= 'comb= combs.newLoadCombination('
        output+= '"'+cod+'","'+comb.name+'")\n'
        f.write(output)
        conta+= 1
