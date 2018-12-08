#ifndef TYPES_H
#define TYPES_H
#include "Cutter.h"

typedef struct CommentAdded {
    RVA addr;
    QString cmt;
} CommentAdded;

typedef struct CommentDeleted {
    RVA addr;
} CommentDeleted;

typedef enum MessageType {
    MessageCommentAdded,
    MessageCommentDeleted,
} MessageType;

typedef struct Message {
public:
    MessageType type;
    union {
        CommentAdded commentAdded;
        CommentDeleted commentDeleted;
    };
    Message() { memset( this, 0, sizeof(Message) ); }
    ~Message(){}
} Message;

#endif // TYPES_H
