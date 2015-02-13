#ifndef __QUESTION_H__
#define __QUESTION_H__

typedef struct _Question Question;
struct _Question{
	char text[512];
	char answer[4][512];
};

void Question_print(Question q);

#endif