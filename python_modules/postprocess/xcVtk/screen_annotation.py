# -*- coding: utf-8 -*-

import datetime
from vtk.vtkRenderingAnnotation import vtkCornerAnnotation
from misc_utils import data_struct_utils as su
import xc

class ScreenAnnotation(object):
    lowerLeft= 0 # Lower left corner
    lowerRight= 1 # Lower right corner.
    upperLeft= 2 # Upper left corner
    upperRight= 3 # Upper right corner
    captionWidth= 80

    def __init__(self):
        self.version= 'XC finite element analysis\n'+ xc.getXCVersion()

    def getVtkCornerAnnotation(self, caption= '', unitDescription= ''):
        ''' Create the corner annotations.

        :param caption: caption of the image.
        '''
        self.annotation = vtkCornerAnnotation()
        self.date= datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        self.annotation.SetText(self.upperRight, self.date) 
        self.annotation.SetText(self.upperLeft, self.version)
        #vtkCornerAnnotation doesn't seems to support UTF-8 very well.
        caption= str(su.remove_accents(caption))
        if(len(caption)>self.captionWidth):
            caption= caption[:(self.captionWidth-4)]+'...'
        self.annotation.SetText(self.lowerLeft,caption)
        if(len(unitDescription)>0):
            self.annotation.SetText(self.lowerRight, unitDescription)
        return self.annotation
