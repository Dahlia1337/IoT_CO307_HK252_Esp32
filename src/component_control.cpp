#include "component_control.h"
#include "tinyml.h"

void fan_setup()
{
    pinMode(5, OUTPUT);
    pinMode(6, OUTPUT);
    pinMode(7, OUTPUT);
    digitalWrite(6, LOW);
    digitalWrite(7, HIGH);
};

void fan_control(int state)
{
    fan_setup();
    switch (state)
    {
    case 0:
        analogWrite(5, 0);
        break;
    case 1:
        analogWrite(5, 150);
        break;
    case 2:
        analogWrite(5, 255);
        break;

    case 3:
        if (result < 0.5)
            analogWrite(5, 0);
        else if (result < 0.6)
            analogWrite(5, 150);
        else
            analogWrite(5, 255);
        break;

    default:
        analogWrite(5, 0);
        break;
    }
};

void component_reset()
{
    analogWrite(5, 0);
}