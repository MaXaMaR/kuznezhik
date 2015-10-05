#include <iostream>
#include <fstream>

using namespace std;

#include "w128.h"
#include "galois.h"
#include "kuznechik.h"

const uint8_t testvec_key[32] = {
    0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
    0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10,
    0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF
};

kuz_key_t encrypt_key;
kuz_key_t decrypt_key;

void key_init() {
    kuz_set_encrypt_key(&encrypt_key, testvec_key);
    kuz_set_decrypt_key(&decrypt_key, testvec_key);
}

int main() {
	kuz_init();
	key_init();

	ofstream tables_defines;
	tables_defines.open ("tables_defines.inc");
	ofstream tables_data;
	tables_data.open ("tables_data.inc");
	ofstream tables_galois_defines;
	tables_galois_defines.open ("tables_galois_defines.inc");
	ofstream tables_galois_data;
	tables_galois_data.open ("tables_galois_data.inc");
	ofstream tables_mul_defines;
	tables_mul_defines.open ("tables_mul_defines.inc");
	ofstream tables_mul_data;
	tables_mul_data.open ("tables_mul_data.inc");
	ofstream tables_precompiled_data;
	tables_precompiled_data.open ("tables_precompiled_data.inc");
	ofstream tables_keys_defines;
	tables_keys_defines.open ("tables_keys_defines.inc");

	for (unsigned int j = 0; j < sizeof(kuz_lvec)/sizeof(kuz_lvec[0]); j++) {
		tables_defines << "%define KUZ_LVEC_" << j << " " << (int) kuz_lvec[j] << endl;
	}
	for (unsigned int j = 0; j < sizeof(kuz_pi)/sizeof(kuz_pi[0]); j++) {
		tables_defines << "%define KUZ_PI_" << j << " " << (int) kuz_pi[j] << endl;
	}
	for (unsigned int j = 0; j < sizeof(kuz_pi_inv)/sizeof(kuz_pi_inv[0]); j++) {
		tables_defines << "%define KUZ_PI_INV_" << j << " " << (int) kuz_pi_inv[j] << endl;
	}
	tables_data << "kuz_lvec:" << endl;
	for (unsigned int i = 0; i < sizeof(kuz_lvec)/sizeof(kuz_lvec[0]); i++) {
		tables_data << "\t" << "db " << (int) kuz_lvec[i] << endl;
	}
	tables_data << "kuz_pi:" << endl;
	for (unsigned int i = 0; i < 256; i++) {
		tables_data << "\t" << "db " << (int) kuz_pi[i] << endl;
	}
	tables_data << "kuz_pi_inv:" << endl;
	for (unsigned int i = 0; i < 256; i++) {
		tables_data << "\t" << "db " << (int) kuz_pi_inv[i] << endl;
	}

	for (unsigned int i = 0; i < 256; i++) {
		tables_galois_defines << "%define ALPHA_TO_" << i << " " << (int) alpha_to[i] << endl;
	}
	for (unsigned int i = 0; i < 256; i++) {
		tables_galois_defines << "%define INDEX_OF_" << i << " " << (int) index_of[i] << endl;
	}
	tables_galois_data << "alpha_to:" << endl;
	for (unsigned int i = 0; i < 256; i++) {
		tables_galois_data << "\t" << "db " << (int) alpha_to[i] << endl;
	}
	tables_galois_data << "index_of:" << endl;
	for (unsigned int i = 0; i < 256; i++) {
		tables_galois_data << "\t" << "db " << (int) index_of[i] << endl;
	}

	for (unsigned int i = 0; i < 256; i++) {
		for (unsigned int j = 0; j < 256; j++) {
			tables_mul_defines << "%define KUZ_MUL_TABLE_" << i << "_" << j << " " << (int) kuz_mul_gf256_func(i, j) << endl;
		}
	}
	tables_mul_data << "mul_table:" << endl;
	for (unsigned int i = 0; i < 256; i++) {
		tables_mul_data << "mul_table_" << i << ":" << endl;
		for (unsigned int j = 0; j < 256; j++) {
			tables_mul_data << "\t" << "db " << (int) kuz_mul_gf256_func(i, j) << endl;
		}
	}

	tables_precompiled_data << "kuz_pil_enc128:" << endl;
	for (unsigned int i = 0; i < 16; i++) {
		tables_precompiled_data << "kuz_pil_enc128_" << i << ":" << endl;
		for (unsigned int j = 0; j < 256; j++) {
			tables_precompiled_data << "kuz_pil_enc128_" << i << "_" << j << ":" << endl;
			for (unsigned int k = 0; k < 16; k++) {
				tables_precompiled_data << "\t" << "db " << (int) kuz_pil_enc128[i][j].b[k] << endl;
			}
		}
	}
	tables_precompiled_data << "kuz_l_dec128:" << endl;
	for (unsigned int i = 0; i < 16; i++) {
		tables_precompiled_data << "kuz_l_dec128_" << i << ":" << endl;
		for (unsigned int j = 0; j < 256; j++) {
			tables_precompiled_data << "kuz_l_dec128_" << i << "_" << j << ":" << endl;
			for (unsigned int k = 0; k < 16; k++) {
				tables_precompiled_data << "\t" << "db " << (int) kuz_l_dec128[i][j].b[k] << endl;
			}
		}
	}
	tables_precompiled_data << "kuz_pil_dec128:" << endl;
	for (unsigned int i = 0; i < 16; i++) {
		tables_precompiled_data << "kuz_pil_dec128_" << i << ":" << endl;
		for (unsigned int j = 0; j < 256; j++) {
			tables_precompiled_data << "kuz_pil_dec128_" << i << "_" << j << ":" << endl;
			for (unsigned int k = 0; k < 16; k++) {
				tables_precompiled_data << "\t" << "db " << (int) kuz_pil_dec128[i][j].b[k] << endl;
			}
		}
	}

	for (unsigned int i = 0; i < 10; i++) {
		for (unsigned int k = 0; k < 16; k++) {
			tables_keys_defines << "%define ENCRYPT_KEY_" << i << "_" << k << " " << (int) encrypt_key.k[i].b[k] << endl;
		}
	}
	for (unsigned int i = 0; i < 10; i++) {
		for (unsigned int k = 0; k < 16; k++) {
			tables_keys_defines << "%define DECRYPT_KEY_" << i << "_" << k << " " << (int) decrypt_key.k[i].b[k] << endl;
		}
	}
}



