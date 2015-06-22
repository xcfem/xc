# -*- coding: utf-8 -*-

import math

class BetonSIA262(object):
  fck= 20e6
  def __init__(self, fck):
    self.fck= fck
  def nufc(self):
    return min(math.pow(30e6/self.fck,1/3.0),1.0)
  def fctm(self):
    return 0.3*math.pow(self.fck/1e6,2/3.0)*1e6

c12_15= BetonSIA262(12e6)
c16_20= BetonSIA262(16e6)
c20_25= BetonSIA262(20e6)
c25_30= BetonSIA262(25e6)
c30_37= BetonSIA262(30e6)
c35_45= BetonSIA262(35e6)
c40_50= BetonSIA262(40e6)
c45_55= BetonSIA262(45e6)
c50_60= BetonSIA262(50e6)
