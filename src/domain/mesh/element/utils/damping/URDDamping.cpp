/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 1999, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */

// $Revision: 1.0 $
// $Date: 2021-07-02 14:29:01 $
// $Source: /usr/local/cvs/OpenSees/SRC/damping/URDDamping.h,v $

// Revised by: Y Tian
// Created: 02/2020
// Revision: A
//
// Description: This file contains the definition for the URDDamping class.
// URDDamping provides the abstraction of an elemental damping imposition
// providing user-define damping over a frequency range
//
// Reference:
//

// What: "@(#) URDDamping.cpp, revA"

#include "URDDamping.h"
#include "domain/mesh/element/utils/Information.h"
#include "domain/domain/Domain.h"
#include "solution/analysis/analysis/StaticAnalysis.h"
#include "utility/actor/actor/CommMetaData.h"
#include "domain/load/pattern/TimeSeries.h"
#include "domain/component/Parameter.h"
#include "utility/utils/misc_utils/colormod.h"

//extern StaticAnalysis *theStaticAnalysis;

// constructor:
XC::URDDamping::URDDamping(int tag, int nfreq, const Matrix &etaf, double tol, double t1, double t2, TimeSeries *f, int ptag, int iter)
  : UDampingBase(tag, DMP_TAG_URDDamping, t1, t2, f),
    numfreq(nfreq), prttag(ptag), maxiter(iter), dptol(tol),
    alpha(), omegaetaf(), 
    Freqlog(), Fredif(), Freqk(), Freqb(),
    etaFreq(etaf)
  {
    Initialize();
  }


// constructor:
XC::URDDamping::URDDamping(int tag, int nfreq, const Matrix &etaf, double tol, double t1, double t2, TimeSeries *f, int nF, const Vector &a, const Vector &w, const Vector &ef, int ptag, int iter)
  : UDampingBase(tag, DMP_TAG_URDDamping, t1, t2, f),
    numfreq(nfreq), prttag(ptag), maxiter(iter), dptol(tol),
    alpha(), omegaetaf(),
    Freqlog(), Fredif(), Freqk(), Freqb(),
    etaFreq(etaf)
  {
    if (nF > 0 && a.Size() == nF && w.Size() == nF && ef.Size() == nF)
      {
	alpha= Vector(a);
	omegaetaf= Vector(ef);
      }
    else
      {
	Initialize();
      }
  }


// constructor:
// invoked by a FEM_ObjectBroker, recvSelf() needs to be invoked on this object.
XC::URDDamping::URDDamping(void)
  : UDampingBase(0, DMP_TAG_URDDamping, 0.0, 0.0, nullptr),
    numfreq(0), prttag(0), maxiter(0), dptol(0.0),
    alpha(), omegaetaf(), 
    Freqlog(), Fredif(), Freqk(), Freqb(),
    etaFreq()
  {}

std::tuple<int, int> XC::URDDamping::compute_count_freq(int numfreq, const double &df, Vector &tmptag)
  {
    int countfreq2= 0;
    int countfreq1= 0;

    for (int i = 0; i < numfreq - 1; i++)
      {
	countfreq1 = floor(Fredif(i) / df) + 1;
	if (countfreq1 > countfreq2)
	  {
	    for (int j = countfreq2; j < countfreq1; j++)
	      { tmptag(j) = i; }
	    countfreq2 = countfreq1;
	  }
      }
    return std::make_tuple(countfreq1, countfreq2);
  }

bool XC::URDDamping::computeErrorA(const int &nf, const double &flog, const double &df, const Vector &tmpeta, const double &delta1, const double &delta2) const
  {
    bool converged = true;
    for (int i = 0; i < nf; ++i)
      {
	const double omega = 6.28318530718 * pow(10.0, flog + i * df);
	double err = 0.0;
	for (int j = 0; j < nFilter; ++j)
	  {
	    const double wjn = omega / omegac[j];
	    const double phij = 2.0 * wjn / (1.0 + wjn * wjn);
	    err += alpha(j) * phij * omegaetaf(j);
	  }
	err -= tmpeta(i);
	if(tmpeta(i) > 0.005)
	  {
	    if (err / tmpeta(i) > delta1 || err / tmpeta(i) < -delta1)
	      { converged = false; break; }
	  }
	else
	  {
	    if (err > delta2 || err < -delta2)
	      { converged = false; break; }
	  }
      }
    return converged;
  }

