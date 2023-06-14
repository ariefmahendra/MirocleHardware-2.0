#include <Arduino.h>

#ifndef ROTATION_COUNTER
#define ROTATION_COUNTER 1
#include "config.h"

#define SPEED_AVERAGING 5
enum State
{
    INIT,
    FORW1,
    FORW2,
    FORW3,
    FORWF,
    BACKW1,
    BACKW2,
    BACKW3,
    BACKWF
};

enum Transition
{
    TRANS00 = 0b00,
    TRANS10 = 0b10,
    TRANS11 = 0b11,
    TRANS01 = 0b01
};

enum Direction
{
    DIR_UNSET,
    DIR_FORWARD,
    DIR_BACKWARD
};

enum SensorMode
{
    MODE_ACTIVE_HIGH = true,
    MODE_ACTIVE_LOW = false
};

class RotationCounter
{
private:
    State state = INIT;
    Direction valid_direction = DIR_UNSET;
    uint
        step_forward,
        step_backward,
        rotation_forward,
        rotation_backward,
        rotation_forward_last_2_sec = 0,
        rotation_backward_last_2_sec = 0,
        last_2_sec_counter = 0;
    uint8_t
        step_per_rotation,
        step_forward_validator = 0,
        step_backward_validator = 0,
        transition = TRANS00,
        pin_ir_a,
        pin_ir_b;
    bool
        pin_state_a,
        pin_state_b,
        is_counting,
        _debug = false;
    long last_forward_msec,
        last_backward_msec,
        last_2_sec_timer;
    float circular_radius_m = 0,
          speed_forward_mps = 0,
          speed_backward_mps = 0,
          speed_forward_avg[SPEED_AVERAGING],
          speed_backward_avg[SPEED_AVERAGING];

public:
    RotationCounter(uint8_t pin_a, uint8_t pin_b, SensorMode sensor_mode, uint8_t step, float radius);
    State getState();
    void reset_counter();
    void start_counter();
    void stop_counter();
    uint getStepForward();
    uint getStepBackward();
    uint getRotationForward();
    uint getRotationBackward();
    uint getRotationValid();
    float getValidLinearSpeedMps();
    void runner();
    void setDebug(bool debug);
};

RotationCounter::RotationCounter(uint8_t pin_a, uint8_t pin_b, SensorMode sensor_mode, uint8_t step, float radius)
{
    pin_ir_a = pin_a;
    pin_ir_b = pin_b;
    pinMode(pin_ir_a, (sensor_mode == MODE_ACTIVE_HIGH) ? INPUT_PULLDOWN : INPUT_PULLUP);
    pinMode(pin_ir_b, (sensor_mode == MODE_ACTIVE_HIGH) ? INPUT_PULLDOWN : INPUT_PULLUP);

    step_per_rotation = step;
    circular_radius_m = radius;
}

State RotationCounter::getState()
{
    return this->state;
}

void RotationCounter::reset_counter()
{
    step_forward = 0;
    step_backward = 0;
    rotation_forward = 0;
    rotation_backward = 0;
    step_forward_validator = 0;
    step_backward_validator = 0;
    valid_direction = DIR_UNSET;
}

void RotationCounter::start_counter()
{
    is_counting = true;
    reset_counter();
}

void RotationCounter::stop_counter()
{
    is_counting = false;
}

uint32_t RotationCounter::getStepForward() { return this->step_forward; }

uint32_t RotationCounter::getStepBackward() { return this->step_backward; }

uint32_t RotationCounter::getRotationForward()
{
    return rotation_forward;
}

uint32_t RotationCounter::getRotationBackward()
{
    return rotation_backward;
}

uint32_t RotationCounter::getRotationValid()
{
    switch (valid_direction)
    {
    case DIR_FORWARD:
        return rotation_forward;
        break;
    case DIR_BACKWARD:
        return rotation_backward;
        break;
    default:
        return 0;
        break;
    }
    return 0;
}

float RotationCounter::getValidLinearSpeedMps()
{
    switch (valid_direction)
    {
    case DIR_FORWARD:
        if (last_2_sec_counter >= SPEED_AVERAGING)
        {
            float speed_sum = 0;
            for (uint8_t i = 0; i < SPEED_AVERAGING; i++)
            {
                speed_sum += speed_forward_avg[i];
            }
            return speed_sum / SPEED_AVERAGING;
        }
        else if (last_2_sec_counter < SPEED_AVERAGING)
        {
            float speed_sum = 0;
            for (uint8_t i = 0; i < last_2_sec_counter; i++)
            {
                speed_sum += speed_forward_avg[i];
            }
            return speed_sum / last_2_sec_counter;
        }
        // return speed_forward_mps;
        break;
    case DIR_BACKWARD:
        if (last_2_sec_counter >= SPEED_AVERAGING)
        {
            float speed_sum = 0;
            for (uint8_t i = 0; i < SPEED_AVERAGING; i++)
            {
                speed_sum += speed_backward_avg[i];
            }
            return speed_sum / SPEED_AVERAGING;
        }
        else if (last_2_sec_counter < SPEED_AVERAGING)
        {
            float speed_sum = 0;
            for (uint8_t i = 0; i < last_2_sec_counter; i++)
            {
                speed_sum += speed_backward_avg[i];
            }
            return speed_sum / last_2_sec_counter;
            // return speed_sum / rotation_backward;
        }
        return speed_backward_mps;
        break;
    default:
        return 0;
        break;
    }
    return 0;
}

