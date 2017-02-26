# -*- coding: utf-8 -*-

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com" "ana.Ortega.Ort@gmail.com"

import xc_base
import geom
import xc
from miscUtils import LogMessages as lmsg

# Typical material definition.

class BasicElasticMaterial(object):
  '''Basic elastic material'''

  def __init__(self, E, nu):
    self.E= E
    self.nu= nu

  def G(self):
    return self.E/(2*(1+self.nu))

def defElasticMaterial(preprocessor,name,E):
  '''Constructs an elastic uniaxial material.

  :param preprocessor: preprocessor name
  :param name:         name identifying the material
  :param E:            tangent in the stress-strain diagram
  '''
  materiales= preprocessor.getMaterialLoader
  materiales.newMaterial("elastic_material",name)
  retval= materiales.getMaterial(name)
  retval.E= E
  return retval

def defElasticPPMaterial(preprocessor,name,E,fyp,fyn):
  '''Constructs an elastic perfectly-plastic uniaxial material.

  :param preprocessor: preprocessor name
  :param name:         name identifying the material
  :param E:            tangent in the elastic zone of the stress-strain diagram
  :param fyp:          stress at which material reaches plastic state in tension
  :param fyn:          stress at which material reaches plastic state in compression
  '''
  materiales= preprocessor.getMaterialLoader
  materiales.newMaterial("elasticpp_material",name)
  retval= materiales.getMaterial(name)
  retval.E= E
  retval.name= name
  retval.fyp= fyp
  retval.fyn= fyn
  return retval

def defElastNoTensMaterial(preprocessor,name,E):
  '''Constructs a uniaxial elastic - no tension material.

  :param preprocessor: preprocessor name
  :param name:         name identifying the material
  :param E:            tangent in the elastic zone of the stress-strain diagram
  '''
  materiales= preprocessor.getMaterialLoader
  materiales.newMaterial("elast_no_trac_material",name)
  retval= materiales.getMaterial(name)
  retval.E= E
  return retval

#Cable material.
def defCableMaterial(preprocessor,name,E,prestress,rho):
  '''Constructs a uniaxial bilinear prestressed material. 
  The stress strain ranges from slack (large strain at zero stress) 
  to taught (linear with modulus E).

  :param preprocessor: preprocessor name
  :param name:         name identifying the material
  :param E:            elastic modulus
  :param prestress:    prestress
  :param rho:          effective self weight (gravity component of weight per 
                  volume transverse to the cable)
  '''
  materiales= preprocessor.getMaterialLoader
  materiales.newMaterial("cable_material",name)
  retval= materiales.getMaterial(name)
  retval.E= E
  retval.prestress= prestress
  retval.rho= rho
  return retval


#Steel 01.
def defSteel01(preprocessor,name,E,fy,b):
  '''Constructs a uniaxial bilinear steel material object with kinematic hardening

  :param preprocessor: preprocessor name
  :param name:         name identifying the material
  :param E:            initial elastic tangent 
  :param fy:           yield strength 
  :param b:            strain-hardening ratio: ratio between post-yield tangent
                  and initial elastic tangent
  '''
  materiales= preprocessor.getMaterialLoader
  materiales.newMaterial("steel01",name)
  retval= materiales.getMaterial(name)
  retval.E= E
  retval.fy= fy
  retval.b= b
  return retval

#Steel 02.
def defSteel02(preprocessor,name,E,fy,b,initialStress):
  '''Constructs a uniaxial bilinear Giuffre-Menegotto-Pinto steel material with 
  isotropic strain hardening

  :param preprocessor: preprocessor name
  :param name:         name identifying the material
  :param E:            initial elastic tangent 
  :param fy:           yield strength 
  :param b:            strain-hardening ratio: ratio between post-yield tangent
                  and initial elastic tangent
    initialStress:initial stress
  '''
  materiales= preprocessor.getMaterialLoader
  materiales.newMaterial("steel02",name)
  retval= materiales.getMaterial(name)
  retval.E= E
  retval.fy= fy
  retval.b= b
  retval.initialStress= initialStress
  return retval


#Concrete 01.
def defConcrete01(preprocessor,name,epsc0,fpc,fpcu,epscu):
  ''''Constructs an uniaxial Kent-Scott-Park concrete material object 
  with degraded linear unloading/reloading stiffness according to 
  the work of Karsan-Jirsa and no tensile strength

  :param preprocessor: preprocessor name
  :param name:         name identifying the material
  :param epsc0:        concrete strain at maximum strength 
  :param fpc:          concrete compressive strength at 28 days (compression is negative)
  :param fpcu:         concrete crushing strength 
  :param epscu:        concrete strain at crushing strength 
  '''
  materiales= preprocessor.getMaterialLoader
  materiales.newMaterial("concrete01_material",name)
  retval= materiales.getMaterial(name)
  retval.epsc0= epsc0
  retval.fpc= fpc
  retval.fpcu= fpcu
  retval.epscu= epscu
  return retval


