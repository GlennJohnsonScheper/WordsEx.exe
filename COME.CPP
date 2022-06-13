// This is file: COme.cpp
// Copyright ( C ) 2006, Glenn Scheper

#include "stdafx.h"
#include "CAll.h" // Globals

/*
 * This is a very simple adaptation of my ancient program:
 *    10.C
 *    Video ( 1 ) cylinder + ( 0 ) torus screen display
 *    by Glenn Scheper
 */

    // Wild and crazy idea: Paint my growing Omega,
    // After erase, after splash screen text drawn.
    // Bitmap in DC g_hdcCompat is about to bitblt.
    // Possible input parameter is OmegaElapsedms.
    // Yes, consult that lest SS timer speeds up.
    // Uses initial g_ClientWidth, g_ClientHeight.
    // Note: Resizing can make g_ClientHeight zero!


// Note: I need FINAL_OMEGA_MS * FINAL_N_POINTS to fit in an integer.

// gets two multipliers ( 31 plus 10 ) afterwards:
#ifdef _WIN32_WCE
    #define FINAL_N_POINTS 200
#else // not _WIN32_WCE
    #define FINAL_N_POINTS 1000
#endif // _WIN32_WCE

// Call.h contains these:
// #define GROW_OMEGA_MS                   ...maybe... 200
// #define FINAL_OMEGA_MS                  ...maybe... ( 8 * 60 * 1000 )

// sum to 100% or less
#define CHANGE_FEEDFORWARD 30
#define CHANGE_FEEDBACK 70

#define BASIN_STEEPNESS 10


// During the time the splashscreen text is on display,
// this class will be instantiated in g_pOmega, and a
// GROW_OMEGA_MS timer will invalidate, WmPaint calls.


// Omega constructor
// pelsacross: 830
// pelsdown: 570
// ScreenDiagonalPels: 1007
// MaxCountsPerPel: 3
// roundpel: 1
// pel: 2
// screendiameter: 2014
// FinalSmallRadius: 235
// screenz: 4028
// FurthestZp: 16112
// ZpLookupDivisor: 15
// NearestZp: 1007
// i: 0
// ZpLevel[ i ]: 255
// ...
// i: 67
// ZpLevel[ i ]: 255
// i: 68
// ZpLevel[ i ]: 249
// ...
// ZpLevel[ i ]: 16
// i: 268
// ...
// i: 1023
// ZpLevel[ i ]: 1
//



