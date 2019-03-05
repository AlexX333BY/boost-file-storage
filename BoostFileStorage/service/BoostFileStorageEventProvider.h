 // categories.
//
//  Values are 32 bit values laid out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-+-----------------------+-------------------------------+
//  |Sev|C|R|     Facility          |               Code            |
//  +---+-+-+-----------------------+-------------------------------+
//
//  where
//
//      Sev - is the severity code
//
//          00 - Success
//          01 - Informational
//          10 - Warning
//          11 - Error
//
//      C - is the Customer code flag
//
//      R - is a reserved bit
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//
//
// Define the facility codes
//


//
// Define the severity codes
//


//
// MessageId: DEFAULT_CATEGORY
//
// MessageText:
//
// Boost file storage
//
#define DEFAULT_CATEGORY                 ((WORD)0x00000001L)

 // messages.
//
// MessageId: ERROR_MESSAGE
//
// MessageText:
//
// ERROR: %1.
//
#define ERROR_MESSAGE                    ((DWORD)0xC0000100L)

//
// MessageId: WARNING_MESSAGE
//
// MessageText:
//
// WARNING: %1.
//
#define WARNING_MESSAGE                  ((DWORD)0x80000101L)

//
// MessageId: INFO_MESSAGE
//
// MessageText:
//
// INFO: %1.
//
#define INFO_MESSAGE                     ((DWORD)0x40000102L)

#define EVENT_PROVIDER_NAME "BoostFileStorageEventProvider"

