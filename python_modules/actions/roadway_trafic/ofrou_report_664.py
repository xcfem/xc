# -*- coding: utf-8 -*-
from __future__ import division

'''Roadway traffic load models from OFROU (Switzerland federal highway
   authority) report 664:
   "Charges de trafic actualisées pour les dalles de roulement en béton
   des ponts existants".'''

__author__= "Luis C. Pérez Tato (LCPT) Ana Ortega (A_OO)"
__copyright__= "Copyright 2018, A_OO LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.Ortega.Ort@gmail.com"

import xc_base
import geom
from actions.roadway_trafic import load_model_base as lmb

CraneTruckLoadModel= lmb.LoadModel(wLoads= [lmb.WheelLoad(geom.Pos2d(-3.17,1.0),216e3/2.0),
                                        lmb.WheelLoad(geom.Pos2d(-1.57,1.0),216e3/2.0),
                                        lmb.WheelLoad(geom.Pos2d(0.03,1.0),198e3/2.0),
                                        lmb.WheelLoad(geom.Pos2d(1.63,1.0),180e3/2.0),
                                        lmb.WheelLoad(geom.Pos2d(4.03,1.0),180e3/2.0),
                                        lmb.WheelLoad(geom.Pos2d(-3.17,-1.0),216e3/2.0),
                                        lmb.WheelLoad(geom.Pos2d(-1.57,-1.0),216e3/2.0),
                                        lmb.WheelLoad(geom.Pos2d(0.03,-1.0),198e3/2.0),
                                        lmb.WheelLoad(geom.Pos2d(1.63,-1.0),180e3/2.0),
                                        lmb.WheelLoad(geom.Pos2d(4.03,-1.0),180e3/2.0)])

Det11TruckLoadModel= lmb.LoadModel(wLoads= [lmb.WheelLoad(geom.Pos2d(0.0,1.0),117e3/2.0),
                                        lmb.WheelLoad(geom.Pos2d(1.80,1.0),117e3/2.0),
                                        lmb.WheelLoad(geom.Pos2d(1.80+1.60,1.0),156e3/2.0),
                                        lmb.WheelLoad(geom.Pos2d(1.80+1.60+1.30,1.0),195e3/2.0),
                                        lmb.WheelLoad(geom.Pos2d(1.80+1.60+1.30+1.30,1.0),195e3/2.0),
                                        lmb.WheelLoad(geom.Pos2d(0.0,-1.0),117e3/2.0),
                                        lmb.WheelLoad(geom.Pos2d(1.80,-1.0),117e3/2.0),
                                        lmb.WheelLoad(geom.Pos2d(1.80+1.60,-1.0),156e3/2.0),
                                        lmb.WheelLoad(geom.Pos2d(1.80+1.60+1.30,-1.0),195e3/2.0),
                                        lmb.WheelLoad(geom.Pos2d(1.80+1.60+1.30+1.30,-1.0),195e3/2.0)])
Det12TruckLoadModel= lmb.LoadModel(wLoads= [lmb.WheelLoad(geom.Pos2d(0.0,1.0),81e3/2.0),
                                        lmb.WheelLoad(geom.Pos2d(1.80,1.0),81e3/2.0),
                                        lmb.WheelLoad(geom.Pos2d(1.80+1.60,1.0),108e3/2.0),
                                        lmb.WheelLoad(geom.Pos2d(1.80+1.60+1.30,1.0),135e3/2.0),
                                        lmb.WheelLoad(geom.Pos2d(1.80+1.60+1.30+1.30,1.0),135e3/2.0),
                                        lmb.WheelLoad(geom.Pos2d(0.0,-1.0),81e3/2.0),
                                        lmb.WheelLoad(geom.Pos2d(1.80,-1.0),81e3/2.0),
                                        lmb.WheelLoad(geom.Pos2d(1.80+1.60,-1.0),108e3/2.0),
                                        lmb.WheelLoad(geom.Pos2d(1.80+1.60+1.30,-1.0),135e3/2.0),
                                        lmb.WheelLoad(geom.Pos2d(1.80+1.60+1.30+1.30,-1.0),135e3/2.0)])

Det2front= 0.75+1.50
Det2points= [geom.Pos2d(-Det2front,-1.5),geom.Pos2d(10.0-Det2front,-1.5),geom.Pos2d(10.0-Det2front,1.5),geom.Pos2d(-Det2front,1.5)]
Det21TruckLoadModel= lmb.LoadModel(wLoads= [lmb.WheelLoad(geom.Pos2d(0.0,1.0),90e3/2.0),
                                        lmb.WheelLoad(geom.Pos2d(1.80,1.0),90e3/2.0),
                                        lmb.WheelLoad(geom.Pos2d(1.80+1.60,1.0),120e3/2.0),
                                        lmb.WheelLoad(geom.Pos2d(1.80+1.60+1.30,1.0),150e3/2.0),
                                        lmb.WheelLoad(geom.Pos2d(1.80+1.60+1.30+1.30,1.0),150e3/2.0),
                                        lmb.WheelLoad(geom.Pos2d(0.0,-1.0),90e3/2.0),
                                        lmb.WheelLoad(geom.Pos2d(1.80,-1.0),90e3/2.0),
                                        lmb.WheelLoad(geom.Pos2d(1.80+1.60,-1.0),120e3/2.0),
                                        lmb.WheelLoad(geom.Pos2d(1.80+1.60+1.30,-1.0),150e3/2.0),
                                        lmb.WheelLoad(geom.Pos2d(1.80+1.60+1.30+1.30,-1.0),150e3/2.0)], vBoundary= Det2points)

Det22TruckLoadModel= lmb.LoadModel(wLoads= [lmb.WheelLoad(geom.Pos2d(0.0,1.0),67.5e3/2.0),
                                        lmb.WheelLoad(geom.Pos2d(1.80,1.0),67.5e3/2.0),
                                        lmb.WheelLoad(geom.Pos2d(1.80+1.60,1.0),90e3/2.0),
                                        lmb.WheelLoad(geom.Pos2d(1.80+1.60+1.30,1.0),112.5e3/2.0),
                                        lmb.WheelLoad(geom.Pos2d(1.80+1.60+1.30+1.30,1.0),112.5e3/2.0),
                                        lmb.WheelLoad(geom.Pos2d(0.0,-1.0),67.5e3/2.0),
                                        lmb.WheelLoad(geom.Pos2d(1.80,-1.0),67.5e3/2.0),
                                        lmb.WheelLoad(geom.Pos2d(1.80+1.60,-1.0),90e3/2.0),
                                        lmb.WheelLoad(geom.Pos2d(1.80+1.60+1.30,-1.0),112.5e3/2.0),
                                        lmb.WheelLoad(geom.Pos2d(1.80+1.60+1.30+1.30,-1.0),112.5e3/2.0)], vBoundary= Det2points)
