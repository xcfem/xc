# -*- coding: utf-8 -*-
'''Classes to store limit state control variables (internal forces, 
   strains, stresses,...) calculated in the analysis.
   THESE CLASSES ARE INTENDED TO REPLACE THE PROPERTIES DEFINED
   IN def_vars_control.py WICH MUST DISSAPEAR.
'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT), Ana Ortega(AO_O)"
__copyright__= "Copyright 2016,LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.Ortega@ciccp.es"


import os
import scipy
import inspect
from misc_utils import log_messages as lmsg
import xc_base
import geom
import xc
from postprocess.reports import common_formats as fmt
from postprocess import extrapolate_elem_attr as ext

def getElementInternalForceComponentData(elem, component):
    '''Return the data to use to represent the diagram over the element

    :param elem: element to deal with.
    :param component: component to show. 
           Valid components are: 'N', 'Qy', 'Vy', 'Qz', 'Vz', 'My', 'Mz', 
           'T'
    '''
    # default values
    value1= 0.0
    value2= 0.0
    elemVDir= elem.getJVector3d(True) #initialGeometry= True
    values= elem.getValuesAtNodes(component, False)
    if(len(values)>1): # component found.
        value1= values[0]; value2= values[1]
    if((component == 'Qy') or (component == 'Vy')):
        elemVDir= elem.getJVector3d(True) # initialGeometry= True 
    elif((component == 'Qz') or (component == 'Vz')):
        elemVDir= elem.getKVector3d(True) # initialGeometry= True 
    elif(component == 'My'):
        elemVDir= elem.getKVector3d(True) # initialGeometry= True 
    elif(component == 'Mz'):
        elemVDir= -elem.getJVector3d(True) # initialGeometry= True 
    return [elemVDir,value1,value2]


class ControlVarsBase(object):
    '''Base class for the control of variables (internal forces, 
     strains, stresses,...) calculated in the analysis.

    :ivar combName: name of the load combination to deal with
    '''
    def __init__(self,combName= 'nil'):
        ''' Constructor.

        :param combName: name of the load combinations to deal with
        '''
        self.combName= combName #Name of the corresponding load combination

    def getCF(self):
        ''' Return the capacity factor.'''
        return -1.0

    def __call__(self,arguments):
        ''' Allow the object behave like a funcion.

        :param arguments: function arguments.
        '''
        retval= None
        obj= self
        args= arguments.split('.')
        for arg in args:
            if(hasattr(obj,arg)):
                obj= getattr(obj,arg)
                if(hasattr(obj,'__call__')):
                    obj= obj.__call__()
            else:
                lmsg.error('argument: '+ arg+' not found')
        retval= obj
        return retval
  
    def getFieldNames(self,parent= ''):
        ''' Return the object field names.

        :param parent: parent object name.
        '''
        retval= list()
        for key in self.__dict__:
            obj= getattr(self,key)
            try:
                retval.extend(obj.getFieldNames(key+'.'))
            except AttributeError:
                retval.append(parent+key)
        return retval

    def getLaTeXFields(self,factor= 1e-3):
        ''' Returns a string with the intermediate fields of the LaTeX string.

        :param factor: factor for units (default 1e-3 -> kN)'''
        dct= self.getDict()
        retval= ''
        for key in dct:
            value= dct[key]
            initial= key[0]
            if(initial in ['N', 'M','V']): # internal force
                value= fmt.Esf.format(float(value)*factor)
            if('Stress' in key):
                value= fmt.Stress.format(value*factor*factor)
            if('stress' in key):
                value= fmt.Stress.format(value*factor*factor)
            retval+= str(value)+' & '
        retval= retval.strip(' &')
        return retval

    def getLaTeXString(self,eTag,factor= 1e-3):
        ''' Returns a string that we can insert in a LaTeX table.

        :param eTag:   element identifier.
        :param factor: factor for units (default 1e-3 -> kN)'''
        return str(eTag)+" & "+self.getLaTeXFields(factor)+" & "+fmt.Esf.format(self.getCF())+"\\\\\n"

    def getAnsysStrings(self,eTag, axis, factor= 1e-3):
        ''' Returns a string to represent fields in ANSYS (R).

        :param eTag:   element identifier.
        :param axis:   section 1 or 2
        :param factor: factor for units (default 1e-3 -> kN)'''
        retval= []
        retval.append("detab,"+str(eTag)+",CF" +axis+","+str(self.getCF())+"\n")
        return retval

    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        return {'combName': self.combName, 'CF':self.getCF()}

    def setFromDict(self, dct):
        ''' Set the data values from the dictionary argument.'''
        self.combName= dct['combName']
        self.CF= dct['CF']
       
    def getStrArguments(self):
        '''Returns a string for a 'copy' (kind of) constructor.

        '''
        dct= self.getDict()
        retval= ''
        for key in dct:
            value= dct[key]
            if(isinstance(value, str)):
                value= '"'+str(value)+'"'
            else:
                value= str(value)
            retval+= str(key)+'= '+value+','
        retval= retval.strip(',')
        return retval

    def getModuleImportString(self):
        ''' Return the string to import the module where the control 
            var is defined.'''
        retval= 'from '+ self.__module__
        retval+= ' import ' + self.__class__.__name__
        return retval
        
    def getStrConstructor(self):
        ''' Return a python sentence that can construct a clone
            of this object: kin of naïve serialization.'''
        retval= self.__class__.__name__ 
        retval+= '(' + self.getStrArguments() + ')'
        return retval

    def strElementProp(self,eTag,nmbProp):
        '''Writes a string that will serve to read the element property from a file.

        :param eTag:    element identifier.
        :param nmbProp: name of the element property
        '''
        retval= 'preprocessor.getElementHandler.getElement('
        retval+= str(eTag)
        retval+= ").setProp("
        retval+= '"' + nmbProp + '"'
        retval+= ',' + self.getStrConstructor()
        retval+= ")\n"
        return retval
  
class N(ControlVarsBase):
    '''Uniaxial tension or compression. Internal force [N] for a combination.

    :ivar N:        axial force (defaults to 0.0)
    '''
    def __init__(self,combName= 'nil', NN= 0.0):
        '''
        Constructor.

        :param combName: name of the load combinations to deal with
        :param NN:        axial force (defaults to 0.0)
        '''
        super(N,self).__init__(combName)
        self.N= NN # Axial force.

    def getAnsysStrings(self, eTag, axis, factor= 1e-3):
        ''' Returns a string to represent fields in ANSYS (R).

        :param eTag: element identifier.
        :param axis: section 1 or 2
        :param factor: factor for units (default 1e-3 -> kN)
        '''
        retval= super(N,self).getAnsysStrings(eTag,axis,factor)
        retval.append("detab,"+str(eTag)+",N" +axis+","+str(self.N*factor)+"\n")
        return retval

    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= super(N,self).getDict()
        retval.update({'N':self.N})
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.'''
        super(N,self).setFromDict(dct)
        self.N= dct['N']
  