COme::COme( )
{
    // Compute fixed point scaling factors slightly smaller than pels.
    // My XP's max resolution setting says it can be 1600 pels across.

    #if DO_DEBUG_OMEGA1
        Spew( L"Omega constructor" );
    #endif

    // These are fixed on original screen size,
    // because I will malloc a byte array once.
    pelsacross = g_ClientWidth;
    if( pelsacross < 128 )
        pelsacross = 128; // CYA

    pelsdown = g_ClientHeight;
    if( pelsdown < 128 )
        pelsdown = 128; // CYA

    #if DO_DEBUG_OMEGA1
        SpewValue( L"pelsacross", pelsacross );
        SpewValue( L"pelsdown", pelsdown );
    #endif

    // A place to hold my bitmap colors before transfer.
    pColorArray = new unsigned char[ pelsacross * pelsdown ];
    memset( pColorArray, 0, pelsacross * pelsdown );

    ScreenDiagonalPels = ( int ) IntegerSqrt( pelsacross * pelsacross + pelsdown * pelsdown );
    if( ScreenDiagonalPels < 128 )
        ScreenDiagonalPels = 128; // CYA! ( 0,0 is possible, seen, crashes )

    #if DO_DEBUG_OMEGA1
        SpewValue( L"ScreenDiagonalPels", ScreenDiagonalPels );
    #endif

    // Adjust scale factor to keep all values under MAX_DIMENSION 11585.
    // The max dimension I have to deal with is the viewer's eye distance,
    // which is about 3* the screeen size. Use 2* to emphasis perspective.
    // So if I constrain ( motion+size ) to a box of 1.5 x screen diameter,
    // then the max dimension is at a Z furthest from viewer, or 2 + 1.5,
    // so the maximum, or unity value of all scaled math should be such
    // that max_dim = 3.5 screen diameters, so set the scale factor used
    // to renormalize after every multiplication, in variable ctmscale,
    // well, that should be max_dim; scale one pel accordingly.

    int MaxDimension = ScreenDiagonalPels * 7 / 2;
    int MaxCountsPerPel = MAX_DIMENSION / MaxDimension;

    #if DO_DEBUG_OMEGA1
        SpewValue( L"MaxCountsPerPel", MaxCountsPerPel );
    #endif

    roundpel = MaxCountsPerPel / 2;
    pel = roundpel * 2;
    screendiameter = ScreenDiagonalPels * pel;

    TranslationChangeStimulusAmount = screendiameter * 2;
    TranslationChangeBasinLimit = screendiameter * 3;

    RotationChangeStimulusAmount = 3600;
    RotationChangeBasinLimit = 7200;

    #if DO_DEBUG_OMEGA1
        SpewValue( L"roundpel", roundpel );
        SpewValue( L"pel", pel );
        SpewValue( L"screendiameter", screendiameter );
    #endif

    // Figure the small radius when it has grown to max size.
    // Based on the torus major diameter ( dl ) as 1/2 screen.
    // See computation of torus and cylinder dimensions below.
    // Divide by 4, and by ( pi-1 ).
    // pi = 3.14159265358979323846264338327950288419716939937510
    // so divisor, ( pi-1 ) * 4 = 8.56637061435917295384
    FinalSmallRadius = screendiameter * 10000 / 85664;

    #if DO_DEBUG_OMEGA1
        SpewValue( L"FinalSmallRadius", ( int ) FinalSmallRadius );
    #endif

    // Viewer's eye is at ( 0,0,-screenz ). Screen center is at ( 0,0,0 ).
    // This affects the amount of 3D to 2D perspective transformation.
    // Physically, I figure viewer is at about 3 screen diameters.
    // However, use 2 screen diameters to emphasize perspective.

    screenz = screendiameter * 2; // positive number, Z coordinate of screen.

    #if DO_DEBUG_OMEGA1
        SpewValue( L"screenz", screenz );
    #endif

    // Based on a plan to keep max 3-d location ( motion+size )
    // under 1.5 screen diameters, and viewer's eye at 2 diam,
    // then the 3-d Z value might range from 0.5 to 3.5 diams.

    // Make an array sufficient to look up every illumination
    // level in that range. I would like to do brightness as a
    // power function ( 1 / square of distance ), which is about
    // 1/0.7 when close, to 1/13 when far. That must be turned
    // into N ( about 255? ) integer illumination levels, which
    // can index N RGB lookup values, not necessarily linear.
    // So I need a divisor of the Zp dimension to turn 3.5 x
    // screen diameter into that max lookup array index:

    // Suppose, as an excess, I use a vector of [N_ZP_LEVELS],
    // to span 3-d Z space from easy 0 to over 3.5 diameters.
    // Because of quantization of a scale factor, make it 8.
    // Then I shouldn't have to test and clamp every lookup.
    // That was no good. divisor was 45, so it took Z movement
    // of 45 counts to move one step in 1024 level lookup list.
    // Make that list run from MinZ to MaxZ, and clamp outside.

    // So again, make live range of Zo be +/- 1.5 x screendiameter.
    // with screenz 2.0 x screendiameter, that's Zp = 0.5 to 3.5
    // Zo = Z in 3-d object space;
    // Zp = Z in perspective space, eye=( 0,0,0 ), screen=( x,y,screenz ).

    // At 0.5 to 3.5, all the action near screen barely varies.
    // Blow it up! ( Remember the 2.0 in screenz? ) So 1.0 to 3.0.
    // Blow it up more! ( Remember the 2.0? ) So 1.5 to 2.5.
    FurthestZp = screendiameter * 5 / 2;
    NearestZp = screendiameter * 3 / 2;

    ZpLookupDivisor = ( FurthestZp - NearestZp ) / N_ZP_LEVELS;

    #if DO_DEBUG_OMEGA1
        SpewValue( L"FurthestZp", FurthestZp );
            SpewValue( L"NearestZp", NearestZp );
        SpewValue( L"ZpLookupDivisor", ZpLookupDivisor );
    #endif

    // Now I need to populate final RGB color? No, level
    // of illumination, in array ZpLevel[ N_ZP_LEVELS ];
    // I need levels, to do an if( brighter )-keep test.
    // I may keep more than 256 levels to map into RGB.
    // No, I need level to fit in a byte, in big array.
    // The value stored in each ZpLevel[ N_ZP_LEVELS ]
    // must be valid index into ZpColor[ N_MAP_LEVELS ].

    // The highest level of 1/sqrt( 0.5 ), when Zp is .5,
    // must index N_MAP_LEVELS, and the rest fall off
    // according to 1/square of distance power function.

    // What am I trying to think here? The formula
    // to fill in ZpLevel[ N_ZP_LEVELS ]. Everything
    // under some NearestZp = screendiameter * 0.5
    // indexes L0 = N_MAP_LEVELS-1, and past that N0,
    // each L = f( N ) = ( L0 * ( N0/N )^2 ). Ahh, Finally!

    {
        // fill up ZpLevel[ N_ZP_LEVELS ]
        // The level lookup will index using Zp, not Zo.

        // For reference, from above:
        // ZpLookupDivisor = ( FurthestZp - NearestZp ) / N_ZP_LEVELS;

        int i = 0; // This i is indexing in [0 to N_ZP_LEVELS - 1].
        for( ;; )
        {
            int ZpForThisi = i * ZpLookupDivisor + NearestZp;
            // So, when i == N_ZP_LEVELS, ZpForThisi == FurthestZp.

            // Now do the power function.
            // Work scaled up slightly, but well within 31 bits:

            // This is default if skip the IF, very close to eye:
            int LevelAtThisZp = N_MAP_LEVELS - 1; // brightest value

            if( ZpForThisi > NearestZp ) // prevent divide by 0 when close
            {
                // Do this math scaled up 100, just for this line:
                // The same mul-div twice does the power function.
                // The brightest level is reached at the NearestZp.
                LevelAtThisZp = ( ( N_MAP_LEVELS - 1 ) * 100
                * NearestZp / ZpForThisi
                * NearestZp / ZpForThisi + 50 ) / 100; // + 50 to round.

                if( LevelAtThisZp > N_MAP_LEVELS - 1 )
                    LevelAtThisZp = N_MAP_LEVELS - 1; // CYA
                if( LevelAtThisZp < 1 )
                    LevelAtThisZp = 1; // I need to avoid zero = no drawing.
            }

            ZpLevel[ i ] = LevelAtThisZp;

            #if DO_DEBUG_OMEGA1
                SpewValue( L"i", i );
                SpewValue( L"ZpForThisi", ZpForThisi );
                SpewValue( L"LevelAtThisZp", LevelAtThisZp );
            #endif

            if( ++ i == N_ZP_LEVELS )
                break;
        }
    }

    // Now I need a table to map those brightness levels into some
    // range of golden yellow-to-white colors such as I remember.
    // Use Paint to chose the limits of colors, then interpolate.

    // So I click near the saturated top of yellow band
    // And then I click in the right-hand linear chooser
    // and I see from the top to mid-point, RG stays up
    // while B falls off, perhaps linearly; from the
    // mid-point to the bottom, B is near zero, and RG
    // fall off perhaps linearly.
    //
    // rgb bright = 253 245 196, or 254 251 224
    // rgb dim = 243 214 7, or 233 205 7
    //
    // If I click lower in left-hand part, then the B
    // component remains higher as I move to midpoint:
    // rgb dim = 226 210 18
    //
    // These seem good. Keep slightly more R than G.

    {
        // fill up ZpColor[ N_MAP_LEVELS ];

        #define R_BRIGHT 255
        #define G_BRIGHT 245
        #define B_BRIGHT 125

        #define R_DIM 125
        #define G_DIM 110
        #define B_DIM 60

        // reach dim at i = 0
        // reach bright at i = 255 == N_MAP_LEVELS - 1
        int i = 0;
        for( ;; )
        {
            int brightpart = i; // bright part is same
            int dimpart = 255 - i; // dim part is opposite
            int R = ( brightpart * R_BRIGHT + dimpart * R_DIM + 127 ) / 255;
            int G = ( brightpart * G_BRIGHT + dimpart * G_DIM + 127 ) / 255;
            int B = ( brightpart * B_BRIGHT + dimpart * B_DIM + 127 ) / 255;
            if( R > 255 )
                R = 255;
            if( G > 255 )
                G = 255;
            if( B > 255 )
                B = 255;

            ZpColor[ i ] = ( B<<16 ) | ( G<<8 ) | ( R );

            if( ++i == N_MAP_LEVELS )
                break;
        }
    }

    // revised to here...


    // Need these to center ( fixed scale, 2D, ( x=0, y=0 ) ).
    screenhalfx = pelsacross * roundpel;
    screenhalfy = pelsdown * roundpel;

    #if DO_DEBUG_OMEGA1
        SpewValue( L"screenhalfx", screenhalfx );
        SpewValue( L"screenhalfy", screenhalfy );
    #endif


    InitializeSindCosdTables( );

    r1x = rand( ) % 3600;
    r1y = rand( ) % 3600;
    r1z = rand( ) % 3600;

    // Init the rotation and translation state variables
    {
        int i = 0;
        for( ;; )
        {
            rx[i] = ry[i] = rz[i] = 0;
            ox[i] = oy[i] = oz[i] = 0;
            tx[i] = ty[i] = tz[i] = 0;
            if( ++i == ndv )
                break;
        }
    }

    // Initialize a 256 vector to map levels into various "gold" colors.
    {
        int i = 0;
        for( ;; )
        {
            // I don't trust RGB macro. I should do my own math.
            // was... unsigned int color = RGB( level, level, 60 );
            // layout: 0x00bbggrr - as an integer, not as in memory.

            // Get more brightness, say in a range of R=G= 170 to 255.
            // No, I need more variation from front to back, use 128 up.
            // I need still more variation from front to back, to prove it.
            unsigned char RedGrn = ( 20 * i + 10 * 255 ) / 30;

            // I need to add some blue, so bright yellows go to white.
            // let blue range ( with level 0-256 ) from about 50 to 200.
            // ( that is, when r,g parts were about 180 up, even 128 up. )
            unsigned int Blue = i / 2 + 20;

            ColorMapping[ i ] = ( Blue << 16 ) |  ( RedGrn << 8 ) | ( RedGrn << 0 );

            if( ++i == 256 )
                break;
        }
    }
}

