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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>
#include <iostream>
//#include <strstream>
#include <iomanip>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>

using namespace std;

#include "IntlDefs.h"
#include "String.h"


void Error(const bool fatal, const String& function_name, const String& error_msg);

String::String()
{
  buf_size = 0;
  length = 0;
  allocate_array(0);
  array[0] = '\0';
}

String::String(const String& str)
{
  buf_size = 0;
  *this = str;
}

String::String(const char* str)
{
  buf_size = 0;
  *this = str;
}

String::String(const char c)
{
  buf_size = 0;
  *this = c;
}

String::String(const int n)
{
  //  *this = String((long)n);
  buf_size = 0;
  length = 15;
  allocate_array(15);
  array[15] = '\0';
  int pos = 14;

  int k = ( n>0 ? n : -n );

  if( k == 0 ) 
    array[pos--] = '0';
  else 
    for(; k > 0; k/=10) array[pos--] = '0' + k%10;

  if( n < 0 ) 
    {
      array[0] = '-';
      erase(1, pos);
    }
  else
    erase(0, pos+1);
}

String::String(const long int n)
{
  buf_size = 0;
  length = 15;
  allocate_array(15);
  array[15] = '\0';
  int pos = 14;

  int k = ( n>0 ? n : -n );

  if( k == 0 ) 
    array[pos--] = '0';
  else 
    for(; k > 0; k/=10) array[pos--] = '0' + k%10;

  if( n < 0 ) 
    {
      array[0] = '-';
      erase(1, pos);
    }
  else
    erase(0, pos+1);
}

String::String(const double x, const int digits, const string_double_t form)
{
  if( form == STRING_NORMAL_FORM ) {
	// ss << setprecision(digits) << x << endl;
	// Problems with internationalisation
	// Fall back to good old shitty sprintf() 
	  
	buf_size = 0;
    allocate_array(15);
	  
	length=sprintf(array , "%f", x);
	
  } 
  else {
	/*buf_size = 0;
  	strstream ss;  
    ss << (form == STRING_EXP_FORM ? setiosflags(ios::scientific) : setiosflags(ios::fixed)) 
       << setprecision(digits) << x << endl;
    ss >> *this;*/
	buf_size = 0;
 	length = 15;
    allocate_array(15);
	  
	length=sprintf(array , "%e", x);
	  
  }
}

String::~String()
{
  delete [] array;
}

void
String::allocate_array(const int size)
{
  if( buf_size > 0 ) delete [] array;
  buf_size = (size / 8 + 1) * 8;
  array = new char[buf_size];
}

void
String::enlarge_array(const int size)
{
  if( buf_size <= size )
    {
      char* temp = array;
      buf_size = (size / 8 + 1) * 8;
      array = new char[buf_size];
      strcpy(array, temp);
      delete [] temp;
    }
}

String&
String::operator=(const String& str)
{
  length = str.length;
  allocate_array(length);
  strcpy(array, str.array);
  return *this;
}

String&
String::operator=(const char* str)
{
  length = strlen(str);
  allocate_array(length);
  strcpy(array, str);
  return *this;
}

String&
String::operator=(const char c)
{
  length = 1;
  allocate_array(length);
  array[0] = c;
  array[1] = '\0';
  return *this;
}

String&
String::operator+=(const String& str)
{
  int newlength = length + str.length;
  enlarge_array(newlength);
  strcpy(&array[length], str.array);
  length = newlength;

  return *this;
}

String
operator+(const String& str1, const String& str2)
{
  String sum(str1);
  sum += str2;

  return sum;
}

int
operator==(const String& str1, const String& str2)
{
  return strcmp(str1.array, str2.array) == 0;
}

int
operator!=(const String& str1, const String& str2)
{
  return strcmp(str1.array, str2.array) != 0;
}

char
String::operator[](int pos) const
{
  if( pos < 0 || pos > length ) Error(true, "String out of range", "String::operator[]");

  return array[pos];
}