class NMy(N):
    '''Uniaxial bending. Internal forces [N,My] for a combination.

    :ivar My:       bending moment about Y axis (defaults to 0.0)
    '''
    def __init__(self,combName= 'nil',N= 0.0,My= 0.0):
        '''
        Constructor.

        :param combName: name of the load combinations to deal with
        :param N:        axial force (defaults to 0.0)
        :param My:       bending moment about Y axis (defaults to 0.0)
        '''
        super(NMy,self).__init__(combName, N)
        self.My= My # Bending moment about y axis.

    def getAnsysStrings(self,eTag,axis, factor= 1e-3):
      ''' Returns a string to represent fields in ANSYS (R).

      :param eTag: element identifier.
      :param axis: section 1 or 2
      :param factor: factor for units (default 1e-3 -> kN)'''
      retval= super(NMy,self).getAnsysStrings(eTag,axis,factor)
      retval.append("detab,"+str(eTag)+",My" +axis+","+str(self.My*factor)+"\n")
      return retval

    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= super(NMy,self).getDict()
        retval.update({'My':self.My})
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.'''
        super(NMy,self).setFromDict(dct)
        self.My= dct['My']

class NMyMz(NMy):
    '''Biaxial bending. Internal forces [N,My,Mz] for a combination.

    :ivar Mz:       bending moment about Z axis (defaults to 0.0)
    '''
    def __init__(self,combName= 'nil',N= 0.0,My= 0.0, Mz= 0.0):
        '''
        Constructor.

        :param combName: name of the load combinations to deal with
        :param N:        axial force (defaults to 0.0)
        :param My:       bending moment about Y axis (defaults to 0.0)
        :param Mz:       bending moment about Z axis (defaults to 0.0)
        '''
        super(NMyMz,self).__init__(combName,N,My)
        self.Mz= Mz #Bending moment about z axis.

    def getAnsysStrings(self,eTag,axis, factor= 1e-3):
      ''' Returns a string to represent fields in ANSYS (R).

      :param eTag: element identifier.
      :param axis: section 1 or 2
      :param factor: factor for units (default 1e-3 -> kN)'''
      retval= super(NMyMz,self).getAnsysStrings(eTag,axis,factor)
      retval.append("detab,"+str(eTag)+",Mz" +axis+","+str(self.Mz*factor)+"\n")
      return retval

    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= super(NMyMz,self).getDict()
        retval.update({'Mz':self.Mz})
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.'''
        super(NMyMz,self).setFromDict(dct)
        self.Mz= dct['Mz']

class CFN(N):
    '''Uniaxial tension or compression. Normal stresses limit state variables.

    :ivar CF: capacity factor (efficiency) (defaults to -1.0; CF<1.0 -> Ok; CF>1.0 -> KO)
    '''
    def __init__(self,combName= 'nil',CF= -1.0,N= 0.0):
        '''
        Constructor.

        :param combName: name of the load combinations to deal with
        :param CF:       capacity factor (efficiency) (defaults to -1.0; CF<1.0 -> Ok; CF>1.0 -> KO)
        :param N:        axial force (defaults to 0.0)
        '''
        super(CFN,self).__init__(combName,N)
        self.CF= CF # Capacity factor or efficiency

    def getCF(self):
        ''' Return the capacity factor.'''
        return self.CF

class CFNMy(NMy):
    '''Uniaxial bending. Normal stresses limit state variables.

    :ivar CF:       capacity factor (efficiency) (defaults to -1.0; CF<1.0 -> Ok; CF>1.0 -> KO)
    '''
    def __init__(self,combName= 'nil',CF= -1.0,N= 0.0,My= 0.0):
        '''
        Constructor.

        :param combName: name of the load combinations to deal with
        :param CF:       capacity factor (efficiency) (defaults to -1.0; CF<1.0 -> Ok; CF>1.0 -> KO)
        :param N:        axial force (defaults to 0.0)
        :param My:       bending moment about Y axis (defaults to 0.0)
        '''
        super(CFNMy,self).__init__(combName,N,My)
        self.CF= CF # Capacity factor or efficiency

    def getCF(self):
        ''' Return the capacity factor.'''
        return self.CF

class ShVy(ControlVarsBase):
    '''Shear along Y axis. Internal forces [Vy] for a combination.

    :ivar combName: name of the load combinations to deal with
    :ivar Vy:       shear along Y axis (defaults to 0.0)
    '''
    def __init__(self,combName= 'nil',Vy= 0.0):
        ''' Constructor.

        :param combName: name of the load combinations to deal with
        :param Vy:       shear along Y axis (defaults to 0.0)
        '''
        super(ShVy,self).__init__(combName)
        self.Vy= Vy #Shear along y axis.

    def getAnsysStrings(self,eTag,axis, factor= 1e-3):
        ''' Returns a string to represent fields in ANSYS (R).

        :param eTag: element identifier.
        :param axis: section 1 or 2
        :param factor: factor for units (default 1e-3 -> kN)'''
        retval= super(ShVy,self).getAnsysStrings(eTag,axis,factor)
        retval.append("detab,"+str(eTag)+",Vy" +axis+","+str(self.Vy*factor)+"\n")
        return retval

    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= super(ShVy,self).getDict()
        retval.update({'Vy':self.Vy})
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.'''
        super(ShVy,self).setFromDict(dct)
        self.Vy= dct['Vy']

class CFVy(ShVy):
    '''Uniaxial bending. Shear stresses limit state variables.

    :ivar CF:       capacity factor (efficiency) (defaults to -1.0; CF<1.0 -> Ok; CF>1.0 -> KO)
    '''
    def __init__(self,combName= 'nil',CF= -1.0,Vy= 0.0):
        '''
        Constructor.

        :param combName: name of the load combinations to deal with
        :param CF:       capacity factor (efficiency) (defaults to -1.0; CF<1.0 -> Ok; CF>1.0 -> KO)
        :param Vy:       shear along Y axis (defaults to 0.0)
        '''
        super(CFVy,self).__init__(combName,Vy)
        self.CF= CF # Capacity factor or efficiency

    def getCF(self):
        ''' Return the capacity factor.'''
        return self.CF

class ShearYControlVars(CFVy):
    '''Shear along Y axis. Limit state variables [CF,Vy].

    :ivar idSection: section identifier
    '''
    def __init__(self,idSection= 'nil',combName= 'nil',CF= -1.0,Vy= 0.0):
        '''
        Constructor.

        :param idSection: section identifier
        :param combName: name of the load combinations to deal with
        :param CF:       capacity factor (efficiency) (defaults to -1; CF<1.0 -> Ok; CF>1.0 -> KO)
        :param Vy:       shear along Y axis (defaults to 0.0)
        '''
        super(ShearYControlVars,self).__init__(combName,CF,Vy)
        self.idSection= idSection # Section identifier.

    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= super(ShearYControlVars,self).getDict()
        retval.update({'idSection':self.idSection})
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.'''
        super(ShearYControlVars,self).setFromDict(dct)
        self.idSection= dct['idSection']

