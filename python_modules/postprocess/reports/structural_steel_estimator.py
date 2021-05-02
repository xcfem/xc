# -*- coding: utf-8 -*-
''' Get an estimate of the steel quantities.'''

from __future__ import division
from __future__ import print_function
import csv

class StructuralSteelEstimator(object):
    ''' Structural steel weight estimation.'''
    def __init__(self, title, connectionPercentage= .08):
        ''' Constructor.

        :param title: name of the structure to estimate.
        :param connectionPercentage: percentage of connections and fittings
                                     weight by weight.
        '''
        self.title= title
        self.connectionPercentage= connectionPercentage

    def compute(self, xcSet):
        ''' Compute steel quantities.

        :param xcSet: XC set with the elements to consider
                      in the estimation.
        '''
        self.lightSteelWeight= 0.0
        self.mediumSteelWeight= 0.0
        self.heavySteelWeight= 0.0
        self.extraHeavySteelWeight= 0.0
        self.connectionsAndFittings= 0.0

        self.totalWeight= 0.0
        for e in xcSet.elements:
            if((e.getDimension==1) and e.hasProp('crossSection')):
                length= e.getLength(True)
                crossSection= e.getProp('crossSection')
                name= crossSection.get('nmb')
                P= crossSection.get('P')
                W= P*length
                if(P<30):
                    self.lightSteelWeight+= W
                elif(P<60):
                    self.mediumSteelWeight+= W
                elif(P<90):
                    self.heavySteelWeight+= W
                else:
                    self.extraHeavySteelWeight+= W
                self.totalWeight+= W
        self.connectionsAndFittings= self.connectionPercentage*self.totalWeight
        self.totalWeight+= self.connectionsAndFittings


    def writeCSV(self, xcSet, fileName):
        ''' Write the data to a comma separated values
            file.

        :param xcSet: XC set with the elements to consider
                      in the estimation.
        '''
        self.compute(xcSet)
        f = open(fileName, 'w')
        with f:
            writer = csv.writer(f)
            writer.writerow([self.title])
            writer.writerow(['','Light steel weight: ', self.lightSteelWeight/1e3, 't']) 
            writer.writerow(['','Medium steel weight: ', self.mediumSteelWeight/1e3, 't'])
            writer.writerow(['','Heavy steel weight: ', self.heavySteelWeight/1e3, 't'])
            writer.writerow(['','Extra heavy steel weight: ', self.extraHeavySteelWeight/1e3, 't'])
            writer.writerow(['','Conections and fittings('+str(self.connectionPercentage*100)+'%): ', self.connectionsAndFittings/1e3, 't'])
            writer.writerow(['','Total weight: ', self.totalWeight/1e3, 't'])
