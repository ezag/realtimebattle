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

#ifndef __LIST__
#define __LIST__

#include <iostream>
#include <String.h>

using namespace std;

template<class T>
struct ListNode
{
  T* element;
  ListNode* next;
  ListNode* prev;

  ListNode(T* e = NULL, ListNode* n = NULL, ListNode* p = NULL) 
    : element(e), next(n), prev(p) { }
};

template<class T>
struct ListIterator
{
  ListNode<T>* listp;

  ListIterator(ListNode<T>* p = NULL) : listp(p) {} 
  
  inline const ListIterator<T>& ListIterator<T>::operator++ (int);
  inline const ListIterator<T>& ListIterator<T>::operator-- (int);
  inline T* operator() () const;
  //  bool operator! () const { return listp == NULL; }
  inline bool ok() const { return listp != NULL;}
};



template <class T>
class List
{ 
public:
  List<T>::List(const bool resp=true);
  ~List ();

  const List& operator= (const List&);
  
  void print_list(ostream&) const;
    
  bool is_empty() const { return list_head->next == NULL; }
  bool find(const T*, ListIterator<T>&);
  T* get_nth(const int n) const;
  int  number_of_elements() const;

  void first(ListIterator<T>& li) const { li.listp = list_head->next; };
  void last(ListIterator<T>& li)  const { li.listp = list_head->prev; };
  //  void header(ListIterator<T>& li)  { li.listp = list_head; };


  void remove(ListIterator<T>&);
  bool remove(const T*);

  void insert_first( T* );
  void insert_last( T* );

  void delete_list();

  void set_deletion_responsibility(const bool r) { responsible_for_deletion = r; }
    
private:

  ListNode<T>*  list_head;
  bool responsible_for_deletion;
};

void Error(const bool fatal, const String& error_msg, const String& function_name);

template <class T>
inline T*
ListIterator<T>::operator() () const
{
  if ( listp == NULL ) Error(true, "NULL pointer", "ListIterator::operator()");

  return listp->element;
}

template <class T>
inline const ListIterator<T>&
ListIterator<T>::operator++ (int)
{
  if ( listp == NULL ) Error(true, "NULL pointer", "ListIterator::operator++");
  listp = listp->next;
  return *this;
}

template <class T>
inline const ListIterator<T>&
ListIterator<T>::operator-- (int)
{
  if ( listp == NULL ) Error(true, "NULL pointer", "ListIterator::operator--");
  listp = listp->prev;
  return *this;
}


#endif
