# -*- coding: utf-8 -*-
from __future__ import print_function
from __future__ import division
'''Classes to store limit state control variables (internal forces, 
   strains, stresses,...) calculated in the analysis.
   THESE CLASSES ARE INTENDED TO REPLACE THE PROPERTIES DEFINED
   IN def_vars_control.py WICH MUST DISSAPEAR.
'''

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
        return self.combName

    def getLaTeXString(self,eTag,factor= 1e-3):
        ''' Returns a string that we can insert in a LaTeX table.

        :param eTag:   element identifier.
        :param factor: factor for units (default 1e-3 -> kN)'''
        return str(eTag)+" & "+self.getLaTeXFields(factor)+" & "+fmt.Esf.format(self.getCF())+"\\\\\n"

    def getAnsysStrings(self,eTag,axis, factor= 1e-3):
        ''' Returns a string to represent fields in ANSYS (R).

        :param eTag:   element identifier.
        :param axis:   section 1 or 2
        :param factor: factor for units (default 1e-3 -> kN)'''
        retval= []
        retval.append("detab,"+str(eTag)+",CF" +axis+","+str(self.getCF())+"\n")
        return retval

    def getStrArguments(self):
        '''Returns a string for a 'copy' (kind of) constructor.

        '''
        retval= 'combName= "' + self.combName 
        retval+= '", CF=' + str(self.getCF())
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

    def getLaTeXFields(self,factor= 1e-3):
      ''' Returns a string with the intermediate fields of the LaTeX string.

      :param factor: factor for units (default 1e-3 -> kN)'''
      retval= super(N,self).getLaTeXFields(factor)
      retval+= " & "+fmt.Esf.format(self.N*factor)
      return retval

    def getAnsysStrings(self, eTag, axis, factor= 1e-3):
      ''' Returns a string to represent fields in ANSYS (R).

      :param eTag: element identifier.
      :param axis: section 1 or 2
      :param factor: factor for units (default 1e-3 -> kN)
      '''
      retval= super(N,self).getAnsysStrings(eTag,axis,factor)
      retval.append("detab,"+str(eTag)+",N" +axis+","+str(self.N*factor)+"\n")
      return retval

    def getStrArguments(self):
      '''Returns a string for a 'copy' (kind of) constructor.'''
      retval= super(N,self).getStrArguments()
      retval+= ',N= ' + str(self.N) 
      return retval
  
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

    def getLaTeXFields(self,factor= 1e-3):
        ''' Returns a string with the intermediate fields of the LaTeX string.

        :param factor: factor for units (default 1e-3 -> kN)'''
        retval= super(NMy,self).getLaTeXFields(factor)
        retval+= " & "+fmt.Esf.format(self.My*factor)
        return retval

    def getAnsysStrings(self,eTag,axis, factor= 1e-3):
      ''' Returns a string to represent fields in ANSYS (R).

      :param eTag: element identifier.
      :param axis: section 1 or 2
      :param factor: factor for units (default 1e-3 -> kN)'''
      retval= super(NMy,self).getAnsysStrings(eTag,axis,factor)
      retval.append("detab,"+str(eTag)+",My" +axis+","+str(self.My*factor)+"\n")
      return retval

    def getStrArguments(self):
      '''Returns a string for a 'copy' (kind of) constructor.'''
      retval= super(NMy,self).getStrArguments()
      retval+= ',My= ' + str(self.My)
      return retval

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

    def getLaTeXFields(self,factor= 1e-3):
      ''' Returns a string with the intermediate fields of the LaTeX string.

      :param factor: factor for units (default 1e-3 -> kN)'''
      retval= super(NMyMz,self).getLaTeXFields(factor)
      retval+= " & "+fmt.Esf.format(self.Mz*factor)
      return retval

    def getAnsysStrings(self,eTag,axis, factor= 1e-3):
      ''' Returns a string to represent fields in ANSYS (R).

      :param eTag: element identifier.
      :param axis: section 1 or 2
      :param factor: factor for units (default 1e-3 -> kN)'''
      retval= super(NMyMz,self).getAnsysStrings(eTag,axis,factor)
      retval.append("detab,"+str(eTag)+",Mz" +axis+","+str(self.Mz*factor)+"\n")
      return retval

    def getStrArguments(self):
      '''Returns a string for a 'copy' (kind of) constructor.'''
      retval= super(NMyMz,self).getStrArguments()
      retval+= ',Mz= ' + str(self.Mz)
      return retval

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

    def getLaTeXFields(self,factor= 1e-3):
        ''' Returns a string with the intermediate fields of the LaTeX string.

        :param factor: factor for units (default 1e-3 -> kN)'''
        retval= super(ShVy,self).getLaTeXFields(factor)
        retval+= " & "+ fmt.Esf.format(self.Vy*factor)
        return retval

    def getAnsysStrings(self,eTag,axis, factor= 1e-3):
        ''' Returns a string to represent fields in ANSYS (R).

        :param eTag: element identifier.
        :param axis: section 1 or 2
        :param factor: factor for units (default 1e-3 -> kN)'''
        retval= super(ShVy,self).getAnsysStrings(eTag,axis,factor)
        retval.append("detab,"+str(eTag)+",Vy" +axis+","+str(self.Vy*factor)+"\n")
        return retval

    def getStrArguments(self):
        '''Returns a string for a 'copy' (kind of) constructor.'''
        retval= super(ShVy,self).getStrArguments()
        retval+= ',Vy= ' + str(self.Vy)
        return retval

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

    def getLaTeXFields(self,factor= 1e-3):
        ''' Returns a string with the intermediate fields of the LaTeX string.

        :param factor: factor for units (default 1e-3 -> kN)'''
        retval= self.idSection+" & "
        retval+= super(ShearYControlVars,self).getLaTeXFields(factor)
        return retval

    def getStrArguments(self):
        '''Returns a string for a 'copy' (kind of) constructor.
        '''
        retval= 'idSection= "' + self.idSection +'", ' 
        retval+= super(ShearYControlVars,self).getStrArguments()
        return retval

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

    def getLaTeXFields(self,factor= 1e-3):
        ''' Returns a string with the intermediate fields of the LaTeX string.

        :param factor: factor for units (default 1e-3 -> kN)'''
        retval= self.idSection+" & "
        retval+= super(UniaxialBendingControlVars,self).getLaTeXFields(factor)
        return retval

    def getStrArguments(self):
        '''Returns a string for a 'copy' (kind of) constructor.
        '''
        retval= 'idSection= "' + self.idSection +'", ' 
        retval+= super(UniaxialBendingControlVars,self).getStrArguments()
        return retval

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

    def getLaTeXFields(self,factor= 1e-3):
        ''' Returns a string with the intermediate fields of the LaTeX string.

        :param factor: factor for units (default 1e-3 -> kN)'''
        retval= super(CFNMyMz,self).getLaTeXFields(factor)+" & "+fmt.Esf.format(self.Mz*factor)
        return retval

    def getAnsysStrings(self,eTag,axis, factor= 1e-3):
        ''' Returns a string to represent fields in ANSYS (R).

        :param eTag: element identifier.
        :param axis: section 1 or 2
        :param factor: factor for units (default 1e-3 -> kN)'''
        retval= super(CFNMyMz,self).getAnsysStrings(eTag,axis,factor)
        retval.append("detab,"+str(eTag)+",Mz" +axis+","+str(self.Mz*factor)+"\n")
        return retval

    def getStrArguments(self):
        '''Returns a string for a 'copy' (kind of) constructor.'''
        retval= super(CFNMyMz,self).getStrArguments()
        retval+= ',Mz= ' + str(self.Mz)
        return retval

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

    def getLaTeXFields(self,factor= 1e-3):
      ''' Returns a string with the intermediate fields of the LaTeX string.

      :param factor: factor for units (default 1e-3 -> kN)'''
      retval= self.idSection+" & "
      retval+= super(AxialForceControlVars,self).getLaTeXFields(factor)
      return retval

    def getAnsysStrings(self,eTag,axis, factor= 1e-3):
      ''' Returns a string to represent fields in ANSYS (R).

      :param eTag: element identifier.
      :param axis: section 1 or 2
      :param factor: factor for units (default 1e-3 -> kN)'''
      retval= 'idSection= "' + self.idSection +'", ' 
      retval+= super(AxialForceControlVars,self).getAnsysStrings(eTag,axis,factor)
      return retval

    def getStrArguments(self):
      '''Returns a string for a 'copy' (kind of) constructor.'''
      retval= 'idSection= "' + self.idSection +'", ' 
      retval+= super(AxialForceControlVars,self).getStrArguments()
      return retval

        
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

    def getLaTeXFields(self,factor= 1e-3):
        ''' Returns a string with the intermediate fields of the LaTeX string.

        :param factor: factor for units (default 1e-3 -> kN)'''
    #    print('super(BiaxialBendingControlVars,self).getLaTeXFields(factor) =', super(BiaxialBendingControlVars,self).getLaTeXFields(factor))
        retval= super(BiaxialBendingControlVars,self).getLaTeXFields(factor)+" & "+fmt.Esf.format(self.Mz*factor)
        return retval

    def getAnsysStrings(self,eTag,axis, factor= 1e-3):
        ''' Returns a string to represent fields in ANSYS (R).

        :param eTag: element identifier.
        :param axis: section 1 or 2
        :param factor: factor for units (default 1e-3 -> kN)'''
        retval= super(BiaxialBendingControlVars,self).getAnsysStrings(eTag,axis,factor)
        retval.append("detab,"+str(eTag)+",Mz" +axis+","+str(self.Mz*factor)+"\n")
        return retval

    def getStrArguments(self):
        '''Returns a string for a 'copy' (kind of) constructor.'''
        retval= super(BiaxialBendingControlVars,self).getStrArguments()
        retval+= ',Mz= ' + str(self.Mz)
        return retval