COme::~COme( )
{
    #if DO_DEBUG_OMEGA1
        Spew( L"Omega destructor" );
    #endif
    delete pColorArray;
}

void COme::DrawOmega( )
{
    // Here is a trivial proof of paint connections.
    // This is not relevant to the omega drawing.
    // if( g_ClientWidth == 0
    // || g_ClientHeight == 0 ) // CYA
    //     return;
    // int i = 0;
    // int n = OmegaElapsedms / 10;
    // for( ;; )
    // {
    //     if( ++i > n )
    //         break;
    //     int color = rand( ) << 4;
    //     int x = rand( ) % g_ClientWidth;
    //     int y = rand( ) % g_ClientHeight;
    //     SetPixel( g_hdcCompat, x, y, color );
    // }

    #if DO_DEBUG_OMEGA1
        Spew( L"Draw Omega" );
    #endif

    // Set parameters according to OmegaElapsedms.
    // Clamp to near screendiameter at 60 seconds.

    // If the torus major diameter ( dl ) is 1/2 screen,
    // and the cylinder length ( cl ) equals major+minor diam,
    // and cylinder radius = torus minor radius ( ds ),
    // AND I REQUIRE that cylinder length = circumference,
    // then, at final 60,000 ms, I want to reach:
    // dl = screendiameter / 2,
    // cl = dl + ds;
    // cl = ds * pi.
    // So, dl + ds == ds * pi,
    // so dl = ds * ( pi - 1 );
    // maximum ds = ( screendiameter / 2 ) / ( pi - 1 )
    // finally, ds = ( screendiameter / 2 ) / ( pi - 1 ) * ms/60000
    // FinalSmallRadius = ( screendiameter / 4 ) / 2.141592653;
    // I have pre-computed FinalSmallRadius

    // Set Radius small, Radius large, according to OmegaElapsedms.

    int factor = OmegaElapsedms;
    if( factor > FINAL_OMEGA_MS )
        factor = FINAL_OMEGA_MS;

    // RS = Radius, small
    rs = FinalSmallRadius * factor / FINAL_OMEGA_MS;

    // RL = Radius, large
    rl = rs * 21416 / 10000; // ratio is ( pi-1 )

    // HL = Half length of cylinder.
    hl = rs + rl;

    // RQ = Radius to quit drawing, just larger than small radius.
    // At near final size, + 1 * pel is a good amount to make gap,
    // but at a small size + 1 * pel caused a void along the join.
    // Now I decided the big end needs less gap, trying / 2.
    // work x 200:
    rq = rs + ( pel * 200 * factor / FINAL_OMEGA_MS + 50 ) / 100;

    // RQSQ = RQ squared
    rqsq = rq * rq;

    #if DO_DEBUG_OMEGA1
        SpewValue( L"rl", ( int ) rl );
        SpewValue( L"hl", ( int ) hl );
        SpewValue( L"rs", ( int ) rs );
        SpewValue( L"rq", ( int ) rq );
        SpewValue( L"rqsq", ( int ) rqsq );
    #endif

    // Do it!
    memset( pColorArray, 0, pelsacross * pelsdown );

    RandomMovement( );

    int nPoints = FINAL_N_POINTS * factor / FINAL_OMEGA_MS * factor / FINAL_OMEGA_MS;

    #if DO_DEBUG_OMEGA1
        // Keep a closest and furthest to output for study
        study_ztmin = 0;
        study_ztminDistanceFromEyeZp = 0;
        study_ztminIndexForZp = 0;
        study_ztminLevel = 0;
        study_ztmax = 0;
        study_ztmaxDistanceFromEyeZp = 0;
        study_ztmaxIndexForZp = 0;
        study_ztmaxLevel = 0;
    #endif

    DrawTorus( 31 * nPoints ); // get more density
    DrawCylinder( 10 * nPoints );

    #if DO_DEBUG_OMEGA1
        // Keep a closest and furthest to output for study
        SpewValue( L"study_ztmin", study_ztmin );
        SpewValue( L"study_ztminDistanceFromEyeZp", study_ztminDistanceFromEyeZp );
        SpewValue( L"study_ztminIndexForZp", study_ztminIndexForZp );
        SpewValue( L"study_ztminLevel", study_ztminLevel );
        SpewValue( L"study_ztmax", study_ztmax );
        SpewValue( L"study_ztmaxDistanceFromEyeZp", study_ztmaxDistanceFromEyeZp );
        SpewValue( L"study_ztmaxIndexForZp", study_ztmaxIndexForZp );
        SpewValue( L"study_ztmaxLevel", study_ztmaxLevel );
    #endif

    BitmapTransfer( );
}