void RotationCounter::runner()
{
    if (!is_counting)
        return;

    // baca nilai sensor
    pin_state_a = digitalRead(pin_ir_a);
    pin_state_b = digitalRead(pin_ir_b);

    // masking transisi
    transition = 0x00;
    transition |= (pin_state_a);
    // geser nilai transisi saat ini 1 bit ke kiri
    transition = transition << 1;
    transition |= (pin_state_b);

    // transition
    switch (this->state)
    {
    case INIT:
        switch (this->transition)
        {
        case TRANS00:
            state = INIT;
            break;
        case TRANS10:
            state = FORW1;
            break;
        case TRANS11:
            state = INIT;
            break;
        case TRANS01:
            state = BACKW1;
            break;
        default:
            break;
        }
        break;
    case FORW1:
        switch (this->transition)
        {
        case TRANS00:
            state = INIT;
            break;
        case TRANS10:
            state = FORW1;
            break;
        case TRANS11:
            state = FORW2;
            break;
        case TRANS01:
            state = INIT;
            break;
        default:
            break;
        }
        break;
    case FORW2:
        switch (this->transition)
        {
        case TRANS00:
            state = INIT;
            break;
        case TRANS10:
            state = INIT;
            break;
        case TRANS11:
            state = FORW2;
            break;
        case TRANS01:
            state = FORW3;
            break;
        default:
            break;
        }
        break;
    case FORW3:
        switch (this->transition)
        {
        case TRANS00:
            state = FORWF;
            break;
        case TRANS10:
            state = INIT;
            break;
        case TRANS11:
            state = INIT;
            break;
        case TRANS01:
            state = FORW3;
            break;
        default:
            break;
        }
        break;
    case FORWF:
        // TODO: final step_forward
        state = INIT;
        step_forward++;
        step_forward_validator++;
        if (step_backward_validator != 0)
            step_backward_validator--;
        if (step_forward_validator == step_per_rotation)
        {
            step_forward_validator = 0;
            rotation_forward++;
            rotation_forward_last_2_sec++;
            // speed_forward_mps = (2 * PI * circular_radius_m) / ((float)(millis() - last_forward_msec) / 1000);
            // speed_forward_avg[rotation_backward % SPEED_AVERAGING] = speed_forward_mps;
            last_forward_msec = millis();
        }
        break;
    case BACKW1:
        switch (this->transition)
        {
        case TRANS00:
            state = INIT;
            break;
        case TRANS10:
            state = INIT;
            break;
        case TRANS11:
            state = BACKW2;
            break;
        case TRANS01:
            state = BACKW1;
            break;
        default:
            break;
        }
        break;
    case BACKW2:
        switch (this->transition)
        {
        case TRANS00:
            state = INIT;
            break;
        case TRANS10:
            state = BACKW3;
            break;
        case TRANS11:
            state = BACKW2;
            break;
        case TRANS01:
            state = INIT;
            break;
        default:
            break;
        }
        break;
    case BACKW3:
        switch (this->transition)
        {
        case TRANS00:
            state = BACKWF;
            break;
        case TRANS10:
            state = BACKW3;
            break;
        case TRANS11:
            state = INIT;
            break;
        case TRANS01:
            state = INIT;
            break;
        default:
            break;
        }
        break;
    case BACKWF:
        // TODO: final step_backward
        state = INIT;
        step_backward++;
        step_backward_validator++;
        if (step_forward_validator != 0)
            step_forward_validator--;
        if (step_backward_validator == step_per_rotation)
        {
            step_backward_validator = 0;
            rotation_backward_last_2_sec++;
            rotation_backward++;
            // speed_backward_mps = (float)(2 * PI * circular_radius_m) / ((millis() - last_backward_msec) / 1000);
            // speed_backward_avg[rotation_backward % SPEED_AVERAGING] = speed_backward_mps;

            last_backward_msec = millis();
        }
        break;
    default:
        break;
    }

    if (valid_direction == DIR_UNSET)
    {
        if (step_forward == 1)
            valid_direction = DIR_FORWARD;
        if (step_backward == 1)
            valid_direction = DIR_BACKWARD;
    }

    if (millis() - last_2_sec_timer > 2000)
    {
        last_2_sec_timer = millis();
        rotation_forward_last_2_sec = 0;
        rotation_backward_last_2_sec = 0;
        last_2_sec_counter++;
    }
    else
    {
        speed_forward_avg[last_2_sec_counter % SPEED_AVERAGING] = rotation_forward_last_2_sec * (2 * PI * this->circular_radius_m) / 2000;
        speed_backward_avg[last_2_sec_counter % SPEED_AVERAGING] = rotation_backward_last_2_sec * (2 * PI * this->circular_radius_m) / 2000;
    }
}

void RotationCounter::setDebug(bool debug) { this->_debug = debug; }
#endif