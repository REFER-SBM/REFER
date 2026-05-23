//+-----------------------------------------------------------------------------+
//| Copyleft (C) 2025 REGEO (http://www.regeo.uji.es/)
//|
//| This program is free software
//| You can redistribute it and/or modify it under the terms of the GNU General Public License,
//| as published by the Free Software Foundation.
//| 
//| This program is distributed in the hope that it will be useful,
//| but WITHOUT ANY WARRANTY; without even the implied warranty of
//| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//+----------------------------------------------------------------------------*/

//Get labels from a graph-like representation of a polyhedrical shape

//Based on:
	//Varley P.A.C.
	//Automatic Creation of Boundary-Representation Models 
	//from Single Line Drawings, PhD Thesis, University of Wales. 2003

#ifndef _CUE_LABELS_VARLEY_CONSTANTS_DEFINED
	#define _CUE_LABELS_VARLEY_CONSTANTS_DEFINED

#define COMPILE_DEBUGGING 1
//#define BADNEWS (255)

#define POLYHEDRAL_VERSION
#undef CURVED_VERSION

	#ifndef M_PI
	#define M_PI 3.14159
	#endif


	//Labelling procedure
	#define LABELLING_COMBIN_ONLY          0x0001
	#define LABELLING_RELAXN_COMBIN        0x0002
	#define LABELLING_LAMBAN_RELAXN_COMBIN 0x0004
	#define LABELLING_COMBIN_LAMBAN_RELAXN 0x0008
	#define LABELLING_COMBIN_RELAXN        0x0010

	#define LambanAxesFreeK            (0)
	#define LambanAxesFixedK           (1)
	#define LambanAxesOptK             (2)
	#define LambanAxesMiddleK          (4)
	#define LambanAxesWhatK            (7)

	#define LambanAxesFixedIJ          (0)
	#define LambanAxesFrequentIJ       (8)
	#define LambanAxesLongestIJ        (0x10)

	#define LambanAxesFixedIJK          (LambanAxesFixedIJ|LambanAxesFixedK)
	#define LambanAxesFrequentIJK       (LambanAxesFrequentIJ|LambanAxesFreeK)
	#define LambanAxesFrequentIJFixedK  (LambanAxesFrequentIJ|LambanAxesFixedK)
	#define LambanAxesFrequentIJOptK    (LambanAxesFrequentIJ|LambanAxesOptK)
	#define LambanAxesFrequentIJMiddleK (LambanAxesFrequentIJ|LambanAxesMiddleK)
	#define LambanAxesLongestIJK        (LambanAxesLongestIJ|LambanAxesFreeK)
	#define LambanAxesLongestIJFixedK   (LambanAxesLongestIJ|LambanAxesFixedK)


	#define MAX_STORED_LABELLINGS (20)



/* ================================================================================ */

