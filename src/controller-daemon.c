// C controller daemon entry point

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <stdint.h>

// Button bit positions
enum
{
    BTN_A_BIT = 0,
    BTN_B_BIT = 1,
    BTN_X_BIT = 2,
    BTN_Y_BIT = 3,
    BTN_LB_BIT = 4,
    BTN_RB_BIT = 5,
    BTN_SELECT_BIT = 6,
    BTN_START_BIT = 7,
    BTN_LS_BIT = 8,
    BTN_RS_BIT = 9,
    BTN_DPAD_UP_BIT = 10,
    BTN_DPAD_DOWN_BIT = 11,
    BTN_DPAD_LEFT_BIT = 12,
    BTN_DPAD_RIGHT_BIT = 13
};

// Gamepad state struct
struct gamepad_state
{
    uint16_t buttons; // bitmask for A/B/X/Y/etc.
    int8_t lx;        // left stick X
    int8_t ly;        // left stick Y
    int8_t rx;        // right stick X
    int8_t ry;        // right stick Y
    uint8_t lt;       // left trigger
    uint8_t rt;       // right trigger
};

struct hid_report
{
    uint16_t buttons; // 16 bits of buttons
    int8_t lx;        // left stick X
    int8_t ly;        // left stick Y
    int8_t rx;        // right stick X
    int8_t ry;        // right stick Y
    uint8_t lt;       // left trigger
    uint8_t rt;       // right trigger
};

void build_hid_report(struct hid_report *r, struct gamepad_state *s)
{
    r->buttons = s->buttons;
    r->lx = s->lx;
    r->ly = s->ly;
    r->rx = s->rx;
    r->ry = s->ry;
    r->lt = s->lt;
    r->rt = s->rt;
}

void write_hid_report(int hid_fd, struct hid_report *r)
{
    ssize_t n = write(hid_fd, r, sizeof(*r));
    if (n < 0)
    {
        perror("Failed to write HID report");
    }
}

// Deadzone setup
int8_t apply_deadzone(int8_t v, int8_t dz)
{
    if (v > -dz && v < dz)
        return 0;
    return v;
}

