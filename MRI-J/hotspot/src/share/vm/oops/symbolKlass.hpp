/*
 * Copyright 1997-2006 Sun Microsystems, Inc.  All Rights Reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Sun Microsystems, Inc., 4150 Network Circle, Santa Clara,
 * CA 95054 USA or visit www.sun.com if you need additional information or
 * have any questions.
 *  
 */
// This file is a derivative work resulting from (and including) modifications
// made by Azul Systems, Inc.  The date of such changes is 2010.
// Copyright 2010 Azul Systems, Inc.  All Rights Reserved.
//
// Please contact Azul Systems, Inc., 1600 Plymouth Street, Mountain View, 
// CA 94043 USA, or visit www.azulsystems.com if you need additional information 
// or have any questions.
#ifndef SYMBOLKLASS_HPP
#define SYMBOLKLASS_HPP

#include "klass.hpp"

// a symbolKlass is the klass for a symbolOop

class symbolKlass : public Klass {
 private:
  juint    _alloc_size;        // allocation profiling support
 public:
  // Allocation
  DEFINE_ALLOCATE_PERMANENT(symbolKlass);
  static klassOop create_klass(TRAPS);
  symbolOop allocate_symbol(u1* name, int len, TRAPS);   // Assumes no characters larger than 0x7F
  bool allocate_symbols(int names_count, const char** names, int* lengths, symbolOop* sym_oops, TRAPS);

  // Test operation
  bool oop_is_symbol() const { return true; }

  // Casting from klassOop
  static symbolKlass* cast(klassOop k) {
    assert(k->klass_part()->oop_is_symbol(), "cast to symbolKlass");
    return (symbolKlass*) k->klass_part(); 
  }

  static int header_size()       { return oopDesc::header_size() + sizeof(symbolKlass)/HeapWordSize; }
  int oop_size(oop obj) const;
int GC_oop_size(oop obj)const;
  int klass_oop_size() const     { return object_size(); }
int object_size()const{return header_size();}

    // Garbage collection
  void oop_follow_contents(oop obj);
  int  oop_adjust_pointers(oop obj);
  bool oop_is_parsable(oop obj) const;

  // Parallel Scavenge and Parallel Old
  PARALLEL_GC_DECLS

  // GenPauselessGC
  void GPGC_oop_follow_contents(GPGC_GCManagerNewStrong* gcm, oop obj);
  void GPGC_oop_follow_contents(GPGC_GCManagerNewFinal* gcm, oop obj);
  void GPGC_oop_follow_contents(GPGC_GCManagerOldStrong* gcm, oop obj);
  void GPGC_oop_follow_contents(GPGC_GCManagerOldFinal* gcm, oop obj);
void GPGC_newgc_oop_update_cardmark(oop obj);
void GPGC_oldgc_oop_update_cardmark(oop obj);
void GPGC_mutator_oop_update_cardmark(oop obj);
			   
  // Allocation profiling support
  juint alloc_size() const              { return _alloc_size; }
  void set_alloc_size(juint n)          { _alloc_size = n; }

  // Iterators
  int  oop_oop_iterate(oop obj, OopClosure* blk);
  int  oop_oop_iterate_m(oop obj, OopClosure* blk, MemRegion mr);

#ifndef PRODUCT
  // Printing
  void oop_print_value_on(oop obj, outputStream* st);
  void oop_print_on(oop obj, outputStream* st);
#endif
  const char* internal_name() const;
};

#endif // SYMBOLKLASS_HPP