int COme::IntegerSqrt( int num )
{
    // Funny, the Vista .NET build says sqrt( ) is ambiguous.
    // What kind of maniac codes his own sqrt?
    // This uses Newton's approximation.
    // The input number must be < 0x7fffffff, a positive in only 31 bits
    if( num <= 0 )  //zero is zero, and negatives zero too
        return 0;

    unsigned int test = 0x80000000;
    unsigned int best = 0x00008000;

    //first, shift bits into approx range
    for( ;; )
    {
        test >>= 1;
        if( ( test & num ) != 0 )
        {
            best += best >> 2; // improved guess at 1.0 may be 1.25
            break;
        }
        best >>= 1;
        test >>= 1;
        if( ( test & num ) != 0 )
        {
            best += best >> 1; // improved guess at 1.0 may be 1.5
            break;
        }
        // So suppose num was between 0x400... and 0x7fff...
        // At shift 1 of test, it was 0x400... -> best = 0x0000a000
        // 0x0000a000 squared = 0x64000000, that's reasonable.
        // So suppose num was between 0x200... and 0x3fff...
        // At shift 2 of test, it was 0x200... -> best = 0x00006000
        // 0x00006000 squared = 0x24000000, that's reasonable.
    }

    // Take the average of ( num/best + best )
    // Thanks to Newton!
    // Let's go two rounds, with rounding ( of half divisor, >> 1 ):

    if( best == 0 ) // CYA
        return 0;
    best = ( ( ( num + ( best >> 1 ) ) / best ) + best ) >> 1;

    if( best == 0 ) // CYA
        return 0;
    best = ( ( ( num + ( best >> 1 ) ) / best ) + best ) >> 1;

    return best;
}