////#define OPTION_UNIFIED_FRONTAL_GEOMETRY ("af")
////#define OPTION_BINARY_FRONTAL_GEOMETRY  ("bf")
////
////#define OPTION_BUNDLING_LENIENT         ("bl")
////#define OPTION_BUNDLING_MERGE_SINGLE    ("bm")
////#define OPTION_BUNDLING_STRICT          ("bs")
////#define OPTION_PARALLEL_GENERAL_PRIOR   ("bp")
////#define OPTION_PARALLEL_QFACE_PRIOR     ("bq")
////#define OPTION_PARALLEL_REDUCTION       ("br")
////
////#define OPTION_FORCE_GENERAL_TOPOLOGY   ("cg")
////#define OPTION_CHEAT_FORCE_K            ("ck")
////#define OPTION_CHEAT_FORCE_M            ("cm")
////#define OPTION_FORCE_SYMMETRIC_TOPOLOGY ("cs")
////#define OPTION_CHEAT_FORCE_TRIHEDRAL    ("ct")
////#define OPTION_CHEAT_FORCE_EXTRUSION    ("cx")
////
////#define OPTION_USE_BUNDLES_IN_DEPTH     ("db")
////#define OPTION_NO_BUNDLES_IN_DEPTH      ("dc")
////#define OPTION_DEFAULT_DEPTH_SETTINGS   ("dd")
////#define OPTION_USE_PLANARITY_IN_DEPTH   ("df")
////#define OPTION_ISOMETRIC_DEPTH          ("di")
////#define OPTION_LIST_DEPTH_DATA          ("dl")
////#define OPTION_NO_PERKINS_DEPTH         ("dn")
////#define OPTION_USE_LT_AND_YT_IN_DEPTH   ("do")
////#define OPTION_PERKINS_DEPTH            ("dp")
////#define OPTION_QFACE_DEPTH              ("dq")
////#define OPTION_USE_TJUNCTIONS_IN_DEPTH  ("dt")
////
////#define OPTION_FEATURE_BDIV             ("fb")
////#define DEFAULT_FEATURE_BDIV            (1.06)
////#define OPTION_FEATURE_OPOW             ("fc")
////#define DEFAULT_FEATURE_OPOW            (3)
////#define OPTION_MERIT_OUTER_SUBGRAPH     ("fo")
////#define OPTION_MERIT_UNDERSLOT_FEATURE  ("fu")
////#define DEFAULT_MERIT_UNDERSLOT_FEATURE (1.000)
////#define OPTION_MERIT_VALLEY_FEATURE     ("fv")
////
////#define OPTION_SKIP_COPLANARITY_CONSTRAINTS   ("gc")
////#define OPTION_SKIP_EQUAL_LENGTH_CONSTRAINTS  ("ge")
////#define OPTION_SKIP_LENGTH_RATIO_CONSTRAINTS  ("gl")
////#define OPTION_SKIP_NON_TRIHEDRAL_CONSTRAINTS ("gv")
////
////#define OPTION_GEOMETRY_DISTANCE_BESTFIT ("gb")
////#define OPTION_GEOMETRY_DISTANCE_GREEDY  ("gg")
////#define OPTION_GEOMETRY_DISTANCE_SIMPLE  ("gq")
////#define OPTION_GEOMETRY_DISTANCE_CLEVER  ("gr")
////
////#define OPTION_GEOMETRY_NORMALS_ANGULAR  ("ga")
////#define OPTION_GEOMETRY_NORMALS_DOWNHILL ("gd")
////#define OPTION_GEOMETRY_NORMALS_SIMPLE   ("gs")
////#define OPTION_GEOMETRY_NORMALS_CLEVER   ("gt")
////
////#define OPTION_GEOMETRY_DEFAULT_SETTINGS ("go")
////
////#define OPTION_TUNE_NORMALON_GEOMETRY    ("gx")
////#define DEFAULT_NORMALON_GEOMETRY        (0.010)
////#define OPTION_TUNE_NONNORMALON_GEOMETRY ("gy")
////#define DEFAULT_NONNORMALON_GEOMETRY     (0.252)
////#define MULTIPLIER_TUNE_GEOMETRY         (10.0)
////
////#define OPTION_JMSKETCH_FORMAT          ("jm")
////#define OPTION_JMSKETCH_STROKES         ("js")
////
////#define OPTION_JUNCTIONS_TRIHEDRAL      ("jt")
////
/////* leave K free for tuning constants */
////
////#define OPTION_LIST_EDGE_DATA           ("le")
////#define OPTION_LIST_FINAL_OBJECT        ("lf")
////#define OPTION_LIST_INITIAL_SKETCH      ("li")
////#define OPTION_LIST_JUNCTION_INFO       ("lj")
////#define OPTION_LIST_TIMING              ("lt")
////
////#define OPTION_TUNE_DISTANCE_FUSS_S     ("mc")
////#define OPTION_TUNE_DISTANCE_FUSSINESS  ("md")
////#define OPTION_TUNE_DISTANCE_FUSS_T     ("me")
////#define OPTION_TUNE_PARALLEL_FUSS_S     ("mo")
////#define OPTION_TUNE_PARALLEL_FUSSINESS  ("mp")
////#define OPTION_TUNE_PARALLEL_FUSS_T     ("mq")
////#define OPTION_TUNE_RATIO_FUSSINESS     ("mr")
////#define OPTION_TUNE_LAMBAN_EXPAND       ("oe")
////#define OPTION_TUNE_LAMBAN_KAXIS        ("ok")
////#define OPTION_TUNE_LAMBAN_OCCLUDING    ("oo")
////#define OPTION_TUNE_LAMBAN_OUT_OF_PLANE ("op")
////#define OPTION_TUNE_LAMBAN_CONFIDENCE   ("oq")
////#define OPTION_TUNE_LAMBAN_MAJOR_AXIS   ("oy")
////#define OPTION_TUNE_LAMBAN_MINOR_AXIS   ("oz")
////
////#define OPTION_CREATE_POSTSCRIPT_FILE   ("ps")
////#define OPTION_POSTSCRIPT_HEIGHT        ("ph")
////#define OPTION_POSTSCRIPT_WIDTH         ("pw")
////#define OPTION_POSTSCRIPT_TEXTSIZE      ("pt")
////#define OPTION_POSTSCRIPT_BORDER        ("pb")
////#define OPTION_POSTSCRIPT_VERTICES      ("pv")
////#define OPTION_POSTSCRIPT_EDGES         ("pe")
////#define OPTION_POSTSCRIPT_LABELS        ("pl")
////#define OPTION_POSTSCRIPT_BUNDLES       ("pp")
////#define OPTION_POSTSCRIPT_MIRRORS       ("pm")
////
////#define OPTION_USE_VERTEX_ROTATIONS     ("rv")
////
////#define OPTION_SINGLE_STEP              ("ss")
////#define OPTION_TIME_LIMIT               ("st")
////
////#define OPTION_MERIT_RELAXN_VFINALTYPE   ("sm")
////#define OPTION_MERIT_RELAXN_TOCCLUSION   ("sn")
////#define OPTION_MERIT_RELAXN_SLOWEDGES    ("so")
////#define OPTION_MERIT_RELAXN_SLOWVERTICES ("sp")
////#define OPTION_MERIT_RELAXN_CONFIGS      ("sq")
////#define OPTION_MERIT_RELAXN_FEATURES     ("sr")
////#define OPTION_MERIT_VERTEX_DEPTHS       ("su")
////#define OPTION_MERIT_TJUNCTION_DEPTH     ("sv")
////#define OPTION_MERIT_CROSSMIRROR_EDGE    ("sw")
////#define OPTION_MERIT_NORMAL_CROSSING     ("sx")
////#define OPTION_MERIT_DIFFERENT_SUBGRAPHS ("sy")
////#define OPTION_MERIT_DIFFERENT_FEATURES  ("sz")
////#define OPTION_MERIT_LAST2_EDGE          ("ta")
////#define OPTION_MERIT_LAST2_VERTEX        ("tb")
////#define OPTION_MERIT_LAST3_VERTEX        ("tc")
////#define OPTION_MERIT_LAST4_EDGE          ("td")
////#define OPTION_MERIT_ADDITIONAL_EDGE     ("te")
////#define OPTION_MERIT_LOCAL_THYPOTHESIS   ("tf")
////#define OPTION_MERIT_DISTANT_THYPOTHESIS ("tg")
////#define OPTION_MERIT_AA232_CONVEX        ("th")
////#define OPTION_MERIT_AA232_MIXED         ("ti")
////#define OPTION_MERIT_AA2332_CONVEX       ("tj")
////#define OPTION_MERIT_AA2332_MIXED        ("tk")
////#define OPTION_MERIT_MIRROR_DISCRETE     ("tl")
////#define OPTION_MERIT_MIRROR_MACRO        ("tm")
////#define OPTION_MERIT_SUSPICIOUS_EDGE     ("tn")
////#define OPTION_MERIT_NEW_TRIANGLES       ("to")
////#define OPTION_MERIT_BAD_VERTEX_POSITION ("tp")
////#define OPTION_MERIT_QUADRILATERAL_LOOP  ("tq")
////#define OPTION_MERIT_EDGELENGTH_DROPOFF  ("tr")
////#define OPTION_MERIT_EDGELENGTH_POWER    ("ts")
////#define OPTION_MERIT_V232_CONVEX         ("tt")
////#define OPTION_MERIT_V232_MIXED          ("tu")
////#define OPTION_MERIT_V2332_CONVEX        ("tv")
////#define OPTION_MERIT_V2332_MIXED         ("tw")
////#define OPTION_MERIT_BIAS_2332           ("tx")
////#define OPTION_MERIT_BIAS_232            ("ty")
////#define OPTION_MERIT_K_INTERPRETATION    ("tz")

	#define DEFAULT_MERIT_DIFFERENT_SUBGRAPHS (0.851)
	#define DEFAULT_MERIT_DIFFERENT_FEATURES (0.863)
	#define DEFAULT_MERIT_AA2332_MIXED       (0.511)
	#define DEFAULT_MERIT_MIRROR_DISCRETE    (0.885)
	#define DEFAULT_MERIT_MIRROR_MACRO       (0.656)
	#define DEFAULT_MERIT_SUSPICIOUS_EDGE    (0.413)
	#define DEFAULT_MERIT_NEW_TRIANGLES      (0.614)
	#define DEFAULT_MERIT_BAD_VERTEX_POSITION (0.145)
	#define DEFAULT_MERIT_QUADRILATERAL_LOOP (0.924)
	#define DEFAULT_MERIT_EDGELENGTH_DROPOFF (0.696)
	#define DEFAULT_MERIT_EDGELENGTH_POWER   (0.748)
	#define DEFAULT_MERIT_V232_MIXED         (0.918)
	#define DEFAULT_MERIT_V2332_CONVEX       (0.998)
	#define DEFAULT_MERIT_V2332_MIXED        (0.950)
	#define DEFAULT_MERIT_BIAS_2332          (0.307)
	#define DEFAULT_MERIT_BIAS_232           (0.071)
	#define DEFAULT_MERIT_K_INTERPRETATION   (0.555)

