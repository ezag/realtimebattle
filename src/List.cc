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

#include "List.h"

#include <iostream>
#include <string>
using namespace std;

void Error(const bool fatal, const string& error_msg, const string& function_name);

template <class T>
List<T>::List(bool resp)
  : list_head(new ListNode<T>), responsible_for_deletion(resp)
{
}

template <class T>
List<T>::~List() 
{
  delete_list();     // delete the actual list
  delete list_head;  // delet the header node
}

template <class T>
void
List<T>::delete_list()
{
  ListNode<T>*  p = list_head->next;  // keep the header node
  ListNode<T>*  temp;  

  while ( p != NULL )
    {
      temp = p->next;
      if( responsible_for_deletion && p->element ) delete p->element;
      delete p;
      p = temp;
    }
  
  list_head->next = NULL;
  list_head->prev = NULL;
}

//
// List Member Operators
//

template <class T>
const List<T>&
List<T>::operator= (const List<T>& old)
{
  if( this == &old ) return *this; // don't copy yourself!
  
  if( list_head != NULL )
    delete_list();  // delete all nodes but the header node. 
  else
    list_head = new ListNode<T>;


  ListNode<T>* p;

  for( p=old.list_head->next; p != NULL; p=p->next )
    insert_last(p->element);

  return *this;
}





template <class T>
T*
List<T>::get_nth(const int n) const
{
  ListNode<T>*  p;
  int index = 1;

  if( n < 1 ) Error(true, "n < 1", "List::get_nth");

  for( p=list_head->next; p != NULL && index < n; p=p->next ) index++;
    
  if( p == NULL ) Error(true, "No such element", "List::get_nth");

  return p->element;
}


template <class T>
void
List<T>::print_list(ostream& os) const
{
  ListNode<T>*  p;

  for( p=list_head->next; p != NULL; p=p->next );
    //    os << *(p->element) << " ";
}


//
// member functions
//

template <class T>
bool
List<T>::find(const T* x, ListIterator<T>& li)
{
  for( first(li); li.ok(); li++ )
    {
      if ( li() == x )
        {
          return true;
        }
    }

  return false; 
}


template <class T>
int
List<T>::number_of_elements() const
{
  ListNode<T>* p;
  int index=0;

  for( p=list_head->next; p != NULL; p=p->next ) index++;

  return(index);
}

// Removes element 
// current_pos becomes the previous element
template <class T>
void
List<T>::remove(ListIterator<T>& li)
{
  if( li.listp == NULL ) Error(true, "Nothing to remove", "List::remove");

  ListNode<T>* current_pos = li.listp;

  if( current_pos->prev )
    {
      current_pos->prev->next = current_pos->next;
      li.listp = current_pos->prev;
    }
  else
    {
      list_head->next = current_pos->next;
      li.listp = list_head;
    }

  if( current_pos->next )
    current_pos->next->prev = current_pos->prev;
  else
    list_head->prev = current_pos->prev;
  
  if( responsible_for_deletion && current_pos->element ) delete current_pos->element;
  delete current_pos;
}


template <class T>
bool
List<T>::remove(const T* x)
{
  ListIterator<T> li;
  if ( find(x, li) )
    {
      remove(li);
      return true;
    }
  
  return false;
}


template <class T>
void
List<T>::insert_first(T* x)
{
  ListNode<T>*  p = new ListNode<T>(x, list_head->next, NULL);

  if( p == NULL ) Error(true, "Out of memory", "List::insert_first");

  if( list_head->next == NULL ) list_head->prev = p;
  if( p->next ) p->next->prev = p;
  list_head->next = p;
}

template <class T>
void
List<T>::insert_last(T* x)
{
  ListNode<T>*  p = new ListNode<T>(x, NULL, list_head->prev);

  if( p == NULL ) Error(true, "Out of memory", "List::insert_last");

  if( list_head->prev == NULL ) list_head->next = p;
  if( p->prev ) p->prev->next = p;
  list_head->prev = p;
}


template class List<string>;

#include "Vector2D.h"
template class List<Vector2D>;

#include "Shape.h"
template class List<Shape>;

#include "Robot.h"
template class List<Robot>;

#include "Structs.h"
template class List<stat_t>;
template class List<start_tournament_info_t>;

#include "ArenaReplay.h"
template class List<ArenaReplay::object_pos_info_t>;
