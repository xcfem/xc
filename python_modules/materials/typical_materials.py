# -*- coding: utf-8 -*-

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es" "ana.Ortega@ciccp.es"

import xc_base
import geom
import xc
from misc_utils import log_messages as lmsg
import uuid

# Typical material definition.

class BasicElasticMaterial(object):
    '''Basic elastic material

     :ivar E:            Young’s modulus of the material
     :ivar nu:           Poisson’s ratio
     :ivar rho:          mass density
    '''
    def __init__(self, E, nu, rho= 0.0):
        '''Base class to construct some material definition classes

         :param E:            Young’s modulus of the material
         :param nu:           Poisson’s ratio
         :param rho:          mass density
        '''
        self.E= E
        self.nu= nu
        self.rho= rho
        
    def G(self):
        '''shear modulus'''
        return self.E/(2*(1+self.nu))
    
    def defElasticMaterial(self, preprocessor, name= None):
        ''' Return an elastic material appropiate for example for
            truss elements

        :param  preprocessor: preprocessor object.
        '''        
        materialHandler= preprocessor.getMaterialHandler
        matName= name
        if(not matName):
            matName= uuid.uuid1().hex
        retval= materialHandler.newMaterial("elastic_material",matName)
        retval.E= self.E
        retval.rho= self.rho
        return retval

    def getDict(self):
        ''' Returns a dictionary whith the values of the internal forces.
            Makes easier export it to json.'''
        return {'E':self.E,'nu':self.nu,'rho':self.rho}
    
    def setFromDict(self,dct):
        '''Sets the internal forces from the dictionary argument.'''
        self.E= dct['E']
        self.nu= dct['nu']
        self.rho= dct['rho']
        

def defElasticMaterial(preprocessor,name, E, rho= 0.0, nu= 0.3):
    '''Constructs an elastic uniaxial material.

    :param preprocessor: preprocessor
    :param name:         name identifying the material
    :param E:            tangent in the stress-strain diagram
    '''
    tmp= BasicElasticMaterial(E, nu, rho)
    return tmp.defElasticMaterial(preprocessor, name)

def defElasticPPMaterial(preprocessor,name,E,fyp,fyn):
    '''Constructs an elastic perfectly-plastic uniaxial material.

    :param preprocessor: preprocessor
    :param name:         name identifying the material
    :param E:            tangent in the elastic zone of the stress-strain diagram
    :param fyp:          stress at which material reaches plastic state in tension
    :param fyn:          stress at which material reaches plastic state in compression
    '''
    materialHandler= preprocessor.getMaterialHandler
    retval= materialHandler.newMaterial("elasticpp_material",name)
    retval.E= E
    retval.fyp= fyp
    retval.fyn= fyn
    return retval

def defElastNoTensMaterial(preprocessor,name,E):
    '''Constructs a uniaxial elastic - no tension material.

    :param preprocessor: preprocessor
    :param name:         name identifying the material
    :param E:            tangent in the elastic zone of the stress-strain diagram
    '''
    materialHandler= preprocessor.getMaterialHandler
    retval= materialHandler.newMaterial("elast_no_trac_material",name)
    retval.E= E
    return retval

#Cable material.
def defCableMaterial(preprocessor,name,E,prestress,rho):
    '''Constructs a uniaxial bilinear prestressed material. 
    The stress strain ranges from slack (large strain at zero stress) 
    to taught (linear with modulus E).

    :param preprocessor: preprocessor
    :param name:         name identifying the material
    :param E:            elastic modulus
    :param prestress:    prestress
    :param rho:          effective self weight (gravity component of weight per 
                    volume transverse to the cable)
    '''
    materialHandler= preprocessor.getMaterialHandler
    retval= materialHandler.newMaterial("cable_material",name)
    retval.E= E
    retval.prestress= prestress
    retval.rho= rho
    return retval


