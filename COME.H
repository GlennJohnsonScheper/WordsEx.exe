// This is file: COme.h
// Copyright ( C ) 2006, Glenn Scheper

class CAll; // Globals
class CAsb; // ASCII string buffer
class CBud; // Base of sorted fruit
class CFio; // Local file in / out
class CHtm; // Parses html into text
class CIni; // Initiate operations
class CInt; // Integer list
class CIdx; // Text Indexing offsets
class CFwd; // Find Words in texts
class CTop; // Holds top-level views
class CRev; // Holds Book of Revelation
class CUse; // Holds Help Usage text
class CPag; // CodePage conversions
class CSee; // Displays, navigation
class CTxt; // Parses text into sense
class CSol; // Holds one sorted list
class CLin; // Link cross-ref info
class CWsb; // Wide string buffer
class CWww; // Web and cache access
class CPor; // Porter stemming
class CUrl; // Search Engine Urls
class COme; // Omega painting


class COneFetch; // Controls one Add One Web Page or Add Links thread
class COneQuery; // Controls one Add Internet Search thread
class COneKwic; // Controls one Add Word thread
class COneCache; // Controls one Add All Files in Cache thread
class COnePhrases; // Controls one Add Ranked Phrases thread
class COneFolder; // Controls one Add Directory thread

class COnePaper; // For each HTML or TEXT source page
class COneForm; // For each HTML FORM encountered
class COneSurl; // For each GET search engine listed

// Worst case is ( x^2 + y^2 + z^2 ), which 3-sum must fit in 31 bits:
// More than that, I need the sum of 4 products to fit in an integer.
// So keep things scaled such that max dimension anywhere is less than
// the sqrt of 1/16 of max signed value, or 31 bits - 4 bits = 27 bits.
// No, not 1/16, there are only 4 multiplies in matrix multiplication.
// What then?
// 0x8000,0000 is 1 more than the max representable signed;
// at 1/16:
// 0x0800,0000 is 1/16 of that; = 134,217,728
// 0x0000,2d41 is sqrt of that; = 11,585
// Use 1/4:
// 0x2000,0000 is 1/4 of that; = 536,870,912
// 0x0000,5A82 is sqrt of that; = 23,170
#define MAX_DIMENSION 23170

// The max dimension I have to deal with is the viewer's eye distance,
// which is about 3* the screeen size. Use 2* to emphasis perspective.
// Constrain ( motion+size ) to 1.5 screen diameters; So max is 2 + 1.5.

// I do not recognize why original 10.c had this unity scale factor.
// Aha! I used to have scaled fixed point, and this was divided out
// after every multiplication to maintain the same level of scaling.
// I am going back to scaled fixed point math. Make this a variable.
// No, it is fixed by the worst case math; Compute pel size instead.
#define ctmscale MAX_DIMENSION

// Suppose, as an excess, I use a vector of [N_ZP_LEVELS] to lookup:
#define N_ZP_LEVELS 1024

// And again, I lookup some N illumination levels into RGB colors.
#define N_MAP_LEVELS 256

// The number of derivatives to smooth out random motion work.
#define ndv 7

class COme
{

public:

    COme( );
    ~COme( );
    void DrawOmega( );

private:

    int IntegerSqrt( int num );

    // Fixed, unscaled x,y sizes of original client area and bitmap.
    int pelsacross;
    int pelsdown;

    // A place to save colors before transfer to bitap;
    unsigned char * pColorArray;

    // Compute fixed point scaling factors slightly smaller than pels.
    // My XP's max resolution setting says it can be 1600 pels across.
    int pel;    // scaling factor of fixed math counts per pel
    int roundpel;   // one-half that for rounding
    int screendiameter; // size of screen in same fixed point math
    int ScreenDiagonalPels;

    int FinalSmallRadius;
    int rs; // RS = Radius, small
    int rl; // RL = Radius, large
    int hl; // HL = Half length of cylinder.
    int rq; // RQ = Radius to quit drawing, just larger than small radius.
    int rqsq; // RQSQ = RQ squared

    // Viewer's eye is at ( 0,0,-screenz ). Screen center is at ( 0,0,0 ).
    int screenz; // scaled, and positive.

    // Precomputed colors based on zp ( Z in Perspective space ).

    unsigned char ZpLevel[ N_ZP_LEVELS ];
    int FurthestZp;
    int NearestZp;
    int ZpLookupDivisor;

    int ZpColor[ N_MAP_LEVELS ];

    // Need these to center ( fixed scale, 2D, ( x=0, y=0 ) ).
    int screenhalfx;
    int screenhalfy;

    int ctm[ 16 ]; // Current transform matrix, 3 dimensional.
    int cat[ 16 ]; // Concatenation matrix, 3 dimensional.

    void InitializeCurrentTransformMatrix( );
    void InitializeConcatenationMatrix( );
    void Concatenate( );

    // Sine and Cosine tables for rotations, scaled by ctmscale and by .1 degree
    int ctmsin[ 3600 ];
    int ctmcos[ 3600 ];
    void InitializeSindCosdTables( );
    int Sind( int tenthdegree );
    int Cosd( int tenthdegree );

    // random motion generator

    int TranslationChangeStimulusAmount;
    int TranslationChangeBasinLimit;

    int RotationChangeStimulusAmount;
    int RotationChangeBasinLimit;

    int ChangeVariable( int * var, int stimulus, int BasinLimit  );

    // Object space random translations ( offsets )
    int ox[ndv];
    int oy[ndv];
    int oz[ndv];

    // Initial random rotations
    int r1x;
    int r1y;
    int r1z;

    // Object space random rotations
    int rx[ndv];
    int ry[ndv];
    int rz[ndv];

    // Viewing space random translations
    int tx[ndv];
    int ty[ndv];
    int tz[ndv];

    void RandomMovement( );
    void DrawTorus( int nPoints );
    void DrawCylinder( int nPoints );
    void BitmapTransfer( );
    void PaintXYZ( int xo, int yo, int zo );

    unsigned int ColorMapping[ 256 ];

    #if DO_DEBUG_OMEGA1
        // Keep a closest and furthest to output for study
        int study_ztmin;
        int study_ztminDistanceFromEyeZp;
        int study_ztminIndexForZp;
        int study_ztminLevel;
        int study_ztmax;
        int study_ztmaxDistanceFromEyeZp;
        int study_ztmaxIndexForZp;
        int study_ztmaxLevel;
    #endif
};