////
////#define OPTION_VALIDATE_LABELLING       ("vl")
////#define OPTION_VALIDATE_OCCLUSION       ("vo")
////#define OPTION_VALIDATE_PARALLEL        ("vp")
////#define OPTION_VALIDATE_DEPTH           ("vd")
////#define OPTION_VALIDATE_DEPTH_COORDS    ("vz")
////#define OPTION_VALIDATE_FRONT_GEOMETRY  ("vf")
////#define OPTION_VALIDATE_TOPOLOGY        ("vt")
////#define OPTION_VALIDATE_FINISHED_OBJECT ("vg")
////
////#define OPTION_RUN_NOTHING              ("xn")
////#define OPTION_RUN_LABELLING_ONLY       ("xl")
////#define OPTION_RUN_PARALLEL_ONLY        ("xp")
////#define OPTION_RUN_DEPTH_ONLY           ("xd")
////#define OPTION_RUN_FRONT_GEOMETRY_ONLY  ("xf")
////#define OPTION_RUN_TOPOLOGY             ("xt")
////
////#define OPTION_TUNE_UFG_CONEANGLE       ("aa")
////#define OPTION_TUNE_UFG_CCSLOWS         ("ab")
////#define OPTION_TUNE_UFG_CUBICS          ("ac")
////#define OPTION_TUNE_UFG_CCSLOWT         ("ad")
////#define OPTION_TUNE_UFG_CUBICT          ("ae")
////#define OPTION_TUNE_UFG_PARALLELS       ("af")
////#define OPTION_TUNE_UFG_VTSLOWS         ("ag")
////#define OPTION_TUNE_UFG_IJKSLOWS        ("ah")
////#define OPTION_TUNE_UFG_IJKSLOWT        ("ai")
////#define OPTION_TUNE_UFG_TJUNC           ("aj")
////#define OPTION_TUNE_UFG_VTSLOWT         ("ak")
////#define OPTION_TUNE_UFG_AXESS           ("al")
////#define OPTION_TUNE_UFG_XSLOWS          ("am")
////#define OPTION_TUNE_UFG_INPLANES        ("an")
////#define OPTION_TUNE_UFG_OTHER           ("ao")
////#define OPTION_TUNE_UFG_PARALLELT       ("ap")
////#define OPTION_TUNE_UFG_LLSLOWS         ("aq")
////#define OPTION_TUNE_UFG_OCCSLOWS        ("ar")
////#define OPTION_TUNE_UFG_XSLOWT          ("as")
////#define OPTION_TUNE_UFG_PARTIAL         ("at")
////#define OPTION_TUNE_UFG_OCCSLOWT        ("au")
////#define OPTION_TUNE_UFG_INPLANET        ("av")
////#define OPTION_TUNE_UFG_IJKINIT         ("aw")
////#define OPTION_TUNE_UFG_AXEST           ("ax")
////#define OPTION_TUNE_UFG_LLSLOWT         ("ay")
////#define OPTION_TUNE_UFG_ZSLOW           ("az")
////#define OPTION_TUNE_UFG_TJUNC_K         ("bg")
////#define OPTION_TUNE_UFG_LLHALFWAY       ("bh")
////#define OPTION_TUNE_UFG_CLSLOWS         ("bi")
////#define OPTION_TUNE_UFG_CLSLOWT         ("bj")
////#define OPTION_TUNE_UFG_COLLINEARS      ("bk")
////#define OPTION_TUNE_UFG_COLLINEART      ("bl")
////#define OPTION_TUNE_UFG_KANADE          ("bm")
////#define OPTION_TUNE_UFG_PRSLOWS         ("bn")
////#define OPTION_TUNE_UFG_PRSLOWT         ("bo")
////#define OPTION_TUNE_UFG_PRMERITS        ("bp")
////#define OPTION_TUNE_UFG_PRMERITT        ("bq")
////#define OPTION_TUNE_UFG_FULLFACE        ("br")
////#define OPTION_TUNE_UFG_LBA             ("bs")
////#define OPTION_TUNE_UFG_LAB             ("bt")
////#define OPTION_TUNE_UFG_WBCA            ("bu")
////#define OPTION_TUNE_UFG_WCDC            ("bv")
////#define OPTION_TUNE_UFG_WDCD            ("bw")
////#define OPTION_TUNE_UFG_YABD            ("bx")
////#define OPTION_TUNE_UFG_YCCC            ("by")
////#define OPTION_TUNE_UFG_YDDD            ("bz")
////
////#define OPTION_TUNE_OCCLUDE             ("ea")
////#define OPTION_TUNE_CONVEX              ("ec")
////#define OPTION_TUNE_CONCAVE             ("ed")
////
////#define OPTION_TUNE_LBA                 ("ua")
////#define OPTION_TUNE_LAB                 ("ub")
////#define OPTION_TUNE_LAC                 ("uc")
////#define OPTION_TUNE_LBD                 ("ud")
////#define OPTION_TUNE_LAA                 ("ue")
////#define OPTION_TUNE_XCCCC               ("uf")
////#define OPTION_TUNE_XCCCD               ("ug")
////#define OPTION_TUNE_XABCD               ("uh")
////#define OPTION_TUNE_XABDD               ("ui")
////#define OPTION_TUNE_MBCCA               ("um")
////#define OPTION_TUNE_MBCDA               ("un")
////#define OPTION_TUNE_MCCDC               ("uo")
////#define OPTION_TUNE_MCDCD               ("up")
////#define OPTION_TUNE_MCDDC               ("uq")
////#define OPTION_TUNE_MDCCD               ("ur")
////#define OPTION_TUNE_TBAA                ("ut")
////#define OPTION_TUNE_TBAC                ("uu")
////#define OPTION_TUNE_TBAD                ("uv")
////#define OPTION_TUNE_TBDA                ("uw")
////#define OPTION_TUNE_TBDC                ("ux")
////#define OPTION_TUNE_WBCA                ("wb")
////#define OPTION_TUNE_WCDC                ("wc")
////#define OPTION_TUNE_WDCD                ("wd")
////#define OPTION_TUNE_WBDA                ("we")
////#define OPTION_TUNE_WBAA                ("wf")
////#define OPTION_TUNE_WACD                ("wg")
////#define OPTION_TUNE_WCAC                ("wh")
////#define OPTION_TUNE_WBCD                ("wi")
////#define OPTION_TUNE_WBDC                ("wj")
////#define OPTION_TUNE_KABCD               ("wk")
////#define OPTION_TUNE_KCCCD               ("wl")
////#define OPTION_TUNE_KCDCD               ("wm")
////#define OPTION_TUNE_KDDCD               ("wn")
////#define OPTION_TUNE_TABA                ("wt")
////#define OPTION_TUNE_TBCA                ("wu")
////#define OPTION_TUNE_TDDA                ("wv")
////#define OPTION_TUNE_YABD                ("ya")
////#define OPTION_TUNE_YCCC                ("yc")
////#define OPTION_TUNE_YDDD                ("yd")
////#define OPTION_TUNE_YAAB                ("ye")
////#define OPTION_TUNE_YABC                ("yf")
////#define OPTION_TUNE_YACC                ("yg")
////#define OPTION_TUNE_YADD                ("yh")
////#define OPTION_TUNE_YACD                ("yi")

/* ================================================================================ */

#define DEPTH_AUTO_SELECT         'A'
#define DEPTH_PERKINS             'P'
#define DEPTH_JLP                 'J'
#define DEPTH_ALL_FACES           'F'
#define DEPTH_QUADRILATERAL_FACES 'Q'
#define DEPTH_NO_FACES            'N'



#define NO_VERTEX (-1)
#define NO_EDGE   (-1)
#define NO_FACE   (-1)
#define BACKGROUND_FACE (-2)

#ifdef POLYHEDRAL_VERSION

// The highest number of vertices in practice is 98
#define MAX_VERTICES 120
// The highest number of edges in practice is 152
#define MAX_EDGES    180
#define MAX_FACES    120
#define MAX_FEATURES  16
#define MAX_COFACIAL  16
#define MAX_ELLIPSES   2

#define MAX_FACES_PER_VERTEX   8
#define MAX_EDGES_PER_VERTEX   8

// This needs to be big because the object outline is a loop
#define MAX_SIDES_PER_LOOP    64
#define MAX_LOOPS_PER_FACE     8

#define MAX_BUNDLES           90
#define MAX_MIRROR           120
#define MAX_GRAPHLOOPS         8
#define MAX_SPLITS            MAX_GRAPHLOOPS

#else

