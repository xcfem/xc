// -*-c++-*-
//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
//  XC is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------

#ifndef MC10CreepShrinkageParameters_h
#define MC10CreepShrinkageParameters_h

#include "utility/kernel/CommandEntity.h"
#include "utility/actor/actor/MovableObject.h"
namespace XC {

//! @brief Parameters that define creep and shrinkage behaviour according
//! to Model Code 2010.
class MC10CreepShrinkageParameters: public CommandEntity, public MovableObject
  {
  private:
    double epsba; //!< ultimate basic shrinkage strain, εcbs,0, as per Model Code 2010
    double epsbb; //!< fitting parameter within the basic shrinkage time evolution function as per Model Code 2010 and prEN1992-1-1:2017.
    double epsda; //!< product of εcds,0 and βRH, as per Model Code 2010.
    double epsdb; //!< fitting parameter within the drying shrinkage time evolution function as per Model Code 2010 and prEN1992-1-1:2017.
    double phiba; //!< parameter for the effect of compressive strength on basic creep βbc(fcm), as per Model Code 2010.
    double phibb; //!< fitting parameter within the basic creep time evolution function as per Model Code 2010 and prEN1992-1-1:2017.
    double phida; //!< product of βdc(fcm) and β(RH), as per Model Code 2010.
    double phidb; //!< fitting constant within the drying creep time evolution function as per Model Code 2010.
    double cem; //!< coefficient dependent on the type of cement: –1 for 32.5N, 0 for 32.5R and 42.5N and 1 for 42.5R, 52.5N and 52.5R.

  protected:
    int sendData(Communicator &);
    int recvData(const Communicator &);
  public:
    MC10CreepShrinkageParameters(void);
    MC10CreepShrinkageParameters(const double &_epsba, const double &_epsbb, const double &_epsda, const double &_epsdb, const double &_phiba, const double &_phibb, const double &_phida, const double &_phidb, const double &_cem);

    void setup_parameters(void);
    
    double getEpsba(void) const;
    void setEpsba(const double &);
    double getEpsbb(void) const;
    void setEpsbb(const double &);
    double getEpsda(void) const;
    void setEpsda(const double &);
    double getEpsdb(void) const;
    void setEpsdb(const double &);

    double getPhiba(void) const;
    void setPhiba(const double &);
    double getPhibb(void) const;
    void setPhibb(const double &);
    double getPhida(void) const;
    void setPhida(const double &);
    double getPhidb(void) const;
    void setPhidb(const double &);

    double getCem(void) const;
    void setCem(const double &);

    double getPhiBasic(double time, double tp) const;
    double getPhiDrying(double time, double tp) const;
    double getShrinkBasic(double time);
    double getShrinkDrying(double age, double time);
    
    int sendSelf(Communicator &);  
    int recvSelf(const Communicator &);
    
    void Print(std::ostream &s, int flag =0) const;

  };
} // end of XC namespace


#endif
