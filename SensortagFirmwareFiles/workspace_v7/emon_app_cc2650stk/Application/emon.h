/*
 * pwmled.h
 *
 *  Created on: 13 Jan 2018
 *      Author: Olivia
 */

#ifndef APPLICATION_EMON_H_
#define APPLICATION_EMON_H_

//So project_zero.c can see the global variables
extern char g_currentReadAttribute;
extern int g_maxtemp;
extern int g_mintemp;

extern uint32_t g_maxpress;
extern uint32_t g_minpress;

extern uint32_t g_maxhum;
extern uint32_t g_minhum;

extern int g_minrain;

void updateResult();


#endif /* APPLICATION_EMON_H_ */
