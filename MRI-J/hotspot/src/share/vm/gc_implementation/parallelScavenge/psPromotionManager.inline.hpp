/*
 * Copyright 2002-2006 Sun Microsystems, Inc.  All Rights Reserved.
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
#ifndef PSPROMOTIONMANAGER_INLINE_HPP
#define PSPROMOTIONMANAGER_INLINE_HPP


#include "objectRef_pd.hpp"
#include "psScavenge.hpp"

inline PSPromotionManager* PSPromotionManager::manager_array(int index) {
  assert(_manager_array != NULL, "access of NULL manager_array");
  assert(index >= 0 && index <= (int)ParallelGCThreads, "out of range manager_array access");
  return _manager_array[index];
}

inline void PSPromotionManager::claim_or_forward_internal_depth(heapRef* p) {
  if (p != NULL) {
objectRef pref=ALWAYS_UNPOISON_OBJECTREF(*p);
    oop o = pref.as_oop();
    if (o->is_forwarded()) {
      objectRef ref = o->forwarded_ref();
      // Card mark
if(PSScavenge::is_obj_in_young((HeapWord*)ref.as_oop())){
PSScavenge::card_table()->inline_write_ref_field_gc(p,ref.as_oop());
      }
      POISON_AND_STORE_REF(p,ref);

    } else {
      if (!claimed_stack_depth()->push(p)) {
	overflow_stack_depth()->push(p);
      }
    }
  }
}

inline void PSPromotionManager::claim_or_forward_internal_breadth(heapRef* p) {
  if (p != NULL) {
objectRef pref=ALWAYS_UNPOISON_OBJECTREF(*p);
    oop o = pref.as_oop();
    heapRef fref = o->is_forwarded()
      ? (heapRef)o->forwarded_ref()
      : copy_to_survivor_space(o, false, p);
    
    // Card mark
if(fref.is_new()){
PSScavenge::card_table()->inline_write_ref_field_gc(p,fref.as_oop());
    }
    POISON_AND_STORE_REF(p,fref);
  }
}

inline void PSPromotionManager::flush_prefetch_queue() {
  assert(!depth_first(), "invariant");
  for (int i=0; i<_prefetch_queue.length(); i++) {
    claim_or_forward_internal_breadth(_prefetch_queue.pop());
  }
}

inline void PSPromotionManager::claim_or_forward_depth(heapRef* p) {
  assert(depth_first(), "invariant");
assert(PSScavenge::should_scavenge(ALWAYS_UNPOISON_OBJECTREF(*p),true),"revisiting object?");
  assert(Universe::heap()->kind() == CollectedHeap::ParallelScavengeHeap, "Sanity");
  assert(Universe::heap()->is_in(p), "pointer outside heap");

  claim_or_forward_internal_depth(p);
}

inline void PSPromotionManager::claim_or_forward_breadth(heapRef* p) {
  assert(!depth_first(), "invariant");
assert(PSScavenge::should_scavenge(ALWAYS_UNPOISON_OBJECTREF(*p),true),"revisiting object?");
  assert(Universe::heap()->kind() == CollectedHeap::ParallelScavengeHeap, "Sanity");
  assert(Universe::heap()->is_in(p), "pointer outside heap");

  if (UsePrefetchQueue) {
    claim_or_forward_internal_breadth(_prefetch_queue.push_and_pop(p));
  } else {
    // This option is used for testing.  The use of the prefetch
    // queue can delay the processing of the objects and thus
    // change the order of object scans.  For example, remembered
    // set updates are typically the clearing of the remembered
    // set (the cards) followed by updates of the remembered set
    // for young-to-old pointers.  In a situation where there
    // is an error in the sequence of clearing and updating
    // (e.g. clear card A, update card A, erroneously clear
    // card A again) the error can be obscured by a delay
    // in the update due to the use of the prefetch queue
    // (e.g., clear card A, erroneously clear card A again,
    // update card A that was pushed into the prefetch queue
    // and thus delayed until after the erronous clear).  The
    // length of the delay is random depending on the objects
    // in the queue and the delay can be zero.
    claim_or_forward_internal_breadth(p);
  }
}

inline void PSPromotionManager::process_popped_location_depth(heapRef* p) {
  /*
   * TODO: we dont have array chunking support yet
  if (is_oop_masked(p)) {
    assert(PSChunkLargeArrays, "invariant");
    oop const old = unmask_chunked_array_oop(p);
    process_array_chunk(old);
  } else {
    PSScavenge::copy_and_push_safe_barrier(this, p); 
  }
    */
  PSScavenge::copy_and_push_safe_barrier(this, p); 
}

#endif //  PSPROMOTIONMANAGER_INLINE_HPP
