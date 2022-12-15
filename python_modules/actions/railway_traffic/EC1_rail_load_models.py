# -*- coding: utf-8 -*-
from __future__ import division

'''Roadway traffic load model definition according to Eurocode 1 part 2.'''

__author__= "Luis C. Pérez Tato (LP) "
__copyright__= "Copyright 2015, LP"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import geom
from actions.railway_traffic import load_model_base as lmb

locomotiveLM1= lmb.LocomotiveLoad(nAxes= 4, axleLoad= 250e3, xSpacing= 1.6, ySpacing= 1.435)

class TrackAxes(lmb.TrackAxes):
    ''' Notional lanes for a road section according to clause 4.2.3 
    of EC1-2:2003.

    '''
    def __init__(self, trackAxesPolylines):
        ''' Constructor.

        :param railAxesPolylines: 3D polylines defining the axis of track.
        '''
        super().__init__(trackAxesPolylines)