#Steel 01.
def defSteel01(preprocessor,name,E,fy,b):
    '''Constructs a uniaxial bilinear steel material object with kinematic hardening

    :param preprocessor: preprocessor
    :param name:         name identifying the material
    :param E:            initial elastic tangent 
    :param fy:           yield strength 
    :param b:            strain-hardening ratio: ratio between post-yield tangent
                    and initial elastic tangent
    '''
    materialHandler= preprocessor.getMaterialHandler
    retval= materialHandler.newMaterial("steel01",name)
    retval.E= E
    retval.fy= fy
    retval.b= b
    return retval

#Steel 02.
def defSteel02(preprocessor,name,E,fy,b,initialStress):
    '''Constructs a uniaxial bilinear Giuffre-Menegotto-Pinto steel material with 
    isotropic strain hardening

    :param preprocessor: preprocessor
    :param name:         name identifying the material
    :param E:            initial elastic tangent 
    :param fy:           yield strength 
    :param b:            strain-hardening ratio: ratio between post-yield tangent
                    and initial elastic tangent
    :param initialStress: initial stress
    '''
    materialHandler= preprocessor.getMaterialHandler
    retval= materialHandler.newMaterial("steel02",name)
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

    :param preprocessor: preprocessor
    :param name:         name identifying the material
    :param epsc0:        concrete strain at maximum strength 
    :param fpc:          concrete compressive strength at 28 days (compression is negative)
    :param fpcu:         concrete crushing strength 
    :param epscu:        concrete strain at crushing strength 
    '''
    materialHandler= preprocessor.getMaterialHandler
    retval= materialHandler.newMaterial("concrete01_material",name)
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

    :param preprocessor: preprocessor
    :param name:         name identifying the material
    :param epsc0:        concrete strain at maximum strength 
    :param fpc:          concrete compressive strength at 28 days (compression is negative)
    :param fpcu:         concrete crushing strength 
    :param epscu:        concrete strain at crushing strength 
    :param ratioSlope:   ratio between unloading slope at epscu and initial slope 
    :param ft:           tensile strength
    :param Ets:          tension softening stiffness (absolute value) (slope of the linear tension softening branch) 

    '''
    materialHandler= preprocessor.getMaterialHandler
    retval= materialHandler.newMaterial("concrete02_material",name)
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

#Elastic section 1d.
def defElasticSection1d(preprocessor,name,A,E,linearRho= 0.0):
    '''Constructs an elastic section appropiate for 1D beam analysis.

    :param preprocessor: preprocessor
    :param name:         name identifying the section
    :param A:            cross-sectional area of the section
    :param E:            Young’s modulus of material
    :param I:            second moment of area about the local z-axis
    '''
    materialHandler= preprocessor.getMaterialHandler
    retval= materialHandler.newMaterial("elastic_section_1d",name)
    retval.sectionProperties.A= A
    retval.sectionProperties.E= E
    retval.sectionProperties.linearRho= linearRho
    return retval

#Elastic section 2d.
def defElasticSection2d(preprocessor,name,A,E,I, linearRho= 0.0):
    '''Constructs an elastic section appropiate for 2D beam analysis.

    :param preprocessor: preprocessor
    :param name:         name identifying the section
    :param A:            cross-sectional area of the section
    :param E:            Young’s modulus of material
    :param I:            second moment of area about the local z-axis
    '''
    materialHandler= preprocessor.getMaterialHandler
    retval= materialHandler.newMaterial("elastic_section_2d",name)
    retval.sectionProperties.A= A
    retval.sectionProperties.E= E
    retval.sectionProperties.I= I
    retval.sectionProperties.linearRho= linearRho
    return retval

#Elastic shear section 2d.
def defElasticShearSection2d(preprocessor,name,A,E,G,I,alpha, linearRho= 0.0):
    '''Constructs an elastic section appropiate for 2D beam analysis, 
    including shear deformations.

    :param preprocessor: preprocessor
    :param name:         name identifying the section
    :param A:            cross-sectional area of the section
    :param E:            Young’s modulus of the material
    :param G:            Shear modulus of the material          
    :param I:            second moment of area about the local z-axis
     :param alpha:        shear shape factor
    '''
    materialHandler= preprocessor.getMaterialHandler
    retval= materialHandler.newMaterial("elasticShearSection2d",name)
    retval.sectionProperties.A= A
    retval.sectionProperties.E= E
    retval.sectionProperties.G= G
    retval.sectionProperties.I= I
    retval.sectionProperties.Alpha= alpha
    retval.sectionProperties.linearRho= linearRho
    return retval