bool XC::URDDamping::computeErrorB(const double &flog) const
  {
    bool converged = true;
    for (int i = 1; i < 8; ++i)
      {
	const double omega = 6.28318530718 * pow(10.0, flog - i * log10(2.0));
	double err = 0.0;
	for (int j = 0; j < nFilter; ++j)
	  {
	    const double wjn = omega / omegac[j];
	    const double phij = 2.0 * wjn / (1.0 + wjn * wjn);
	    err += alpha(j) * phij * omegaetaf(j);
	  }
	if (err < 0.0)
	  { converged = false; break; }
	else if (err > omegaetaf(0))
	  { converged = false; break; }
      }
    return converged;
  }

int XC::URDDamping::Initialize(void)
  {
    // Update order
    double tmpf, tempeta;
    for (int i = 0; i < numfreq-1; i++)
      {
	for (int j = i+1; j < numfreq; j++)
	  {
	    if (etaFreq(i, 0) > etaFreq(j, 0))
	      {
		tmpf = etaFreq(j, 0);
		tempeta = etaFreq(j, 1);
		etaFreq(j, 0) = etaFreq(i, 0);
		etaFreq(j, 1) = etaFreq(i, 1);
		etaFreq(i, 0) = tmpf;
		etaFreq(i, 1) = tempeta;
	      }
	    else if(etaFreq(i, 0) == etaFreq(j, 0))
	      {
		std::clog << Color::yellow << getClassName() << "::" << __FUNCTION__
			  << ": WARNING invalid frequency series - freqi cannot equal freqj."
			  << Color::def << std::endl;
		return  1;
	      }
	  }
      }

    //Calculate interpolation parameters
    Freqlog= Vector(numfreq);
    Fredif = Vector(numfreq - 1);
    Freqk = Vector(numfreq - 1);
    Freqb = Vector(numfreq - 1);
    for (int i = 0; i < numfreq; i++)
      {
	Freqlog(i) = log10(etaFreq(i, 0));
      }
    for (int i = 0; i < numfreq - 1; i++)
      {
	Fredif(i) = Freqlog(i + 1) - Freqlog(0);
	Freqk(i) = (etaFreq(i + 1, 1) - etaFreq(i, 1)) / (Freqlog(i + 1) - Freqlog(i));
	Freqb(i) = etaFreq(i, 1) - Freqk(i) * Freqlog(i);
      }

    int omigacount1, omigacount2;
    int tagFindFilter = 0;
    int iter = 0;
    double delta1 = dptol; //0.05; //relative damping ratio error threshold
    double delta2 = 0.0002; //absolute damping ratio error threshold
    double f1log = Freqlog(0);
    double f2log = Freqlog(numfreq-1);
    nFilter = 1;
    //Calculate alpha
    while (iter < maxiter) 
      {
	++iter;
	if ((tagFindFilter == 0))
	  {
	    ++nFilter;
	  }
	else if ((tagFindFilter == 1) || (tagFindFilter == 2))
	  {
	    nFilter = nFilter + numfreq - 1;
	  }

	alpha = Vector(nFilter);
	omegac = Vector(nFilter);
	omegaetaf = Vector(nFilter);

	//Calculate omegac
	if (tagFindFilter == 0)
	  {
	    //Method #0
	    double dfreq = (f2log - f1log) / (nFilter - 1);
	    Vector omegactag= Vector(nFilter);
	    omigacount2 = 0;
	    for (int i = 0; i < numfreq - 1; i++)
	      {
		omigacount1 = floor(Fredif(i) / dfreq) + 1;
		if (omigacount1 > omigacount2)
		  {
		    for (int j = omigacount2; j < omigacount1; j++)
		      {
			omegactag(j) = i;
		      }
		    omigacount2 = omigacount1;
		  }
	      }
	    omegactag(0) = 0;
	    omegactag(nFilter - 1) = numfreq - 2;
	    tmpf = f1log - dfreq;
	    for (int i = 0; i < nFilter; i++)
	      {
		tmpf += dfreq;
		omegaetaf(i) = Freqk(omegactag(i)) * tmpf + Freqb(omegactag(i));
	      }

	    for (int i = 0; i < nFilter; ++i)
	      {
		omegac(i) = 6.28318530718 * pow(10.0, f1log + i * dfreq);
	      }
	  }
	else if ((tagFindFilter == 1) || (tagFindFilter == 2))
	  {
	    //Method #1
	    for (int i = 0; i < numfreq - 1; i++)
	      {
		for (int j = 0; j < iter; j++)
		  {
		    omegac(i * iter + j) = Freqlog(i) + (Freqlog(i + 1) - Freqlog(i)) / iter * j;
		    omegaetaf(i * iter + j) = Freqk(i) * omegac(i * iter + j) + Freqb(i);
		    omegac(i * iter + j) = 6.28318530718 * pow(10.0, omegac(i * iter + j));
		  }
	      }
	    omegac(nFilter - 1) = Freqlog(numfreq - 1);
	    omegac(nFilter - 1) = 6.28318530718 * pow(10.0, omegac(nFilter - 1));
	    omegaetaf(nFilter - 1) = etaFreq(numfreq-1, 1);
	  }


	Vector y = Vector(nFilter);
	Matrix X = Matrix(nFilter, nFilter);
	//Calculate y and X
	if ((tagFindFilter == 0) || (tagFindFilter == 1))
	  {
	    //Prepare interpolated damping ratio for integral
	    double df= 0.005;
	    const int nf = ceil((f2log - f1log) / df) + 1;
	    df = (f2log - f1log) / (nf - 1);
	    Vector tmptag= Vector(nf);
            int countfreq1, countfreq2;
	    std::tie(countfreq1, countfreq2)= this->compute_count_freq(numfreq, df, tmptag);
	    tmptag(0) = 0;
	    tmptag(nf - 1) = numfreq - 2;

	    tmpf = f1log - df;
	    Vector tmpeta= Vector(nf);
	    for (int i = 0; i < nf; i++)
	      {
		tmpf += df;
		tmpeta(i) = Freqk(tmptag(i)) * tmpf + Freqb(tmptag(i));
	      }

	    for (int i = 0; i < nf; ++i)
	      {
		const double omega = 6.28318530718 * pow(10.0, f1log + i * df);
		for (int j = 0; j < nFilter; ++j)
		  {
		    const double wjn = omega / omegac[j];
		    const double phij = 2.0 * wjn / (1.0 + wjn * wjn);
		    y(j) += (phij * tmpeta(i) * omegaetaf(j));
		    for (int k = 0; k < nFilter; ++k)
		      {
			double wkn = omega / omegac[k];
			double phik = 2.0 * wkn / (1.0 + wkn * wkn);
			X(j, k) += (phij * phik * omegaetaf(j) * omegaetaf(k));
		      }
		  }
	      }
	    alpha= y / X;

	    bool converged = this->computeErrorA(nf, f1log, df, tmpeta, delta1, delta2);
	    //Check out-zone
	    converged= this->computeErrorB(f1log);
	    converged= this->computeErrorB(f2log);
	    if(converged)
	      {
		if(prttag == 1) 
		  {
		    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
			      << "; damping URD: nFilter: " << nFilter << std::endl
			      << " damping URD: alpha: " << alpha << std::endl
			      << " damping URD: nf " << nf << std::endl
			      << " damping URD: omega_cn " << omegac << std::endl
			      << " damping URD: ita_omega_cn " << omegaetaf  << std::endl
			      << Color::def << std::endl;
		  }

		//system("pause");
		break;
	      }
	    else if(iter == maxiter)
	      {
		if (tagFindFilter == 0)
		  {
		    tagFindFilter = 2; //Give up method 1
		    nFilter = 1;
		    iter = 0;
		    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
			      << "; change nFilter calculation method to Method #"<< tagFindFilter
			      << Color::def << std::endl;
		  }
		else if (tagFindFilter == 1)
		  {
		    for (int i = 0; i < numfreq; i++)
		      { etaFreq(i, 1) /= (2.0); }
		    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
			      << "; error: Damping model not converged."
			      << etaFreq << std::endl
			      << Color::def << std::endl;
		    return  1;
		    break;
		    //tagFindFilter = 2;
		    //nFilter = 1;
		    //iter = 0;
		    //opserr << "Change nFilter calculation method to Method #"<< tagFindFilter <<"\n";
		  }
	      }
	  }
	else if (tagFindFilter == 2)
	  {
	    for (int i = 0; i < nFilter; ++i)
	      {
		y(i) = omegaetaf(i);
		for (int j = 0; j < nFilter; ++j)
		  {
		    double wjn = omegac[i] / omegac[j];
		    double phij = 2.0 * wjn / (1.0 + wjn * wjn);
		    X(i, j) = omegaetaf(j) * phij;
		  }
	      }
	    alpha = y / X;
	    bool converged= this->computeErrorB(f1log);
	    converged= this->computeErrorB(f2log);
	    if (converged)
	      {
		if (prttag == 1) 
		  {
		    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
			      << "; damping URD: nFilter: " << nFilter << std::endl
			      << " damping URD: alpha: " << alpha << std::endl
			      << " damping URD: omega_cn " << omegac << std::endl
			      << " damping URD: ita_omega_cn " << omegaetaf  << std::endl
			      << Color::def << std::endl;
		  }

		//system("pause");
		break;
	      }
	    else if (iter == maxiter) 
	      {              
		tagFindFilter = 1;
		nFilter = 1;
		iter = 0;
		std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
			  << "; change nFilter calculation method to Method #"<< tagFindFilter
			  << Color::def << std::endl;
	      }
	  }

      }

    return 0;
  }

