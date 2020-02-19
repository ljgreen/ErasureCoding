/*
 * August 20, 2019
 * Jian Liu
 */
#include "parity.h"
#include <jerasure.h>
#include <galois.h>
#include <reed_sol.h>
#include <string.h>

/* Borrowed from Linux kernel */
static void xor_8regs_2(unsigned long bytes, unsigned long *p1, unsigned long *p2)
{
	long lines = bytes / (sizeof (long)) / 8;

	do {
		p1[0] ^= p2[0];
		p1[1] ^= p2[1];
		p1[2] ^= p2[2];
		p1[3] ^= p2[3];
		p1[4] ^= p2[4];
		p1[5] ^= p2[5];
		p1[6] ^= p2[6];
		p1[7] ^= p2[7];
		p1 += 8;
		p2 += 8;
	} while (--lines > 0);
}

/* Borrowed from Linux kernel */
static void xor_8regs_3(unsigned long bytes, unsigned long *p1, unsigned long *p2,
	    unsigned long *p3)
{
	long lines = bytes / (sizeof (long)) / 8;

	do {
		p1[0] ^= p2[0] ^ p3[0];
		p1[1] ^= p2[1] ^ p3[1];
		p1[2] ^= p2[2] ^ p3[2];
		p1[3] ^= p2[3] ^ p3[3];
		p1[4] ^= p2[4] ^ p3[4];
		p1[5] ^= p2[5] ^ p3[5];
		p1[6] ^= p2[6] ^ p3[6];
		p1[7] ^= p2[7] ^ p3[7];
		p1 += 8;
		p2 += 8;
		p3 += 8;
	} while (--lines > 0);
}

/* Borrowed from Linux kernel */
static void xor_8regs_4(unsigned long bytes, unsigned long *p1, unsigned long *p2,
	    unsigned long *p3, unsigned long *p4)
{
	long lines = bytes / (sizeof (long)) / 8;
	do {
		p1[0] ^= p2[0] ^ p3[0] ^ p4[0];
		p1[1] ^= p2[1] ^ p3[1] ^ p4[1];
		p1[2] ^= p2[2] ^ p3[2] ^ p4[2];
		p1[3] ^= p2[3] ^ p3[3] ^ p4[3];
		p1[4] ^= p2[4] ^ p3[4] ^ p4[4];
		p1[5] ^= p2[5] ^ p3[5] ^ p4[5];
		p1[6] ^= p2[6] ^ p3[6] ^ p4[6];
		p1[7] ^= p2[7] ^ p3[7] ^ p4[7];
		p1 += 8;
		p2 += 8;
		p3 += 8;
		p4 += 8;
	} while (--lines > 0);
}

/* Borrowed from Linux kernel */
static void xor_8regs_5(unsigned long bytes, unsigned long *p1, unsigned long *p2,
	    unsigned long *p3, unsigned long *p4, unsigned long *p5)
{
	long lines = bytes / (sizeof (long)) / 8;

	do {
		p1[0] ^= p2[0] ^ p3[0] ^ p4[0] ^ p5[0];
		p1[1] ^= p2[1] ^ p3[1] ^ p4[1] ^ p5[1];
		p1[2] ^= p2[2] ^ p3[2] ^ p4[2] ^ p5[2];
		p1[3] ^= p2[3] ^ p3[3] ^ p4[3] ^ p5[3];
		p1[4] ^= p2[4] ^ p3[4] ^ p4[4] ^ p5[4];
		p1[5] ^= p2[5] ^ p3[5] ^ p4[5] ^ p5[5];
		p1[6] ^= p2[6] ^ p3[6] ^ p4[6] ^ p5[6];
		p1[7] ^= p2[7] ^ p3[7] ^ p4[7] ^ p5[7];
		p1 += 8;
		p2 += 8;
		p3 += 8;
		p4 += 8;
		p5 += 8;
	} while (--lines > 0);
}
/*
 * k is the number of data blocks; m is the number of parity blocks; 
 */