class UniaxialBendingControlVars(CFNMy):
    '''Uniaxial bending. Normal stresses limit state variables [CF,N,My].

    :ivar idSection: section identifier
    '''
    def __init__(self,idSection= 'nil',combName= 'nil',CF= -1.0,N= 0.0,My= 0.0):
        '''
        Constructor.

        :param idSection: section identifier
        :param combName: name of the load combinations to deal with
        :param CF:       capacity factor (efficiency) (defaults to -1; CF<1.0 -> Ok; CF>1.0 -> KO)
        :param N:        axial force (defaults to 0.0)
        :param My:       bending moment about Y axis (defaults to 0.0)
        '''
        super(UniaxialBendingControlVars,self).__init__(combName,CF,N,My)
        self.idSection= idSection # Section identifier.

    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= super(UniaxialBendingControlVars,self).getDict()
        retval.update({'idSection':self.idSection})
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.'''
        super(UniaxialBendingControlVars,self).setFromDict(dct)
        self.idSection= dct['idSection']

class CFNMyMz(CFNMy):
    '''Biaxial bending. Normal stresses limit state variables. [CF,N,My,Mz].

    :ivar Mz:       bending moment about Z axis (defaults to 0.0)
    '''
    def __init__(self,combName= 'nil',CF= -1.0,N= 0.0,My= 0.0,Mz= 0.0):
        '''
        Constructor.

        :param combName: name of the load combinations to deal with
        :param CF:       capacity factor (efficiency) (defaults to -1; CF<1.0 -> Ok; CF>1.0 -> KO)
        :param N:        axial force (defaults to 0.0)
        :param My:       bending moment about Y axis (defaults to 0.0)
        :param Mz:       bending moment about Z axis (defaults to 0.0)
        '''
        super(CFNMyMz,self).__init__(combName,CF,N,My)
        self.Mz= Mz #Bending moment about z axis.

    def getAnsysStrings(self,eTag,axis, factor= 1e-3):
        ''' Returns a string to represent fields in ANSYS (R).

        :param eTag: element identifier.
        :param axis: section 1 or 2
        :param factor: factor for units (default 1e-3 -> kN)'''
        retval= super(CFNMyMz,self).getAnsysStrings(eTag,axis,factor)
        retval.append("detab,"+str(eTag)+",Mz" +axis+","+str(self.Mz*factor)+"\n")
        return retval

    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= super(CFNMyMz,self).getDict()
        retval.update({'Mz':self.Mz})
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.'''
        super(CFNMyMz,self).setFromDict(dct)
        self.Mz= dct['Mz']

class AxialForceControlVars(CFN):
    '''Axial force. Internal forces [N] for a combination.

    :ivar idSection: section identifier
    '''
    def __init__(self,idSection= 'nil',combName= 'nil',N= 0.0):
        '''
        Constructor.

        :param idSection: section identifier
        :param combName: name of the load combinations to deal with
        :param N:        axial force (defaults to 0.0)
        '''
        super(AxialForceControlVars,self).__init__(combName, N)
        self.idSection= idSection # Section identifier.

    def getAnsysStrings(self,eTag,axis, factor= 1e-3):
      ''' Returns a string to represent fields in ANSYS (R).

      :param eTag: element identifier.
      :param axis: section 1 or 2
      :param factor: factor for units (default 1e-3 -> kN)'''
      retval= 'idSection= "' + self.idSection +'", ' 
      retval+= super(AxialForceControlVars,self).getAnsysStrings(eTag,axis,factor)
      return retval

    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= super(AxialForceControlVars,self).getDict()
        retval.update({'idSection':self.idSection})
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.'''
        super(AxialForceControlVars,self).setFromDict(dct)
        self.idSection= dct['idSection']

        
class BiaxialBendingControlVars(UniaxialBendingControlVars):
    '''Biaxial bending. Normal stresses limit state variables. [CF,N,My,Mz].

    :ivar Mz:       bending moment about Z axis (defaults to 0.0)
    '''
    def __init__(self,idSection= 'nil',combName= 'nil',CF= -1.0,N= 0.0,My= 0.0,Mz= 0.0):
        '''
        Constructor.

        :param idSection: section identifier
        :param combName: name of the load combinations to deal with
        :param CF:       capacity factor (efficiency) (defaults to -1)
        :param N:        axial force (defaults to 0.0)
        :param My:       bending moment about Y axis (defaults to 0.0)
        :param Mz:       bending moment about Z axis (defaults to 0.0)
        '''
        super(BiaxialBendingControlVars,self).__init__(idSection,combName,CF,N,My)
        self.Mz= Mz #Bending moment about z axis.

    def getAnsysStrings(self,eTag,axis, factor= 1e-3):
        ''' Returns a string to represent fields in ANSYS (R).

        :param eTag: element identifier.
        :param axis: section 1 or 2
        :param factor: factor for units (default 1e-3 -> kN)'''
        retval= super(BiaxialBendingControlVars,self).getAnsysStrings(eTag,axis,factor)
        retval.append("detab,"+str(eTag)+",Mz" +axis+","+str(self.Mz*factor)+"\n")
        return retval

    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= super(BiaxialBendingControlVars,self).getDict()
        retval.update({'Mz':self.Mz})
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.'''
        super(BiaxialBendingControlVars,self).setFromDict(dct)
        self.Mz= dct['Mz']
    