#define MAX_VERTICES 240
#define MAX_EDGES    480
#define MAX_FACES    240
#define MAX_FEATURES  16
#define MAX_COFACIAL  16
#define MAX_ELLIPSES  16

#define MAX_FACES_PER_VERTEX   8
#define MAX_EDGES_PER_VERTEX   8
#define MAX_SIDES_PER_LOOP   128
#define MAX_LOOPS_PER_FACE     8

#define MAX_BUNDLES           90
#define MAX_MIRROR           180
#define MAX_GRAPHLOOPS         8
#define MAX_SPLITS            MAX_GRAPHLOOPS

#endif


/* -------------------------------------------------------------------------------- */
// Combinatorial labelling options

#define LABELLING_TRIHEDRAL     '3'
#define LABELLING_TRIHEDRAL_LWY 'B'
#define LABELLING_NTAA          'A'
#define LABELLING_TETRAHEDRAL   '4'
#define LABELLING_PENTAHEDRAL   '5'
#define LABELLING_HEXAHEDRAL    '6'

/* ================================================================================ */

#define LineIsConvex    0x0001
#define LineIsConcave   0x0002
#define LineIsArrow     0x0004
#define LineIsBackArrow 0x0008
#define LineIsUnknown   0x000F
#define LineIsUnknown3D (LineIsConvex|LineIsConcave)
#define LineIsVisible   (LineIsConvex|LineIsConcave)

/* ================================================================================ */
//JUNCTION CATALOGUE

/* cryptic labels, a == arrow head, b == arrow back, c == convex (cross), d == concave (dent) */

/* -------------------------------------------------------------------------------- */

//#define JunctionIsL     'L'
#define Lac   0x0001
#define Lab   0x0002
#define Lcb   0x0004
#define Lbd   0x0008
#define Lba   0x0010
#define Lda   0x0020

#define Laa   0x0040
#define Lbb   0x0080

#define Ltrihedral (Lba|Lab|Lac|Lcb|Lbd|Lda)
#define Lnontrihedral (Laa|Lbb)
#define Lall (Ltrihedral|Lnontrihedral)

#define Lax (Lac|Lab|Laa)
#define Lxa (Lba|Lda|Laa)

#define Lbx (Lba|Lbd|Lbb)
#define Lxb (Lab|Lcb|Lbb)

#define Lcx (Lcb)
#define Lxc (Lac)

#define Ldx (Lda)
#define Lxd (Lbd)

/* -------------------------------------------------------------------------------- */

//#define JunctionIsW     'W'
#define Wbca  0x0001
#define Wdcd  0x0002
#define Wcdc  0x0004

#define Wabc  0x00000008
#define Wabd  0x00000010
#define Wacc  0x00000020
#define Wacd  0x00000040
#define Wadc  0x00000080
#define Wbaa  0x00000100
#define Wbba  0x00000200
#define Wbcd  0x00000400
#define Wbda  0x00000800
#define Wbdc  0x00001000
#define Wbdd  0x00002000
#define Wcab  0x00004000
#define Wcac  0x00008000
#define Wcbc  0x00010000
#define Wcbd  0x00020000
#define Wccb  0x00040000
#define Wcda  0x00080000
#define Wcdb  0x00100000
#define Wdab  0x00200000
#define Wdac  0x00400000
#define Wdad  0x00800000
#define Wdbd  0x01000000
#define Wdca  0x02000000
#define Wdcb  0x04000000
#define Wdda  0x08000000

#define Wtrihedral (Wbca|Wcdc|Wdcd)
#define Wnontrihedral (Wabc|Wabd|Wacc|Wacd|Wadc|Wbaa|Wbba|Wbcd|Wbda|Wbdc|Wbdd|Wcab|Wcac|Wcbc|Wcbd|Wccb|Wcda|Wcdb|Wdab|Wdac|Wdad|Wdbd|Wdca|Wdcb|Wdda)
#define Wall (Wtrihedral|Wnontrihedral)

#define Waxx (Wabc|Wabd|Wacc|Wacd|Wadc)
#define Wxax (Wbaa|Wcab|Wcac|Wdab|Wdac|Wdad)
#define Wxxa (Wbca|Wbaa|Wbba|Wbda|Wcda|Wdca|Wdda)

#define Wbxx (Wbca|Wbaa|Wbba|Wbcd|Wbda|Wbdc|Wbdd)
#define Wxbx (Wabc|Wabd|Wbba|Wcbc|Wcbd|Wdbd)
#define Wxxb (Wcab|Wccb|Wcdb|Wdab|Wdcb)

#define Wcxx (Wcdc|Wcab|Wcac|Wcbc|Wcbd|Wccb|Wcda|Wcdb)
#define Wxcx (Wbca|Wdcd|Wacc|Wacd|Wbcd|Wccb|Wdca|Wdcb)
#define Wxxc (Wcdc|Wabc|Wacc|Wadc|Wbdc|Wcac|Wcbc|Wdac)

#define Wdxx (Wdcd|Wdab|Wdac|Wdad|Wdbd|Wdca|Wdcb|Wdda)
#define Wxdx (Wcdc|Wadc|Wbda|Wbdc|Wbdd|Wcda|Wcdb|Wdda)
#define Wxxd (Wdcd|Wabd|Wacd|Wbcd|Wbdd|Wcbd|Wdad|Wdbd)

/* -------------------------------------------------------------------------------- */

//#define JunctionIsY     'Y'
#define Yccc  0x0001
#define Yddd  0x0002
#define Yabd  0x0004
#define Ydab  0x0008
#define Ybda  0x0010

#define Yaab  0x00000020
#define Yaba  0x00000040
#define Yabb  0x00000080
#define Yabc  0x00000100
#define Yacc  0x00000200
#define Yacd  0x00000400
#define Yadd  0x00000800
#define Ybaa  0x00001000
#define Ybab  0x00002000
#define Ybba  0x00004000
#define Ybca  0x00008000
#define Ybcc  0x00010000
#define Ybdc  0x00020000
#define Ybdd  0x00040000
#define Ycab  0x00080000
#define Ycac  0x00100000
#define Ycbc  0x00200000
#define Ycbd  0x00400000
#define Ycca  0x00800000
#define Yccb  0x01000000
#define Ycda  0x02000000
#define Ydac  0x04000000
#define Ydad  0x08000000
#define Ydbd  0x10000000
#define Ydcb  0x20000000
#define Ydda  0x40000000
#define Yddb  0x80000000

#define Ytrihedral (Yccc|Yddd|Yabd|Ybda|Ydab)
#define Ynontrihedral (Yaab|Yaba|Yabb|Yabc|Yacc|Yacd|Yadd|Ybaa|Ybab|Ybba|Ybca|Ybcc|Ybdc|Ybdd|Ycab|Ycac|Ycbc|Ycbd|Ycca|Yccb|Ycda|Ydac|Ydad|Ydbd|Ydcb|Ydda|Yddb)
#define Yall (Ytrihedral|Ynontrihedral)

#define Yaxx (Yabd|Yaab|Yaba|Yabb|Yabc|Yacc|Yacd|Yadd)
#define Yxax (Ydab|Yaab|Ybaa|Ybab|Ycab|Ycac|Ydac|Ydad)
#define Yxxa (Ybda|Yaba|Ybaa|Ybba|Ybca|Ycca|Ycda|Ydda)

#define Ybxx (Ybda|Ybaa|Ybab|Ybba|Ybca|Ybcc|Ybdc|Ybdd)
#define Yxbx (Yabd|Yaba|Yabb|Yabc|Ybba|Ycbc|Ycbd|Ydbd)
#define Yxxb (Ydab|Yaab|Yabb|Ybab|Ycab|Yccb|Ydcb|Yddb)

