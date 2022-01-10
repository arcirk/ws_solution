#ifndef WS_CLIENT_GLOBAL_H
#define WS_CLIENT_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(WS_CLIENT_LIBRARY)
#  define WS_CLIENT_EXPORT Q_DECL_EXPORT
#else
#  define WS_CLIENT_EXPORT Q_DECL_IMPORT
#endif

#endif // WS_CLIENT_GLOBAL_H
