/*
RealTimeBattle, a robot programming game for Unix
Copyright (C) 1998-2000  Erik Ouchterlony and Ragnar Ouchterlony

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation,
Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#ifndef __STRING__
#define __STRING__

#include <iostream>

using namespace std;

enum string_double_t { STRING_NORMAL_FORM, STRING_EXP_FORM, STRING_FIXED_FORM };

class String
{
public:
  String();
  String(const String& str);
  String(const char* str);
  String(const char c);
  String(const int);
  String(const long int);
  String(const double, const int digits = 10, const string_double_t form = STRING_NORMAL_FORM);
  ~String();

  String& operator=(const String&);
  String& operator=(const char);
  String& operator=(const char*);
  friend String operator+(const String&, const String&);
  friend int operator==(const String&, const String&);
  friend int operator!=(const String&, const String&);
  String& operator+=(const String&);

  char operator[](int) const;

   friend ostream& operator<<(ostream&, const String&);
   friend istream& operator>>(istream&, String&);  

//    friend int str2int(const String&);
//    friend long str2long(const String&);
//    friend double str2dbl(const String&);
  friend long str2hex(const String&);
  friend String hex2str(const long);

  String& erase(const int pos, const int size = 1);
  String& insert(const String&, const int pos = 0);
//   int find( const char c, const int start = 0, const bool reverse = false ) const;
  friend String get_segment(const String& str, const int start, const int end);
//   friend String make_lower_case(const String& str);

  int get_length() const { return length; }
  bool is_empty() const { return length == 0; }
  const char* chars() const { return array; }

   //  Warning! This allows for changing the private member array.
  char* non_const_chars() const { return array; }

  // allocates an array of chars, remember to delete afterwards!
  char* copy_chars(); 

private:
  char* array;
  int length;
  int buf_size;

  //  String& copy(const String&);
  void allocate_array(const int size);
  void enlarge_array(const int size);

  class Range {};
};

#endif