#Concrete 02.
def defConcrete02(preprocessor,name,epsc0,fpc,fpcu,epscu,ratioSlope,ft,Ets):
  ''''Constructs an uniaxial concrete material with linear tension
  softening. Compressive concrete parameters should be input as negative values.
  The initial slope for this model is (2*fpc/epsc0) 

  :param preprocessor: preprocessor name
  :param name:         name identifying the material
  :param epsc0:        concrete strain at maximum strength 
  :param fpc:          concrete compressive strength at 28 days (compression is negative)
  :param fpcu:         concrete crushing strength 
  :param epscu:        concrete strain at crushing strength 
  :param ratioSlope:   ratio between unloading slope at epscu and initial slope 
  :param ft:           tensile strength
  :param Ets:          tension softening stiffness (absolute value) (slope of the linear tension softening branch) 

  '''
  materiales= preprocessor.getMaterialLoader
  materiales.newMaterial("concrete02_material",name)
  retval= materiales.getMaterial(name)
  retval.epsc0= epsc0
  retval.fpc= fpc
  retval.fpcu= fpcu
  retval.epscu= epscu
  retval.ratioSlope=ratioSlope
  retval.ft=ft
  retval.Ets=Ets
  if fpc==fpcu:
    lmsg.warning("concrete02 compressive strength fpc is equal to crushing strength fpcu => the solver can return wrong stresses or have convergence problems ")
  return retval

#Elastic section 2d.
def defElasticSection2d(preprocessor,name,A,E,I):
  '''Constructs an elastic section appropiate for 2D beam analysis.

  :param preprocessor: preprocessor name
  :param name:         name identifying the section
  :param A:            cross-sectional area of the section
  :param E:            Young’s modulus of material
  :param I:            second moment of area about the local z-axis
  '''
  materiales= preprocessor.getMaterialLoader
  materiales.newMaterial("elastic_section_2d",name)
  retval= materiales.getMaterial(name)
  retval.sectionProperties.A= A
  retval.sectionProperties.E= E
  retval.sectionProperties.I= I
  return retval

#Elastic shear section 2d.
def defElasticShearSection2d(preprocessor,name,A,E,G,I,alpha):
  '''Constructs an elastic section appropiate for 2D beam analysis, 
  including shear deformations.

  :param preprocessor: preprocessor name
  :param name:         name identifying the section
  :param A:            cross-sectional area of the section
  :param E:            Young’s modulus of the material
  :param G:            Shear modulus of the material          
  :param I:            second moment of area about the local z-axis
   :param alpha:        shear shape factor
  '''
  materiales= preprocessor.getMaterialLoader
  materiales.newMaterial("elasticShearSection2d",name)
  retval= materiales.getMaterial(name)
  retval.sectionProperties.A= A
  retval.sectionProperties.E= E
  retval.sectionProperties.G= G
  retval.sectionProperties.I= I
  retval.sectionProperties.Alpha= alpha
  return retval

def defElasticSectionFromMechProp2d(preprocessor,name,mechProp2d):
  '''Constructs an elastic section appropiate for 2D beam analysis, 
  taking mechanical properties of the section form a MechProp2d object.

  :param preprocessor: preprocessor name
  :param name:         name identifying the section
  :param mechProp2d:   object of type MechProp2d that contains the mechanical 
                  properties of the section
  '''  
  return defElasticSection2d(preprocessor,name,mechProp2d.A,mechProp2d.E,mechProp2d.I)

#Elastic section 3d.
def defElasticSection3d(preprocessor,name,A,E,G,Iz,Iy,J):
  '''Constructs an elastic section appropiate for 3D beam analysis

  :param preprocessor: preprocessor name
  :param name:         name identifying the section
  :param A:            cross-sectional area of the section
  :param E:            Young’s modulus of the material
  :param G:            Shear modulus of the material          
  :param Iz:           second moment of area about the local z-axis
  :param Iy:           second moment of area about the local y-axis
  :param J:            torsional moment of inertia of the section

  '''
  materiales= preprocessor.getMaterialLoader
  materiales.newMaterial("elastic_section_3d",name)
  retval= materiales.getMaterial(name)
  retval.sectionProperties.A= A
  retval.sectionProperties.E= E
  retval.sectionProperties.G= G
  retval.sectionProperties.Iz= Iz
  retval.sectionProperties.Iy= Iy
  retval.sectionProperties.J= J
  return retval

def defElasticSectionFromMechProp3d(preprocessor,name,mechProp3d):
  '''Constructs an elastic section appropiate for 3D beam analysis, 
  taking mechanical properties of the section form a MechProp3d object.

  :param preprocessor: preprocessor name
  :param name:         name identifying the section
  :param mechProp2d:   instance of the class MechProp3d that contains the 
                       mechanical properties of the section
  '''  
  return defElasticSection3d(preprocessor,name,mechProp3d.A,mechProp3d.E,mechProp3d.G,mechProp3d.Iz,mechProp3d.Iy,mechProp3d.J)