void xor_encoding(int k, char **data_ptrs, char *code_ptrs, int size){
	int i = 1;
	int DataNum = k - 1;
	int xor_src_cnt = 0;
	memcpy(code_ptrs, data_ptrs[0], size);
	while(DataNum > 0){
		xor_src_cnt = DataNum < 4 ? DataNum : 4;
		if(xor_src_cnt == 1){
			xor_8regs_2((unsigned long)size,(unsigned long*)code_ptrs, (unsigned long*)data_ptrs[i++]);
		}
		if(xor_src_cnt == 2){
			xor_8regs_3((unsigned long)size, (unsigned long*)code_ptrs, (unsigned long*)data_ptrs[i++], (unsigned long*)data_ptrs[i++]);
		}
		if(xor_src_cnt == 3){
			xor_8regs_4((unsigned long)size, (unsigned long*)code_ptrs, (unsigned long*)data_ptrs[i++], (unsigned long*)data_ptrs[i++], (unsigned long*)data_ptrs[i++]);
		}
		if(xor_src_cnt == 4){
			xor_8regs_5((unsigned long)size, (unsigned long*)code_ptrs, (unsigned long*)data_ptrs[i++], (unsigned long*)data_ptrs[i++], (unsigned long*)data_ptrs[i++], data_ptrs[i++]);
		}
		DataNum -= xor_src_cnt;
	}
}
void xor_delta_encoding(int* delta_di, int delta_num, char **old_data_ptrs, char **new_data_ptrs, char *code_ptrs, int size){
	int i;
	int j;
	for(i = 0; i < delta_num; i++){
		j = delta_di[i];
		xor_8regs_2(size, code_ptrs, old_data_ptrs[j]);
	}
	int xor_src_cnt = 0;
	int DataNum = delta_num;
	i = 0;
	while(DataNum > 0){
		xor_src_cnt = DataNum < 4 ? DataNum : 4;
		if(xor_src_cnt == 1){
			xor_8regs_2((unsigned long)size, (unsigned long*)code_ptrs, (unsigned long*)new_data_ptrs[delta_di[i++]]);
		}
		if(xor_src_cnt == 2){
			xor_8regs_3((unsigned long)size, (unsigned long*)code_ptrs, (unsigned long*)new_data_ptrs[delta_di[i++]], (unsigned long*)new_data_ptrs[delta_di[i++]]);
		}
		if(xor_src_cnt == 3){
			xor_8regs_4((unsigned long)size, (unsigned long*)code_ptrs, (unsigned long*)new_data_ptrs[delta_di[i++]], (unsigned long*)new_data_ptrs[delta_di[i++]], (unsigned long*)new_data_ptrs[delta_di[i++]]);
		}
		if(xor_src_cnt == 4){
			xor_8regs_5((unsigned long)size, (unsigned long*)code_ptrs, (unsigned long*)new_data_ptrs[delta_di[i++]], (unsigned long*)new_data_ptrs[i++], (unsigned long*)new_data_ptrs[delta_di[i++]], (unsigned long*)new_data_ptrs[delta_di[i++]]);
		}
		DataNum -= xor_src_cnt;
	}
}
void xor_decoding(int k, int erased_di, char **data_ptrs, char *code_ptrs, int size){
	int i = 0;
	int j = 0;
	char** tmp = (char **)malloc(sizeof(char *) * (k+1));
	for(i = 0; i <= k; i++){
		if(i != erased_di){
			tmp[j] = malloc(size * sizeof(char));
			if(i < k){
				memcpy(tmp[j++], data_ptrs[i], size);
			} else {
				memcpy(tmp[j++], code_ptrs, size);
			}
		} 
	}
	i = 0;
	memcpy(data_ptrs[erased_di], tmp[i++], size);
	int xor_src_cnt = 0;
	int DataNum = k - 1;
	while(DataNum > 0){
		xor_src_cnt = DataNum < 4 ? DataNum : 4;
		if(xor_src_cnt == 1){
			xor_8regs_2((unsigned long)size, (unsigned long*)data_ptrs[erased_di], (unsigned long*)tmp[i++]);
		}
		if(xor_src_cnt == 2){
			xor_8regs_3((unsigned long)size, (unsigned long*)data_ptrs[erased_di], (unsigned long*)tmp[i++], (unsigned long*)tmp[i++]);
		}
		if(xor_src_cnt == 3){
			xor_8regs_4((unsigned long)size, (unsigned long*)data_ptrs[erased_di], (unsigned long*)tmp[i++], (unsigned long*)tmp[i++], (unsigned long*)tmp[i++]);
		}
		if(xor_src_cnt == 4){
			xor_8regs_5((unsigned long)size, (unsigned long*)data_ptrs[erased_di], (unsigned long*)tmp[i++], (unsigned long*)tmp[i++], (unsigned long*)tmp[i++], (unsigned long*)tmp[i++]);
		}
		DataNum -= xor_src_cnt;
	}
	for(i = 0; i <= k; i++){
		free(tmp[i]);
	}
	free(tmp);
}


/*
 * k is the number of data blocks; m is the number of parity blocks; w is the bit-words limited to 8, 16, 32,
 * and default set to 8. 
 */
void reed_sol_encoding(int k, int m, int w, int* matrix, char **data_ptrs, char **code_ptrs, int size){
	jerasure_matrix_encode(k, m, w, matrix, data_ptrs, code_ptrs, size);
}
void reed_sol_delta_encoding(int k, int m, int w, int* matrix, int* delta_di, int delta_num, char **old_data_ptrs, char **new_data_ptrs, char **code_ptrs, int size){
        int i;
        int j;
        int p;
        int q;
        int* matrix_row = NULL;
        char *diff = (char *)malloc(size * sizeof(char));
        for (i = 0; i < delta_num; i++) {
                p = delta_di[i];
                if(memcmp(old_data_ptrs[p], new_data_ptrs[p], size) != 0){
                        memcpy(diff,new_data_ptrs[p],size);
                        switch (w){
                                case 8:
                                        galois_w08_region_multiply(old_data_ptrs[p], 1, size, diff, 1);
                                	break;
                                case 16:
                                        galois_w16_region_multiply(old_data_ptrs[p], 1, size, diff, 1);
                                	break;
                                case 32:
                                        galois_w32_region_multiply(old_data_ptrs[p], 1, size, diff, 1);
                                	break;
                                default:
                                        galois_w08_region_multiply(old_data_ptrs[p], 1, size, diff, 1);
                                	break;
                        }
                        for (j = 0; j < m; j++) {
                                matrix_row = matrix+(j*k);
                                switch (w){
                                        case 8:
                                                galois_w08_region_multiply(diff, matrix_row[p], size, code_ptrs[j], 1);
                                                break;
                                        case 16:
                                                galois_w16_region_multiply(diff, matrix_row[p], size, code_ptrs[j], 1);
                                                break;
                                        case 32:
                                                galois_w32_region_multiply(diff, matrix_row[p], size, code_ptrs[j], 1);
                                                break;
                                        default:
                                                galois_w08_region_multiply(diff, matrix_row[p], size, code_ptrs[j], 1);
                                                break;
                                }

                        }
                }
        }
        if(diff != NULL){
                free(diff);
        }

}

void reed_sol_decoding(int k, int m, int w, int* matrix, int *erasures, char **data_ptrs, char **code_ptrs, int size){
	jerasure_matrix_decode(k, m, w, matrix, 1, erasures, data_ptrs, code_ptrs, size);
}
