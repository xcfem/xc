# -*- coding: utf-8 -*-
''' Export displacement values. '''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2016,LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# from misc_utils import log_messages as lmsg

def getModesDict(nmbComb, xcSet):
    '''Creates a dictionary with the nodes modes.

    :param nmbComb: combination name.
    :param xcSet: set containing the nodes to export the modes on.
    '''           
    combModesDict= dict()
    outDict= dict()
    combModesDict[nmbComb]= outDict
    preprocessor= xcSet.getPreprocessor
    domain= preprocessor.getDomain
    eigenvalues= domain.getEigenvalues()
    for mode, eigenvalue in enumerate(eigenvalues):
        norm= preprocessor.getDomain.getMesh.normalizeEigenvectors(mode+1)
        if __debug__:
            if(not norm):
                AssertionError('Can\'t normalize eigenvectors.')
        modeDict= dict()
        outDict[mode]= modeDict
        modeDict['eigenvalue']= eigenvalue
        eigenvectorDict= dict()
        modeDict['eigenvectors']= eigenvectorDict
        for n in xcSet.nodes:
            nodeDict= dict()
            eigenvectorDict[n.tag]= nodeDict
            disp3d= n.getEigenvectorDisp3dComponents(mode)
            nodeDict['disp3d']= [disp3d.x, disp3d.y, disp3d.z]
            rot3d= n.getEigenvectorRot3dComponents(mode)
            nodeDict['rot3d']= [rot3d.x, rot3d.y, rot3d.z]
    return combModesDict
