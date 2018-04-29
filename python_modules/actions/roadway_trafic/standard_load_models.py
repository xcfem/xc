# -*- coding: utf-8 -*-
from __future__ import division

'''Roadway traffic load model definition according to different standards.
'''
import xc_base
import geom
from actions.roadway_trafic import load_model_base as lmb

# pos: (transversal,longitudinal)
IAP_carril_virt1=lmb.LoadModel(wLoads=
[lmb.WheelLoad(pos=geom.Pos2d(1.0,-0.6),ld=150e3,lx=0.4,ly=0.4),
lmb.WheelLoad(pos=geom.Pos2d(-1.0,-0.6),ld=150e3,lx=0.4,ly=0.4),
lmb.WheelLoad(pos=geom.Pos2d(1.0,0.6),ld=150e3,lx=0.4,ly=0.4),
lmb.WheelLoad(pos=geom.Pos2d(-1.0,0.6),ld=150e3,lx=0.4,ly=0.4)])

IAP_carril_virt2=lmb.LoadModel(wLoads=
[lmb.WheelLoad(pos=geom.Pos2d(1.0,-0.6),ld=100e3,lx=0.4,ly=0.4),
lmb.WheelLoad(pos=geom.Pos2d(-1.0,-0.6),ld=100e3,lx=0.4,ly=0.4),
lmb.WheelLoad(pos=geom.Pos2d(1.0,0.6),ld=100e3,lx=0.4,ly=0.4),
lmb.WheelLoad(pos=geom.Pos2d(-1.0,0.6),ld=100e3,lx=0.4,ly=0.4)])

IAP_carril_virt3=lmb.LoadModel(wLoads=
[lmb.WheelLoad(pos=geom.Pos2d(1.0,-0.6),ld=50e3,lx=0.4,ly=0.4),
lmb.WheelLoad(pos=geom.Pos2d(-1.0,-0.6),ld=50e3,lx=0.4,ly=0.4),
lmb.WheelLoad(pos=geom.Pos2d(1.0,0.6),ld=50e3,lx=0.4,ly=0.4),
lmb.WheelLoad(pos=geom.Pos2d(-1.0,0.6),ld=50e3,lx=0.4,ly=0.4)])

#Carro IAP concomitante con frenado (0.75*Q)
IAP_carril_virt1_fren=lmb.LoadModel(wLoads=
[lmb.WheelLoad(pos=geom.Pos2d(1.0,-0.6),ld=112.5e3,lx=0.4,ly=0.4),
lmb.WheelLoad(pos=geom.Pos2d(-1.0,-0.6),ld=112.5e3,lx=0.4,ly=0.4),
lmb.WheelLoad(pos=geom.Pos2d(1.0,0.6),ld=112.5e3,lx=0.4,ly=0.4),
lmb.WheelLoad(pos=geom.Pos2d(-1.0,0.6),ld=112.5e3,lx=0.4,ly=0.4)])

IAP_carril_virt2_fren=lmb.LoadModel(wLoads=
[lmb.WheelLoad(pos=geom.Pos2d(1.0,-0.6),ld=75e3,lx=0.4,ly=0.4),
lmb.WheelLoad(pos=geom.Pos2d(-1.0,-0.6),ld=75e3,lx=0.4,ly=0.4),
lmb.WheelLoad(pos=geom.Pos2d(1.0,0.6),ld=75e3,lx=0.4,ly=0.4),
lmb.WheelLoad(pos=geom.Pos2d(-1.0,0.6),ld=75e3,lx=0.4,ly=0.4)])

IAP_carril_virt3_fren=lmb.LoadModel(wLoads=
[lmb.WheelLoad(pos=geom.Pos2d(1.0,-0.6),ld=37.5e3,lx=0.4,ly=0.4),
lmb.WheelLoad(pos=geom.Pos2d(-1.0,-0.6),ld=37.5e3,lx=0.4,ly=0.4),
lmb.WheelLoad(pos=geom.Pos2d(1.0,0.6),ld=37.5e3,lx=0.4,ly=0.4),
lmb.WheelLoad(pos=geom.Pos2d(-1.0,0.6),ld=37.5e3,lx=0.4,ly=0.4)])

