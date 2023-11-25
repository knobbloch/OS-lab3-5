#pragma once

//int FileMappingSave(int cell, int width, int lenght, int rbackground, int gbackground, int bbackground,
  //  int rgrid, int ggrid, int bgrid);

int SaveFile(std::string str, int typeofsave);
int* ReadFile(int typeofsave);
int CloseBuf();
int Init(UINT adr);
int RewiteBuf(std::string str);
extern UINT msgReg;
extern char* pBufExchange;
