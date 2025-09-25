#include <Arduino.h>
#include "Tasks/TaskEncoder.h"
#include "Tasks/TaskUI.h"
#include "Tasks/TaskMotor.h"

void setup()
{
    TaskEncoder_init();
    TaskUI_init();
    TaskMotor_init();
}

void loop()
{
}
