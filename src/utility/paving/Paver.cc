//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
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

//Paver.cc

#include "Paver.h"

void report_ivector(std::ostream &os, const std::string &name, const std::vector<integer> &v)
  {
    os << name << ": ";
    for(std::vector<integer>::const_iterator i= v.begin(); i!=v.end(); i++)
      os << *i << ' ';
    os << std::endl;
  }

void report_fvector(std::ostream &os, const std::string &name, const std::vector<real> &v)
  {
    os << name << ": ";
    for(std::vector<real>::const_iterator i= v.begin(); i!=v.end(); i++)
      os << *i << ' ';
    os << std::endl;
  }

//! @brief Default constructor.
XC::Paver::Paver(void)
  : nbnode(0), nprm(0)  { std::cout << "constr." << std::endl; }

void XC::Paver::report(std::ostream &os)
  {
    std::cout << "Enters : " << getClassName() << "::" << __FUNCTION__
	      << std::endl;
    os << "nbnode: " << nbnode << std::endl
       << "nprm: " << nprm << std::endl
       << "nbnode= " << nbnode << std::endl
       << "mxnd= " << mxnd << std::endl
       << "lxk size: " << lxk.size() << std::endl
       << "lnodes size: " << lnodes.size() << std::endl
       << "linkpr size: " << linkpr.size() << std::endl;
    report_ivector(os, "iptper", iptper);
    report_ivector(os, "numper", numper);
    report_ivector(os, "lperim", lperim);
    //report_fvector(os, "x: ", x);
    //report_fvector(os, "y: ", y);
    //report_fvector(os, "z: ", z);
  }

//! @brief Call paving function.
//!
//! @param extContour: 3D polygon wich defines the external contour, each
//!                    vertex must correspond to a perimeter node.
//! @param intContours: 3D polygons that define the internal contours.
int XC::Paver::call_paving(const Polygon3d &extContour, const std::deque<Polygon3d> &intContours) 
  {
    std::cout << "Enters : " << getClassName() << "::" << __FUNCTION__
	      << std::endl;
    int retval= 0;
    Polygon3d ext= extContour;
    nprm= 1;
    if(ext.clockwise())
      ext.swap();
    nbnode= extContour.GetNumVertices();
    nnn= nbnode;
    if(nbnode==0)
      {
	  std::clog << getClassName() << "::" << __FUNCTION__
	            << "; error, empty external contour."
		    << " Exiting." << std::endl;
      }
    else
      {
	std::deque<Polygon3d> int_c= intContours;
	for(std::deque<Polygon3d>::const_iterator i= intContours.begin(); i!= intContours.end(); i++)
	  {
	    Polygon3d tmp= *i;
	    if(tmp.counterclockwise())
	      tmp.swap();
	    const int nv= (*i).GetNumVertices();
	    if(nv>0)
	      {
		nbnode+= nv; // number of contour nodes.
		nprm++; // number of separate perimeters.
		int_c.push_back(tmp);
	      }
	    else
	      std::clog << getClassName() << "::" << __FUNCTION__
			<< "; error, empty internal contour ignored." << std::endl;
	  }
	std::cout << " here A." << std::endl;
	iptper.resize(nprm);
	numper.resize(nprm);
	lperim.resize(nbnode);
	// We guess about the maximum number of nodes:
	mxnd= nbnode*nbnode;
	x.resize(mxnd); y.resize(mxnd); z.resize(mxnd);

        // Insert contours.
	int vertexCounter= 1; // Vertex counter.
	int plgCounter= 1; // Contour counter.
	int nv= ext.GetNumVertices();
	for(int i= 1; i<=nv; i++, vertexCounter++)
	  {
	    Pos3d p= ext.Vertice(i);
	    x[vertexCounter-1]= p.x();
	    y[vertexCounter-1]= p.y();
	    z[vertexCounter-1]= p.z();
	    lperim[vertexCounter-1]= vertexCounter;
	  }
	numper[plgCounter-1]= nv;
	iptper[plgCounter-1]= 1; // Exterior contour starts in the first node.
	std::cout << " here B." << std::endl;
	for(std::deque<Polygon3d>::const_iterator i= intContours.begin(); i!= intContours.end(); i++, plgCounter++)
	  {
	    Polygon3d tmp= *i;
	    iptper[plgCounter-1]= vertexCounter;
	    nv= ext.GetNumVertices();
	    numper[plgCounter-1]= nv;
	    for(int j= 1; j<=nv; j++, vertexCounter++)
	      {
		Pos3d p= ext.Vertice(j);
		x[vertexCounter-1]= p.x();
		y[vertexCounter-1]= p.y();
		z[vertexCounter-1]= p.z();
		lperim[vertexCounter-1]= vertexCounter;
	      }
	  }
	std::cout << " here C." << std::endl;
	// Alloc memory
	angle.resize(mxnd); // size ok
	bnsize.resize(2*mxnd); // size ok
	std::cout << " here C.1." << std::endl;
	std::cout << " mln=" << mln << " mxnd= " << mxnd
		  << " mln*mxnd= " << mln*mxnd << std::endl;
	std::cout << " lnodes.size()= " << lnodes.size() << std::endl;
	lnodes.resize(mln*mxnd); // size ok
	std::cout << " here C.2." << std::endl;
        linkpr.resize(3*nprm); // size ok
	nperim.resize(nprm); // size ok
	std::cout << " here C.3." << std::endl;
	iexk.resize(4*mxnd); // size ok
	inxe.resize(2*3*mxnd); // size ok
	nuid.resize(mxnd); // size ok
	lxk.resize(4*mxnd); // size ok
	kxl.resize(2*3*mxnd); // size ok
	std::cout << " here D." << std::endl;
	nxl.resize(2*3*mxnd); // size ok
	lxn.resize(4*mxnd); // size ok
	amesur.resize(npeold); // size ok
	bmesur.resize(npeold); // size ok
	xnold.resize(npnold); ynold.resize(npnold); // size ok
	nxkold.resize(nnxk*npeold); // size ok
	mmpold.resize(3*nprold); // size ok
	linkeg.resize(2*mlink); // size ok
	listeg.resize(2*npeold); // size ok
	std::cout << " here E." << std::endl;
	
	report(std::cout);

	real sizmin= 0.0;
	real emax= 0.0;
	real emin= 0.0;
	ftnlen dev1_len= 0;
	
	retval= paving_(&nbnode, &nprm, &mln,  iptper.data(), numper.data(), lperim.data(), x.data(), y.data(), z.data(), iexk.data(), inxe.data(), &nnn, &lll, &kkk, &mxnd, angle.data(), bnsize.data(), lnodes.data(), linkpr.data(), nperim.data(), lxk.data(), kxl.data(), nxl.data(), lxn.data(), nuid.data(), &iavail, &navail, &graph, &timer, &video, &defsiz, &sizeit, dev1, &kreg, &batch, &noroom, &err, amesur.data(), xnold.data(), ynold.data(), nxkold.data(), mmpold.data(), linkeg.data(), listeg.data(), bmesur.data(), &mlink, &nprold, &npnold, &npeold, &nnxk, &remesh, &rexmin, &rexmax, &reymin, &reymax, &idivis, &sizmin, &emax, &emin, dev1_len);
      }
    std::cout << "exits paving." << std::endl;
    return retval;
  }