int XC::URDDamping::update(const Vector &q)
  {
    const double t = theDomain->getCurrentTime();
    const double dT = theDomain->getDt();
    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
              << "; not implemented yet."
              << Color::def << std::endl;
    // StaticAnalysis **theStaticAnalysis= OPS_GetStaticAnalysis();
    StaticAnalysis *theStaticAnalysis= nullptr;
    if(theStaticAnalysis)
      {
	q0 = q;
	qd.Zero();
	for (int i = 0; i < nFilter; ++i)
	  for (int j = 0; j < nComp; ++j)
	    qL(j,i) = q(j);
      }
    else if (dT > 0.0)
      {
	q0 = q;
	qd.Zero();
	if (t < td)
	  {
	    if (t > ta)
	      {
		for (int i = 0; i < nFilter; ++i)
		  {
		    double dTomegac = dT * omegac(i);
		    double cd = 4.0 * alpha(i) * omegaetaf(i) / (2.0 + dTomegac);
		    double c0 = dTomegac / (2.0 + dTomegac);
		    double cL = (2.0 - dTomegac) / (2.0 + dTomegac);
		    for (int j = 0; j < nComp; ++j)
		      {
			qd(j) += cd * (q0C(j) + q0(j) - 2.0 * qLC(j,i));
			qL(j,i) = c0 * (q0C(j) + q0(j)) + cL * qLC(j,i);
		      }
		  }
		qd -= qdC;
	      }
	    else
	      {
		for (int i = 0; i < nFilter; ++i)
		  for (int j = 0; j < nComp; ++j)
		    qL(j,i) = q(j);
	      }
	    if (fac) qd *= fac->getFactor(t);
	  }
      }
    return 0;
  }

