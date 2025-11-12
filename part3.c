#include "lcd.h"
#include "Timer.h"
#include <math.h>
#include "movement.h"
#include "open_interface.h"
#include "uart.h"
#include "adc.h"
#include "servo.h"
#include "ping.h"

//struct that represents each object that the robot sees
struct object_val
{
    int number;
    int startingAngle;
    int endingAngle;
    float distance;
    float width;
    int middleAngle;
};
void scan(float *sensor_data_array);
void directRobot(int angle, oi_t *drive_data);
int findObjects(struct object_val *objects, float *sensor_data_array);
void sendingString(char *sendString);
void calculatingLinearWidth(int structNum, struct object_val *objects);
void cleanIRData(float *sensor_data_array);
int findSmallestWidth(struct object_val *objects, int structNum,
                      int *smallestObjectNum);
int hitObject = 0;
int main(void)
{

    timer_init();
    lcd_init();
    adc_init();
    //cyBOT_init_Scan(0b0111);
    uart_init(115200);
    oi_t *drive_data = oi_alloc();
    oi_init(drive_data);
    ping_init();
    servo_init();

    //variable array of our struct type that holds our objects
    struct object_val objects[3];
    float sensor_data_array[91];
    //int i;
    int j;
    int smallestObjectNum;
    int structNum = 0;
    char sendString[50];
    sendingString("Starting in autonomous, press t to switch");
    sendingString("\r\nPress 'h' to scan\r\n");

    char mode = 'a'; //a for autonomous, m for manual

    while (1)
    {
        char receivedByte = uart_receive();

        if (receivedByte == 't')
        {
            uart_flush_input();
            if (mode == 'a')
            {
                mode = 'm';
                sendingString("\r\nSwitching to manual");
            }
            else
            {
                mode = 'a';
                sendingString("\r\nSwitching to autonomous");
                sendingString("\r\nPress 'h' to scan\r\n");
                servo_move(0);
                //cyBOT_Scan(0, &sensor_data);

            }
        }
        //scan, return what movement should be, then pause
        // wait until h is pressed again
        else if (mode == 'a')
        {
            if (receivedByte == 'h')
            {
                servo_move(0);
                //cyBOT_Scan(0, &sensor_data);
                timer_waitMillis(200);
                sendingString("\r\nScanning...\r\n");
                scan(sensor_data_array);
                cleanIRData(sensor_data_array);

                //interate through the 90 data points held in the array
                structNum = findObjects(objects, sensor_data_array);
                calculatingLinearWidth(structNum, objects);

                //finding the smallest widtg from the objects
                int smallestAngle = findSmallestWidth(objects, structNum,
                                                      &smallestObjectNum);
                sendingString("\r\nObject#\tAngle\t\tDistance\tWidth\r\n");

                for (j = 0; j < 3; j++)
                {
                    sprintf(sendString, "%d\t%d\t\t%.3f\t\t%f\r\n",
                            objects[j].number, objects[j].middleAngle,
                            objects[j].distance, objects[j].width);
                    sendingString(sendString);
                }
                sprintf(sendString,
                        "\r\nThe object is at %d degrees and %.1f away\r\n",
                        smallestAngle, objects[smallestObjectNum].distance);
                sendingString(sendString);

                sendingString(
                        "\r\nPress 'h' to execute movement, or t to toggle\r\n");

                char confirm = uart_receive_blocking();
                uart_flush_input();

                if (confirm == 'h')
                {
                    directRobot(smallestAngle, drive_data);
                    move_forward(drive_data,
                                 (objects[smallestObjectNum].distance * 100));
                }
                else if (confirm == 't')
                {
                    mode = 'm';
                    sendingString("\r\nSwitching to manual");
                }
                uart_flush_input();
            }

        }
        else if (mode == 'm')
        {
            oi_update(drive_data);
            if (drive_data->bumpLeft)
            {
                hitObject = 1;
                oi_setWheels(0, 0);
                sendingString("\r\nObject hit on left side");
            }
            else if (drive_data->bumpRight)
            {
                hitObject = 1;
                oi_setWheels(0, 0);
                sendingString("\r\nObject hit on right side");
            }

            if (hitObject == 0)
            {

                if (receivedByte == 'w')
                {
                    oi_setWheels(50, 50);
                    uart_flush_input();

                }
                else if (receivedByte == 'a')
                {
                    oi_setWheels(50, -50);
                    uart_flush_input();

                }
                else if (receivedByte == 'd')
                {
                    oi_setWheels(-50, 50);
                    uart_flush_input();

                }
                else if (receivedByte == 's')
                {
                    oi_setWheels(-50, -50);
                    uart_flush_input();

                }
                else if (receivedByte == 'q')
                {
                    uart_flush_input();
                    break;
                }
                else if (receivedByte == 'b')
                {
                    uart_flush_input();
                    oi_setWheels(0, 0);
                }
                else if (receivedByte == 'm')
                {
                    sendingString("\r\nScanning...\r\n");
                    uart_flush_input();
                    scan(sensor_data_array);
                    cleanIRData(sensor_data_array);

                    //interate through the 90 data points held in the array
                    structNum = findObjects(objects, sensor_data_array);
                    calculatingLinearWidth(structNum, objects);

                    //finding the smallest widtg from the objects
                    int smallestAngle = findSmallestWidth(objects, structNum,
                                                          &smallestObjectNum);
                    sendingString("\r\nObject#\tAngle\t\tDistance\tWidth\r\n");

                    for (j = 0; j < 3; j++)
                    {
                        sprintf(sendString, "%d\t%d\t\t%.1f\t\t%f\r\n",
                                objects[j].number, objects[j].middleAngle,
                                objects[j].distance, objects[j].width);
                        sendingString(sendString);
                    }
                }
            }
            else
            {
                sendingString(
                        "\r\nHit 'r' to resume manual mode, or 't' to toggle");
                char returnManual = uart_receive_blocking();
                uart_flush_input();

                if (returnManual == 'r')
                {
                    hitObject = 0;
                    sendingString("\r\nManual resumed");
                }
                else if (returnManual == 't')
                {
                    hitObject = 0;
                    mode = 'a';
                    sendingString("\r\nSwitching to autonomous");
                    sendingString("\r\nPress 'h' to scan\r\n");
                    servo_move(0);
                    //cyBOT_Scan(0, &sensor_data);
                }
            }
        }
        receivedByte = '0';
    }
    oi_free(drive_data);
    return 0;
}

