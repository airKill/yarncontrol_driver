#ifndef __DES_H
#define __DES_H

void encrypt(unsigned char *input, unsigned char *key, unsigned char *output);
void decrypt(unsigned char *input, unsigned char *key, unsigned char *output);
void TripleDES(unsigned char *input, unsigned char *key, unsigned char *output);
void TripleDES_1(unsigned char *input, unsigned char *key, unsigned char *output);

#endif







