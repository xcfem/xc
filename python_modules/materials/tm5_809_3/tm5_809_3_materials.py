# -*- coding: utf-8 -*-
''' Masonry structural design according to TM 5-809-3.'''
from __future__ import division
from __future__ import print_function

import math
import collections
import bisect
import scipy.interpolate
from materials import concrete_base
from materials.aci import ACI_materials
from miscUtils import LogMessages as lmsg
from materials.sections.fiber_section import defSimpleRCSection

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

class CMUWall(object):
    """ Masonry wall 

    :ivar thickness: nominal wall thickness.
    :ivar groutedCellsSpacing: spacing of grouted cells.
    """    
    def __init__(self,thickness, spacing):
        '''
        Constructor.

        :param thickness: wall thickness.
        :param spacing: spacing of grouted cells.
        '''
        self.thickness= thickness
        self.groutedCellsSpacing= spacing
    
    def getEquivalentWallThickness(self):
        ''' Return the equivalent wall thickness according to
            table 5-2 of TM 5-809-3.'''
        x= [6,8,10,12] # nominal wall thickness (inches)
        y= [0.00,16.00,24.00,32.00,40.00,48.00,56.00,64.00] # spacing inches
        z= [[5.62, 7.62, 9.62, 11.62],
        [3.7, 4.9, 5.98, 7.04],
        [3.13, 4.1, 4.91, 5.7],
        [2.85, 3.7, 4.37, 5.02],
        [2.68, 3.46, 4.05, 4.62],
        [2.57, 3.3, 3.83, 4.35],
        [2.49, 3.19, 3.67, 4.16],
        [2.42, 3.1, 3.56, 4.01]]
        sp_inch= self.groutedCellsSpacing/0.0254
        th_inches= self.thickness/0.0254
        fEquivalentThickness= scipy.interpolate.interp2d(x,y,z)
        tol= 1e-3
        if(th_inches<=6.0):
            if(th_inches<6.0-1e3):
                lmsg.error('thickness: '+str(self.thickness)+' out of range (too thin).')
            else:
                th_inches= 6.0
        elif(th_inches>=12.0):
            if(th_inches>12.0+tol):
                lmsg.error('thickness: '+str(self.thickness)+' out of range (too thick).')
            else:
                th_inches= 12.0
        return fEquivalentThickness(th_inches,sp_inch)[0]*0.0254

