#pragma once

void Display_Init(void);
__attribute((format(printf, 1, 2))) void Display_Write(const char *fmt, ...);
void Display_Clear(void);