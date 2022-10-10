#include "Event.h"

// #define IS_EV_KEY

struct Slot
{
    bool mInUse;
    bool mHaveAbsMTTouchMinor;
    bool mHaveAbsMTWidthMinor;
    bool mHaveAbsMTToolType;

    int32_t mAbsMTPositionX;
    int32_t mAbsMTPositionY;
    int32_t mAbsMTTouchMajor;
    int32_t mAbsMTTouchMinor;
    int32_t mAbsMTWidthMajor;
    int32_t mAbsMTWidthMinor;
    int32_t mAbsMTOrientation;
    int32_t mAbsMTTrackingId;
    int32_t mAbsMTPressure;
    int32_t mAbsMTDistance;
    int32_t mAbsMTToolType;

    int32_t mAbsMTCount;
    int32_t action;

#ifdef IS_EV_KEY
    bool has_down;
#endif

} slot = {false};

motion_event *process(input_event event)
{
    if (event.type == EV_ABS)
    {
        switch (event.code)
        {
        case ABS_MT_POSITION_X:
            // printf("ABS_MT_POSITION_X\n");
            slot.mInUse = true;
            slot.mAbsMTPositionX = event.value;
            slot.mAbsMTCount++;
            break;
        case ABS_MT_POSITION_Y:
            // printf("ABS_MT_POSITION_Y\n");
            slot.mInUse = true;
            slot.mAbsMTPositionY = event.value;
            slot.mAbsMTCount++;
            break;
        case ABS_MT_TOUCH_MAJOR:
            // printf("ABS_MT_TOUCH_MAJOR\n");
            slot.mInUse = true;
            slot.mAbsMTTouchMajor = event.value;
            break;
        case ABS_MT_TOUCH_MINOR:
            // printf("ABS_MT_TOUCH_MINOR\n");
            slot.mInUse = true;
            slot.mAbsMTTouchMinor = event.value;
            slot.mHaveAbsMTTouchMinor = true;
            break;
        case ABS_MT_WIDTH_MAJOR:
            // printf("ABS_MT_WIDTH_MAJOR\n");
            slot.mInUse = true;
            slot.mAbsMTWidthMajor = event.value;
            break;
        case ABS_MT_WIDTH_MINOR:
            // printf("ABS_MT_WIDTH_MINOR\n");
            slot.mInUse = true;
            slot.mAbsMTWidthMinor = event.value;
            slot.mHaveAbsMTWidthMinor = true;
            break;
        case ABS_MT_ORIENTATION:
            // printf("ABS_MT_ORIENTATION\n");
            slot.mInUse = true;
            slot.mAbsMTOrientation = event.value;
            break;
        case ABS_MT_TRACKING_ID:
            // printf("ABS_MT_TRACKING_ID\n");
            if (event.value < 0)
            {
                // The slot is no longer in use but it retains its previous contents,
                // which may be reused for subsequent touches.
                // printf("event.value < 0\n");
                slot.mInUse = false;
            }
            else
            {
#ifndef IS_EV_KEY
                slot.mAbsMTCount = 0;
#endif
                slot.mInUse = true;
                slot.mAbsMTTrackingId = event.value;
            }
            break;
        case ABS_MT_PRESSURE:
            // printf("ABS_MT_PRESSURE\n");
            slot.mInUse = true;
            slot.mAbsMTPressure = event.value;
            break;
        case ABS_MT_DISTANCE:
            // printf("ABS_MT_DISTANCE\n");
            slot.mInUse = true;
            slot.mAbsMTDistance = event.value;
            break;
        case ABS_MT_TOOL_TYPE:
            // printf("ABS_MT_TOOL_TYPE\n");
            slot.mInUse = true;
            slot.mAbsMTToolType = event.value;
            slot.mHaveAbsMTToolType = true;
            break;
        }
    }
#ifdef IS_EV_KEY
    else if (event.type == EV_KEY)
    {

        // printf("EV_KEY:%d\n", event.code);
        if (slot.has_down == false)
        {
            slot.has_down = true;
            slot.action = AMOTION_EVENT_ACTION_DOWN;
        }
        else
        {
            slot.has_down = false;
            slot.action = AMOTION_EVENT_ACTION_UP;
            slot.mAbsMTCount = 0;
        }
    }
#endif

    if (event.type == EV_SYN && event.code == SYN_REPORT)
    {
#ifdef IS_EV_KEY
        if (slot.mAbsMTCount > 2)
        {
            slot.action = AMOTION_EVENT_ACTION_MOVE;
        }
#else
        if (slot.mInUse == false)
        {
            slot.action = AMOTION_EVENT_ACTION_UP;
        }
        else if (slot.mAbsMTCount <= 2)
        {
            slot.action = AMOTION_EVENT_ACTION_DOWN;
        }
        else
        {
            slot.action = AMOTION_EVENT_ACTION_MOVE;
        }
#endif
        struct motion_event *m_event = new motion_event();
        m_event->action = slot.action;
        m_event->x = slot.mAbsMTPositionX;
        m_event->y = slot.mAbsMTPositionY;
        return m_event;
    }
    else
    {
        return nullptr;
    }
}