#define Ycxx (Yccc|Ycab|Ycac|Ycbc|Ycbd|Ycca|Yccb|Ycda)
#define Yxcx (Yccc|Yacc|Yacd|Ybca|Ybcc|Ycca|Yccb|Ydcb)
#define Yxxc (Yccc|Yabc|Yacc|Ybcc|Ybdc|Ycac|Ycbc|Ydac)

#define Ydxx (Yddd|Ydab|Ydac|Ydad|Ydbd|Ydcb|Ydda|Yddb)
#define Yxdx (Yddd|Ybda|Yadd|Ybdc|Ybdd|Ycda|Ydda|Yddb)
#define Yxxd (Yddd|Yabd|Yacd|Yadd|Ybdd|Ycbd|Ydad|Ydbd)

/* -------------------------------------------------------------------------------- */

//#define JunctionIsT     'T'
#define Tbaa  0x0001
#define Tbab  0x0002
#define Tbac  0x0004
#define Tbad  0x0008

#define Ttrihedral (Tbaa|Tbab|Tbac|Tbad)

//#define JunctionIsT4    '4'
#define Tbda  0x0010
#define Tbdc  0x0020
#define Tdab  0x0040
#define Tdac  0x0080


#define Taba  0x00000100
#define Tabb  0x00000200
#define Tbca  0x00000400
#define Tbcc  0x00000800
#define Tcab  0x00001000
#define Tcac  0x00002000
#define Tcca  0x00004000
#define Tccb  0x00008000
#define Tcda  0x00010000
#define Tdcb  0x00020000
#define Tdda  0x00040000
#define Tddb  0x00080000

#define TNTAA (Tbda|Tbdc|Tdab|Tdac)
#define Tnontrihedral (Taba|Tabb|Tbca|Tbcc|Tcab|Tcac|Tcca|Tccb|Tcda|Tdcb|Tdda|Tddb)
#define Tall (Ttrihedral|TNTAA|Tnontrihedral)

#define Taxx (Taba|Tabb)
#define Txax (Tbaa|Tbab|Tbac|Tbad|Tdab|Tdac|Tcab|Tcac)
#define Txxa (Tbaa|Tbda|Taba|Tbca|Tcca|Tcda|Tdda)

#define Tbxx (Tbaa|Tbab|Tbac|Tbad|Tbda|Tbdc|Tbca|Tbcc)
#define Txbx (Taba|Tabb)
#define Txxb (Tbab|Tdab|Tabb|Tcab|Tccb|Tdcb|Tddb)

#define Tcxx (Tcab|Tcac|Tcca|Tccb|Tcda)
#define Txcx (Tbca|Tbcc|Tcca|Tccb|Tdcb)
#define Txxc (Tbac|Tbdc|Tdac|Tbcc|Tcac)

#define Tdxx (Tdab|Tdac|Tdcb|Tdda|Tddb)
#define Txdx (Tbda|Tbdc|Tcda|Tdda|Tddb)
#define Txxd (Tbad)

/* -------------------------------------------------------------------------------- */

//#define JunctionIsZ     'Z'
#define Zcbda   0x00000001
#define ZNTAA (Zcbda)

/* -------------------------------------------------------------------------------- */

//#define JunctionIsX     'X'

#define Xcccc   0x00000004
#define Xcccd   0x00000008
#define Xccdc   0x00000010
#define Xcdcc   0x00000020
#define Xdccc   0x00000040
#define Xcdcd   0x00000080
#define Xdcdc   0x00000100
#define Xcddd   0x00000200
#define Xdcdd   0x00000400
#define Xddcd   0x00000800
#define Xdddc   0x00001000
#define Xdddd   0x00002000
#define Xabcd   0x00004000
#define Xbcda   0x00008000
#define Xcdab   0x00010000
#define Xdabc   0x00020000
#define Xabdc   0x00040000
#define Xbdca   0x00080000
#define Xdcab   0x00100000
#define Xcabd   0x00200000
#define Xabdd   0x00400000
#define Xbdda   0x00800000
#define Xddab   0x01000000
#define Xdabd   0x02000000

#define Xnontrihedral (Xcccc|Xcccd|Xccdc|Xcdcc|Xdccc|Xcdcd|Xdcdc|Xcddd|Xdcdd|Xddcd|Xdddc|Xdddd|Xabcd|Xbcda|Xcdab|Xdabc|Xabdc|Xbdca|Xdcab|Xcabd|Xabdd|Xbdda|Xddab|Xdabd)
#define Xall (Xnontrihedral)

#define Xaxxx (Xabcd|Xabdc|Xabdd)
#define Xxaxx (Xdabc|Xcabd|Xdabd)
#define Xxxax (Xcdab|Xdcab|Xddab)
#define Xxxxa (Xbcda|Xbdca|Xbdda)

#define Xbxxx (Xbcda|Xbdca|Xbdda)
#define Xxbxx (Xabcd|Xabdc|Xabdd)
#define Xxxbx (Xdabc|Xcabd|Xdabd)
#define Xxxxb (Xcdab|Xdcab|Xddab)

#define Xcxxx (Xcccc|Xcccd|Xccdc|Xcdcc|Xcdcd|Xcddd|Xcdab|Xcabd)
#define Xxcxx (Xcccc|Xcccd|Xccdc|Xdccc|Xdcdc|Xdcdd|Xbcda|Xdcab)
#define Xxxcx (Xcccc|Xcccd|Xcdcc|Xdccc|Xcdcd|Xddcd|Xabcd|Xbdca)
#define Xxxxc (Xcccc|Xccdc|Xcdcc|Xdccc|Xdcdc|Xdddc|Xdabc|Xabdc)

#define Xdxxx (Xdccc|Xdcdc|Xdcdd|Xddcd|Xdddc|Xdddd|Xdabc|Xdcab|Xddab|Xdabd)
#define Xxdxx (Xcdcc|Xcdcd|Xcddd|Xddcd|Xdddc|Xdddd|Xcdab|Xbdca|Xbdda|Xddab)
#define Xxxdx (Xccdc|Xdcdc|Xcddd|Xdcdd|Xdddc|Xdddd|Xbcda|Xabdc|Xabdd|Xbdda)
#define Xxxxd (Xcccd|Xcdcd|Xcddd|Xdcdd|Xddcd|Xdddd|Xabcd|Xcabd|Xabdd|Xdabd)

/* -------------------------------------------------------------------------------- */

//#define JunctionIsK     'K'
#define Kabcd   0x0001
#define Kabdc   0x0002
#define Kcccd   0x0004
#define Kccdc   0x0008
#define Kcdcd   0x0010
#define Kdcdc   0x0020
#define Kddcd   0x0040
#define Kdddc   0x0080

#define Knontrihedral (Kabcd|Kabdc|Kcccd|Kccdc|Kcdcd|Kdcdc|Kddcd|Kdddc)
#define Kall (Knontrihedral)

#define Kaxxx (Kabcd|Kabdc)
#define Kxaxx (0)
#define Kxxax (0)
#define Kxxxa (0)

#define Kbxxx (0)
#define Kxbxx (Kabcd|Kabdc)
#define Kxxbx (0)
#define Kxxxb (0)

#define Kcxxx (Kcccd|Kccdc|Kcdcd)
#define Kxcxx (Kcccd|Kccdc|Kdcdc)
#define Kxxcx (Kcccd|Kcdcd|Kddcd|Kabcd)
#define Kxxxc (Kccdc|Kdcdc|Kdddc|Kabdc)

