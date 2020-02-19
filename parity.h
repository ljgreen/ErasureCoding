/*
 * August 20, 2019
 * Jian Liu
 */
#ifndef _PARITY_H_
#define _PARITY_H_

void xor_encoding(int k, char **data_ptrs, char *code_ptrs, int size);
void xor_delta_encoding(int* delta_di, int delta_num, char **old_data_ptrs, char **new_data_ptrs, char *code_ptrs, int size);
void xor_decoding(int k, int erased_di, char **data_ptrs, char *code_ptrs, int size);
void reed_sol_encoding(int k, int m, int w, int* matrix, char **data_ptrs, char **code_ptrs, int size);
void reed_sol_delta_encoding(int k, int m, int w, int* matrix, int* delta_di, int delta_num, char **old_data_ptrs, char **new_data_ptrs, char **code_ptrs, int size);
void reed_sol_decoding(int k, int m, int w, int* matrix, int *erasures, char **data_ptrs, char **code_ptrs, int size);
#endif