String&
String::erase(const int pos, const int size)
{
  if( pos < 0 || pos + size > length || size <= 0 ) Error(true, "String out of range", "String::erase");

  strcpy(&array[pos], &array[pos+size]); // ???
  length -= size;
  
  return *this;
}

String&
String::insert(const String& str, const int pos)
{
  if( pos < 0 || pos > length ) Error(true, "String out of range", "String::insert");

  char temp[length - pos + 1];
  strcpy(temp, &array[pos]);
      
  length += str.length;
  enlarge_array(length);
      
  strcpy(&array[pos], str.array);
  strcpy(&array[pos+str.length], temp);
      
  return *this;
}

// Returns -1 if char was not found in string
// If char was found => returns its position
// If reverse: start also counts from the end
int
String::find( const char c, const int start, const bool reverse ) const
{
  char * ans;

  int st = ( start < 0 ? length + start : start );
  if( st < 0 || st >= length ) return -1;

  if(NULL != ( reverse ? (ans = strrchr(array + st, c)) : (ans = strchr(array+st, c))))
    return (int)(ans-array);

  return -1;
}

//   if start or end negativ, count from the end. -1 == the last.
// start = 0 and end = -1 => return the string as it is
String
get_segment(const String& str, const int start, const int end)
{
  String segment;
  int st = ( start < 0 ? str.length + start : start );
  int en = ( end < 0 ? str.length + end : end );
  if( st < 0 || st > str.length || en < 0 || en > str.length || st > en + 1 ) 
    Error(true, "String out of range", "String::get_segment");

  if( st == en + 1 ) return segment;

  segment.length = en - st + 1;
  segment.enlarge_array(segment.length);
  strncpy( segment.array, &str.array[st], segment.length ); 
  segment.array[segment.length] = '\0';

  return segment;
}

String
make_lower_case(const String& str)
{
  String lower_str(str);
  for(int i = 0;i<lower_str.length;i++)
    lower_str.array[i] = tolower(lower_str.array[i]);

  return lower_str;
}
int
str2int(const String& str)
{
  return atoi(str.array);
}

long
str2long(const String& str)
{
  return atol(str.array);
}

double
str2dbl(const String& str)
{
  return atof(str.array);
}

long
str2hex(const String& str)
{
  int pos = 0;
  long sign = 1;
  char c;
  long number = 0;

  if( str.array[pos] == '-' )
    {
      pos++;
      sign = -1;
    }

  for( ; pos < str.length; pos++)
    {
      c = str.array[pos];
      if( c >= '0' && c <= '9' ) 
        number=number*16+(c-'0');
      else if( c >='a' && c <= 'f' ) 
        number=number*16+10+(c-'a');
      else if( c >='A' && c <= 'F' ) 
        number=number*16+10+(c-'A');
      else break;
    }

  return sign*number;
}

String
hex2str(long n)
{
  String str;
  str.length = 15;
  str.allocate_array(15);
  str.array[15] = '\0';
  int pos = 14;

  int k = ( n>0 ? n : -n );

  if( k == 0 ) 
    str.array[pos--] = '0';
  else 
    for(; k > 0; k/=16) 
      {
        if( k%16 < 10 ) 
          str.array[pos--] = '0' + k%16;
        else
          str.array[pos--] = 'A' - 10 + k%16;
      }

  if( n < 0 ) 
    {
      str.array[0] = '-';
      str.erase(1, pos);
    }
  else
    str.erase(0, pos+1);  

  return str;
}

char*
String::copy_chars()
{ 
  char* str = new char[length+1];
  
  strcpy(str, array);
  
  return str;
}

istream&
operator>>(istream& is, String& str)
{
  char buf[500];
  is.get(buf, 500, '\n');

  str = buf;

  return( is );
}

ostream&
operator<<(ostream& os, const String& str)
{
  os << str.array;
  return( os );
}