#define Kdxxx (Kdcdc|Kddcd|Kdddc)
#define Kxdxx (Kcdcd|Kddcd|Kdddc)
#define Kxxdx (Kccdc|Kdcdc|Kdddc|Kabdc)
#define Kxxxd (Kcccd|Kcdcd|Kddcd|Kabcd)

//#define JunctionIsKplus '#'

/* -------------------------------------------------------------------------------- */

//#define JunctionIsM     'M'
#define Mbcca   0x0001
#define Mbcda   0x0002
#define Mbdca   0x0004
#define Mccdc   0x0008
#define Mcdcc   0x0010
#define Mcdcd   0x0020
#define Mcddc   0x0040
#define Mdccd   0x0080
#define Mdcdc   0x0100
#define Mdcdd   0x0200
#define Mddcd   0x0400

#define Mnontrihedral (Mbcca|Mbcda|Mbdca|Mccdc|Mcdcc|Mcdcd|Mcddc|Mdccd|Mdcdc|Mdcdd|Mddcd)
#define Mall (Mnontrihedral)

#define Maxxx (0)
#define Mxaxx (0)
#define Mxxax (0)
#define Mxxxa (Mbcca|Mbcda|Mbdca)

#define Mbxxx (Mbcca|Mbcda|Mbdca)
#define Mxbxx (0)
#define Mxxbx (0)
#define Mxxxb (0)

#define Mcxxx (Mccdc|Mcdcc|Mcdcd|Mcddc)
#define Mxcxx (Mbcca|Mbcda|Mccdc|Mdcdd|Mdcdc|Mdcdd)
#define Mxxcx (Mbcca|Mbdca|Mcdcc|Mcdcd|Mdccd|Mddcd)
#define Mxxxc (Mccdc|Mcdcc|Mcddc|Mdcdc)

#define Mdxxx (Mdccd|Mdcdc|Mdcdd|Mddcd)
#define Mxdxx (Mbdca|Mcdcc|Mcdcd|Mcddc|Mddcd)
#define Mxxdx (Mbcda|Mccdc|Mcddc|Mdcdc|Mdcdd)
#define Mxxxd (Mcdcd|Mdccd|Mdcdd|Mddcd)

/* -------------------------------------------------------------------------------- */

//#define JunctionIs5way  '5'

#define Xccccc 0x000001
#define Xddddd 0x000002
#define Xbccca 0x000004
#define Xabccc 0x000008
#define Xcabcc 0x000010
#define Xccabc 0x000020
#define Xcccab 0x000040
#define Xbcdca 0x000080
#define Xabcdc 0x000100
#define Xcabcd 0x000200
#define Xdcabc 0x000400
#define Xcdcab 0x000800
#define Xccccd 0x001000
#define Xcccdc 0x002000
#define Xccdcc 0x004000
#define Xcdccc 0x008000
#define Xdcccc 0x010000
#define Xccdcd 0x020000
#define Xcdcdc 0x040000
#define Xdcdcc 0x080000
#define Xcdccd 0x100000
#define Xdccdc 0x200000

#define X5all (Xccccc|Xddddd|Xbccca|Xabccc|Xcabcc|Xccabc|Xcccab|Xbcdca|Xabcdc|Xcabcd|Xdcabc|Xcdcab|Xccccd|Xcccdc|Xccdcc|Xcdccc|Xdcccc|Xccdcd|Xcdcdc|Xdcdcc|Xcdccd|Xdccdc)

#define Xaxxxx (Xabccc|Xabcdc)
#define Xxaxxx (Xcabcc|Xcabcd)
#define Xxxaxx (Xccabc|Xdcabc)
#define Xxxxax (Xcccab|Xcdcab)
#define Xxxxxa (Xbccca|Xbcdca)

#define Xbxxxx (Xbccca|Xbcdca)
#define Xxbxxx (Xabccc|Xabcdc)
#define Xxxbxx (Xcabcc|Xcabcd)
#define Xxxxbx (Xccabc|Xdcabc)
#define Xxxxxb (Xcccab|Xcdcab)

#define Xcxxxx (Xccccc|Xcabcc|Xccabc|Xcccab|Xcabcd|Xcdcab|Xccccd|Xcccdc|Xccdcc|Xcdccc|Xccdcd|Xcdcdc|Xcdccd)
#define Xxcxxx (Xccccc|Xbccca|Xccabc|Xcccab|Xbcdca|Xdcabc|Xccccd|Xcccdc|Xccdcc|Xdcccc|Xccdcd|Xdcdcc|Xdccdc)
#define Xxxcxx (Xccccc|Xbccca|Xabccc|Xcccab|Xabcdc|Xcdcab|Xccccd|Xcccdc|Xcdccc|Xdcccc|Xcdcdc|Xcdccd|Xdccdc)
#define Xxxxcx (Xccccc|Xbccca|Xabccc|Xcabcc|Xbcdca|Xcabcd|Xccccd|Xccccd|Xcdccc|Xdcccc|Xccdcd|Xcdccd|Xdcdcc)
#define Xxxxxc (Xccccc|Xabccc|Xcabcc|Xccabc|Xabcdc|Xdcabc|Xcccdc|Xccdcc|Xcdccc|Xdcccc|Xcdcdc|Xdcdcc|Xdccdc)

#define Xdxxxx (Xddddd|Xdcabc|Xdcccc|Xdccdc|Xdcdcc)
#define Xxdxxx (Xddddd|Xcdcab|Xcdccc|Xcdcdc|Xcdccd)
#define Xxxdxx (Xddddd|Xbcdca|Xccdcc|Xdcdcc|Xccdcd)
#define Xxxxdx (Xddddd|Xabcdc|Xcccdc|Xcdcdc|Xdccdc)
#define Xxxxxd (Xddddd|Xcabcd|Xccccd|Xccdcd|Xcdccd)

//#define JunctionIs6way  '6'
//#define JunctionIsZ6    '*'

#define Xcccccc 0x0001
#define Xcdcdcd 0x0002
#define Xdcdcdc 0x0004
#define Xdddddd 0x0008
#define Xcccccd 0x0010
#define Xccccdc 0x0020
#define Xcccdcc 0x0040
#define Xccdccc 0x0080
#define Xcdcccc 0x0100
#define Xdccccc 0x0200

#define X6all (Xcccccc|Xcdcdcd|Xdcdcdc|Xdddddd|Xcccccd|Xccccdc|Xcccdcc|Xccdccc|Xcdcccc|Xdccccc)

//#define JunctionIs7way  '7'

//#define JunctionIs8way  '8'

//#define JunctionIsHidden 'H'

/* ================================================================================ */

