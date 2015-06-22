# -*- coding: utf-8 -*-
from __future__ import division
import sys
from materials import typical_materials


class sectionProperties(object):
  """Abstract section properties (area, moments of inertia,...)"""
  nmb= "" # section's name.
  E= 0.0 # elastic modulus
  nu= 0.3 # Poisson's ratio
  def __init__(self,name,E,nu):
    self.nmb= name
    self.E= E
    self.nu= nu
  def A(self):
    """ Section's area."""
    raise "Abstract method, please override"
    return 0.0
  def Iy(self):
    raise "Abstract method, please override"
    return 0.0    
  def Iz(self):
    raise "Abstract method, please override"
    return 0.0    

  def J(self):
    raise "Abstract method, please override"
    return 0.0    

  def G(self):
    return self.E/(2*(1+self.nu))
  def Wyel(self):
    raise "Abstract method, please override"
    return 0.0    

  def Wzel(self):
    raise "Abstract method, please override"
    return 0.0    

  def defSeccElastica3d(self,mdlr):
    materiales= mdlr.getMaterialLoader
    if(materiales.materialExists(self.nmb)):
      sys.stderr.write("Section: "+self.nmb+" is already defined.")
    else:
      retval= typical_materials.defElasticSection3d(mdlr,self.nmb,self.A(),self.E,self.G(),self.Iz(),self.Iy(),self.J())
      return retval;
  def defSeccShElastica3d(self,mdlr):
    materiales= mdlr.getMaterialLoader
    if(materiales.materialExists(self.nmb)):
      sys.stderr.write("Section: "+self.nmb+" is already defined.")
    else:
      retval= typical_materials.defElasticShearSection3d(mdlr,self.nmb,self.A(),self.E,self.G(),self.Iz(),self.Iy(),self.J(),self.alphaY())
      return retval;
  def defSeccElastica2d(self,mdlr):
    # Define una sección elástica para elementos 2d a partir de los datos del registro.
    materiales= mdlr.getMaterialLoader
    if(materiales.materialExists(self.nmb)):
      sys.stderr.write("Section: "+self.nmb+" is already defined.")
    else:
      retval= typical_materials.defElasticSection2d(mdlr,self.nmb,self.A(),self.E,self.Iz())
      return retval;
  def defSeccShElastica2d(self,mdlr):
    # Define una sección elástica para elementos 2d a partir de los datos del registro.
    materiales= mdlr.getMaterialLoader
    if(materiales.materialExists(self.nmb)):
      sys.stderr.write("Section: "+self.nmb+" is already defined.")
    else:
      retval= typical_materials.defElasticShearSection2d(mdlr,self.nmb,self.A(),self.E,self.G(),self.Iz(),self.alphaY())
      return retval;


# Define una sección elástica para elementos 3d a partir de los datos del registro.
def defSeccElastica3d(mdlr,defSecc):
  print "DEPRECATED; use object's method defSeccElastica3d"
  return defSecc.defSeccElastica3d(mdlr)

def defSeccShElastica3d(mdlr,defSecc):
  print "DEPRECATED; use object's method defSeccShElastica3d"
  return defSecc.defSeccShElastica3d(mdlr)

# Define una sección elástica para elementos 2d a partir de los datos del registro.
def defSeccElastica2d(mdlr,defSecc):
  print "DEPRECATED; use object's method defSeccElastica2d"
  return defSecc.defSeccElastica2d(mdlr)

# Define una sección elástica para elementos 2d a partir de los datos del registro.
def defSeccShElastica2d(mdlr,defSecc):
  print "DEPRECATED; use object's method defSeccShElastica2d"
  return defSecc.defSeccShElastica2d(mdlr)

