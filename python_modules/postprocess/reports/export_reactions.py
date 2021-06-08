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
            nDOF= n.getNumberDOF
            nodeDict['nDOF']= nDOF
            nPosition= n.getInitialPos3d
            nodeDict['position']= {'x':nPosition.x,'y':nPosition.y,'z':nPosition.z}
            nReactionForce= n.getReactionForce3d
            nReactionMoment= n.getReactionMoment3d
            nodeDict['reactions']= {'Fx':nReactionForce.x,'Fy':nReactionForce.y,'Fz':nReactionForce.z,'Mx':nReactionForce.x,'My':nReactionForce.y,'Mz':nReactionForce.z}
    return combReactionsDict

def exportReactions(reactionsDict, fName):
    '''Writes a comma separated values file with the nodes reactions.

    :param reactionsDict: dictionary populated with the values of the reactions.
    :param fName: name of the output file.
    '''
    f= open(ulsReactionsFileName, 'w')
    writer= csv.writer(f)
    writer.writerow(['comb', 'ID', 'x', 'y', 'z', 'Fx', 'Fy', 'Fz', 'Mx', 'My', 'Mz'])
    for combName in reactionsDict:
        combReactions= reactionsDict['combName']
        for nTag in combReactions:
            nodeReactions= combReactions[nTag]
            pos= nodeReactions['position']
            reactions= nodeReactions['reactions']
            writer.writerow([combName, nTag, pos['x'], pos['y'], pos['z'], reactions['Fx'], reactions['Fy'], reactions['Fz'], reactions['Mx'], reactions['My'], reactions['Mz']])

    f.close()
            
           