class BiaxialBendingStrengthControlVars(BiaxialBendingControlVars):
    '''Control variables for biaxial bending normal stresses LS 
    verification en steel-shape elements.

    :ivar Ncrd:  design strength to axial compression
    :ivar McRdy: design moment strength about Y (weak) axis
    :ivar McRdz: design moment strength about Z (strong) axis
    :ivar chiLT: reduction factor for lateral-torsional buckling (defaults to 1)
    :ivar chiN:  reduction factor for compressive strength (defaults to 1)
    '''
    def __init__(self,idSection= 'nil',combName= 'nil',CF= -1.0,N= 0.0,My= 0.0,Mz= 0.0,Ncrd=0.0,McRdy=0.0,McRdz=0.0,chiLT=1.0, chiN= 1.0):
        '''
        Constructor.

        :param idSection: section identifier
        :param combName: name of the load combinations to deal with
        :param CF:       capacity factor (efficiency) (defaults to -1)
        :param N:        axial force (defaults to 0.0)
        :param My:       bending moment about Y axis (defaults to 0.0)
        :param Mz:       bending moment about Z axis (defaults to 0.0)
        :param Ncrd:     design strength to axial compression
        :param McRdy:    design moment strength about Y (weak) axis
        :param McRdz:    design moment strength about Z (strong) axis
        :param chiLT:    reduction factor for lateral-torsional buckling (defaults to 1)
        :param chiN:     reduction factor for compressive strength (defaults to 1)
        '''
        super(BiaxialBendingStrengthControlVars,self).__init__(idSection,combName,CF,N,My,Mz)
        self.Ncrd=Ncrd
        self.McRdy=McRdy
        self.McRdz=McRdz
        self.chiLT=chiLT
        self.chiN= chiN
        
    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= super(BiaxialBendingControlVars,self).getDict()
        retval.update({'Ncrd':self.Ncrd, 'McRdy':self.McRdy, 'McRdz':self.McRdz, 'chiLT':self.chiLT, 'chiN':self.chiN})
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.'''
        super(BiaxialBendingControlVars,self).setFromDict(dct)
        self.Ncrd= dct['Ncrd']
        self.McRdy= dct['McRdy']
        self.McRdz= dct['McRdz']
        self.chiLT= dct['chiLT']
        self.chiN= dct['chiN']
        
class SteelShapeBiaxialBendingControlVars(BiaxialBendingStrengthControlVars):
    '''Control variables for biaxial bending normal stresses LS 
    verification en steel-shape elements.

    :ivar MvRdz:    reduced design moment strength about Z (strong) axis for shear interaction
    :ivar MbRdz:    reduced design moment strength about Z (strong) axis for lateral-torsional bucking
    '''
    def __init__(self,idSection= 'nil',combName= 'nil',CF= -1.0,N= 0.0,My= 0.0,Mz= 0.0,Ncrd=0.0,McRdy=0.0,McRdz=0.0,MvRdz=0.0,MbRdz=0.0, chiLT=1.0, chiN=1.0):
        '''
        Constructor.

        :param idSection: section identifier
        :param combName: name of the load combinations to deal with
        :param CF:       capacity factor (efficiency) (defaults to -1)
        :param N:        axial force (defaults to 0.0)
        :param My:       bending moment about Y axis (defaults to 0.0)
        :param Mz:       bending moment about Z axis (defaults to 0.0)
        :param Ncrd:     design strength to axial compression
        :param McRdy:    design moment strength about Y (weak) axis
        :param McRdz:    design moment strength about Z (strong) axis
        :param MvRdz:    reduced design moment strength about Z (strong) axis for shear interaction
        :param MbRdz:    reduced design moment strength about Z (strong) axis for lateral-torsional bucking
        :param chiLT:    reduction factor for lateral-torsional buckling (defaults to 1)
        :param chiN:     reduction factor for compressive strength (defaults to 1)
        '''
        super(SteelShapeBiaxialBendingControlVars,self).__init__(idSection= idSection,combName= combName,CF= CF,N= N, My= My, Mz= Mz, Ncrd= Ncrd, McRdy= McRdy, McRdz= McRdz, chiLT=chiLT, chiN= chiN)
        self.MvRdz=MvRdz
        self.MbRdz=MbRdz
        
    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= super(SteelShapeBiaxialBendingControlVars,self).getDict()
        retval.update({'MvRdz':self.MvRdz, 'MbRdz':self.MbRdz})
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.'''
        super(SteelShapeBiaxialBendingControlVars,self).setFromDict(dct)
        self.MvRdz= dct['MvRdz']
        self.MbRdz= dct['MbRdz']

