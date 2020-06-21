# -*- coding: utf-8 -*-
from __future__ import division

'''Vehicles to perform load tests in roadway traffic bridges according to different standards.
'''
import xc_base
import geom
from actions.roadway_trafic import load_model_base as lmb
from actions import load_cases as lcases

# Standard vehicles from «Recomendaciones para la realización de pruebas
# de carga de recepción de puentes de carretera.

truck38t_forward=lmb.LoadModel(wLoads=
[lmb.WheelLoad(pos=geom.Pos2d(-1.0,4.8),ld=35e3,lx=0.08,ly=0.08),
 lmb.WheelLoad(pos=geom.Pos2d(1.0,4.8),ld=35e3,lx=0.08,ly=0.08),
 lmb.WheelLoad(pos=geom.Pos2d(-1.0,1.3),ld=55e3,lx=0.08,ly=0.08),
 lmb.WheelLoad(pos=geom.Pos2d(1.0,1.3),ld=55e3,lx=0.08,ly=0.08),
 lmb.WheelLoad(pos=geom.Pos2d(-1.0,-1.7),ld=50e3,lx=0.08,ly=0.08),
 lmb.WheelLoad(pos=geom.Pos2d(1.0,-1.7),ld=50e3,lx=0.08,ly=0.08),
 lmb.WheelLoad(pos=geom.Pos2d(-1.0,-3.05),ld=50e3,lx=0.08,ly=0.08),
 lmb.WheelLoad(pos=geom.Pos2d(1.0,-3.05),ld=50e3,lx=0.08,ly=0.08)])

truck38t_backward=lmb.LoadModel(wLoads=
[lmb.WheelLoad(pos=geom.Pos2d(-1.0,-4.8),ld=35e3,lx=0.08,ly=0.08),
 lmb.WheelLoad(pos=geom.Pos2d(1.0,-4.8),ld=35e3,lx=0.08,ly=0.08),
 lmb.WheelLoad(pos=geom.Pos2d(-1.0,-1.3),ld=55e3,lx=0.08,ly=0.08),
 lmb.WheelLoad(pos=geom.Pos2d(1.0,-1.3),ld=55e3,lx=0.08,ly=0.08),
 lmb.WheelLoad(pos=geom.Pos2d(-1.0,1.7),ld=50e3,lx=0.08,ly=0.08),
 lmb.WheelLoad(pos=geom.Pos2d(1.0,1.7),ld=50e3,lx=0.08,ly=0.08),
 lmb.WheelLoad(pos=geom.Pos2d(-1.0,3.05),ld=50e3,lx=0.08,ly=0.08),
 lmb.WheelLoad(pos=geom.Pos2d(1.0,3.05),ld=50e3,lx=0.08,ly=0.08)])

truck26t_forward=lmb.LoadModel(wLoads=
[lmb.WheelLoad(pos=geom.Pos2d(-0.9,3.34),ld=35e3,lx=0.08,ly=0.08),
 lmb.WheelLoad(pos=geom.Pos2d(0.9,3.34),ld=35e3,lx=0.08,ly=0.08),
 lmb.WheelLoad(pos=geom.Pos2d(-0.9,-.56),ld=47.5e3,lx=0.08,ly=0.08),
 lmb.WheelLoad(pos=geom.Pos2d(0.9,-.56),ld=47.5e3,lx=0.08,ly=0.08),
 lmb.WheelLoad(pos=geom.Pos2d(-0.9,-1.91),ld=47.5e3,lx=0.08,ly=0.08),
 lmb.WheelLoad(pos=geom.Pos2d(0.9,-1.91),ld=47.5e3,lx=0.08,ly=0.08)])

truck26t_backward=lmb.LoadModel(wLoads=
[lmb.WheelLoad(pos=geom.Pos2d(-0.9,-3.34),ld=35e3,lx=0.08,ly=0.08),
 lmb.WheelLoad(pos=geom.Pos2d(0.9,-3.34),ld=35e3,lx=0.08,ly=0.08),
 lmb.WheelLoad(pos=geom.Pos2d(-0.9,.56),ld=47.5e3,lx=0.08,ly=0.08),
 lmb.WheelLoad(pos=geom.Pos2d(0.9,.56),ld=47.5e3,lx=0.08,ly=0.08),
 lmb.WheelLoad(pos=geom.Pos2d(-0.9,1.91),ld=47.5e3,lx=0.08,ly=0.08),
 lmb.WheelLoad(pos=geom.Pos2d(0.9,1.91),ld=47.5e3,lx=0.08,ly=0.08)])


def load_test_LC(lcName,deckSet,lstVehiclData,hDistrPL,slopeDistrPL=1.0):
    '''Return load case that reproduces one configuration of a load 
       in a bridge.

    :param lcName: load case name
    :param deckSet: deck set of elements (used to distribute point loads)
    :param lstVehiclData: list of data for each vehicle used in the load test.
           for each vehicle, data are its type (instance of the class LoadModel)
           and the coordinates (x,y)  where to place the centroid of the vehicle
           ex: lstVehiclData=[[truck38t_forward,(0,15)],
                              [truck26t_backward,(2,15), ...] 
    :param hDistrPL: height considered to distribute each point load with
                  slope slopeDistr 
    :param slopeDistrPL: slope (H/V) through hDistr to distribute the load of 
                  a wheel (defaults to 1)
                 
    '''
    preprocessor=deckSet.getPreprocessor
    lc=lcases.LoadCase(preprocessor,lcName,"default","constant_ts")
    lc.create()
    for vehicDat in lstVehiclData:
        vehicType=vehicDat[0]
        xCentr=vehicDat[1][0]
        yCentr=vehicDat[1][1]
        lc.addLstLoads([lmb.VehicleDistrLoad(name=lcName+'QpVL1',xcSet=deckSet,loadModel=vehicType, xCentr=xCentr,yCentr=yCentr,hDistr=hDistrPL,slopeDistr=slopeDistrPL)])
    return lc
    
    
