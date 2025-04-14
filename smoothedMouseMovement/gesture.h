#include <math.h>
#include <stdbool.h>
#include <string.h>

// threshholds
#define THUMB_THRES 60
#define INDEX_THRES 40
#define MIDDLE_THRES 50
#define RING_THRES 50  // to be determined
#define PINKY_THRES 50 // to be determined

static constexpr float THUMB_MEAN = 70.74102628811778f;
static constexpr float THUMB_STD = 14.03024956789845f;
static constexpr float INDEX_MEAN = 40.35706988843323f;
static constexpr float INDEX_STD = 9.874159515428804f;
static constexpr float MIDDLE_MEAN = 54.75783806519453f;
static constexpr float MIDDLE_STD = 16.56953736860662f;
static constexpr float RING_MEAN = 0.08021313543638275f;
static constexpr float RING_STD = 0.3653566227998348f;
static constexpr float PINKY_MEAN = 46.55428675707886f;
static constexpr float PINKY_STD = 13.14342743761899f;

typedef enum Gestures
{
    /*
    Having a letter implies that the finger is flexed.
    T - thumb
    I - index finger
    M - middle finger
    R - ring finger
    P - pinky finger
    */
    // five fingers
    TIMRP,

    // four fingers
    TIMR,
    TMRP,
    TIRP,
    TIMP,
    IMRP,

    // three fingers
    TIM,
    TIR,
    TIP,
    TMR,
    TMP,
    TRP,
    IMR,
    IMP,
    IRP,
    MRP,

    // two fingers
    TI,
    TM,
    TR,
    TP,
    IM,
    IR,
    IP,
    MR,
    MP,
    RP,

    // one finger
    T,
    I,
    M,
    R,
    P,

    // no gesture
    NONE
} Gestures;

/// @private
Gestures threshGesture(float thumb, float index, float middle, float ring, float pinky)
{
    bool thumbBool, indexBool, middleBool, ringBool, pinkyBool;

    thumbBool = thumb > THUMB_THRES;
    indexBool = index > INDEX_THRES;
    middleBool = middle > MIDDLE_THRES;
    ringBool = ring > RING_THRES;
    pinkyBool = pinky > PINKY_THRES;

    int mask = (thumbBool << 0) |
               (indexBool << 1) |
               (middleBool << 2) |
               (ringBool << 3) |
               (pinkyBool << 4);

    switch (mask)
    {

    // five fingers
    case 0b11111:
        return TIMRP;

    // four fingers
    case 0b11110:
        return TIMR;
    case 0b10111:
        return TMRP;
    case 0b11011:
        return TIRP;
    case 0b11101:
        return TIMP;
    case 0b01111:
        return IMRP;

    // three fingers
    case 0b11100:
        return TIM;
    case 0b11010:
        return TIR;
    case 0b11001:
        return TIP;
    case 0b10110:
        return TMR;
    case 0b10101:
        return TMP;
    case 0b10011:
        return TRP;
    case 0b01110:
        return IMR;
    case 0b01101:
        return IMP;
    case 0b01011:
        return IRP;
    case 0b00111:
        return MRP;

    // two fingers
    case 0b11000:
        return TI;
    case 0b10100:
        return TM;
    case 0b10010:
        return TR;
    case 0b10001:
        return TP;
    case 0b01100:
        return IM;
    case 0b01010:
        return IR;
    case 0b01001:
        return IP;
    case 0b00110:
        return MR;
    case 0b00101:
        return MP;
    case 0b00011:
        return RP;

    // one finger
    case 0b10000:
        return T;
    case 0b01000:
        return I;
    case 0b00100:
        return M;
    case 0b00010:
        return R;
    case 0b00001:
        return P;

    default:
        return NONE;
    }
}

Gestures gesture(float thumb, float index, float middle, float ring, float pinky)
{
    constexpr int REQUIRED_STABLE_COUNT = 3;
    static int stableCount = 0;
    
    // Static storage to keep state between calls.
    static Gestures lastStableGesture = NONE;

    // Compute candidate gesture from thresholds (ignoring stability).
    Gestures candidateGesture = threshGesture(thumb, index, middle, ring, pinky);

    if (candidateGesture != lastStableGesture)
        {
            stableCount++;
            if (stableCount >= REQUIRED_STABLE_COUNT)
            {
                lastStableGesture = candidateGesture;
                stableCount = 0;
            }
        }
    else
        {
            // Our candidate is identical to our stored gesture; consider it stable.
            stableCount = REQUIRED_STABLE_COUNT;
        }

    return lastStableGesture;
}
