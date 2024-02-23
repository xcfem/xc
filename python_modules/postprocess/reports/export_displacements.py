# -*- coding: utf-8 -*-
''' Export displacement values. '''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2016,LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from misc_utils import log_messages as lmsg

def getDisplacementsDict(nmbComb, nodes, structuralModel= True):
    '''Creates a dictionary with the nodes displacements.

    :param nmbComb: combination name.
    :param nodes: node set.
    :param structuralModel: true if structural model, false if solid mechanics model.
    '''           
    combDisplacementsDict= dict()
    outDict= dict()
    combDisplacementsDict[nmbComb]= outDict
    for n in nodes:
        nodeDict= dict()
        outDict[n.tag]= nodeDict
        disp= n.getDisp
        nDOF= len(disp)
        if(nDOF==6):
            nodeDict['ux']= disp[0]
            nodeDict['uy']= disp[1]
            nodeDict['uz']= disp[2]
            nodeDict['rx']= disp[3]
            nodeDict['ry']= disp[4]
            nodeDict['rz']= disp[5]
        elif(nDOF==3): # 2D structural or 3D solid mechanics
            if(structuralModel):
                nodeDict['ux']= disp[0]
                nodeDict['uy']= disp[1]
                nodeDict['rz']= disp[2]
            else: # solid mechanics.
                nodeDict['ux']= disp[0]
                nodeDict['uy']= disp[1]
                nodeDict['uz']= disp[2]
        elif(nDOF==2):
            nodeDict['ux']= disp[0]
            nodeDict['uy']= disp[1]
        elif(nDOF==1):
            nodeDict['ux']= disp[0]
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; unknown number of degrees of freedom: '+str(nDOF))
    return combDisplacementsDict