//! @brief Get data from Python and call paving.
int XC::Paver::mesh(const Polygon3d &ext, const boost::python::list &l)
  {
    std::cout << "Enters : " << getClassName() << "::" << __FUNCTION__
	      << std::endl;
    std::deque<Polygon3d> intContours;
    const size_t sz= len(l);
    for(size_t i=0; i<sz; i++)
      intContours.push_back(boost::python::extract<Polygon3d>(l[i]));
    int paving= call_paving(ext,intContours);
    return extract_mesh();
  }

//! @brief Return the element nodes from its edges.
std::vector<int> XC::Paver::get_elem_nodes(const std::vector<int> &edges)
  {
    std::cout << "Enters : " << getClassName() << "::" << __FUNCTION__
	      << std::endl;
    const int edge0= edges[0];
    const int iEdge0= edge0*2;
    const int nA= inxe[iEdge0];
    const int nB= inxe[iEdge0+1];
    const int edge1= edges[1];
    const int iEdge1= edge1*2;
    const int nC= inxe[iEdge1];
    const int nD= inxe[iEdge1+1];
    std::vector<int> retval(4,0);
    if(nA==nC)
      {
        retval[0]= nB; retval[1]= nA; retval[2]= nD;
      }
    else if(nA==nD) // nA==nD or nC==nB
      {
        retval[0]= nB; retval[1]= nA; retval[2]= nC;
      }
    else // nC==nB
      {
        retval[0]= nA; retval[1]= nB; retval[2]= nD;
      }
    // Only one node left.
    const int edge2= edges[2];
    const int iEdge2= edge2*2;
    const int nE= inxe[iEdge2];
    const int nF= inxe[iEdge2+1];
    if((nE!=retval[0]) && (nE!=retval[1]) && (nE!=retval[2]))
      retval[3]= nE; // this the new one.
    else
      retval[3]= nF; // this is the new one.
    // C++ indexing:
    retval[0]--; retval[1]--; retval[2]--; retval[3]--;
    std::cout << "nodes: " << retval[0]+1 << ' ' << retval[1]+1  << ' ' << retval[2]+1 << ' ' << retval[3]+1 << std::endl;
    return retval;
  }

//! @brief Extract mesh data  
int XC::Paver::extract_mesh(void)
  {
    std::cout << "Enters : " << getClassName() << "::" << __FUNCTION__
	      << std::endl;
    
    nodePos= std::vector<Pos3d>(nnn);
    for(int i= 0;i<nnn; i++)
      nodePos[i]= Pos3d(x[i], y[i], z[i]);
    elemEdges= std::vector<std::vector<int> >(kkk);
    elemNodes= std::vector<std::vector<int> >(kkk);
    for(int i= 0;i<kkk;i++)
      {
	std::vector<int> edges(4);
	size_t ielem= i*4;
	edges[0]= iexk[ielem]-1;
	edges[1]= iexk[ielem+1]-1;
	edges[2]= iexk[ielem+2]-1;
	edges[3]= iexk[ielem+3]-1;
	elemEdges[i]= edges;
        std::vector<int> quad= get_elem_nodes(edges);
	elemNodes[i]= quad;
      }
    return kkk;
  }

//! @brief Return a list containing the positions of the nodes.
boost::python::list XC::Paver::getNodePositions(void) const
  {
    boost::python::list retval;
    for(std::vector<Pos3d>::const_iterator i= nodePos.begin(); i!= nodePos.end(); i++)
      retval.append(*i);
    return retval;
  }

//! @brief Return the node indexes for each element.
boost::python::list XC::Paver::getQuads(void) const
  {
    boost::python::list retval;
    for(std::vector<std::vector<int> >::const_iterator i= elemNodes.begin(); i!= elemNodes.end(); i++)
      {
	std::vector<int> quad= (*i);
	boost::python::list tmp;
	tmp.append(quad[0]);
	tmp.append(quad[1]);
	tmp.append(quad[2]);
	tmp.append(quad[3]);
	retval.append(tmp);
      }
    return retval;
  }