double XC::URDDamping::getStiffnessMultiplier(void) const
  {
    const double t = theDomain->getCurrentTime();
    const double dT = theDomain->getDt();
    double km = 0.0;
    // StaticAnalysis **theStaticAnalysis= OPS_GetStaticAnalysis();
    StaticAnalysis *theStaticAnalysis= nullptr;
    std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
              << "; not implemented yet."
              << Color::def << std::endl;
    if(!theStaticAnalysis && dT > 0.0 && t > ta && t < td)
      {
	for (int i = 0; i < nFilter; ++i)
	  {
	    km += 4.0 * alpha(i) * omegaetaf(i) / (2.0 + omegac(i) * dT); 
	  }
	if (fac) km *= fac->getFactor(t);
      }
    return 1.0 + km;
  }

XC::Damping *XC::URDDamping::getCopy(void) const
  { return new URDDamping(*this); }

//! @brief Send object members through the communicator argument.
int XC::URDDamping::sendData(Communicator &comm)
  {
    int res= UDampingBase::sendData(comm);
    res+= comm.sendInts(numfreq, prttag, maxiter, getDbTagData(),CommMetaData(12));
    res+= comm.sendDouble(dptol, getDbTagData(),CommMetaData(13));
    res+= comm.sendVector(alpha, getDbTagData(), CommMetaData(14));
    res+= comm.sendVector(omegaetaf, getDbTagData(), CommMetaData(15));
    res+= comm.sendVector(Freqlog, getDbTagData(), CommMetaData(16));
    res+= comm.sendVector(Fredif, getDbTagData(), CommMetaData(17));
    res+= comm.sendVector(Freqk, getDbTagData(), CommMetaData(18));
    res+= comm.sendVector(Freqb, getDbTagData(), CommMetaData(19));
    res+= comm.sendMatrix(etaFreq, getDbTagData(), CommMetaData(20));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::URDDamping::recvData(const Communicator &comm)
  {
    int res= UDampingBase::recvData(comm);
    res+= comm.receiveInts(numfreq, prttag, maxiter, getDbTagData(),CommMetaData(12));
    res+= comm.receiveDouble(dptol, getDbTagData(),CommMetaData(13));
    res+= comm.receiveVector(alpha, getDbTagData(), CommMetaData(14));
    res+= comm.receiveVector(omegaetaf, getDbTagData(), CommMetaData(15));
    res+= comm.receiveVector(Freqlog, getDbTagData(), CommMetaData(16));
    res+= comm.receiveVector(Fredif, getDbTagData(), CommMetaData(17));
    res+= comm.receiveVector(Freqk, getDbTagData(), CommMetaData(18));
    res+= comm.receiveVector(Freqb, getDbTagData(), CommMetaData(19));
    res+= comm.receiveMatrix(etaFreq, getDbTagData(), CommMetaData(20));
    return res;
  }

//! @brief Sends object through the communicator argument.
int XC::URDDamping::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(21);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << "; failed to send data."
	        << Color::def << std::endl;
    return res;
  }

//! @brief Receives object through the communicator argument.
int XC::URDDamping::recvSelf(const Communicator &comm)
  {
    inicComm(21);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << "; failed to receive ids."
	        << Color::def << std::endl;
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(comm);
        if(res<0)
          std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		    << "; failed to receive data."
	            << Color::def << std::endl;
      }
    return res;
  }

void XC::URDDamping::Print(std::ostream &s, int flag)
  {
    s << "\nDamping: " << this->getTag() << " Type: URDDamping";
    s << "\tnumber of frequencies: " << numfreq << std::endl;
    s << ", \"frequency\": [";
    for (int i = 0; i < numfreq; ++i)
    {
        s << etaFreq(i, 0);
    }
    s << "]";
    s << ", \"loss factor\": [";
    for (int i = 0; i < numfreq; ++i)
    {
        s << etaFreq(i, 1);
    }
    s << "]";
    s << "\ttol: " << dptol << std::endl;
    s << "\tactivation time: " << ta << std::endl;
    s << "\tdeactivation time: " << td << std::endl;
  }


