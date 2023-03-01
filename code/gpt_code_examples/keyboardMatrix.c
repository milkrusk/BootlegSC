#include <bcm2835.h>
#include <stdio.h>
#include <stdint.h>

// Define the pin numbers for the rows and columns of the keyboard matrix
#define ROW1_PIN RPI_GPIO_P1_11
#define ROW2_PIN RPI_GPIO_P1_12
#define ROW3_PIN RPI_GPIO_P1_13
#define COL1_PIN RPI_GPIO_P1_15
#define COL2_PIN RPI_GPIO_P1_16

// Define the key codes for the keys on the keyboard matrix
#define KEY_1 1
#define KEY_2 2
#define KEY_3 3
#define KEY_4 4
#define KEY_5 5
#define KEY_6 6

// Define the delay time between row and column scans
#define DELAY_TIME_US 10

// Define a lookup table for the key codes based on the row and column indices
uint8_t key_lookup[3][2] = {
    {KEY_1, KEY_2},
    {KEY_3, KEY_4},
    {KEY_5, KEY_6}
};

// Initialize the GPIO pins for the keyboard matrix
void init_keyboard_matrix() {
    bcm2835_gpio_fsel(ROW1_PIN, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(ROW2_PIN, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(ROW3_PIN, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(COL1_PIN, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_fsel(COL2_PIN, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_set_pud(COL1_PIN, BCM2835_GPIO_PUD_UP);
    bcm2835_gpio_set_pud(COL2_PIN, BCM2835_GPIO_PUD_UP);
}

// Read the state of the keyboard matrix and return the pressed key, if any
uint8_t read_keyboard_matrix() {
    // Set each row to low and check the state of each column
    for (int i = 0; i < 3; i++) {
        // Set the current row to low and the other rows to high
        bcm2835_gpio_write(ROW1_PIN, i == 0 ? LOW : HIGH);
        bcm2835_gpio_write(ROW2_PIN, i == 1 ? LOW : HIGH);
        bcm2835_gpio_write(ROW3_PIN, i == 2 ? LOW : HIGH);

        // Wait for a short time to allow the signal to stabilize
        bcm2835_delayMicroseconds(DELAY_TIME_US);

        // Check the state of each column
        if (bcm2835_gpio_lev(COL1_PIN) == LOW) {
            return key_lookup[i][0];
        }
        if (bcm2835_gpio_lev(COL2_PIN) == LOW) {
            return key_lookup[i][1];
        }
    }

    // No key was pressed
    return 0;
}

int main() {
    // Initialize the BCM2835 library
    if (!bcm2835_init()) {
        fprintf(stderr, "Failed to initialize BCM2835 library\n");
        return 1;
    }

    // Initialize the GPIO pins for the keyboard matrix
    init_keyboard_matrix();

    // Main loop
    while (1) {
        // Read the state of the keyboard matrix
        uint8_t key = read_keyboard_matrix();

        // If a key was pressed, print the key code to the console
    if (key > 0) {
        printf("Key %d was pressed\n", key);
    }

    // Wait for a short time before scanning the keyboard matrix again
    bcm2835_delayMicroseconds(DELAY_TIME_US);
}

// Close the BCM2835 library
bcm2835_close();

return 0;
}