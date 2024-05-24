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

def get_modes_dict(nmbComb, xcSet, eigenvectorNormThreshold= 1e-3):
    '''Creates a dictionary with the nodes modes.

    :param nmbComb: combination name.
    :param xcSet: set containing the nodes to export the modes on.
    :param eigenvectorNormThreshold: if the node eigenvector has a norm smaller
                                     than this threshold it is considered null.
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
        xcMode= mode+1
        modeDict= dict()
        outDict[mode]= modeDict
        modeDict['eigenvalue']= eigenvalue
        eigenvectorDict= dict()
        modeDict['eigenvectors']= eigenvectorDict
        for n in xcSet.nodes:
            nodeDict= dict()
            eigenvectorDict[n.tag]= nodeDict
            eigenvectorNorm= n.getEigenvector(xcMode).Norm()
            if(eigenvectorNorm>eigenvectorNormThreshold):
                disp3d= n.getEigenvectorDisp3dComponents(xcMode)
                disp3dList= [disp3d.x, disp3d.y, disp3d.z]
                rot3d= n.getEigenvectorRot3dComponents(xcMode)
                rot3dList= [rot3d.x, rot3d.y, rot3d.z]
            else:
                disp3dList= [0.0, 0.0, 0.0]
                rot3dList= [0.0, 0.0, 0.0]
            nodeDict['disp3d']= disp3dList
            nodeDict['rot3d']= rot3dList
    return combModesDict