def defElasticSectionFromMechProp1d(preprocessor,name,mechProp1d):
    '''Constructs an elastic section appropiate for 1D beam analysis, 
    taking mechanical properties of the section form a MechProp1d object.

    :param preprocessor: preprocessor
    :param name:         name identifying the section
    :param mechProp1d:   object of type MechProp1d that contains the mechanical 
                    properties of the section
    '''  
    retval= defElasticSection1d(preprocessor,name,mechProp1d.A,mechProp1d.E,linearRho= mechProp1d.linearRho)
    return retval

def defElasticSectionFromMechProp2d(preprocessor,name,mechProp2d):
    '''Constructs an elastic section appropiate for 2D beam analysis, 
    taking mechanical properties of the section form a MechProp2d object.

    :param preprocessor: preprocessor
    :param name:         name identifying the section
    :param mechProp2d:   object of type MechProp2d that contains the mechanical 
                    properties of the section
    '''  
    retval= defElasticSection2d(preprocessor,name,mechProp2d.A,mechProp2d.E,mechProp2d.I,linearRho= mechProp2d.linearRho)
    return retval

#Elastic section 3d.
def defElasticSection3d(preprocessor,name,A,E,G,Iz,Iy,J, linearRho= 0.0):
    '''Constructs an elastic section appropiate for 3D beam analysis

    :param preprocessor: preprocessor
    :param name:         name identifying the section
    :param A:            cross-sectional area of the section
    :param E:            Young’s modulus of the material
    :param G:            Shear modulus of the material          
    :param Iz:           second moment of area about the local z-axis
    :param Iy:           second moment of area about the local y-axis
    :param J:            torsional moment of inertia of the section

    '''
    materialHandler= preprocessor.getMaterialHandler
    retval= materialHandler.newMaterial("elastic_section_3d",name)
    retval.sectionProperties.A= A
    retval.sectionProperties.E= E
    retval.sectionProperties.G= G
    retval.sectionProperties.Iz= Iz
    retval.sectionProperties.Iy= Iy
    retval.sectionProperties.J= J
    retval.sectionProperties.linearRho= linearRho
    return retval

def defElasticSectionFromMechProp3d(preprocessor,name,mechProp3d):
    '''Constructs an elastic section appropiate for 3D beam analysis, 
    taking mechanical properties of the section form a MechProp3d object.

    :param preprocessor: preprocessor
    :param name:         name identifying the section
    :param mechProp2d:   instance of the class MechProp3d that contains the 
                         mechanical properties of the section
    '''  
    return defElasticSection3d(preprocessor,name,mechProp3d.A,mechProp3d.E,mechProp3d.G,mechProp3d.Iz,mechProp3d.Iy,mechProp3d.J,linearRho= mechProp3d.linearRho)

#Elastic shear section 3d.
def defElasticShearSection3d(preprocessor,name,A,E,G,Iz,Iy,J,alpha, linearRho= 0.0):
    '''Constructs an elastic section appropiate for 3D beam analysis, 
    including shear deformations.

    :param preprocessor: preprocessor
    :param name:         name identifying the section
    :param A:            cross-sectional area of the section
    :param E:            Young’s modulus of the material
    :param G:            shear modulus of the material          
    :param Iz:           second moment of area about the local z-axis
    :param Iy:           second moment of area about the local y-axis
    :param J:            torsional moment of inertia of the section
    :param alpha:        shear shape factor
    '''
    materialHandler= preprocessor.getMaterialHandler
    retval= materialHandler.newMaterial("elasticShearSection3d",name)
    retval.sectionProperties.A= A
    retval.sectionProperties.E= E
    retval.sectionProperties.G= G
    retval.sectionProperties.Iz= Iz
    retval.sectionProperties.Iy= Iy
    retval.sectionProperties.J= J
    retval.sectionProperties.Alpha= alpha
    retval.sectionProperties.linearRho= linearRho
    return retval