class RCShearControlVars(BiaxialBendingControlVars):
    '''Control variables for shear limit state verification in reinforced concrete elements.

    :ivar Mu:       ultimate bending moment
    :ivar Vy:       shear force parallel to the y axis
    :ivar Vz:       shear force parallel to the z axis
    :ivar theta:    angle between the concrete compression struts and the beam axis
    :ivar Vcu:      Vcu component of the shear strength (defined in the codes)
    :ivar Vsu:      Vsu component of the shear strength (defined in the codes)
    :ivar Vu:       shear strength

    '''
    def __init__(self,idSection=-1,combName= 'nil',CF= -1.0,N= 0.0, My= 0.0, Mz= 0.0, Mu= 0.0, Vy= 0.0, Vz= 0.0, theta= 0.0, Vcu= 0.0, Vsu= 0.0, Vu= 0.0):
        '''
        Constructor.

        :param idSection: section identifier
        :param combName: name of the load combinations to deal with
        :param CF:       capacity factor (efficiency) (defaults to -1)
        :param N:        axial force (defaults to 0.0)
        :param My:       bending moment about Y axis (defaults to 0.0)
        :param Mz:       bending moment about Z axis (defaults to 0.0)
        :param Mu:       ultimate bending moment
        :param Vy:       shear force parallel to the y axis
        :param Vz:       shear force parallel to the z axis
        :param theta:    angle between the concrete compression struts and the beam axis
        :param Vcu:      Vcu component of the shear strength (defined in the codes)
        :param Vsu:      Vsu component of the shear strength (defined in the codes)
        :param Vu:       shear strength
        '''
        super(RCShearControlVars,self).__init__(idSection,combName,CF,N,My,Mz)
        self.Mu= Mu #Ultimate bending moment.
        self.Vy= Vy #Shear parallel to the y axis.
        self.Vz= Vz #Shear parallel to the z axis.
        self.theta= theta #Strut angle.
        self.Vcu= Vcu #Vcu component of the shear strength (defined in the codes).
        self.Vsu= Vsu #Vsu component of the shear strength (defined in the codes).
        self.Vu= Vu # Shear strength.

    def getAnsysStrings(self,eTag,axis, factor= 1e-3):
        ''' Returns a string to represent fields in ANSYS (R).

        :param eTag: element identifier.
        :param axis: section 1 or 2
        :param factor: factor for units (default 1e-3 -> kN)'''
        retval= super(RCShearControlVars,self).getAnsysStrings(eTag,axis,factor)
        retval.append("detab,"+str(eTag)+",Mu" +axis+","+str(self.Mu*factor)+"\n")
        retval.append("detab,"+str(eTag)+",Vy" +axis+","+str(self.Vy*factor)+"\n")
        retval.append("detab,"+str(eTag)+",Vz" +axis+","+str(self.Vz*factor)+"\n")
        retval.append("detab,"+str(eTag)+",Vcu" +axis+","+str(self.Vcu*factor)+"\n")
        retval.append("detab,"+str(eTag)+",Vsu" +axis+","+str(self.Vsu*factor)+"\n")
        retval.append("detab,"+str(eTag)+",Vu" +axis+","+str(self.Vu*factor)+"\n")
        return retval

    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= super(RCShearControlVars,self).getDict()
        retval.update({'Mu':self.Mu, 'Vy':self.Vy, 'Vz':self.Vz, 'theta':self.theta, 'Vcu':self.Vcu, 'Vsu':self.Vsu, 'Vu':self.Vu})
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.'''
        super(RCShearControlVars,self).setFromDict(dct)
        self.Mu= dct['Mu']
        self.Vy= dct['Vy']
        self.Vz= dct['Vz']
        self.theta= dct['theta']
        self.Vcu= dct['Vcu']
        self.Vsu= dct['Vsu']
        self.Vu= dct['Vu']

class CrackControlBaseVars(CFNMyMz):
    '''Biaxial bending. Cracking serviceability limit state variables.

    :ivar steelStress: maximum stress in the reinforcement bars
    '''
    def __init__(self,combName= 'nil',CF= -1.0,N= 0.0, My= 0.0, Mz= 0.0, steelStress= 0.0):
        '''
        Constructor.

        :param combName: name of the load combinations to deal with
        :param CF:       capacity factor (efficiency)
        :param N:        axial force
        :param My:       bending moment about Y axis
        :param Mz:       bending moment about Z axis
        :param steelStress: maximum stress in the reinforcement bars
        '''
    #    super(CrackControlBaseVars,self).__init__(combName,CF,N,My) #Jan 26th 2017
        super(CrackControlBaseVars,self).__init__(combName,CF,N,My,Mz)
        self.steelStress= steelStress #Stress in rebars.

    def getAnsysStrings(self,eTag,axis, factor= 1e-3):
        ''' Returns a string to represent fields in ANSYS (R).

        :param eTag: element identifier.
        :param axis: section 1 or 2
        :param factor: factor for units (default 1e-3 -> kN)'''
        retval= super(CrackControlBaseVars,self).getAnsysStrings(eTag,axis,factor)
        retval.append("detab,"+str(eTag)+",steelStress" +axis+","+str(self.steelStress*factor)+"\n")
        return retval

    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= super(CrackControlBaseVars,self).getDict()
        retval.update({'steelStress':self.steelStress})
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.'''
        super(CrackControlBaseVars,self).setFromDict(dct)
        self.steelStress= dct['steelStress']


