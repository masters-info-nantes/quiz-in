#ifndef __QUESTION_H__
#define __QUESTION_H__

typedef struct _Question Question;
struct _Question{
	char text[255];
	char answer[4][255];
};

typedef struct _Response Response;
struct _Response{
	bool currentStatus;
	int score;
};

#endif