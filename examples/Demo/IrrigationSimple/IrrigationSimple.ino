/**
 * Simple irrigation system with Arduino EdgeControl
 * 
 * Circuit:
 * - Arduino EdgeControl
 * - MicroSD card
 * - CR2032 Battery and/or 12V Lead Battery
 * 
 * Usage:
 * - Define your callback functions in CustomTasks.{h,cpp}
 * - Map the callback functions to alarmatab command names in
 *   CustomTasks.h::commandMap map.
 * - Add tasks to the alarmtab.txt file and copy it on the SD card;
 *   See alarmtab.txt and AlarmTasks.cpp for documentation.
 */

#include <Arduino_EdgeControl.h>

#include "AlarmTasks.h"
#include "SensorsData.h"
#include "TimeHelpers.h"
#include "TasksHelpers.h"
#include "Helpers.h"

/** UI Management **/
// Button statuses
enum ButtonStatus : byte {
    ZERO_TAP,
    SINGLE_TAP,
    DOUBLE_TAP,
    TRIPLE_TAP,
    LOT_OF_TAPS
};

// ISR: count the button taps
volatile byte taps { 0 };
// ISR: keep elapsed timings
volatile unsigned long previousPress { 0 };
// ISR: Final button status
volatile ButtonStatus buttonStatus { ZERO_TAP };

/**
 * Used to keep track of the running alarms from
 * alarmtab.txt.
 *
 * The loadAndSetTasks() function will fill this
 * with tasks defined in CustomTasks.{h.cpp} and
 * loaded from the alarmtab.txt on the SD.
 *
 */
std::list<AlarmID_t> alarmTabIDs;

/**
 * Used to keep track of the alarms defined in the
 * sketch.
 */
std::list<AlarmID_t> alarmSketchIDs;

std::list<DataPoint> dataPoints;

void setup()
{
    Serial.begin(9600);
    while (!Serial) // wait for Arduino Serial Monitor
        ;
    delay(1000);

    // Set System and Alarm clock
    // Use compile datetime as proxy clock synch
    // Use a CR2032 battery for persisten RTC.
    setSystemClock();

    // Init board-related objects
    EdgeControl.begin();

    // Enable the 3V3 and 12V power rails.
    // Needed for IO Expander and SD card.
    Power.on(PWR_3V3);
    Power.on(PWR_VBAT);

    // Init the IO Expander for LCD and
    // I/O operations (sensors and actuators).
    Wire.begin();
    while (!Expander.begin())
        ;

    // Init the LCD display
    LCD.begin(16, 2);
    LCD.leftToRight();

    // Load alarm tasks list from file on SD.
    // See example alarmtab.txt for example and
    // AlarmTasks.{h, cpp} source files for documentation.
    loadAndSetTasks(alarmTabIDs);

    // Use the LCD button for houskeeping stuff:
    // Single Tap: Show status
    // Double Tap: Reload alarm tasks from SD
    // Triple Tap: Force saving data to SD (TODO)
    pinMode(POWER_ON, INPUT);
    attachInterrupt(POWER_ON, buttonPress, RISING);

    auto id = Alarm.timerRepeat(10, getSensors);
    alarmSketchIDs.push_back(id);

    // Init the sensors inputs
    Inputs.begin();

}

void loop()
{
    // Do alarms processing
    Alarm.delay(10);

    // Detect Single Tap, Double Tap, etc.
    // to buttonStatus
    detectTaps();

    switch (buttonStatus) {
    case ZERO_TAP:
        break;
    case SINGLE_TAP:
        Serial.println("Single Tap");
        statusLCD();
        buttonStatus = ZERO_TAP;
        break;
    case DOUBLE_TAP:
        Serial.println("Double Tap");
        loadAndSetTasks(alarmTabIDs, true);
        buttonStatus = ZERO_TAP;
        break;
    case TRIPLE_TAP:
        Serial.println("Triple Tap");
        saveData();
        buttonStatus = ZERO_TAP;
        break;
    default:
        Serial.println("Too Many Taps");
        buttonStatus = ZERO_TAP;
        break;
    }
}

void buttonPress()
{
    const auto now = millis();
    // Poor man debouncing
    if (now - previousPress > 100)
        taps++;

    previousPress = now;
}

void detectTaps()
{
    // Timeout to validate the button taps counter
    constexpr unsigned int buttonTapsTimeout { 300 };

    // If button has been pressed at least once and button taps validation
    // timeout has been reached set the button status and reset the taps counter
    if (taps > 0 && millis() - previousPress >= buttonTapsTimeout) {
        buttonStatus = static_cast<ButtonStatus>(taps);
        taps = 0;
    }
}