void COme::InitializeCurrentTransformMatrix( )
{
    ctm[0] = ctm[5] = ctm[10] = ctm[15] = ctmscale;
    ctm[1] = ctm[2] = ctm[3] = ctm[4] = 0;
    ctm[6] = ctm[7] = ctm[8] = ctm[9] = 0;
    ctm[11] = ctm[12] = ctm[13] = ctm[14] = 0;
}

void COme::InitializeConcatenationMatrix( )
{
    cat[0] = cat[5] = cat[10] = cat[15] = ctmscale;
    cat[1] = cat[2] = cat[3] = cat[4] = 0;
    cat[6] = cat[7] = cat[8] = cat[9] = 0;
    cat[11] = cat[12] = cat[13] = cat[14] = 0;
}

void COme::Concatenate( )
{
    //matrix multiplication: CTM = CAT x CTM.
    int cnew[ 16 ];
    int i = 0;
    int row;
    for( row = 0; row < 16; row += 4 )
    {
        int col;
        for( col = 0; col < 4; col ++ )
        {
            cnew[ i ] = (
            cat [ row + 0 ] * ctm [ col + ( 0 << 2 ) ] +
            cat [ row + 1 ] * ctm [ col + ( 1 << 2 ) ] +
            cat [ row + 2 ] * ctm [ col + ( 2 << 2 ) ] +
            cat [ row + 3 ] * ctm [ col + ( 3 << 2 ) ] ) / ctmscale;
            i++;
        }
    }
    memcpy( ctm, cnew, sizeof( ctm ) );
}

