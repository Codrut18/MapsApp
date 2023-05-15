#pragma once

#ifdef __GNUC__
using LargeInteger = std::int64_t;
using LargeUnsignedInteger = std::uint64_t;
#else
using LargeInteger = __int64;
using LargeUnsignedInteger = unsigned __int64;
#endif

struct Xy
{
    Xy( int aX = 0, int aY = 0 )
        : x( aX )
        , y( aY ) 
    {}

    int x;
    int y;
};

struct Size
{
    explicit Size( int aWidth = 0, int aHeight = 0 )
        : width( aWidth )
        , height( aHeight ) 
    {}

    int width;
    int height;
};

struct RectF
{
    RectF( float left = 0, float top = 0, float right = 0, float bottom = 0 ) :
        x( left ), y( top ), width( right - left ), height( bottom - top )
    {
    }

    float x;
    float y;
    float width;
    float height;
};


struct Coordinates
{
    Coordinates( double latitude, double longitude )
        : latitude(latitude)
        , longitude(longitude)
    {

    }

    double latitude;
    double longitude;
};

enum class ETouchEvent
{
    TE_Down,
    TE_Move,
    TE_Up,
    TE_Cancel
};

enum class EKey
{
    K_Space = 32,	///< Space
    K_Apostrophe = 39,	///< '
    K_Plus = 43,	///< +
    K_Comma = 44,	///< ,
    K_Minus = 45,	///< -
    K_Period = 46,	///< .
    K_Slash = 47,	///< /
    K_0 = 48,	///< 0
    K_1 = 49,	///< 1
    K_2 = 50,	///< 2
    K_3 = 51,	///< 3
    K_4 = 52,	///< 4
    K_5 = 53,	///< 5
    K_6 = 54,	///< 6
    K_7 = 55,	///< 7
    K_8 = 56,	///< 8
    K_9 = 57,	///< 9
    K_Semicolon = 59,	///< ;
    K_Equal = 61,	///< =
    K_A = 65,	///< A
    K_B = 66,	///< B
    K_C = 67,	///< C
    K_D = 68,	///< D
    K_E = 69,	///< E
    K_F = 70,	///< F
    K_G = 71,	///< G
    K_H = 72,	///< H
    K_I = 73,	///< I
    K_J = 74,	///< J
    K_K = 75,	///< K
    K_L = 76,	///< L
    K_M = 77,	///< M
    K_N = 78,	///< N
    K_O = 79,	///< O
    K_P = 80,	///< P
    K_Q = 81,	///< Q
    K_R = 82,	///< R
    K_S = 83,	///< S
    K_T = 84,	///< T
    K_U = 85,	///< U
    K_V = 86,	///< V
    K_W = 87,	///< W
    K_X = 88,	///< X
    K_Y = 89,	///< Y
    K_Z = 90,	///< Z
    K_Insert = 260,	///< Insert
    K_Delete = 261,	///< Delete
    K_Right = 262,	///< Arrow right
    K_Left = 263,	///< Arrow left
    K_Down = 264,	///< Arrow down
    K_Up = 265,	///< Arrow up
    K_Page_Up = 266,  ///< Page up
    K_Page_Down = 267,  ///< Page down
    K_Home = 268,  ///< Home
    K_End = 269,  ///< End
    K_LShift = 340,	///< Left shift
    K_LCtrl = 341,	///< Left control
    K_LAlt = 342,	///< Left alt
    K_RShift = 344,	///< Right shift
    K_RCtrl = 345,	///< Right control
    K_RAlt = 346	///< Right alt
};

enum class EKeyAction
{
    KA_Release = 0,	    ///< A key was released.
    KA_Press = 1,	///< A key was pressed.
    KA_Repeat = 2		///< A key was held down until it repeated.
};

enum class ETransportMode
{
    Car,
    Truck,
    Bike,
    Pedestrian,
    RoadBike,
    CrossBike,
    CityBike,
    MountainBike
};