#Linear elastic isotropic plane strain material.
def defElasticIsotropicPlaneStrain(preprocessor,name,E,nu,rho):
    '''Constructs an linear elastic isotropic plane-strain material.

    :param  preprocessor: preprocessor
    :param  name:         name identifying the material
    :param  E:            Young’s modulus of the material
    :param  nu:           Poisson’s ratio
    :param  rho:          mass density, optional (defaults to 0.0)
    '''
    materialHandler= preprocessor.getMaterialHandler
    retval= materialHandler.newMaterial("elastic_isotropic_plane_strain_2d",name)
    retval.E= E
    retval.nu= nu
    retval.rho= rho
    return retval

#Linear elastic isotropic plane stress material.
def defElasticIsotropicPlaneStress(preprocessor,name,E,nu,rho):
    '''Constructs an linear elastic isotropic plane-stress material.

    :param  preprocessor: preprocessor
    :param  name:         name identifying the material
    :param  E:            Young’s modulus of the material
    :param  nu:           Poisson’s ratio
    :param  rho:          mass density, optional (defaults to 0.0)
    '''
    materialHandler= preprocessor.getMaterialHandler
    retval= materialHandler.newMaterial("elastic_isotropic_plane_stress_2d",name)
    retval.E= E
    retval.nu= nu
    retval.rho= rho
    return retval

#Linear elastic isotropic 3d material.
def defElasticIsotropic3d(preprocessor,name,E,nu,rho):
    '''Constructs an linear elastic isotropic 3D material.

    :param  preprocessor: preprocessor
    :param  name:         name identifying the material
    :param  E:            Young’s modulus of the material
    :param  nu:           Poisson’s ratio
    :param  rho:          mass density, optional (defaults to 0.0)
    '''
    materialHandler= preprocessor.getMaterialHandler
    retval= materialHandler.newMaterial("elastic_isotropic_3d",name)
    retval.E= E
    retval.nu= nu
    retval.rho= rho
    return retval

#Elastic plate section.
def defElasticPlateSection(preprocessor,name,E,nu,rho,h):
    '''Constructs an elastic isotropic section material appropiate 
       for plate analysis.

    :param  preprocessor: preprocessor
    :param  name:         name identifying the section
    :param  E:            Young’s modulus of the material
    :param  nu:           Poisson’s ratio
    :param  rho:          mass density
    :param  h:            overall depth of the section
    '''
    materialHandler= preprocessor.getMaterialHandler
    retval= materialHandler.newMaterial("elastic_plate_section",name)
    retval.E= E
    retval.nu= nu
    retval.rho= rho
    retval.h= h
    return retval

#Elastic plate section.
def defJ2PlateFibre(preprocessor, name, E, nu, fy, alpha= .01, rho= 0.0):
    '''Constructs a J2 (Von Mises) material with a linear-strain
       hardening rule appropiate for plate analysis.

    :param  preprocessor: preprocessor
    :param  name:         name identifying the material.
    :param  E:            Young’s modulus of the material.
    :param  nu:           Poisson’s ratio.
    :param  fy:           material yield stress.
    :param  alpha:        strain hardening ratio (default: (0.01), 
                          range: 0 to 1).
    :param  rho:          mass density
    '''
    materialHandler= preprocessor.getMaterialHandler
    retval= materialHandler.newMaterial("J2_plate_fibre",name)
    retval.E= E
    retval.nu= nu
    retval.fy= fy
    if(alpha<0 or alpha>1):
        lmsg.error('alpha value: '+str(alpha)+' out of range (0,1)')
    retval.Hiso= alpha*E/(1-alpha)
    retval.rho= rho
    return retval

#Elastic membrane plate section.
def defElasticMembranePlateSection(preprocessor,name,E,nu,rho,h):
    '''Constructs an elastic isotropic section material appropiate 
       for plate and shell analysis.

    :param  preprocessor: preprocessor
    :param  name:         name identifying the section
    :param  E:            Young’s modulus of the material
    :param  nu:           Poisson’s ratio
    :param  rho:          mass density
    :param  h:            overall depth of the section
    '''
    materialHandler= preprocessor.getMaterialHandler
    retval= materialHandler.newMaterial("elastic_membrane_plate_section",name)
    retval.E= E
    retval.nu= nu
    retval.rho= rho
    retval.h= h
    return retval