void COme::InitializeSindCosdTables( )
{
    int angle;
    for( angle = 0; angle < 3600; angle++ )
    {
        // This is one place that DOUBLE is actually needed.
        double a = angle;
        double rads = a * 3.141592653589 / 1800.0;
        double s = sin( rads ) * ctmscale;
        double c = cos( rads ) * ctmscale;
        ctmsin [angle] = ( int ) s;
        ctmcos [angle] = ( int ) c;
    }
}

int COme::Sind( int tenthdegree )
{
    int angle = tenthdegree % 3600;
    if( angle < 0 )
        angle += 3600;
    return ctmsin[ angle ];
}

int COme::Cosd( int tenthdegree )
{
    int angle = tenthdegree % 3600;
    if( angle < 0 )
        angle += 3600;
    return ctmcos[ angle ];
}

int COme::ChangeVariable( int * var, int stimulus, int BasinLimit )
{
    // Change of Variable, of which there are nine.
    // Each has its array of ndv slots for history.

    // Motion is pleasing after integrating
    // 2-nd order random at 5-th derivatives
    // Whatever I meant by that!

    // I see my frugal self dividing, not shifting.
    // That's because they are signed!

    // First, I subtract the 1/128 part from each slot.
    // That would be to preserve the overall magnitude.

    // sum to 100% or less:
    // CHANGE_FEEDFORWARD
    // CHANGE_FEEDBACK

    var [6] = var[6] * CHANGE_FEEDBACK / 100;
    var [5] = var[5] * CHANGE_FEEDBACK / 100;
    var [4] = var[4] * CHANGE_FEEDBACK / 100;
    var [3] = var[3] * CHANGE_FEEDBACK / 100;
    var [2] = var[2] * CHANGE_FEEDBACK / 100;
    var [1] = var[1] * CHANGE_FEEDBACK / 100;
    var [0] = var[0] * CHANGE_FEEDBACK / 100;

    int rand1 = rand( ) * rand( ) % stimulus;
    int rand2 = rand( ) * rand( ) % stimulus;

    var [6] += ( rand1 - rand2 );
    var [5] += var [6] * CHANGE_FEEDFORWARD / 100;
    var [4] += var [5] * CHANGE_FEEDFORWARD / 100 ;
    var [3] += var [4] * CHANGE_FEEDFORWARD / 100 ;
    var [2] += var [3] * CHANGE_FEEDFORWARD / 100 ;
    var [1] += var [2] * CHANGE_FEEDFORWARD / 100 ;
    var [0] += var [1] * CHANGE_FEEDFORWARD / 100 ;

    // The stimulus that make good startup ends too big/fast.
    // Put a gentle feedback basin around the screendiameter.

    if( var[0] > BasinLimit )
        var[6] -= ( var[0] - BasinLimit ) * BASIN_STEEPNESS / 100;

    if( var[0] < -BasinLimit )
        var[6] -= ( var[0] + BasinLimit ) * BASIN_STEEPNESS / 100;

    return var[0];
}

