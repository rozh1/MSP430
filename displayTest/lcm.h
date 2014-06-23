/*
 * lcm.h
 *
 *  Created on: 13.10.2013
 *      Author: Roman
 */

#ifndef LCM_H_
#define LCM_H_

void PulseLcm();
void SendByte(char ByteToSend, int IsData);
void LcmSetCursorPosition(char Row, char Col);
void ClearLcmScreen();
void InitializeLcm(void);
void PrintStr(char *Text);

#endif /* LCM_H_ */
