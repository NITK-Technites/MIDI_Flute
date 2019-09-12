#define on 0x90
#define off 0x80

void setup() {
  DDRD &= 0b00011111;     // Set pins 5-7 on arduino (port D 5 to 7) as inputs
  DDRB &= 0b11100000;     // Set pins 8-12 on arduino (Port B 0 to 4) as inputs
  PORTD |= 0b11100000;    // Pull up activate on pin 5-7 (port D 5 to 7)
  PORTB |= 0b00011111;    // Pull up activate on pin 8-12 (port B0 to 4) 
                                     // desired baud rate:9600
                                     // actual baud rate:9600 (error 0.0%)
                                     // char size: 8 bit
                                     // parity: Disabled
  UCSR0B = 0x00;            // disable while setting baud rate
  UCSR0A = 0x00;
  UCSR0C = 0x06;
  UBRR0L = 0x67;            // set baud rate low (Refer datasheet)
  UBRR0H = 0x00;            // set baud rate high
  UCSR0B = 0x08;            // Tx/Rx complete interrupt disabled, Buffer empty interrupt disable
                                    // Receiver Disabled (We'll be only transmitting)
}
byte on_flag_1 = 0xFF;       // bit 7-2 -> port D 7 to 2 ( on_flag_1 = D7 D6 D5 B4 B3 B2 B1 B0 ) 
byte now_1 = 0xFF;           // bit 7-2 -> port D 7 to 2 ( now_ 1 = D7 D6 D5 B4 B3 B2 B1 B0 )
byte diff_1 = 0;          // If any bit in this reg is 1 it means note corresponding to that bit is toggled.
byte notes[] = {0x58,0x56,0x54,0x60,0x5F,0x5D,0X5B,0x59}; // Array to store notes corresponding to 8 keys (C4 to C5).
byte index = 0;           
byte mask = 0b10000000;
void loop() {
  now_1 = (PIND&0b11100000)|(PINB&0b00011111);           // now_1 = D7 D6 D5 B4 B3 B2 B1 B0)
  diff_1 = now_1^on_flag_1;                                            // Xor finds if last time and this time there is any bit change
  if(diff_1)  // If diff_1 is not zero (bit change has occured)
  {
    
    // This block gives a note on/off message appropriately.
    index = 0; 
    mask = 0b10000000;
    while((diff_1<<index)!= 0)
    {
      if(mask&diff_1)
      {
        if(mask&now_1)
          midi_send(off,notes[index],0x7F);
        else
          midi_send(on,notes[index],0x7F);
      }
      index++;
      mask = mask>>1;
    }
    
  }

  if(now_1!=0xFF)
  {
    midi_send( 0xB0 , 0x05 , map(analogRead(A0),0,1023,0,127));
    
  }
  on_flag_1 = now_1; // Update status of on_flag_1 to current input
}

void midi_send(byte d1, byte d2, byte d3) // Function to send MIDI Message
{
  while((UCSR0A&0b00100000)==0);  // Wait for transmit buffer to be empty
  UDR0 = d1;                    // Send byte1
  while((UCSR0A&0b00100000)==0);  // Wait for transmit buffer to be empty
  UDR0 = d2;                    // Send byte2
  while((UCSR0A&0b00100000)==0);  // Wait for transmit buffer to be empty
  UDR0 = d3;                    // Send byte3
}
