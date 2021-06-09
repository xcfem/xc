# -*- coding: utf-8 -*-

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2016,LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
from materials.sections import internal_forces
from misc_utils import log_messages as lmsg
from postprocess import output_styles
import csv

def getReactionsDict(combName, restrainedNodes):
    '''Creates a dictionary with the nodes's reactions.

    :param combName: combination name.
    :param restrainedNodes: restrained node set.
    '''
    combReactionsDict= dict()
    outDict= dict()
    combReactionsDict[combName]= outDict
    if(restrainedNodes):
        for n in restrainedNodes:
            nodeDict= dict()
            outDict[n.tag]= nodeDict
            if(n.hasProp('restrainedNodeId')):
                nodeDict['restrainedNodeId']= n.getProp('restrainedNodeId')
            nDOF= n.getNumberDOF
            nodeDict['nDOF']= nDOF
            nPosition= n.getInitialPos3d
            nodeDict['position']= {'x':nPosition.x,'y':nPosition.y,'z':nPosition.z}
            nReactionForce= n.getReactionForce3d
            nReactionMoment= n.getReactionMoment3d
            nodeDict['reactions']= {'Fx':nReactionForce.x,'Fy':nReactionForce.y,'Fz':nReactionForce.z,'Mx':nReactionMoment.x,'My':nReactionMoment.y,'Mz':nReactionMoment.z}
    return combReactionsDict

def exportReactions(reactionsDict, fName, outputStyle= output_styles.defaultOutputStyle):
    '''Writes a comma separated values file with the nodes reactions.

    :param reactionsDict: dictionary populated with the values of the reactions.
    :param fName: name of the output file.
    :param outputStyle
    '''
    unitConversionFactor= outputStyle.getForceUnitsScaleFactor()
    lengthUnitsSymbol= '('+outputStyle.getLengthUnitSymbol()+')'
    forceUnitsSymbol= '('+outputStyle.getForceUnitSymbol()+')'
    momentUnitsSymbol= '('+outputStyle.getMomentUnitSymbol()+')'
    f= open(fName, 'w')
    writer= csv.writer(f)
    writer.writerow(['comb', 'node', 'ID', 'x', 'y', 'z', 'Fx', 'Fy', 'Fz', 'Mx', 'My', 'Mz'])
    writer.writerow(['', '', '', lengthUnitsSymbol, lengthUnitsSymbol, lengthUnitsSymbol, forceUnitsSymbol, forceUnitsSymbol, forceUnitsSymbol, momentUnitsSymbol, momentUnitsSymbol, momentUnitsSymbol])
    for combName in reactionsDict:
        combReactions= reactionsDict[combName]
        for nTag in combReactions:
            nodeReactions= combReactions[nTag]
            restrainedNodeId= nodeReactions['restrainedNodeId']
            pos= nodeReactions['position']
            reactions= nodeReactions['reactions']
            Fx= float(reactions['Fx'])*unitConversionFactor
            Fy= float(reactions['Fy'])*unitConversionFactor
            Fz= float(reactions['Fz'])*unitConversionFactor
            Mx= float(reactions['Mx'])*unitConversionFactor
            My= float(reactions['My'])*unitConversionFactor
            Mz= float(reactions['Mz'])*unitConversionFactor
            writer.writerow([combName, nTag, restrainedNodeId, pos['x'], pos['y'], pos['z'], Fx, Fy, Fz, Mx, My, Mz])

    f.close()
            
           