#Elastic membrane plate section.
def defMembranePlateFiberSection(preprocessor, name, nDMaterial, h):
    '''Constructs a membrane plate fiber section appropiate 
       for plate and shell analysis.

    :param  preprocessor: preprocessor
    :param  name:         name identifying the section
    :param  nDMaterial:   material to put in the section fibers.
    :param  h:            overall depth of the section
    '''
    materialHandler= preprocessor.getMaterialHandler
    retval= materialHandler.newMaterial("membrane_plate_fiber_section",name)
    retval.setMaterial(nDMaterial);
    retval.h= h
    return retval

class MaterialData(BasicElasticMaterial):
    '''Base class to construct some material definition classes

     :ivar name:         name identifying the material
     :ivar xc_material:  pointer to XC material.    
    '''
    def __init__(self,name,E,nu,rho):
        '''Base class to construct some material definition classes

         :param name:         name identifying the material
         :param E:            Young’s modulus of the material
         :param nu:           Poisson’s ratio
         :param rho:          mass density
        '''
        super(MaterialData,self).__init__(E,nu,rho)
        self.name= name
        self.xc_material= None

class DeckMaterialData(MaterialData):
    '''Material for Isotropic elastic sections

    :ivar name:         name identifying the section
    :ivar thickness:    overall depth of the section
    :ivar material:     instance of a class that defines the elastic modulus, 
                        shear modulus and mass density of the material
    '''
    def __init__(self,name,thickness,material):
        super(DeckMaterialData,self).__init__(name,material.E,material.nu,material.rho)
        self.name= name
        self.thickness= thickness
        self.material=material
    def getAreaDensity(self):
        '''return the mass per unit area'''
        return self.rho*self.thickness
    def setupElasticSection(self,preprocessor):
        '''create an elastic isotropic section appropiate for plate and shell analysis
        '''
        if(not self.xc_material):
            materialHandler= preprocessor.getMaterialHandler
            if(materialHandler.materialExists(self.name)):
                lmsg.warning("Section: "+self.name+" already defined.")
                self.xc_material= materialHandler.getMaterial(self.name)
            else:
                self.xc_material= defElasticMembranePlateSection(preprocessor,self.name,self.E,self.nu,self.getAreaDensity(),self.thickness)
        else:
            lmsg.warning('Material: ', self.name, ' already defined.')
        return self.xc_material

class BeamMaterialData(MaterialData):
    '''Elastic section appropiate for 3D beam analysis, including shear deformations.

    :ivar name:         name identifying the section
    :ivar section:      instance of a class that defines the geometric and
                        mechanical characteristiscs of a section
                        e.g: RectangularSection, CircularSection, ISection, ...
    :ivar material:     instance of a class that defines the elastic modulus, 
                        shear modulus and mass density of the material
    '''
    def __init__(self,name,section,material):
        super(BeamMaterialData,self).__init__(name,material.E,material.nu,material.rho)
        self.name= name
        self.section= section
        self.material= material
    def getRho(self):
        '''return the mass per unit length'''
        return self.rho*self.section.A()
    def setupElasticShear3DSection(self,preprocessor):
        '''Return an elastic section appropiate for 3D beam analysis
        '''
        if(not self.xc_material):
            materialHandler= preprocessor.getMaterialHandler
            if(materialHandler.materialExists(self.name)):
                lmsg.warning("Section: "+self.name+" already defined.")
                self.xc_material= materialHandler.getMaterial(self.name)
            else:
                self.xc_material= defElasticShearSection3d(preprocessor,self.name,self.section.A(),self.material.E,self.material.G(),self.section.Iz(),self.section.Iy(),self.section.J(),self.section.alphaZ())
        else:
            lmsg.warning('Material: ', self.name, ' already defined.')
        return self.xc_material