class CrackControlVars(ControlVarsBase):
    '''Cracking serviceability limit state control variables.

    :ivar idSection: section identifier
    :ivar crackControlVarsPos:     Crack control in + face.
    :ivar crackControlVarsNeg:     Crack control in - face.
    '''
    def __init__(self,idSection= 'nil', crackControlBaseVarsPos= None ,crackControlBaseVarsNeg= None):
        '''
        Constructor.

        :param idSection: section identifier
        :param crackControlVarsPos:     Crack control in + face.
        :param crackControlVarsNeg:     Crack control in - face.
        '''
        self.idSection= idSection # Section identifier.
        if(crackControlBaseVarsPos):
            self.crackControlVarsPos= crackControlBaseVarsPos #Cracks in + face.
        else:
            self.crackControlVarsPos= CrackControlBaseVars()
        if(crackControlBaseVarsNeg):
            self.crackControlVarsNeg= crackControlBaseVarsNeg #Cracks in - face.
        else:
            self.crackControlVarsNeg= CrackControlBaseVars()

    def getCF(self):
        ''' Return the capacity factor.'''
        return max(self.crackControlVarsPos.getCF(),self.crackControlVarsNeg.getCF())

    def getMaxSteelStress(self):
        '''Maximum value for rebar stresses.'''
        return max(self.crackControlVarsPos.steelStress,self.crackControlVarsNeg.steelStress)

    def getMaxN(self):
        '''Maximum internal axial force.'''
        return max(self.crackControlVarsPos.N,self.crackControlVarsNeg.N)

    def getStrArguments(self):
        '''Returns a string for a 'copy' (kind of) constructor.'''
        retval= 'idSection= "' + self.idSection + '"' 
        retval+= ', crackControlBaseVarsPos= ' + self.crackControlVarsPos.getStrConstructor()
        retval+= ', crackControlBaseVarsNeg= ' + self.crackControlVarsNeg.getStrConstructor()
        return retval
    
    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= dict()
        retval.update({'idSection':self.idSection})
        retval.update({'crackControlBaseVarsPos':self.crackControlVarsPos.getStrConstructor()})
        retval.update({'crackControlBaseVarsNeg':self.crackControlVarsNeg.getStrConstructor()})
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.'''
        self.idSection= dct['idSection']
        self.crackControlVarsPos= eval(dct['crackControlBaseVarsPos'])
        self.crackControlVarsNeg= eval(dct['crackControlBaseVarsNeg'])
        

class RCCrackStraightControlVars(NMyMz):
    '''Control variables for cracking serviacebility limit state verification
    when when considering a concrete stress-strain diagram that takes account of 
    the effects of tension stiffening.

    :ivar idSection: section identifier
    :ivar s_rmax:   maximum distance between cracks (otherwise a new crack could occur in-between
    :ivar eps_sm:   mean strain in the reinforcement when taking into account the effects of tension stiffening
    :ivar wk:       crack width
    '''
    def __init__(self,idSection=-1,combName= 'nil',CF=-1,N= 0.0, My= 0.0, Mz= 0.0, s_rmax=0.0,eps_sm=0.0,wk=0.0):
        '''
        Constructor.

        :param idSection: section identifier
        :param combName: name of the load combinations to deal with
        :param N:        axial force
        :param My:       bending moment about Y axis
        :param Mz:       bending moment about Z axis
        :param s_rmax:   maximum distance between cracks (otherwise a new crack could occur in-between
        :param eps_sm:   mean strain in the reinforcement when taking into account the effects of tension stiffening
        :param wk:       crack width
        '''
        super(RCCrackStraightControlVars,self).__init__(combName,N,My,Mz)
        self.idSection=idSection
        self.s_rmax=s_rmax
        self.eps_sm= eps_sm
        self.wk=wk

    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= super(RCCrackStraightControlVars,self).getDict()
        retval.update({'s_rmax':self.s_rmax,'eps_sm':self.eps_sm,'wk': self.wk})
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.'''
        super(RCCrackStraightControlVars,self).setFromDict(dct)
        self.s_rmax= dct['s_rmax']
        self.eps_sm= dct['eps_sm']
        self.wk= dct['wk']
        
  
class FatigueControlBaseVars(NMyMz):
    '''Biaxial bending. Fatigue limit state variables.

    :ivar Vy:       shear force parallel to Y axis.
    :ivar posSteelStress: traction stress in rebars.
    :ivar negSteelStress: compression stress in rebars.
    :ivar concreteStress: compression stress in concrete.
    '''
    def __init__(self,combName= 'nil',CF=-1.0,N= 0.0, My= 0.0, Mz= 0.0, Vy= 0.0, posSteelStress= 0.0, negSteelStress= 0.0, concreteStress= 0.0):
        '''
        Constructor.

        :param combName: name of the load combination to deal with
        :param N:        axial force (defaults to 0.0)
        :param My:       bending moment about Y axis (defaults to 0.0)
        :param Mz:       bending moment about Z axis (defaults to 0.0)
        :param Vy:       shear force parallel to Y axis.
        :param posSteelStress: traction stress in rebars.
        :param negSteelStress: compression stress in rebars.
        :param concreteStress: compression stress in concrete.
        '''
        #Note: Currently, CF attribute  has no sense in fatigue verification. Perhaps, in the future, the maximum value
        #of the CF calculated can be represented by CF. For now, this attribute remains in this class only for a purpose
        #of compatibility with the parent classes
        super(FatigueControlBaseVars,self).__init__(combName,N,My,Mz)
        self.Vy= Vy #Shear.
        self.posSteelStress= posSteelStress #traction stress in rebars.
        self.negSteelStress= negSteelStress #compression stress in rebars.
        self.concreteStress= concreteStress #compression stress in concrete.

    def getAnsysStrings(self,eTag,axis, factor= 1e-3):
        ''' Returns a string to represent fields in ANSYS (R).

        :param eTag: element identifier.
        :param axis: section 1 or 2
        :param factor: factor for units (default 1e-3 -> kN)'''
        retval= super(FatigueControlBaseVars,self).getAnsysStrings(eTag,axis,factor)
        retval.append("detab,"+str(eTag)+",Vy" +axis+","+str(self.Vy*factor)+"\n")
        retval.append("detab,"+str(eTag)+",posSteelStress" +axis+","+str(self.posSteelStress*factor*factor)+"\n")
        retval.append("detab,"+str(eTag)+",negSteelStress" +axis+","+str(self.negSteelStress*factor*factor)+"\n")
        retval.append("detab,"+str(eTag)+",concreteStress" +axis+","+str(self.concreteStress*factor*factor)+"\n")
        return retval

    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= super(FatigueControlBaseVars,self).getDict()
        retval.update({'Vy':self.Vy, 'negSteelStress':self.negSteelStress, 'posSteelStress':self.posSteelStress, 'concreteStress': self.concreteStress})
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.'''
        super(FatigueControlBaseVars,self).setFromDict(dct)
        self.Vy= dct['Vy']
        self.negSteelStress= dct['negSteelStress']
        self.posSteelStress= dct['posSteelStress']
        self.concreteStress= dct['concreteStress']

class FatigueControlVars(ControlVarsBase):
    '''Fatigue limit state control variables.

    :ivar idSection: section identifier
    :ivar combName:  name of the load combinations to deal with
    :ivar state0:    Fatigue values (FatigueControlBaseVars) under permanent load.
    :ivar state1:    Fatigue values (FatigueControlBaseVars) under fatigue load.
    :ivar concreteLimitStress: limit for the concrete stress as specified in SIA 262(2013)  4.3.8.3.1
    :ivar concreteBendingCF:   concrete capacity factor under fatigue due to normal stresses.
    :ivar shearLimit:        limit for the shear force as sepecified in SIA 262(2013)  4.3.8.3.2
    :ivar concreteShearCF:   concrete capacity factor under fatigue due to shear forces.
    :ivar Mu:        ultimate bending moment
    :ivar Vu:        ultimate shear force  
    '''
    def __init__(self,idSection= 'nil', controlBaseVars0= None, controlBaseVars1= None, concreteLimitStress= 0.0,concreteBendingCF=-1.0,shearLimit=0.0,concreteShearCF=-1.0,Mu=0.0,Vu=0.0):
        '''
        Constructor.

        :param idSection: section identifier
        :param combName:  name of the load combinations to deal with
        :param state0:    Fatigue values (FatigueControlBaseVars) under permanent load.
        :param state1:    Fatigue values (FatigueControlBaseVars) under fatigue load.
        :param concreteLimitStress: limit for the concrete stress as specified in SIA 262(2013)  4.3.8.3.1
        :param concreteBendingCF:   concrete capacity factor under fatigue due to normal stresses.
        :param shearLimit:        limit for the shear force as sepecified in SIA 262(2013)  4.3.8.3.2
        :param concreteShearCF:   concrete capacity factor under fatigue due to shear forces.
        :param Mu:        ultimate bending moment
        :param Vu:        ultimate shear force  
        '''
        self.idSection= idSection # Section identifier.
        if(controlBaseVars0):
            self.state0=  controlBaseVars0 #Under permanent loads.
        else:
            self.state0= FatigueControlBaseVars()
        if(controlBaseVars1):
            self.state1= controlBaseVars1 #Under fatigue loads.
        else:
            self.state1= FatigueControlBaseVars()
        self.concreteLimitStress= concreteLimitStress #SIA 262(2013)  4.3.8.3.1
        self.concreteBendingCF= concreteBendingCF #Concrete capacity factor.
        self.shearLimit=shearLimit #SIA 262(2013)  4.3.8.3.2
        self.concreteShearCF= concreteShearCF #Concrete shear factor.
        self.Mu= Mu #Ultimate bending moment.
        self.Vu= Vu #Ultimate shear.

    def getSteelPosStressIncrement(self):
        '''Returns positive stress increment in rebars.'''
        return self.state1.posSteelStress-self.state0.posSteelStress

    def getSteelNegStressIncrement(self):
        '''Returns negative stress increment in rebars.'''
        return self.state1.negSteelStress-self.state0.negSteelStress
      
    def getAbsSteelStressIncrement(self):
        '''Returns maximun stress increment in rebars (absolute value).'''
        return max(self.getSteelPosStressIncrement(),-self.getSteelNegStressIncrement())

    def getConcreteStressIncrement(self):
        '''Returns stress increment in concrete.'''
        return self.state1.concreteStress-self.state0.concreteStress

    def getConcreteMaxMinStresses(self):
        '''Used in FatigueController.concreteLimitStress.''' 
        retval= [0.0,0.0]
        retval[0]= min(min(self.state0.concreteStress,self.state1.concreteStress),0.0) # No positive stresses.
        retval[1]= min(max(self.state0.concreteStress,self.state1.concreteStress),0.0) # No positive stresses.
        return retval

    def getConcreteMinStress(self):
        '''Returns minimum (max. compressive) concrete stress between loaded and unloaded states.'''
        sgc0= abs(min(self.state0.concreteStress,0.0))
        sgc1= abs(min(self.state1.concreteStress,0.0))
        return max(sgc0,sgc1)

    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= dict()
        retval.update({'idSection':self.idSection})
        retval.update({'controlBaseVars0':self.state0.getStrConstructor()})
        retval.update({'controlBaseVars1':self.state1.getStrConstructor()})
        retval.update({'concreteLimitStress':self.concreteLimitStress, 'concreteBendingCF':self.concreteBendingCF, 'shearLimit':self.shearLimit, 'concreteShearCF':self.concreteShearCF, 'Mu':self.Mu, 'Vu':self.Vu})
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.'''
        self.idSection= dct['idSection']
        self.controlBaseVars0= eval(dct['controlBaseVars0'])
        self.controlBaseVars1= eval(dct['controlBaseVars1'])
        self.concreteLimitStress= dct['concreteLimitStress']
        self.concreteBendingCF= dct['concreteBendingCF']
        self.shearLimit= dct['shearLimit']
        self.concreteShearCF= dct['concreteShearCF']
        self.Mu= dct['Mu']
        self.Vu= dct['Vu']
  
class VonMisesControlVars(ControlVarsBase):
    '''Von Mises stresses control vars.

    :ivar von_mises_stress:  Von Mises stress (defaults to 0.0)
    :ivar CF: capacity factor (efficiency) (defaults to -1.0; CF<1.0 -> Ok; CF>1.0 -> KO)
    '''
    def __init__(self,combName= 'nil', CF= -1.0, vm_stress= 0.0):
        ''' Constructor.

        :param combName: name of the load combinations to deal with
        :param vm_stress: von mises stress (defaults to 0.0)
        :param CF: capacity factor (efficiency) (defaults to -1.0; CF<1.0 -> Ok; CF>1.0 -> KO)
        '''
        super(VonMisesControlVars,self).__init__(combName)
        self.vm_stress= vm_stress #Shear along y axis.
        self.CF= CF # Capacity factor or efficiency

    def getDict(self):
        ''' Return a dictionary containing the object data.'''
        retval= super(VonMisesControlVars,self).getDict()
        retval.update({'vm_stress':self.vm_stress})
        return retval
       
    def setFromDict(self,dct):
        ''' Set the data values from the dictionary argument.'''
        super(VonMisesControlVars,self).setFromDict(dct)
        self.vm_stress= dct['vm_stress']

    def getCF(self):
        ''' Return the capacity factor.'''
        return self.CF

def writeControlVarsFromPhantomElements(preprocessor,outputFileName,outputCfg):
    '''Writes in file 'outputFileName' the control-variable values calculated for
     the RC elements in the phantom model.

    :param preprocessor:   preprocessor from FEA model.
    :param outputFileName: name to the files (.py and .tex)
    :param outputCfg: instance of class 'VerifOutVars' which defines the 
           variables that control the output of the checking (append or not
           the results to a file, generation or not of lists, ...)
    '''
    elems= preprocessor.getSets['total'].elements
    controlVarName= outputCfg.controller.limitStateLabel
    if outputCfg.appendToResFile.lower()[0]=='y':
        xcOutput= open(outputFileName+".py","a+")
    else:
        xcOutput= open(outputFileName+".py","w+")
    for e in elems:
        eTag= e.getProp("idElem") 
        idSection= e.getProp("idSection")  
        controlVar= e.getProp(controlVarName)
        outStr= controlVar.getLaTeXString(eTag,1e-3)
        sectionName= 'Sect'+str(e.getProp('dir'))
        xcOutput.write(controlVar.strElementProp(eTag,controlVarName+sectionName))
    xcOutput.close()
    if outputCfg.listFile.lower()[0]=='y':
        if outputCfg.appendToResFile.lower()[0]=='y':
            texOutput= open(outputFileName+".tex","a+")
        else:
            texOutput= open(outputFileName+".tex","w+")
        texOutput.write("Section 1\n")
        for e in elems:
            if(e.getProp("dir")==1):
                eTag= e.getProp("idElem") 
                idSection= e.getProp("idSection")  
                controlVar= e.getProp(controlVarName)
                outStr= controlVar.getLaTeXString(eTag,1e-3)
                texOutput.write(outStr)
        texOutput.write("Section 2\n")
        for e in elems:
            if(e.getProp("dir")==2):
                eTag= e.getProp("idElem") 
                idSection= e.getProp("idSection")  
                controlVar= e.getProp(controlVarName)
                outStr= controlVar.getLaTeXString(eTag,1e-3)
                texOutput.write(outStr)
        texOutput.close()
    retval=None
    if outputCfg.calcMeanCF.lower()[0]=='y':
        fcs1= [] #Capacity factors at section 1.
        fcs2= [] #Capacity factors at section 2.
        for e in elems:
            if(e.getProp("dir")==1):
                controlVar= e.getProp(controlVarName)
                fcs1.append(controlVar.getCF())
            if(e.getProp("dir")==2):
                controlVar= e.getProp(controlVarName)
                fcs2.append(controlVar.getCF())
        retval= [scipy.mean(fcs1),scipy.mean(fcs2)]
    return retval

def getControlVarImportModuleStr(preprocessor, outputCfg, sections):
    '''Return the string to import the module where the control var is defined.

    :param preprocessor:    preprocessor from FEA model.
    :param outputCfg: instance of class 'VerifOutVars' which defines the 
           variables that control the output of the checking (set of 
           elements to be analyzed [defaults to 'total'], append or not the 
           results to the result file [defatults to 'N'], generation or not of 
           list file [defatults to 'N', ...)
    :param sections: names of the sections to write the output for.
    '''
    retval= None
    elems= outputCfg.getCalcSetElements(preprocessor) # elements in set 'setCalc'
    controlVarName= outputCfg.controller.limitStateLabel
    if(len(elems)>0):
        e0= elems[0]
        s= sections[0]
        propName= controlVarName+s
        controlVar= e0.getProp(propName)
        retval= controlVar.getModuleImportString()
    else:
        lmsg.error('element set is empty.')
    return retval

def writeControlVarsFromElements(preprocessor, outputFileName, outputCfg, sections):
    '''Writes in file 'outputFileName' the control-variable values calculated for elements in set 'setCalc'. 

    :param preprocessor:    preprocessor from FEA model.
    :param outputFileName: name of the files to write (.py and .tex)
    :param outputCfg: instance of class 'VerifOutVars' which defines the 
           variables that control the output of the checking (set of 
           elements to be analyzed [defaults to 'total'], append or not the 
           results to the result file [defatults to 'N'], generation or not of 
           list file [defatults to 'N', ...)
    :param sections: names of the sections to write the output for.
    '''
    elems= outputCfg.getCalcSetElements(preprocessor) # elements in set 'setCalc'
    controlVarName= outputCfg.controller.limitStateLabel
    if outputCfg.appendToResFile.lower()[0]=='y':
        xcOutput= open(outputFileName+".py","a+")
    else:
        xcOutput= open(outputFileName+".py","w+")
    importString= getControlVarImportModuleStr(preprocessor, outputCfg, sections)
    xcOutput.write(importString+'\n')
    for e in elems:
        for s in sections:
            propName= controlVarName+s
            controlVar= e.getProp(propName)
            xcOutput.write(controlVar.strElementProp(e.tag,propName))
    xcOutput.close()
    if outputCfg.listFile.lower()[0]=='y':
        if outputCfg.appendToResFile.lower()[0]=='y':
            texOutput= open(outputFileName+".tex","a+")
        else:
            texOutput= open(outputFileName+".tex","w+")
        for s in sections:
            texOutput.write("Section: "+s+"\n")
            propName= controlVarName+s
            for e in elems:
                controlVar= e.getProp(propName)
                outStr= controlVar.getLaTeXString(e.tag,1e-3)
                texOutput.write(outStr)
        texOutput.close()
    retval=None
    if outputCfg.calcMeanCF.lower()[0]=='y':
        retval= list() # mean capacity factors
        for s in sections:
            sFcs= list()
            propName= controlVarName+s
            for e in elems:
                controlVar= e.getProp(propName)
                sFcs.append(controlVar.getCF())
            retval.append(scipy.mean(sFcs))
    return retval


def writeControlVarsFromElementsForAnsys(preprocessor,outputFileName, sectionName1, sectionName2):
    '''
    :param   preprocessor:    preprocessor name
    :param   outputFileName:  name of the output file containing tue results of the 
                       verification 
    '''
    controlVarName= outputCfg.controller.limitStateLabel
    texOutput1= open("/tmp/texOutput1.tmp","w")
    texOutput1.write("Section 1\n")
    texOutput2= open("/tmp/texOutput2.tmp","w")
    texOutput2.write("Section 2\n")
    ansysOutput1= open(outputFileName+".mac","w")
    ansysOutput2= open(outputFileName+"esf.mac","w")
    #printCabeceraListadoCapacityFactor("texOutput1","1 ("+ sectionName1 +")")
    #printCabeceraListadoCapacityFactor("texOutput2","2 ("+ sectionName2 +")")
    fcs1= [] #Capacity factors at section 1.
    fcs2= [] #Capacity factors at section 2.
    elems= preprocessor.getSets["total"].elements
    for e in elems:
        eTag= e.getProp("idElem")
        controlVar= e.getProp(controlVarName)
        outStr= controlVar.getLaTeXString(eTag,1e-3)
        if(odd(e.tag)):
            fcs1.append(controlVar.getCF())
            texOutput1.write(outStr)
            ansOut= controlVal.getAnsysStrings(eTag,'1',1e-3)
            for s in ansOut:
                ansysOutput1.write(s)
        else:
            fcs2.append(controlVar.getCF())
            texOutput2.write(outStr)
            ansOut= controlVal.getAnsysStrings(eTag,'2',1e-3)
            for s in ansOut:
                ansysOutput2.write(s)
    #printCierreListadoCapacityFactor("texOutput1")
    #printCierreListadoCapacityFactor("texOutput2")
    texOutput1.close()
    texOutput2.close()
    ansysOutput1.close()
    ansysOutput2.close()
    os.system("cat /tmp/texOutput1.tmp /tmp/texOutput2.tmp > "+outputFileName+".tex")
    # os.system("rm -f "+"/tmp/acciones.xci")
    # os.system("rm -f "+"/tmp/cargas.xci")
    # os.system("rm -f "+"/tmp/elems.xci")
    os.system("rm -f "+"/tmp/texOutput1.tmp")
    os.system("rm -f "+"/tmp/texOutput2.tmp")
    retval= [scipy.mean(fcs1),scipy.mean(fcs2)]
    return retval

def extrapolate_control_var(elemSet,propName,argument,initialValue= 0.0):
    '''Extrapolates element's function values to the nodes.

     :param elemSet: set of elements.
     :param propName: name of the property that contains the control variables.
     :param function: name of the function to call for each element.
     :param argument: name of the control variable to extrapolate.
     :param initialValue: initial value for the prop defined at the nodes.
    '''
    elemSet.fillDownwards()
    eSet= elemSet.elements
    nodePropName= propName+'_'+argument
    nodeTags= ext.create_attribute_at_nodes(eSet,nodePropName,initialValue)
    #Calculate totals.
    for e in eSet:
        elemNodes= e.getNodes
        sz= len(elemNodes) 
        for i in range(0,sz):
            n= elemNodes[i]
            controlVar= e.getProp(propName)
            if(controlVar):
                value= controlVar(argument)
                oldValue= n.getProp(nodePropName)
                n.setProp(nodePropName,oldValue+value)
    #Divide by number of elements in the set that touch the node.
    preprocessor= elemSet.getPreprocessor
    for tag in nodeTags:
        n= preprocessor.getNodeHandler.getNode(tag)
        denom= nodeTags[tag]
        newValue= n.getProp(nodePropName)/denom
        n.setProp(nodePropName,newValue)
    return nodePropName