void scan(float *sensor_data_array)
{
    int i;
    int j;
    for (i = 0; i <= 90; i++)
    {
        int average = 0;
        timer_waitMillis(400);
        servo_move(i * 2);

        //scanning from the sensor data at the angle that i is at
        for (j = 0; j < 3; j++)
        {
            //cyBOT_Scan(i * 2, &sensor_data);
            average += adc_read();
        }
        average /= 3;
        //float distance = 117260457 * pow(average, -2.12481);
        //float distance = 250307.306 * pow(average, -1.30903);
        //float distance = 59807.2509 * pow(average, -1.10394);
        float distance = 130257.491 * pow(average, -1.2102); //robot 6 upstairs

        //updating the send string, so that it is formatted and clean
        sensor_data_array[i] = distance / 100;
    }

}

void directRobot(int angle, oi_t *drive_data)
{
    if (angle < 90)
    {
        turn_clockwise(drive_data, -1 * (76 - angle));
    }
    else
    {
        turn_clockwise(drive_data, (angle - 108));
    }
}

int findObjects(struct object_val *objects, float *sensor_data_array)
{
    int i;
    int structNum = 0;
    //an int that acts as a boolean variable to determine if the values being iterated are object or blank space
    int is_object = 0;
    for (i = 0; i < 90; i++)
    {

        //check if we are currently iterating through an object's data and that the distance is less than 1.0
        if (!is_object && sensor_data_array[i] <= 0.9)
        {
            is_object = 1;
            //save information about the new object
            objects[structNum].number = structNum + 1;
            objects[structNum].startingAngle = i * 2;
            objects[structNum].distance = sensor_data_array[i] * 100;
        }
        //wait until the
        else if (is_object && sensor_data_array[i] > 0.9)
        {
            is_object = 0;
            objects[structNum].endingAngle = (i - 1) * 2;
            if (objects[structNum].endingAngle
                    - objects[structNum].startingAngle > 4)
            {
                structNum++;
            }

        }
    }

    return structNum;
}

void calculatingLinearWidth(int structNum, struct object_val *objects)
{
    int j;
    for (j = 0; j < structNum; j++)
    {
        lcd_printf("%d", j);
        int scanAngle = (objects[j].startingAngle + objects[j].endingAngle) / 2;
        objects[j].middleAngle = scanAngle;

        servo_move(scanAngle);
        timer_waitMillis(2000);

        //float distance = ping_read();
        int average = adc_read();
        float distance = 130257.491 * pow(average, -1.2102); //robot 6 upstairs

        objects[j].distance = distance / 100;
        //calculate linear width n=and save it
        float angle_rad = (objects[j].endingAngle - objects[j].startingAngle)
                * (M_PI / 180.0f);
        float width = sqrtf(
                2.0f * objects[j].distance * objects[j].distance
                        * (1.0f - cosf(angle_rad)));

        objects[j].width = width * 100;

    }
}

void cleanIRData(float *sensor_data_array)
{
    int pass;
    int i;
    for (pass = 0; pass < 3; pass++)
    {
        for (i = 1; i < 90; i++)
        {
            float prev = sensor_data_array[i - 1];
            float curr = sensor_data_array[i];
            float next = sensor_data_array[i + 1];

            if (fabs(curr - prev) > 0.5 && fabs(curr - next) > 0.5)
            {
                sensor_data_array[i] = (prev + next) / 2.0;
            }
        }
    }
}

int findSmallestWidth(struct object_val *objects, int structNum,
                      int *smallestObjectNum)
{
    int j;
    int smallestWidth = objects[0].width;
    int smallestAngle = objects[0].middleAngle;
    *smallestObjectNum = 0;
    //comparing the widths of the held objects
    for (j = 1; j < structNum; j++)
    {
        if (objects[j].width < smallestWidth)
        {
            smallestAngle = objects[j].middleAngle;
            *smallestObjectNum = j;

        }
    }
    return smallestAngle;
}

