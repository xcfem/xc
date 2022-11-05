# -*- coding: utf-8 -*-
from __future__ import division

'''Roadway traffic load model definition according to Eurocode 1 part 2.'''

__author__= "Luis C. Pérez Tato (LP) "
__copyright__= "Copyright 2015, LP"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import geom
from actions.roadway_traffic import load_model_base as lmb

tandem300LM1= lmb.TandemLoad(axleLoad= 300e3)
tandem200LM1= lmb.TandemLoad(axleLoad= 200e3)
tandem100LM1= lmb.TandemLoad(axleLoad= 100e3)

# pos: (transversal,longitudinal)
EC1_notional_lane1=lmb.LoadModel(wLoads= tandem300LM1.getWheelLoads())
EC1_notional_lane2=lmb.LoadModel(wLoads= tandem200LM1.getWheelLoads())
EC1_notional_lane3=lmb.LoadModel(wLoads= tandem100LM1.getWheelLoads())

#Carro EC1 concomitante con frenado (0.75*Q)
EC1_notional_lane1_brake=lmb.LoadModel(wLoads=tandem300LM1.getWheelLoads(loadFactor= 0.75))
EC1_notional_lane2_brake=lmb.LoadModel(wLoads=tandem200LM1.getWheelLoads(loadFactor= 0.75))

EC1_notional_lane3_brake=lmb.LoadModel(wLoads=tandem100LM1.getWheelLoads(loadFactor= 0.75))

def getAdditionalAmplificationFactor(D:float):
    ''' Return the additional amplification factor according to the expression
        4.7 of the clause 4.6.1 (6) of Eurocode 1 part 2.

    :param D: distance (m) of the cross-section under consideration from the 
              expansion joint.
    '''
    return max(1.3*(1-D/26.0),1)

def getNotionalLanesWidths(carriagewayWidth:float, laneWidth= 3.0):
    ''' Return the widths of the notional lanes according to table 4.1 of 
        EC1-2:2003.

    :param carriagewayWidth: width (m) of the carriageway (measured between 
                             kerbs or between the inner limits of vehicle
                             restraint systems).
    :param laneWidth: width of the notional lanes.
    '''
    if(carriagewayWidth<laneWidth):
        retval= [laneWidth]
    elif(carriagewayWidth<5.4):
        retval= [laneWidth, carriagewayWidth-laneWidth]
    elif(carriagewayWidth<6.0):
        w= laneWidth/2.0
        retval= [w, w]
    else:
        n1= int(carriagewayWidth/laneWidth)
        retval= n1*[laneWidth]
        retval.append(carriagewayWidth-n1*laneWidth)
    return retval

def getCarriagewayWidth(firstBorder, lastBorder):
    ''' Compute the width of the carriageway from its borders.

    :param  firstBorder: 2D segment (geom.Segment2d) representing the border of
                        the carriageway.
    :param lastBorder: 2D segment (geom.Segment2d) representing the border of
                       the carriageway.
    '''
    firstBorderMidPoint= firstBorder.getCenterOfMass()
    lastBorderMidPoint= lastBorder.getCenterOfMass()
    midLine= geom.Segment3d(firstBorderMidPoint, lastBorderMidPoint)
    return midLine.getLength()

def getNotionalLanesContours(firstBorder, lastBorder, laneWidth= 3.0, reverse= False):
    ''' Return the contours of the notional lanes according to clause 4.2.3
        of EC1-2:2003.

    :param firstBorder: 2D segment (geom.Segment2d) representing the border of
                        the carriageway.
    :param lastBorder: 2D segment (geom.Segment2d) representing the border of
                       the carriageway.
    '''
    carriagewayWidth= getCarriagewayWidth(firstBorder, lastBorder)
    notionalLanesWidths= getNotionalLanesWidths(carriagewayWidth)
    fractions= list()
    for w in notionalLanesWidths:
        fractions.append(w/carriagewayWidth)

    if(reverse):
        fractions.reverse()

    p1= firstBorder.getFromPoint()
    p2= firstBorder.getToPoint()
    p3= lastBorder.getFromPoint()
    p4= lastBorder.getToPoint()
    startingLine= geom.Segment3d(p1, p3)
    startingLinePoints= startingLine.Divide(fractions)
    finishLine= geom.Segment3d(p2, p4)
    finishLinePoints= finishLine.Divide(fractions)

    retval= list()
    startingPoint= startingLinePoints[0]
    finishPoint= finishLinePoints[0]
    for sp, fp in zip(startingLinePoints[1:],finishLinePoints[1:]):
        contour= [startingPoint, sp, fp, finishPoint]
        retval.append(geom.Polygon3d(contour))
        startingPoint= sp
        finishPoint= fp
    return retval