class SSBiaxialBendingControlVars(BiaxialBendingControlVars):
    '''Control variables for biaxial bending normal stresses LS 
    verification en steel-shape elements.

    :ivar Ncrd:     design strength to axial compression
    :ivar McRdy:    design moment strength about Y (weak) axis
    :ivar McRdz:    design moment strength about Z (strong) axis
    :ivar MvRdz:    reduced design moment strength about Z (strong) axis for shear interaction
    :ivar MbRdz:    reduced design moment strength about Z (strong) axis for lateral-torsional bucking
    :ivar chiLT:    reduction factor for lateral-torsional buckling (defaults to 1)
    '''
    def __init__(self,idSection= 'nil',combName= 'nil',CF= -1.0,N= 0.0,My= 0.0,Mz= 0.0,Ncrd=0.0,McRdy=0.0,McRdz=0.0,MvRdz=0.0,MbRdz=0.0,chiLT=1.0):
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
        '''
        super(SSBiaxialBendingControlVars,self).__init__(idSection,combName,CF,N,My,Mz)
        self.Ncrd=Ncrd
        self.McRdy=McRdy
        self.McRdz=McRdz
        self.MvRdz=MvRdz
        self.MbRdz=MbRdz
        self.chiLT=chiLT
        
    def getLaTeXFields(self,factor= 1e-3):
        ''' Returns a string with the intermediate fields of the LaTeX string.

        :param factor: factor for units (default 1e-3 -> kN)'''
        retval= super(SSBiaxialBendingControlVars,self).getLaTeXFields(factor)+" & "+fmt.Esf.format(self.Ncrd*factor)+" & "+fmt.Esf.format(self.McRdy*factor)+" & "+fmt.Esf.format(self.McRdz*factor)+" & "+fmt.Esf.format(self.MvRdz*factor)+" & "+fmt.Esf.format(self.MbRdz*factor)+" & "+fmt.Esf.format(self.chiLT)
        return retval

    def getStrArguments(self):
        '''Returns a string for a 'copy' (kind of) constructor.'''
        retval= super(SSBiaxialBendingControlVars,self).getStrArguments()
        retval+= ',Ncrd= ' + str(self.Ncrd)
        retval+= ',McRdy= ' + str(self.McRdy)
        retval+= ',McRdz= ' + str(self.McRdz)
        retval+= ',MvRdz= ' + str(self.MvRdz)
        retval+= ',MbRdz= ' + str(self.MbRdz)
        retval+= ',chiLT= ' + str(self.chiLT)
        return retval

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

    def getLaTeXFields(self,factor= 1e-3):
        ''' Returns a string with the intermediate fields of the LaTeX string.

        :param factor: factor for units (default 1e-3 -> kN)'''
        retval= super(BiaxialBendingControlVars,self).getLaTeXFields(factor)+' & '+fmt.Esf.format(self.Mu*factor)+' & '+fmt.Esf.format(self.Vy*factor)+' & '+fmt.Esf.format(self.Vz*factor)+' & '+fmt.Esf.format(self.Vu*factor)
        return retval

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

    def getStrArguments(self):
        '''Returns a string for a 'copy' (kind of) constructor.'''
        retval= super(RCShearControlVars,self).getStrArguments()
        retval+= ',Mu= ' + str(self.Mu)
        retval+= ',Vy= ' + str(self.Vy)
        retval+= ',Vz= ' + str(self.Vz)
        retval+= ',theta= ' + str(self.theta)
        retval+= ',Vcu= ' + str(self.Vcu)
        retval+= ',Vsu= ' + str(self.Vsu)
        retval+= ',Vu= ' + str(self.Vu)
        return retval

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

    def getLaTeXFields(self,factor= 1e-3):
        ''' Returns a string with the intermediate fields of the LaTeX string.

        :param factor: factor for units (default 1e-3 -> kN)'''
        retval= super(CrackControlBaseVars,self).getLaTeXFields(factor)+" & "+fmt.Stress.format(self.steelStress*factor*factor) #Factor for stresses ??
        return retval

    def getAnsysStrings(self,eTag,axis, factor= 1e-3):
        ''' Returns a string to represent fields in ANSYS (R).

        :param eTag: element identifier.
        :param axis: section 1 or 2
        :param factor: factor for units (default 1e-3 -> kN)'''
        retval= super(CrackControlBaseVars,self).getAnsysStrings(eTag,axis,factor)
        retval.append("detab,"+str(eTag)+",steelStress" +axis+","+str(self.steelStress*factor)+"\n")
        return retval

    def getStrArguments(self):
        '''Returns a string for a 'copy' (kind of) constructor.'''
        retval= super(CrackControlBaseVars,self).getStrArguments()
        retval+= ',steelStress= ' + str(self.steelStress)
        return retval


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

    def getLaTeXFields(self,factor= 1e-3):
        ''' Returns a string with the intermediate fields of the LaTeX string.
            factor: factor for units (default 1e-3 -> kN)'''
        retval= self.idSection+" & "
        retval+= self.crackControlVarsPos.getLaTeXFields()+" & "
        retval+= self.crackControlVarsNeg.getLaTeXFields()
        return retval

    def getStrArguments(self):
        '''Returns a string for a 'copy' (kind of) constructor.'''
        retval= 'idSection= "' + self.idSection + '"' 
        retval+= ', crackControlBaseVarsPos= ' + self.crackControlVarsPos.getStrConstructor()
        retval+= ', crackControlBaseVarsNeg= ' + self.crackControlVarsNeg.getStrConstructor()
        return retval

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

    def getLaTeXFields(self,factor= 1e-3):
        ''' Returns a string with the intermediate fields of the LaTeX string.

        :param factor: factor for units (default 1e-3 -> kN)'''
        retval= super(NMyMz,self).getLaTeXFields(factor)+' & '+fmt.Esf.format(self.s_rmax)+' & '+fmt.Esf.format(self.eps_sm)+' & '+fmt.Esf.format(self.wk)
        return retval

    def getStrArguments(self):
        '''Returns a string for a 'copy' (kind of) constructor.'''
        retval= super(RCCrackStraightControlVars,self).getStrArguments()
        retval+= ',s_rmax= ' + str(self.s_rmax)
        retval+= ',eps_sm= ' + str(self.eps_sm)
        retval+= ',wk= ' + str(self.wk)
        return retval
  
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

    def getLaTeXFields(self,factor= 1e-3):
        ''' Returns a string with the intermediate fields of the LaTeX string.

        :param factor: factor for units (default 1e-3 -> kN)'''
        retval= super(FatigueControlBaseVars,self).getLaTeXFields(factor)+" & "+fmt.Esf.format(self.Vy*factor)+" & "+fmt.Stress.format(self.posSteelStress*factor*factor)+" & "+fmt.Stress.format(self.negSteelStress*factor*factor)+" & "+fmt.Stress.format(self.concreteStress*factor*factor) #Factor for stresses == factor*factor ??
        return retval

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

    def getStrArguments(self):
        '''Returns a string for a 'copy' (kind of) constructor.'''
        retval= super(FatigueControlBaseVars,self).getStrArguments()
        retval+= ',Vy= ' + str(self.Vy)
        retval+= ',negSteelStress= ' + str(self.negSteelStress)
        retval+= ',posSteelStress= ' + str(self.posSteelStress)
        retval+= ',concreteStress= ' + str(self.concreteStress)
        return retval

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

    def getLaTeXFields(self,factor= 1e-3):
        ''' Returns a string with the intermediate fields of the LaTeX string.

        :param factor: factor for units (default 1e-3 -> kN)'''
        retval= self.idSection+" & "
        retval+= self.state0.getLaTeXFields()+" & "
        retval+= self.state1.getLaTeXFields()+" & "
        retval+= fmt.Stress.format(self.concreteLimitStress*factor*factor)+" & "
        retval+= fmt.Esf.format(self.concreteBendingCF)+" & "
        retval+= fmt.Esf.format(self.shearLimit*factor)+" & "
        retval+= fmt.Esf.format(self.concreteShearCF)+" & "
        retval+= fmt.Esf.format(self.Mu*factor)+" & "
        retval+= fmt.Esf.format(self.Vu*factor)
        return retval

    def getStrArguments(self):
        '''Returns a string for a 'copy' (kind of) constructor.'''
        retval= 'idSection= "' + self.idSection + '"' 
        retval+= ', controlBaseVars0= ' + self.state0.getStrConstructor()
        retval+= ', controlBaseVars1= ' + self.state1.getStrConstructor()
        retval+= ', concreteLimitStress= ' + str(self.concreteLimitStress)
        retval+= ', concreteBendingCF= ' + str(self.concreteBendingCF)
        retval+= ', shearLimit= ' + str(self.shearLimit)
        retval+= ', concreteShearCF= ' + str(self.concreteShearCF)
        retval+= ', Mu= ' + str(self.Mu)
        retval+= ', Vu= ' + str(self.Vu)
        return retval
  
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

    def getLaTeXFields(self,factor= 1e-3):
        ''' Returns a string with the intermediate fields of the LaTeX string.

        :param factor: factor for units (default 1e-3 -> kN)'''
        retval= super(VonMisesControlVars,self).getLaTeXFields(factor)
        retval+= " & "+ fmt.Esf.format(self.vm_stress*factor)
        return retval

    def getStrArguments(self):
        '''Returns a string for a 'copy' (kind of) constructor.'''
        retval= super(VonMisesControlVars,self).getStrArguments()
        retval+= ', vm_stress= ' + str(self.vm_stress)
        return retval

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
        if(e.getProp("dir")==1):
            xcOutput.write(controlVar.strElementProp(eTag,controlVarName+'Sect1'))
        else:
            xcOutput.write(controlVar.strElementProp(eTag,controlVarName+'Sect2'))
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

