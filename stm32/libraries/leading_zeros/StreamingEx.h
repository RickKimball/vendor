#pragma once

/* 
 *  see also:
 *    http://forum.arduino.cc/index.php/topic,131283.0.html
 */

//-----------------------------------------------------------
struct _FILL {
  const char ch;
  const int len;
  
  _FILL(const char c, const int l) : ch(c), len(l) {}
};

inline Print &operator <<(Print &obj, const _FILL &arg) {
  for(int x=0; x < arg.len; ++x) 
    obj.print(arg.ch);

  return obj;
}
 
//-----------------------------------------------------------
struct _HEXZ {
  unsigned value;
  unsigned len;
  _HEXZ(int i, int l = 2): value(i), len(l) {}
};

inline Print &operator <<(Print &obj, const _HEXZ &arg) {
  int cnt=0;

  if ( arg.len >= 8 && arg.value < 0x10000000 ) ++cnt;
  if ( arg.len >= 7 && arg.value < 0x1000000 ) ++cnt;
  if ( arg.len >= 6 && arg.value < 0x100000 ) ++cnt;
  if ( arg.len >= 5 && arg.value < 0x10000 ) ++cnt;
  if ( arg.len >= 4 && arg.value < 0x1000 ) ++cnt;
  if ( arg.len >= 3 && arg.value < 0x100 ) ++cnt;
  if ( arg.len >= 2 && arg.value < 0x10 ) ++cnt;

  for(int x=0; x < cnt; ++x) 
    obj.write('0');

  obj.print(arg.value, HEX);

  return obj;
}

/*
  EXAMPLES:
  Serial << "0x" << _HEXZ(10) << endl; // prints 0x0A
  Serial << "0x" << _HEXZ(10, 2) << endl; // prints 0x0A
  Serial << "0x" << _HEXZ(10, 3) << endl; // prints 0x00A
*/

//-----------------------------------------------------------
struct _DECZ {
  unsigned value;
  unsigned len;
  _DECZ(int i, int l = 2): value(i), len(l) {}
};

inline Print &operator <<(Print &obj, const _DECZ &arg) {
  int cnt=0;
  if ( arg.len >=10 && arg.value < 1000000000 ) ++cnt;
  if ( arg.len >= 9 && arg.value < 100000000 ) ++cnt;
  if ( arg.len >= 8 && arg.value < 10000000 ) ++cnt;
  if ( arg.len >= 7 && arg.value < 1000000 ) ++cnt;
  if ( arg.len >= 6 && arg.value < 100000 ) ++cnt;
  if ( arg.len >= 5 && arg.value < 10000 ) ++cnt;
  if ( arg.len >= 4 && arg.value < 1000 ) ++cnt;
  if ( arg.len >= 3 && arg.value < 100 ) ++cnt;
  if ( arg.len >= 2 && arg.value < 10 ) ++cnt;

  for(int x=0; x < cnt; ++x) 
    obj.write('0');

  obj.print(arg.value, DEC);

  return obj;
}

/*
  EXAMPLES:
  Serial << _DECZ(7) << endl; // prints 07
  Serial << _DECZ(7, 3) << endl; // prints 007
*/
