/* Move common block declarations here.
*/

#ifndef COMMON_BLOCK_DECLARATIONS_H
#define COMMON_BLOCK_DECLARATIONS_H

/* Common Block Declarations */
#ifdef __cplusplus
extern "C" {
#endif

struct struct_status
  {
    xc_float devcap[23], defout[7];
  };

extern struct struct_status status_;
#define status_1 status_

struct struct_device
  {
    xc_float devp[5];
  }; 

extern struct struct_device device_;

#define device_1 device_

struct struct_color
  {
    xc_float colp[3], palett[48]	/* was [3][16] */;
  };

extern struct struct_color color_;

#define color_1 color_

struct struct_text{
    xc_float textp[40];
};

extern struct struct_text text_;

#define text_1 text_

struct struct_vectrc{
    xc_float vectp[5], xcur, ycur;
};

extern struct struct_vectrc vectrc_;

#define vectrc_1 vectrc_

struct struct_font{
    integer idex[400]	/* was [200][2] */, nvect[400]	/* was [200][2] */;
    xc_float xsize[400]	/* was [200][2] */, ysize[400]	/* was [200][2] */, 
	    x0[4600]	/* was [2300][2] */, y0[4600]	/* was [2300][2] */, 
	    x1[4600]	/* was [2300][2] */, y1[4600]	/* was [2300][2] */;
};

extern struct struct_font font_;

#define font_1 font_

struct struct_graph{
    xc_float graphp[100];
};

extern struct struct_graph graph_;

#define graph_1 graph_

struct struct_mappar{
    xc_float mapp[11];
};

extern struct struct_mappar mappar_;

#define mappar_1 mappar_

struct struct_storag{
    integer memory[1000];
};

extern struct struct_storag storag_;

#define storag_1 storag_
  
struct struct_pltsty{
    xc_float savlen;
    integer idshsv;
};

extern struct struct_pltsty pltsty_;

#define pltsty_1 pltsty_

struct struct_map{
    xc_float model[16]	/* was [4][4] */, view[16]	/* was [4][4] */, 
	    proj[16]	/* was [4][4] */, cpnear, cpfar, vwport[4], mvp[16]	
	    /* was [4][4] */, vp[16]	/* was [4][4] */, cpline[40]	/* 
	    was [2][2][10] */, cpplan[60]	/* was [2][3][10] */, peye[3],
	     plook[3], etwist;
    integer ncplin, ncplan;
    xc_float tmat1[16]	/* was [4][4] */, tmat2[16]	/* was [4][4] */, 
	    tmat3[16]	/* was [4][4] */, tvec1[4], tvec2[4], tvec3[4], tvec4[
	    4], tarr1[32], tarr2[32], tarr3[32], tarr4[32], tarr5[32], tarr6[
	    32], tarr7[32], tarr8[32];
};

extern struct struct_map map_;

#define map_1 map_
  
struct struct_psave{
    xc_float tdevp[50]	/* was [5][10] */, ttextp[400]	/* was [40][10] */, 
	    tvectp[50]	/* was [5][10] */, tgraph[1000]	/* was [100][10] */, 
	    tmapp[110]	/* was [11][10] */;
    integer ipopd, ipopt, ipopv, ipopg, ipopm;
};

extern struct struct_psave psave_;

#define psave_1 psave_
  
struct struct_timing{
    xc_float timea, timep, timec, timepc, timeaj, times;
};

extern struct struct_timing timing_;

#define timing_1 timing_
  
struct struct_grpcoc{
    char devnam[6], devcod[16];
};

extern struct struct_grpcoc grpcoc_;

#define grpcoc_1 grpcoc_
  
struct struct_grpcom{
    integer icurdv;
    logical ishard, devok[2], talkok[2];
    integer nsnap[2], ifont[2];
    logical softch[2], autopl[2];
    integer maxcol[2], numcol[4]	/* was [2][2] */, mapalt[2], mapuse[2]
	    ;
};

extern struct struct_grpcom grpcom_;

#define grpcom_1 grpcom_
struct struct_dbnums{
    integer ndim, numnp, numel, nelblk, nvarhi, nvargl, nvarnp, nvarel, 
	    nvarns, nvarss, nsteps, nstepw, nshl;
};

extern struct struct_dbnums dbnums_;

#define dbnums_1 dbnums_
  
struct struct_d3nums{
    logical is3dim;
    integer nnpsur, numnpf, llnset;
};

extern struct struct_d3nums d3nums_;

#define d3nums_1 d3nums_

struct struct_etcopt{
    xc_float vecscl;
};

extern struct struct_etcopt etcopt_;

#define etcopt_1 etcopt_

struct struct_devdat{
    xc_float dtw, vwscl;
};

extern struct struct_devdat devdat_;

#define devdat_1 devdat_

struct struct_grcol {
    integer lstmap, lstdev, lstcol, iblack, iwhite;
};

extern struct struct_grcol grcol_;

#define grcol_1 grcol_

struct struct_bcolr{
    logical bcolch;
};

extern struct struct_bcolr bcolr_;

#define bcolr_1 bcolr_

struct struct_plcol2{
    integer idfor, idfort, idbck, idbckt;
    xc_float colfor;
};

extern struct struct_plcol2 plcol2_;

#define plcol2_1 plcol2_

struct struct_cmap{
    xc_float colmap[11];
};

extern struct struct_cmap cmap_;

#define cmap_1 cmap_

struct struct_clst{
    char collst[352];
};

extern struct struct_clst clst_;

#define clst_1 clst_
struct struct_debugc{
    char cdebug[8];
};

extern struct struct_debugc debugc_;

#define debugc_1 debugc_

struct struct_debugn{
    integer idebug;
};

extern struct struct_debugn debugn_;

#define debugn_1 debugn_

struct struct_vcmodr{
    f2c_float xinch, yinch;
};

extern struct struct_vcmodr vcmodr_;

#define vcmodr_1 vcmodr_

struct struct_vcvec1{
    integer ivect;
};

extern struct struct_vcvec1 vcvec1_;

#define vcvec1_1 vcvec1_

struct struct_vcndcm{
    f2c_float xndcmx, yndcmx;
};

extern struct struct_vcndcm vcndcm_;

#define vcndcm_1 vcndcm_

struct struct_vcattr{
    f2c_float vector[7];
};

extern struct struct_vcattr vcattr_;

#define vcattr_1 vcattr_

struct struct_vccrps{
    f2c_float xcp, ycp;
};

extern struct struct_vccrps vccrps_;

#define vccrps_1 vccrps_

struct struct_vcpstc{
    integer kwrtfl, krdfl, koutfl, kinfl, kwrdsz, kbytel, kcpw, kbaud, kcomtp;
};

extern struct struct_vcpstc vcpstc_;

#define vcpstc_1 vcpstc_

struct struct_vcscal{
    f2c_float xscale, yscale;
};

extern struct struct_vcscal vcscal_;

#define vcscal_1 vcscal_

struct struct_vcddim{
    integer xpad, ypad, xdevic, ydevic;
};

extern struct struct_vcddim vcddim_;

#define vcddim_1 vcddim_

struct struct_vcjob{
    integer kidsiz, kjobid[4], kusrsz, kusrid[4], kszrou, kjrout[4], ksecur, 
	    kjtime[4], kjdate[4], machin[3], maclen;
};

extern struct struct_vcjob vcjob_;

#define vcjob_1 vcjob_

struct struct_devcap{
    f2c_float dev[33];
};

extern struct struct_devcap devcap_;

#define devcap_1 devcap_

struct struct_vcescp{
    integer pgform, patno, border;
};

extern struct struct_vcescp vcescp_;

#define vcescp_1 vcescp_

struct struct_vcpsta{
    f2c_float pstmlw;
    integer kpstbg, kpstci;
};

extern struct struct_vcpsta vcpsta_;

#define vcpsta_1 vcpsta_

struct struct_vcpstb{
    integer mopoly, mocolr;
};

extern struct struct_vcpstb vcpstb_;

#define vcpstb_1 vcpstb_

struct struct_vcpstd{
    integer kempty;
};

extern struct struct_vcpstd vcpstd_;

#define vcpstd_1 vcpstd_

struct struct_pcolst{
    f2c_float pcols[768]	/* was [3][256] */;
};

extern struct struct_pcolst pcolst_;

#define pcolst_1 pcolst_

struct struct_vcpage{
    integer totpag;
};

extern struct struct_vcpage vcpage_;

#define vcpage_1 vcpage_

struct struct_vcvec2{
    char coord[20], lstcrd[20];
};

extern struct struct_vcvec2 vcvec2_;

#define vcvec2_1 vcvec2_

struct struct_blotans{
    char bltans[2];
};

extern struct struct_blotans blotans_;

#define blotans_1 blotans_
  

#ifdef __cplusplus
	}
#endif

#endif