int main(void)
{
    const char *input_path = "/dev/input/event0";
    const char *hid_path = "/dev/hidg0";

    // Open input device
    int input_fd = open(input_path, O_RDONLY);
    if (input_fd < 0)
    {
        perror("Failed to open input device");
        return 1;
    }

    // Open HID gadget device
    int hid_fd = open(hid_path, O_WRONLY);
    if (hid_fd < 0)
    {
        perror("Failed to open HID device");
        close(input_fd);
        return 1;
    }

    printf("Controller daemon started.\n");
    printf("Reading from %s, writing to %s\n", input_path, hid_path);

    struct gamepad_state state = {0};

    // Timer for reboot
    uint64_t get_time_ms(void)
    {
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        return (uint64_t)ts.tv_sec * 1000ULL + ts.tv_nsec / 1000000ULL;
    }

    // Main loop

    while (1)
    {
        struct input_event ev;

        ssize_t n = read(input_fd, &ev, sizeof(ev));
        if (n == sizeof(ev))
        {
            // 1. Update gamepad_state from event

            if (ev.type == EV_KEY)
            {
                // Buttons
                switch (ev.code)
                {
                case BTN_A:
                    ev.value ? (state.buttons |= (1 << BTN_A_BIT))
                             : (state.buttons &= ~(1 << BTN_A_BIT));
                    break;

                case BTN_B:
                    ev.value ? (state.buttons |= (1 << BTN_B_BIT))
                             : (state.buttons &= ~(1 << BTN_B_BIT));
                    break;

                case BTN_X:
                    ev.value ? (state.buttons |= (1 << BTN_X_BIT))
                             : (state.buttons &= ~(1 << BTN_X_BIT));
                    break;

                case BTN_Y:
                    ev.value ? (state.buttons |= (1 << BTN_Y_BIT))
                             : (state.buttons &= ~(1 << BTN_Y_BIT));
                    break;

                case BTN_TL: // LB
                    ev.value ? (state.buttons |= (1 << BTN_LB_BIT))
                             : (state.buttons &= ~(1 << BTN_LB_BIT));
                    break;

                case BTN_TR: // RB
                    ev.value ? (state.buttons |= (1 << BTN_RB_BIT))
                             : (state.buttons &= ~(1 << BTN_RB_BIT));
                    break;

                case BTN_SELECT:
                    ev.value ? (state.buttons |= (1 << BTN_SELECT_BIT))
                             : (state.buttons &= ~(1 << BTN_SELECT_BIT));
                    break;

                case BTN_START:
                    ev.value ? (state.buttons |= (1 << BTN_START_BIT))
                             : (state.buttons &= ~(1 << BTN_START_BIT));
                    break;

                case BTN_THUMBL: // Left stick click
                    ev.value ? (state.buttons |= (1 << BTN_LS_BIT))
                             : (state.buttons &= ~(1 << BTN_LS_BIT));
                    break;

                case BTN_THUMBR: // Right stick click
                    ev.value ? (state.buttons |= (1 << BTN_RS_BIT))
                             : (state.buttons &= ~(1 << BTN_RS_BIT));
                    break;

                case BTN_DPAD_UP:
                    ev.value ? (state.buttons |= (1 << BTN_DPAD_UP_BIT))
                             : (state.buttons &= ~(1 << BTN_DPAD_UP_BIT));
                    break;

                case BTN_DPAD_DOWN:
                    ev.value ? (state.buttons |= (1 << BTN_DPAD_DOWN_BIT))
                             : (state.buttons &= ~(1 << BTN_DPAD_DOWN_BIT));
                    break;

                case BTN_DPAD_LEFT:
                    ev.value ? (state.buttons |= (1 << BTN_DPAD_LEFT_BIT))
                             : (state.buttons &= ~(1 << BTN_DPAD_LEFT_BIT));
                    break;

                case BTN_DPAD_RIGHT:
                    ev.value ? (state.buttons |= (1 << BTN_DPAD_RIGHT_BIT))
                             : (state.buttons &= ~(1 << BTN_DPAD_RIGHT_BIT));
                    break;
                }
            }

            // Not buttons
            if (ev.type == EV_ABS)
            {
                switch (ev.code)
                {

                case ABS_X: // Left stick X
                    state.lx = apply_deadzone((int8_t)(ev.value >> 8), 10);
                    break;

                case ABS_Y: // Left stick Y
                    state.ly = apply_deadzone((int8_t)(ev.value >> 8), 10);
                    break;

                case ABS_RX: // Right stick X
                    state.rx = apply_deadzone((int8_t)(ev.value >> 8), 10);
                    break;

                case ABS_RY: // Right stick Y
                    state.ry = apply_deadzone((int8_t)(ev.value >> 8), 10);
                    break;

                case ABS_Z: // Left trigger (0–255)
                    state.lt = (uint8_t)ev.value;
                    break;

                case ABS_RZ: // Right trigger (0–255)
                    state.rt = (uint8_t)ev.value;
                    break;
                }
            }

            // ABXY reboot gesture
            static uint64_t abxy_start = 0;
            uint64_t now = get_time_ms();

            // A=bit0, B=bit1, X=bit2, Y=bit3 → lower 4 bits = 0x0F
            bool abxy_pressed = (state.buttons & 0x000F) == 0x000F;

            if (abxy_pressed)
            {
                if (abxy_start == 0)
                    abxy_start = now;

                if (now - abxy_start > 1000)
                {
                    system("reboot");
                }
            }
            else
            {
                abxy_start = 0;
            }

            // 2. Build HID report
            struct hid_report report;
            build_hid_report(&report, &state);

            // 3. Send HID report to phone/computer
            write_hid_report(hid_fd, &report);
        }
    }

    // Cleanup (we'll probably never reach this in practice)
    close(input_fd);
    close(hid_fd);

    return 0;
}