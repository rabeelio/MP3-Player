#ifndef IO_DISPLAY_HPP_
#define IO_DISPLAY_HPP_

void LCD_init(void);
void send_data(char object_ID, char object_index, int data);
void send_bearing_to_LCD(uint32_t bearing_value, bool gps_locked);
void send_static_text(char object_index, char * string);
void read_slider(void);

#endif /* IO_DISPLAY_HPP */