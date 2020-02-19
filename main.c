/* *
 * Copyright (c) 2013, James S. Plank and Kevin Greenan
 * All rights reserved.
 *
 * Jerasure - A C/C++ Library for a Variety of Reed-Solomon and RAID-6 Erasure
 * Coding Techniques
 *
 * Revision 2.0: Galois Field backend now links to GF-Complete
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 *  - Neither the name of the University of Tennessee nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY
 * WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*
 * August 20, 2019
 * Jian Liu
 */
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h> 
#include <jerasure.h>
#include <reed_sol.h>

#define BlockSize 	4096  
#define PATH_MAX 	100

#define XOR 0
#define REED_SOL 1

int k = 4;	// data chunk number
int m = 1;	// parity chunk number
int w = 8; // you can change this number to 8, 16, 32.
int coding_method = 0;

void main(int argc, int *argv[]){
	int ret;
	FILE *fp,*fp2;
	struct stat status;  
	char *block;
	char **data;
	char **ori_data;
	char **coding;
	char **ori_coding;
	char *fname;
	
	int *erasures;
	int *erased;
	int *matrix;

	int blocksize;                                  
	int total;
        int extra;
	
	int buffersize;

	int i; 
	int j;
	int readins;
	int n;
	int size;
	int numerased;
	int delta_num;
	int* delta_di;

	matrix = NULL;
	
	blocksize = BlockSize;
	while (blocksize % sizeof(long) != 0){
		blocksize++;
    } 
	/* Open file and error check */
    fp = fopen(argv[1], "rb");
    if (fp == NULL) {
      fprintf(stderr,  "Unable to open file.\n");
      exit(0);
    }
	 /* Determine original size of file */
    stat(argv[1], &status);
    size = status.st_size;	
	int newsize = size;
	while (newsize % (k * sizeof(long)) != 0){
        newsize++;
    }
	if(newsize < k * blocksize){
		readins = 1;
		block = (char *)malloc(sizeof(char)*(k*blocksize));
		buffersize = k*blocksize;
	} else {
		block = (char *)malloc(sizeof(char)*(k*blocksize));
		buffersize = k*blocksize;
		while(newsize % buffersize != 0){
            newsize++;
        }
		readins = newsize / buffersize;
	}
	erased = (int *)malloc(sizeof(int)*(k+m));
	for (i = 0; i < k+m; i++)
		erased[i] = 0;
	erasures = (int *)malloc(sizeof(int)*(k+m));
	
	
  	fname = (char*)malloc(PATH_MAX);
	data = (char **)malloc(sizeof(char*)*k);
	
	ori_data = (char **)malloc(sizeof(char*)*k);
	for (i = 0; i < k; i++) {
       ori_data[i] = (char *)malloc(sizeof(char)*blocksize);
    }
	
    coding = (char **)malloc(sizeof(char*)*m);
    for (i = 0; i < m; i++) {
       coding[i] = (char *)malloc(sizeof(char)*blocksize);
    }
	ori_coding = (char **)malloc(sizeof(char*)*m);
    for (i = 0; i < m; i++) {
       ori_coding[i] = (char *)malloc(sizeof(char)*blocksize);
    }
	matrix = reed_sol_vandermonde_coding_matrix(k, m, w);
	n = 1;
    	total = 0;
	ret = mkdir("tmp", S_IRWXU);
	while (n <= readins) {
        	if (total < size && total+buffersize <= size) {
		    total += fread(block, sizeof(char), buffersize, fp);
	        } else if (total < size && total+buffersize > size) {
			extra = fread(block, sizeof(char), buffersize, fp);
            		for (i = extra; i < buffersize; i++) {
            			block[i] = '0';
            		}
        	} else if (total == size) {
			for (i = 0; i < buffersize; i++) {
              			block[i] = '0';
            		}
        	}
		for (i = 0; i < k; i++) {
			data[i] = block+(i*blocksize);
		}
		/***************************Data encoding*********************************/
		printf("Data encoding test start!\n");
		switch(coding_method){
			case XOR:
				xor_encoding(k, data, coding[0], blocksize);
				break;
			case REED_SOL:
				reed_sol_encoding(k, m, w, matrix, data, coding, blocksize);
				break;
			default:
				break;
		}
		/********************************end*************************************/
		/***************************Delta encoding*******************************/
		printf("Delta encoding test start!\n");
		switch(coding_method){
			case XOR:
				delta_num = 2;
				delta_di = malloc(sizeof(int) * delta_num);
				for(i = 0; i < k; i++){
					memcpy(ori_data[i], data[i], blocksize);
				}
				for(i = 0; i < delta_num; i++){
					delta_di[i] = i;
					memcpy(data[i],"abcdef",6);
				}
				xor_delta_encoding(delta_di, delta_num, ori_data, data, coding[0], blocksize);
				break;
			case REED_SOL:
				delta_num = 2;
				delta_di = malloc(sizeof(int) * delta_num);
				for(i = 0; i < k; i++){
					memcpy(ori_data[i], data[i], blocksize);
				}
				for(i = 0; i < delta_num; i++){
					delta_di[i] = i;
					memcpy(data[i],"abcdef",6);
				}
				reed_sol_delta_encoding(k, m, w, matrix, delta_di, delta_num, ori_data, data, coding, blocksize);
				break;
			default:
				break;
		}
		/********************************end*************************************/
		for(i = 1; i <= k; i++) {
           		sprintf(fname, "tmp/data_%d", i);
			if (n == 1) {
                	    fp2 = fopen(fname, "wb");
            		} else {
                	    fp2 = fopen(fname, "ab");
           		}
			fseek(fp2, blocksize*(n-1), SEEK_SET);			
	            	fwrite(data[i-1], sizeof(char), blocksize, fp2);
	            	fclose(fp2);
        	}
		for(i = 1; i <= m; i++) {
            		sprintf(fname, "tmp/coding_%d", i);
			if (n == 1) {
			    fp2 = fopen(fname, "wb");
			} else {
			    fp2 = fopen(fname, "ab");
                	}
			fseek(fp2, blocksize*(n-1), SEEK_SET);
            		fwrite(coding[i-1], sizeof(char), blocksize, fp2);
            		fclose(fp2);
           	   
		}
		n++;
	}
	/***************************Data dencoding*******************************/
	printf("Data dencoding test start!\n");
	sprintf(fname, "tmp/data_%d", 1);
	remove(fname);
	n = 1;	
	while (n <= readins) {
		numerased = 0;	
		for (i = 1; i <= k; i++) {
			sprintf(fname, "tmp/data_%d", i);
			fp2 = fopen(fname, "rb");
			if (fp2 == NULL) {
				erasures[numerased] = i-1;
				numerased++;
			} else {
				fseek(fp2, blocksize*(n-1), SEEK_SET); 
				ret = fread(data[i-1], sizeof(char), blocksize, fp2);
				fclose(fp2);
			}
		}
		for (i = 1; i <= m; i++) {
			sprintf(fname, "tmp/coding_%d", i);
			fp2 = fopen(fname, "rb");
			if (fp2 == NULL) {
				erasures[numerased] = k+i-1;
				numerased++;
			} else {
				fseek(fp2, blocksize*(n-1), SEEK_SET);
				ret = fread(coding[i-1], sizeof(char), blocksize, fp2);
				fclose(fp2);
			}
		}
		erasures[numerased] = -1;
		switch(coding_method){
			case XOR:
				xor_decoding(k, erasures[0], data, coding[0], blocksize);
				break;
			case REED_SOL:
				reed_sol_decoding(k, m, w, matrix, erasures, data, coding, blocksize);
				break;
			default:
				break;
		}
		for(i = 0; erasures[i] != -1; i++){
			j = erasures[i];
			sprintf(fname,"tmp/%s_%d", "dencoder_file", j+1);
			if (n == 1) {
				fp2 = fopen(fname, "wb");
			} else {
				fp2 = fopen(fname, "ab");
			}
			fseek(fp2, blocksize*(n-1), SEEK_SET);
			if(j < k){
				fwrite(data[j], sizeof(char), blocksize, fp2);
			} else {
				fwrite(coding[j - k], sizeof(char), blocksize, fp2);
			}
			fclose(fp2);
		}
		n++;
	}
	/********************************end*************************************/
	
	for (i = 0; i < k; i++) {
	  	free(ori_data[i]);
    	}
	free(ori_data);
	for (i = 0; i < m; i++) {
       		free(coding[i]);
    	}
	free(coding);
    	for (i = 0; i < m; i++) {
       		free(ori_coding[i]);
    	}
	free(ori_coding);
	free(fname);
	free(data);
	free(block);
	free(erased);
	free(erasures);
}
