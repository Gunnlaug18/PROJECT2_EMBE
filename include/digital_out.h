#include <stdint.h>

class Digital_out {
    public: 
        Digital_out(uint8_t pin_number);
        void init();
        void set_hi();
        void set_lo();
        void toggle();
    private:
        uint8_t pinMask;
};