void COme::RandomMovement( )
{
    // Start with unity transform
    InitializeCurrentTransformMatrix( );

    {
        // Add object space random translations
        ctm [3] = ChangeVariable( ox, TranslationChangeStimulusAmount, TranslationChangeBasinLimit ) * ctmscale / 2;
        ctm [7] = ChangeVariable( oy, TranslationChangeStimulusAmount, TranslationChangeBasinLimit ) * ctmscale / 2;
        ctm [11] = ChangeVariable( oz, TranslationChangeStimulusAmount, TranslationChangeBasinLimit ) * ctmscale / 2;
    }

    {
        // Add random axis rotations
        //rotate around the x axis: moving +y axis toward +z axis
        InitializeConcatenationMatrix( );
        {
            int r = ( r1x + ChangeVariable( rx, RotationChangeStimulusAmount, RotationChangeBasinLimit ) ) % 3600;
            if( r < 0 )
                r += 3600;
            int c = ctmcos [r];
            int s = ctmsin [r];
            cat [5] = c;
            cat [6] = -s;
            cat [9] = s;
            cat [10] = c;
        }
        Concatenate( );

        //rotate around the y axis: moving +z axis toward +x axis
        InitializeConcatenationMatrix( );
        {
            int r = ( r1y + ChangeVariable( ry, RotationChangeStimulusAmount, RotationChangeBasinLimit ) ) % 3600;
            if( r < 0 )
                r += 3600;
            int c = ctmcos [r];
            int s = ctmsin [r];
            cat [0] = c;
            cat [8] = -s;
            cat [2] = s;
            cat [10] = c;
        }
        Concatenate( );

        //rotate around the z axis: moving +x axis toward +y axis
        InitializeConcatenationMatrix( );
        {
            int r = ( r1z + ChangeVariable( rz, RotationChangeStimulusAmount, RotationChangeBasinLimit ) ) % 3600;
            if( r < 0 )
                r += 3600;
            int c = ctmcos [r];
            int s = ctmsin [r];
            cat [5] = c;
            cat [1] = -s;
            cat [4] = s;
            cat [0] = c;
        }
        Concatenate( );
    }

    {
        // Add viewing space random translations
        ctm [3] = ChangeVariable( tx, TranslationChangeStimulusAmount, TranslationChangeBasinLimit ) * ctmscale / 2;
        ctm [7] = ChangeVariable( ty, TranslationChangeStimulusAmount, TranslationChangeBasinLimit ) * ctmscale / 2;
        ctm [11] = ChangeVariable( tz, TranslationChangeStimulusAmount, TranslationChangeBasinLimit ) * ctmscale / 2;
    }

}

