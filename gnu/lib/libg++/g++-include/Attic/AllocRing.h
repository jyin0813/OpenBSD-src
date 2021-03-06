// This may look like C code, but it is really -*- C++ -*-
/* 
Copyright (C) 1989 Free Software Foundation
    written by Doug Lea (dl@rocky.oswego.edu)

This file is part of the GNU C++ Library.  This library is free
software; you can redistribute it and/or modify it under the terms of
the GNU Library General Public License as published by the Free
Software Foundation; either version 2 of the License, or (at your
option) any later version.  This library is distributed in the hope
that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the GNU Library General Public License for more details.
You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the Free Software
Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

	$Id: AllocRing.h,v 1.1 1995/10/18 08:38:14 deraadt Exp etheisen $
*/

#ifndef _AllocRing_h
#ifdef __GNUG__
#pragma interface
#endif
#define _AllocRing_h 1


/*
  An AllocRing holds the last n malloc'ed strings, reallocating/reusing 
  one only when the queue wraps around. It thus guarantees that the
  last n allocations are intact. It is useful for things like I/O
  formatting where reasonable restrictions may be made about the
  number of allowable live allocations before auto-deletion.
*/

class AllocRing
{

  struct AllocQNode
  {
    void*  ptr;
    int    sz;
  };

  AllocQNode* nodes;
  int         n;
  int         current;

  int         find(void* p);

public:

              AllocRing(int max);
             ~AllocRing();

  void*       alloc(int size);
  int         contains(void* ptr);
  void        clear();
  void        free(void* p);
};


#endif
