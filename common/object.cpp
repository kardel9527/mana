#include "object.h"

kmem::ObjectPool<2, 23> kmem::ObjectPoolAllocator::_allocator;
#ifdef ALLOCATE_TRACE
kmem::AllocateRecorder kmem::ObjectPoolAllocator::_recorder;
#endif // ALLOCATE_TRACE

#ifdef ALLOCATE_TRACE
kmem::AllocateRecorder kmem::DefaultAllocator::_recorder;
#endif // #ifdef ALLOCATE_TRACE

