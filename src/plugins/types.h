#ifndef TYPES_H
#define TYPES_H

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
    MessageType type;
    union {
        CommentAdded commentAdded;
        CommentDeleted commentDeleted;
    };
} Message;

#endif // TYPES_H
