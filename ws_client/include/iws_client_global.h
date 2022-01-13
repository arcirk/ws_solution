#ifndef WS_CLIENT_GLOBAL_H
#define WS_CLIENT_GLOBAL_H

#ifdef USE_QT_CREATOR
#include <QtCore/qglobal.h>

#if defined(WS_CLIENT_LIBRARY)
#  define WS_CLIENT_EXPORT Q_DECL_EXPORT
#else
#  define WS_CLIENT_EXPORT Q_DECL_IMPORT
#endif
#else
#  define WS_CLIENT_EXPORT
#endif
//
#endif // WS_CLIENT_GLOBAL_H
