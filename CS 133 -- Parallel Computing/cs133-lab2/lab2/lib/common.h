#pragma once

#include <cstdlib>
#ifdef __APPLE__
#include <boost/align/aligned_alloc.hpp>
#endif
namespace lab2 {
#ifdef __APPLE__
using boost::alignment::aligned_alloc;
#else
using ::aligned_alloc;
#endif
}