void COme::PaintXYZ( int xo, int yo, int zo )
{
    // Math is arranged to avoid floating point.
    // Need to make sure it will fit in 32 bits.

    // Solve position in a new 3-space after Movement.
    int xt = ( int ) ( ( ctm[ 0] * xo + ctm[ 1] * yo + ctm[ 2] * zo + ctm[ 3] ) / ctmscale );
    int yt = ( int ) ( ( ctm[ 4] * xo + ctm[ 5] * yo + ctm[ 6] * zo + ctm[ 7] ) / ctmscale );
    int zt = ( int ) ( ( ctm[ 8] * xo + ctm[ 9] * yo + ctm[10] * zo + ctm[11] ) / ctmscale );

    // Now do a perspective transformation:
    // Basically, scale X and Y in proportion
    // to their Z distance from the viewer.

    // DIVISOR *IS* THE SCALED DISTANCE IN Z OF POINT FROM VIEWER'S EYE.

    int DistanceFromEyeZp = ( int ) ( zt + screenz );

    // If the 3-space point were as close as the viewer's eye,
    // zt is negative, = -screenz, then DistanceFromEyeZp would be zero,
    // and the magnification would be infinite.
    //
    // If the point were at the plane of the display, the
    // magnification ( from 3-space to 2-space ) should be unity.
    //
    // If the point were opposite the viewer, zt = positive
    // and equal to screeenz, then the magnification should be
    // one-half.

    // I need some different view of z value to compute
    // the "power law" of the fall-off of illumination:
    // It's just that same magnification squared.
    // Make some minimum distance map to max level 255,
    // and some maximum distance map to min of level 1,
    // for level 0 means no drawing action.

    if( DistanceFromEyeZp <= 0 ) // CYA
        DistanceFromEyeZp = 1;

    {
        int xp = screenz * xt / DistanceFromEyeZp;
        int yp = screenz * yt / DistanceFromEyeZp;

        // Compute unscaled pixel x,y location in bitmap.
        int co = ( int ) ( ( screenhalfx + xp + roundpel ) / pel );
        int ro = ( int ) ( ( screenhalfy + yp + roundpel ) / pel );

        // Omega may grow, and travel beyond the screen.
        // Only draw in valid bitmap array index ranges.

        if( co >= 0 && co < pelsacross
        && ro >= 0 && ro < pelsdown )
        {
            // Compute an illumination based on distance.
            // I did my hard work in the constructor!

            int IndexForZp = ( DistanceFromEyeZp - NearestZp ) / ZpLookupDivisor;
            if( IndexForZp > N_ZP_LEVELS - 1 )
                IndexForZp = N_ZP_LEVELS - 1; // CYA
            if( IndexForZp < 1 )
                IndexForZp = 1; // I need to avoid zero = no drawing.

            int Level = ZpLevel[ IndexForZp ];
            // Do not convert this yet, but store in my bitmap as level:
            // unsigned char color = ZpColor[ Level ];

            // Keep a closest and furthest to output for study
            #if DO_DEBUG_OMEGA1
                if( zt < study_ztmin )
                {
                    study_ztmin = zt;
                    study_ztminDistanceFromEyeZp = DistanceFromEyeZp;
                    study_ztminIndexForZp = IndexForZp;
                    study_ztminLevel = Level;
                }
                if( zt > study_ztmax )
                {
                    study_ztmax = zt;
                    study_ztmaxDistanceFromEyeZp = DistanceFromEyeZp;
                    study_ztmaxIndexForZp = IndexForZp;
                    study_ztmaxLevel = Level;
                }
            #endif


            // drawpel... into my array first.
            int Aindex = ro * pelsacross + co;
            // brighter ( higher ) colors can overwrite dim or zero=off.
            if( pColorArray[ Aindex ] < Level )
                pColorArray[ Aindex ] = Level;
        }
    }
}

void COme::DrawTorus( int nPoints )
{
    //draw the torus,
    //with hole centered on x=y=z=0
    //with body centered along trace ( x*x + y*y = rl*rl )
    //with body radius rs

    int n;
    for ( n=0; n<nPoints; n++ )
    {
        // I sweeps out a circuit around large diameter.
        // At each i, pick random spot on circumference.

        int i = rand( ) % 3600;
        int j = rand( );

        // Someday, figure out what this means.

        int rp = rl + ( Cosd( j ) * rs ) / ctmscale;
        int x = Cosd( i ) * rp / ctmscale;
        int y = Sind( i ) * rp / ctmscale;
        int z = Sind( j ) * rs / ctmscale;

        //exclude torus inside cylinder
        if( x < rl-rq
        || ( x-rl )*( x-rl ) + z*z > rqsq )
            PaintXYZ( x, y, z );
    }
}

void COme::DrawCylinder( int nPoints )
{
    //draw the cylinder,
    //with length 2 * hl,
    //with body centered along trace ( x = rl )
    //with body radius rs.

    int n;
    for ( n=0; n<nPoints; n++ )
    {
        // Cosine( i/10 ) sweeps out length of cylinder.
        // I did that to put more density near the ends.
        // At each i, pick random spot on circumference.

        int i = rand( ) % 3600;
        int j = rand( );
        int x = rl + ( Cosd( j ) * rs ) / ctmscale;
        int y = Cosd( i ) * hl / ctmscale;
        int z = Sind( j ) * rs / ctmscale;

        // Surely, RAT means radius translated.
        // Someday, figure out what this means.

        int rat = IntegerSqrt( x*x + y*y ) - rl;
        //exclude cylinder inside torus
        if( rat > rq
        || rat*rat + z*z > rqsq )
        {
            PaintXYZ( x, y, z );
        }
    }
}

void COme::BitmapTransfer( )
{
    int i = 0;
    int y = 0;
    for( ;; )
    {
        int x = 0;
        for( ;; )
        {
            int level = pColorArray[ i++ ];
            // Zero signifies no pel drawn here.
            if( level > 0 )
            {
                unsigned int color = ZpColor[ level ];
                SetPixel( g_hdcCompat, x, y, color );
            }
            if( ++x == pelsacross )
                break;
        }
        if( ++y == pelsdown )
            break;
    }
}