#Elastic shear section 3d.
def defElasticShearSection3d(preprocessor,name,A,E,G,Iz,Iy,J,alpha):
  '''Constructs an elastic section appropiate for 3D beam analysis, 
  including shear deformations.

  :param preprocessor: preprocessor name
  :param name:         name identifying the section
  :param A:            cross-sectional area of the section
  :param E:            Young’s modulus of the material
  :param G:            shear modulus of the material          
  :param Iz:           second moment of area about the local z-axis
  :param Iy:           second moment of area about the local y-axis
  :param J:            torsional moment of inertia of the section
  :param alpha:        shear shape factor
  '''
  materiales= preprocessor.getMaterialLoader
  materiales.newMaterial("elasticShearSection3d",name)
  retval= materiales.getMaterial(name)
  retval.sectionProperties.A= A
  retval.sectionProperties.E= E
  retval.sectionProperties.G= G
  retval.sectionProperties.Iz= Iz
  retval.sectionProperties.Iy= Iy
  retval.sectionProperties.J= J
  retval.sectionProperties.Alpha= alpha
  return retval


#Linear elastic isotropic plane strain material.
def defElasticIsotropicPlaneStrain(preprocessor,name,E,nu,rho):
  '''Constructs an linear elastic isotropic plane-strain material.

  :param  preprocessor: preprocessor name
  :param  name:         name identifying the material
  :param  E:            Young’s modulus of the material
  :param  nu:           Poisson’s ratio
  :param  rho:          mass density, optional (defaults to 0.0)
  '''
  materiales= preprocessor.getMaterialLoader
  materiales.newMaterial("elastic_isotropic_plane_strain_2d",name)
  retval= materiales.getMaterial(name)
  retval.E= E
  retval.nu= nu
  retval.rho= rho
  return retval

#Linear elastic isotropic plane stress material.
def defElasticIsotropicPlaneStress(preprocessor,name,E,nu,rho):
  '''Constructs an linear elastic isotropic plane-stress material.

  :param  preprocessor: preprocessor name
  :param  name:         name identifying the material
  :param  E:            Young’s modulus of the material
  :param  nu:           Poisson’s ratio
  :param  rho:          mass density, optional (defaults to 0.0)
  '''
  materiales= preprocessor.getMaterialLoader
  materiales.newMaterial("elastic_isotropic_plane_stress_2d",name)
  retval= materiales.getMaterial(name)
  retval.E= E
  retval.nu= nu
  retval.rho= rho
  return retval

#Linear elastic isotropic 3d material.
def defElasticIsotropic3d(preprocessor,name,E,nu,rho):
  '''Constructs an linear elastic isotropic 3D material.

  :param  preprocessor: preprocessor name
  :param  name:         name identifying the material
  :param  E:            Young’s modulus of the material
  :param  nu:           Poisson’s ratio
  :param  rho:          mass density, optional (defaults to 0.0)
  '''
  materiales= preprocessor.getMaterialLoader
  materiales.newMaterial("elastic_isotropic_3d",name)
  retval= materiales.getMaterial(name)
  retval.E= E
  retval.nu= nu
  retval.rho= rho
  return retval

#Elastic membrane plate section.
def defElasticPlateSection(preprocessor,name,E,nu,rho,h):
  '''Constructs an elastic isotropic section appropiate for plate analysis

  :param  preprocessor: preprocessor name
  :param  name:         name identifying the section
  :param  E:            Young’s modulus of the material
  :param  nu:           Poisson’s ratio
  :param  rho:          mass density
  :param  h:            overall depth of the section
  '''
  materiales= preprocessor.getMaterialLoader
  materiales.newMaterial("elastic_membrane_plate_section",name)
  retval= materiales.getMaterial(name)
  retval.E= E
  retval.nu= nu
  retval.rho= rho
  retval.h= h
  return retval

#Elastic membrane plate section.
def defElasticMembranePlateSection(preprocessor,name,E,nu,rho,h):
  '''Constructs an elastic isotropic section appropiate for plate and shell analysis

  :param  preprocessor: preprocessor name
  :param  name:         name identifying the section
  :param  E:            Young’s modulus of the material
  :param  nu:           Poisson’s ratio
  :param  rho:          mass density
  :param  h:            overall depth of the section
  '''
  materiales= preprocessor.getMaterialLoader
  materiales.newMaterial("elastic_membrane_plate_section",name)
  retval= materiales.getMaterial(name)
  retval.E= E
  retval.nu= nu
  retval.rho= rho
  retval.h= h
  return retval

class MaterialData(object):
  '''Base class to construct some material definition classes
  
  :ivar name:         name identifying the material
  :ivar E:            Young’s modulus of the material
  :ivar nu:           Poisson’s ratio
  :ivar rho:          mass density
  '''
  def __init__(self,name,E,nu,rho):
    self.name= name
    self.E= E
    self.nu= nu
    self.rho= rho
  def G(self):
    '''shear modulus'''
    return self.E/(2*(1+self.nu))