#define VFinalIs3ccc      0x00000001
#define VFinalIs3ccd      0x00000002
#define VFinalIs3cdd      0x00000004
#define VFinalIs3ddd      0x00000008
#define VFinalIsXcccc     0x00000010
#define VFinalIsXcccd     0x00000020
#define VFinalIsXcdcd     0x00000040
#define VFinalIsXcddd     0x00000080
#define VFinalIsXdddd     0x00000100
#define VFinalIsMccdc     0x00000200
#define VFinalIsMcddc     0x00000400
#define VFinalIsMdccd     0x00000800
#define VFinalIsMcdcd     0x00001000
#define VFinalIsMdcdd     0x00002000
#define VFinalIsKcccd     0x00004000
#define VFinalIsKcdcd     0x00008000
#define VFinalIsKddcd     0x00010000
#define VFinalIs5ccccc    0x00020000
#define VFinalIs5ddddd    0x00040000
#define VFinalIs6cccccc   0x00080000
#define VFinalIs6cdcdcd   0x00100000
#define VFinalIs6dddddd   0x00200000
#define VFinalIs7way      0x00400000
#define VFinalIs8way      0x00800000
#define VFinalIsUnknown   0x003fffff
#define VFinalIsTrihedral (VFinalIs3ccc|VFinalIs3ccd|VFinalIs3cdd|VFinalIs3ddd)
#define VFinalIsNTAA      (VFinalIsTrihedral|VFinalIsKcdcd|VFinalIs6cdcdcd)
#define VFinalIs4exactly  (VFinalIsXcccc|VFinalIsXcccd|VFinalIsXcdcd|VFinalIsXcddd|VFinalIsXdddd|VFinalIsMccdc|VFinalIsMcddc|VFinalIsMdccd|VFinalIsMcdcd|VFinalIsMdcdd|VFinalIsKcccd|VFinalIsKcdcd|VFinalIsKddcd)
#define VFinalIs4hedral   (VFinalIsTrihedral|VFinalIs4exactly)
#define VFinalIs5exactly  (VFinalIs5ccccc|VFinalIs5ddddd)
#define VFinalIs5hedral   (VFinalIs4hedral|VFinalIs5exactly)
#define VFinalIs6exactly  (VFinalIs6cccccc|VFinalIs6cdcdcd|VFinalIs6dddddd)
#define VFinalIs6hedral   (VFinalIs5hedral|VFinalIs6exactly)
#define VFinalIsConvex    (VFinalIs3ccc|VFinalIsXcccc|VFinalIs5ccccc|VFinalIs6cccccc)
#define VFinalIsConcave   (VFinalIs3ddd|VFinalIsXdddd|VFinalIs5ddddd|VFinalIs6dddddd)

#define VFinalIsXany      (VFinalIsXcccc|VFinalIsXcccd|VFinalIsXcdcd|VFinalIsXcddd|VFinalIsXdddd)
#define VFinalIsMany      (VFinalIsMccdc|VFinalIsMcddc|VFinalIsMdccd|VFinalIsMcdcd|VFinalIsMdcdd)
#define VFinalIsKany      (VFinalIsKcccd|VFinalIsKcdcd|VFinalIsKddcd)
#define VFinalIs5any      (VFinalIs5ccccc|VFinalIs5ddddd)
#define VFinalIs6any      (VFinalIs6cccccc|VFinalIs6cdcdcd|VFinalIs6dddddd)
#define VFinalIsZany      (VFinalIsKcdcd|VFinalIs6cdcdcd)

#define VFinalIs0C        (VFinalIs3ddd|VFinalIsXdddd|VFinalIs5ddddd|VFinalIs6dddddd)
#define VFinalIs1C        (VFinalIs3cdd|VFinalIsXcddd|VFinalIsMdcdd|VFinalIsKddcd)
#define VFinalIs2C        (VFinalIs3ccd|VFinalIsXcdcd|VFinalIsMcddc|VFinalIsMdccd|VFinalIsMcdcd|VFinalIsKcdcd)
#define VFinalIs3C        (VFinalIs3ccc|VFinalIsXcccd|VFinalIsMccdc|VFinalIsKcccd|VFinalIs6cdcdcd)
#define VFinalIs4C        (VFinalIsXcccc)
#define VFinalIs5C        (VFinalIs5ccccc)
#define VFinalIs6C        (VFinalIs6cccccc)
#define VFinalIs0D        (VFinalIs3ccc|VFinalIsXcccc|VFinalIs5ccccc|VFinalIs6cccccc)
#define VFinalIs1D        (VFinalIs3ccd|VFinalIsXcccd|VFinalIsMccdc|VFinalIsKcccd)
#define VFinalIs2D        (VFinalIs3cdd|VFinalIsXcdcd|VFinalIsMcddc|VFinalIsMdccd|VFinalIsMcdcd|VFinalIsKcdcd)
#define VFinalIs3D        (VFinalIs3ddd|VFinalIsXcddd|VFinalIsMdcdd|VFinalIsKddcd|VFinalIs6cdcdcd)
#define VFinalIs4D        (VFinalIsXdddd)
#define VFinalIs5D        (VFinalIs5ddddd)
#define VFinalIs6D        (VFinalIs6dddddd)

/* ================================================================================ */

//#define LoopIsTriangle             '3'
//#define LoopIsSimpleQuadrilateral  '4'
//#define LoopIsVQuadrilateral       'V'
//#define LoopIsSimplePentagon       '5'
//#define LoopIsMPentagon            'M'
//#define LoopIsSimpleHexagon        '6'
//#define LoopIsLHexagon             'L'
//#define LoopIsSimpleOctagon        '8'
//#define LoopIsUOctagon             'U'
//#define LoopIsTOctagon             'T'
//#define LoopIsSimpleDecagon        'A'
//#define LoopIsSimpleDodecagon      'C'
//#define LoopIsSomethingElse        (0)

/////* ================================================================================ */
////
////#define DEPTH_AUTO_SELECT         'A'
////#define DEPTH_PERKINS             'P'
////#define DEPTH_JLP                 'J'
////#define DEPTH_ALL_FACES           'F'
////#define DEPTH_QUADRILATERAL_FACES 'Q'
////#define DEPTH_NO_FACES            'N'
////
/////* ================================================================================ */
////
////#define CATEGORY_NORMALON       0x0001
////#define CATEGORY_SEMI_ALIGNED   0x0002
////#define CATEGORY_SEMI_MIRROR    0x0004
////#define CATEGORY_EXTRUSION      0x0008
////#define CATEGORY_FRUSTUM        0x0010
////#define CATEGORY_REGULAR        0x0020
////#define CATEGORY_PRISM          (CATEGORY_EXTRUSION|CATEGORY_REGULAR)
////
////#define GRAPHLOOP_OUTER         'O'
////#define GRAPHLOOP_POCKET        'P'
////#define GRAPHLOOP_BOSS          'B'
////#define GRAPHLOOP_USED          'U'
////
/////* ================================================================================ */
////
/////* Topology Hypotheses */
////
////#ifdef POLYHEDRAL_VERSION
#define MAX_ITERATIONS 64
////#endif
////#ifdef CURVED_VERSION
////#define MAX_ITERATIONS 32
////#endif
////
////#define MECHANISM_GENERAL_CASE 0
////#define MECHANISM_AXIS_ALIGNED 1
////#define MECHANISM_ROTATIONS    2
////
////#define OPERATION_MIRROR 0x0002
////#define OPERATION_C2     0x0004
////#define OPERATION_C3     0x0008
////#define OPERATION_C4     0x0010
////#define OPERATION_C5     0x0020
////#define OPERATION_C6     0x0040
////#define OPERATION_L3     0x0200
////#define OPERATION_T      0x0400
////#define OPERATION_ORTHO  0x0800
////#define OPERATION_232    0x1000
////#define OPERATION_2332   0x2000
////
////#define INVERT TRUE
////#define AS_IS  FALSE
////
////#define TAG_EXTRAPOLATED_LINE     (1)
////#define TAG_IS_COMPLETE           (2)
////#define TAG_CONFIG232             (7)
////#define TAG_CONFIG2332            (8)
////#define TAG_EDGE_HYPOTHESIS      (10)
////#define TAG_EDGET_HYPOTHESIS     (11)
////#define TAG_EDGET2_HYPOTHESIS    (12)
////#define TAG_VER2_HYPOTHESIS      (13)
////#define TAG_VER2T_HYPOTHESIS     (14)
////#define TAG_VER2T2_HYPOTHESIS    (15)
////#define TAG_VER3_HYPOTHESIS      (16)
////#define TAG_VER3T_HYPOTHESIS     (17)
////#define TAG_QFACE_HYPOTHESIS     (18)
////#define TAG_LOOP_HYPOTHESIS      (19)
////#define TAG_MIRROR_HYPOTHESIS    (20)
////#define TAG_UNDERSLOT_HYPOTHESIS (21)
////#define TAG_VALLEY_HYPOTHESIS    (22)
////#define TAG_HOLE_HYPOTHESIS      (23)
////#define TAG_PIT_HYPOTHESIS       (24)
////#define TAG_MATCHFACE_HYPOTHESIS (25)
////
////
/////* ================================================================================ */
////
////#define IDENTIFY_MIRROR   0x0001
////#define IDENTIFY_ROTATION 0x0002
////#define IDENTIFY_PARALLEL 0x0004
////#define IDENTIFY_PERP2    0x0008
////#define IDENTIFY_PERP3    0x0010
////#define IDENTIFY_ALL      0x001F
////
////#define CONSTRAINT_IS_MIRROR        'M'
////#define CONSTRAINT_IS_PARALLEL      'H'
////#define CONSTRAINT_IS_PERPEND2      'N'
////#define CONSTRAINT_IS_PERPEND3      'P'
////#define CONSTRAINT_IS_FACEANGLE     'A'
////#define CONSTRAINT_IS_ROTATION      'R'
////#define CONSTRAINT_IS_TETRAHEDRAL   'T'
////
////#define USE_REJECTED    0
////#define USE_PROVISIONAL 1
////#define USE_ACCEPTED    2
////
////#define CONSTRAINT_IS_COPLANARITY   'C'
////#define CONSTRAINT_IS_NONTRIHEDRAL  '4'
////#define CONSTRAINT_IS_EQUILENGTH    'E'
////#define CONSTRAINT_IS_LENGTHRATIO   'R'

