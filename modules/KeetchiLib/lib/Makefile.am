# Use this line to create an library which is not copies to a system dir when
# executing make install`:
#noinst_LTLIBRARIES = libkeetchi.la

# Install lib in corresponding directory when performing `make install`
lib_LTLIBRARIES = libkeetchi.la

# internal cpp and h files, no public interface
libkeetchi_la_SOURCES = \
						KLAction.cpp KLCommMgr.cpp KLFeedbackMsg.cpp\
						KLResourceMgr.cpp KLAppInfo.cpp KLDataMgr.cpp\
						KLKeetchi.cpp KLCacheEntry.cpp  KLDataMsg.cpp\
						KLNodeInfo.cpp\
						KLAction.h KLCommMgr.h KLFeedbackMsg.h\
						KLResourceMgr.h KLAppInfo.h KLDataMgr.h\
						KLKeetchi.h  KLTypes.h KLCacheEntry.h \
						KLDataMsg.h  KLNodeInfo.h

# Maybe include math lib (in addition to include <math.h>)
#libtest_la_LIBADD = -lm

# public interfaces
include_HEADERS = KLKeetchi.h

