
/**
 * main.c
 */

#include "lcd.h"
#include "Timer.h"
#include <math.h>
#include "movement.h"
#include "open_interface.h"
#include "uart.h"
#include "adc.h"
#include "servo.h"
#include "ping.h"
#include <stdint.h>

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
    unsigned char notes[10] = { 60, 64, 67, 72, 67, 69, 67, 64, 65, 60 };
    unsigned char duration[10] = { 16, 16, 16, 16, 16, 16, 16, 16, 16, 16 };
    /// \param An integer value from 0 - 15 that acts as a label for note sequence
    /// \param An integer value from 1 - 16 indicating the number of notes in the sequence
    /// \param A pointer to a sequence of notes stored as integer values
    /// \param A pointer to a sequence of durations that correspond to the notes
    oi_loadSong(1, 10, notes, duration);

    /// \brief Play song
    /// \param An integer value from 0 - 15 that is a previously establish song index
    oi_play_song(1);

    // while(1){

    //  oi_update(drive_data);
    // lcd_printf("%u, %u", drive_data->cliffLeft, drive_data->cliffFrontLeftSignal);

    //   }
    //  move_forward(drive_data, 100);
    oi_free(drive_data);
    return 0;
}