/* ================================================================================ */

#define EQUALISH(A,B,T)  (fabs((A)-(B)) <= (T))

/* ================================================================================ */
/* Vertex flags for hypothetical faces */

#define FLAG_LEFTTURN  0x0001
#define FLAG_RIGHTTURN 0x0002
#define FLAG_DONTKNOW  0x0000
#define FLAG_TJUNCTION 0x0004
#define FLAG_NEWVERTEX 0x0008
#define FLAG_MUSTBEL   0x0010

/* ================================================================================ */
/* 'Best is' indications */

#define BEST_IS_NONE 0
#define BEST_IS_C2   2
#define BEST_IS_C3   3
#define BEST_IS_C4   4
#define BEST_IS_C5   5
#define BEST_IS_C6   6

#define DEFAULT_MERIT_RATIO_FUSSINESS    (0.516)
#define MULTIPLIER_RATIO_FUSSINESS       (2.0)

#define DEFAULT_MERIT_LAMBAN_OCCLUDING   (0.200)
#define MULTIPLIER_LAMBAN_OCCLUDING      (1000.0)

#define DEFAULT_MERIT_LAMBAN_CONFIDENCE  (0.600)
#define MULTIPLIER_LAMBAN_CONFIDENCE     (1.0)

#define NO_VERTEX (-1)
#define NO_EDGE   (-1)

#define Kabcdmerit (0)
#define Kabdcmerit (1)
#define Kcccdmerit (2)
#define Kccdcmerit (3)
#define Kcdcdmerit (4)
#define Kdcdcmerit (5)
#define Kddcdmerit (6)
#define Kdddcmerit (7)
#define KMERITS    (8)

#define Lbamerit (0)
#define Labmerit (1)
#define Lacmerit (2)
#define Lcbmerit (3)
#define Lbdmerit (4)
#define Ldamerit (5)
#define Laamerit (6)
#define Lbbmerit (7)
#define LMERITS  (8)

#define Mbccamerit ( 0)
#define Mbcdamerit ( 1)
#define Mbdcamerit ( 2)
#define Mccdcmerit ( 3)
#define Mcdccmerit ( 4)
#define Mcdcdmerit ( 5)
#define Mcddcmerit ( 6)
#define Mdccdmerit ( 7)
#define Mdcdcmerit ( 8)
#define Mdcddmerit ( 9)
#define Mddcdmerit (10)
#define MMERITS    (11)

#define Tbaamerit ( 0)
#define Tbabmerit ( 1)
#define Tbacmerit ( 2)
#define Tbadmerit ( 3)
#define Tbdamerit ( 4)
#define Tbdcmerit ( 5)
#define Tdabmerit ( 6)
#define Tdacmerit ( 7)
#define Tabamerit ( 8)
#define Tabbmerit ( 9)
#define Tbcamerit (10)
#define Tbccmerit (11)
#define Tcabmerit (12)
#define Tcacmerit (13)
#define Tccamerit (14)
#define Tccbmerit (15)
#define Tcdamerit (16)
#define Tdcbmerit (17)
#define Tddamerit (18)
#define Tddbmerit (19)
#define TMERITS   (20)

#define Xcccccmerit (0)
#define Xdddddmerit (1)
#define X5MERITS    (2)

#define Xccccccmerit (0)
#define Xcdcdcdmerit (1)
#define Xdcdcdcmerit (2)
#define Xddddddmerit (3)
#define X6MERITS     (4)

#define OPTION_USE_NTAA_LABELLING       ("oa")
#define OPTION_LWY_TRIHEDRAL_LABELLING  ("ob")
#define OPTION_COUNT_LABELLINGS         ("oc")
#define OPTION_USE_DEDUCTIONS_IN_LAMBAN ("od")
#define OPTION_USE_FIXED_LAMBAN_AXES    ("of")
#define OPTION_USE_LAMBAN_LABELLING     ("og")
#define OPTION_USE_MAX_IN_RELAXATION    ("oh")
#define OPTION_USE_JUNCTION_STATISTICS  ("oj")
#define OPTION_LIST_LABELLINGS          ("ol")
#define OPTION_USE_MERGED_LABELLING     ("om")
#define OPTION_USE_SEMIMERGED_LABELLING ("on")
#define OPTION_USE_RELAXATION_LABELLING ("or")
#define OPTION_USE_SHAPEPAIR_STATISTICS ("os")
#define OPTION_USE_TRIHEDRAL_LABELLING  ("ot")
#define OPTION_LABEL_AUTOUPDATE         ("ou")
#define OPTION_USE_VARIABLE_LAMBAN_AXES ("ov")
#define OPTION_LABEL_EXIT_ON_FAILURE    ("ox")

#define FEATURE_WY        (0x0001)
#define FEATURE_UNDERSLOT (0x0002)
#define FEATURE_VALLEY    (0x0003)

#define THRESHOLD (0.5)

#define ONE_O_CLOCK     (30)
#define TWO_O_CLOCK     (60)
#define THREE_O_CLOCK   (90)
#define FOUR_O_CLOCK   (120)
#define FIVE_O_CLOCK   (150)
#define SIX_O_CLOCK    (180)
#define SEVEN_O_CLOCK  (210)
#define EIGHT_O_CLOCK  (240)
#define NINE_O_CLOCK   (270)
#define TEN_O_CLOCK    (300)
#define ELEVEN_O_CLOCK (330)
#define TWELVE_O_CLOCK   (0)

#define MAX_COLLINEAR (MAX_EDGES*3)

#endif
