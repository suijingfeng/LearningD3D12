#pragma once

void Sys_CreateConsole(void);
void Sys_DestroyConsole(void);

char* Sys_ConsoleInput(void);
void Sys_Print(const char * pMsg);
void Sys_ShowConsole(int level, qboolean quitOnClose);
void Sys_SetErrorText(const char * pText);

// the system console is shown when a dedicated server is running
// void	Sys_DisplaySystemConsole(qboolean show);