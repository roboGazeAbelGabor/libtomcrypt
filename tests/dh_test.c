/* LibTomCrypt, modular cryptographic library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */
#include <tomcrypt_test.h>

#if defined(LTC_MDH)

static int s_prime_test(void)
{
   void *p, *g, *tmp;
   int x, err, primality;

   if ((err = ltc_mp_init_multi(&p, &g, &tmp, NULL)) != CRYPT_OK)               { goto error; }

   for (x = 0; ltc_dh_sets[x].size != 0; x++) {
      /* tfm has a problem with larger sizes */
      if ((strcmp(ltc_mp.name, "TomsFastMath") == 0) && (ltc_dh_sets[x].size > 256)) break;

      if ((err = ltc_mp_read_radix(g, ltc_dh_sets[x].base, 16)) != CRYPT_OK)    { goto error; }
      if ((err = ltc_mp_read_radix(p, ltc_dh_sets[x].prime, 16)) != CRYPT_OK)   { goto error; }

      /* ensure p is prime */
      if ((err = ltc_mp_prime_is_prime(p, 8, &primality)) != CRYPT_OK)          { goto done; }
      if (primality != LTC_MP_YES ) {
         err = CRYPT_FAIL_TESTVECTOR;
         goto done;
      }

      if ((err = ltc_mp_sub_d(p, 1, tmp)) != CRYPT_OK)                          { goto error; }
      if ((err = ltc_mp_div_2(tmp, tmp)) != CRYPT_OK)                           { goto error; }

      /* ensure (p-1)/2 is prime */
      if ((err = ltc_mp_prime_is_prime(tmp, 8, &primality)) != CRYPT_OK)        { goto done; }
      if (primality == 0) {
         err = CRYPT_FAIL_TESTVECTOR;
         goto done;
      }

      /* now see if g^((p-1)/2) mod p is in fact 1 */
      if ((err = ltc_mp_exptmod(g, tmp, p, tmp)) != CRYPT_OK)                   { goto error; }
      if (ltc_mp_cmp_d(tmp, 1)) {
         err = CRYPT_FAIL_TESTVECTOR;
         goto done;
      }
   }
   err = CRYPT_OK;
error:
done:
   ltc_mp_deinit_multi(tmp, g, p, NULL);
   return err;
}

static int s_dhparam_test(void)
{
   dh_key k;
   unsigned char buf[1024];
   /* generated by: openssl dhparam -outform der -out dhparam.der 2048 */
   unsigned char dhparam_der[] = {
      0x30, 0x82, 0x01, 0x08, 0x02, 0x82, 0x01, 0x01, 0x00, 0xae, 0xfe, 0x78, 0xce, 0x80, 0xd5, 0xd7,
      0x8e, 0xcc, 0x4f, 0x0c, 0x1b, 0xb0, 0x95, 0x10, 0xe1, 0x41, 0x15, 0x53, 0x4d, 0x0e, 0x68, 0xb0,
      0xf8, 0x5a, 0x41, 0x0e, 0x65, 0x2f, 0x9f, 0xac, 0x9c, 0x30, 0xb0, 0x76, 0xec, 0x02, 0xe9, 0x43,
      0x55, 0x08, 0xb4, 0x20, 0x60, 0xd9, 0x52, 0xda, 0x2d, 0xab, 0x9a, 0xba, 0xe6, 0xcf, 0x11, 0xa7,
      0x00, 0x44, 0xc2, 0x5e, 0xd1, 0xba, 0x9b, 0xaa, 0xfe, 0x03, 0xdd, 0xdc, 0xef, 0x41, 0x89, 0x9c,
      0xac, 0x64, 0x13, 0xd9, 0x6a, 0x8a, 0x55, 0xa0, 0x5b, 0xff, 0x12, 0x92, 0x37, 0x52, 0x6a, 0x91,
      0xa4, 0x6e, 0x9e, 0x61, 0xb7, 0xfe, 0xb0, 0x17, 0x8e, 0x67, 0x0f, 0x88, 0x46, 0xa7, 0x9e, 0xb1,
      0xdb, 0x68, 0x77, 0x70, 0xb5, 0x77, 0xf2, 0x7e, 0x33, 0xb1, 0x3e, 0x10, 0xc4, 0x63, 0x36, 0xd0,
      0x13, 0x27, 0xd3, 0x29, 0xc3, 0xb6, 0x5d, 0xf6, 0x5d, 0xa7, 0xd8, 0x25, 0x5c, 0x0b, 0x65, 0x99,
      0xfa, 0xf9, 0x5f, 0x1d, 0xee, 0xd1, 0x86, 0x64, 0x7c, 0x44, 0xcb, 0xa0, 0x12, 0x52, 0x4c, 0xd4,
      0x46, 0x81, 0xae, 0x07, 0xdb, 0xc7, 0x13, 0x29, 0xce, 0x9b, 0xcf, 0x1c, 0x06, 0xd2, 0x0f, 0x2d,
      0xbb, 0x12, 0x33, 0xb9, 0xb1, 0x0f, 0x67, 0x5d, 0x3f, 0x0c, 0xe4, 0xfa, 0x67, 0x26, 0xe2, 0x89,
      0xa2, 0xd5, 0x66, 0x29, 0x1c, 0xe2, 0x8e, 0xbb, 0x7b, 0xcb, 0xcc, 0x70, 0x7e, 0x4f, 0x0e, 0xd3,
      0x5d, 0x64, 0x64, 0x1b, 0x27, 0xbb, 0xda, 0xa9, 0x08, 0x2b, 0x62, 0xd4, 0xca, 0xc3, 0x3a, 0x23,
      0x39, 0x58, 0x57, 0xaf, 0x7b, 0x8b, 0x0c, 0x5b, 0x2e, 0xfc, 0x42, 0x57, 0x59, 0x39, 0x2e, 0x6d,
      0x39, 0x97, 0xdb, 0x5b, 0x5c, 0xb9, 0x59, 0x71, 0x42, 0xf3, 0xcd, 0xea, 0xda, 0x86, 0x54, 0x86,
      0x61, 0x8d, 0x93, 0x66, 0xc7, 0x65, 0xd1, 0x98, 0xcb, 0x02, 0x01, 0x02
   };
   /* text dump: openssl dh -inform DER -in dhparam.der -text
      DH Parameters: (2048 bit)
          prime:
              00:ae:fe:78:ce:80:d5:d7:8e:cc:4f:0c:1b:b0:95:
              10:e1:41:15:53:4d:0e:68:b0:f8:5a:41:0e:65:2f:
              9f:ac:9c:30:b0:76:ec:02:e9:43:55:08:b4:20:60:
              d9:52:da:2d:ab:9a:ba:e6:cf:11:a7:00:44:c2:5e:
              d1:ba:9b:aa:fe:03:dd:dc:ef:41:89:9c:ac:64:13:
              d9:6a:8a:55:a0:5b:ff:12:92:37:52:6a:91:a4:6e:
              9e:61:b7:fe:b0:17:8e:67:0f:88:46:a7:9e:b1:db:
              68:77:70:b5:77:f2:7e:33:b1:3e:10:c4:63:36:d0:
              13:27:d3:29:c3:b6:5d:f6:5d:a7:d8:25:5c:0b:65:
              99:fa:f9:5f:1d:ee:d1:86:64:7c:44:cb:a0:12:52:
              4c:d4:46:81:ae:07:db:c7:13:29:ce:9b:cf:1c:06:
              d2:0f:2d:bb:12:33:b9:b1:0f:67:5d:3f:0c:e4:fa:
              67:26:e2:89:a2:d5:66:29:1c:e2:8e:bb:7b:cb:cc:
              70:7e:4f:0e:d3:5d:64:64:1b:27:bb:da:a9:08:2b:
              62:d4:ca:c3:3a:23:39:58:57:af:7b:8b:0c:5b:2e:
              fc:42:57:59:39:2e:6d:39:97:db:5b:5c:b9:59:71:
              42:f3:cd:ea:da:86:54:86:61:8d:93:66:c7:65:d1:
              98:cb
          generator: 2 (0x2)
   */
   unsigned char prime[] = {
            0xae, 0xfe, 0x78, 0xce, 0x80, 0xd5, 0xd7, 0x8e, 0xcc, 0x4f, 0x0c, 0x1b, 0xb0, 0x95,
      0x10, 0xe1, 0x41, 0x15, 0x53, 0x4d, 0x0e, 0x68, 0xb0, 0xf8, 0x5a, 0x41, 0x0e, 0x65, 0x2f,
      0x9f, 0xac, 0x9c, 0x30, 0xb0, 0x76, 0xec, 0x02, 0xe9, 0x43, 0x55, 0x08, 0xb4, 0x20, 0x60,
      0xd9, 0x52, 0xda, 0x2d, 0xab, 0x9a, 0xba, 0xe6, 0xcf, 0x11, 0xa7, 0x00, 0x44, 0xc2, 0x5e,
      0xd1, 0xba, 0x9b, 0xaa, 0xfe, 0x03, 0xdd, 0xdc, 0xef, 0x41, 0x89, 0x9c, 0xac, 0x64, 0x13,
      0xd9, 0x6a, 0x8a, 0x55, 0xa0, 0x5b, 0xff, 0x12, 0x92, 0x37, 0x52, 0x6a, 0x91, 0xa4, 0x6e,
      0x9e, 0x61, 0xb7, 0xfe, 0xb0, 0x17, 0x8e, 0x67, 0x0f, 0x88, 0x46, 0xa7, 0x9e, 0xb1, 0xdb,
      0x68, 0x77, 0x70, 0xb5, 0x77, 0xf2, 0x7e, 0x33, 0xb1, 0x3e, 0x10, 0xc4, 0x63, 0x36, 0xd0,
      0x13, 0x27, 0xd3, 0x29, 0xc3, 0xb6, 0x5d, 0xf6, 0x5d, 0xa7, 0xd8, 0x25, 0x5c, 0x0b, 0x65,
      0x99, 0xfa, 0xf9, 0x5f, 0x1d, 0xee, 0xd1, 0x86, 0x64, 0x7c, 0x44, 0xcb, 0xa0, 0x12, 0x52,
      0x4c, 0xd4, 0x46, 0x81, 0xae, 0x07, 0xdb, 0xc7, 0x13, 0x29, 0xce, 0x9b, 0xcf, 0x1c, 0x06,
      0xd2, 0x0f, 0x2d, 0xbb, 0x12, 0x33, 0xb9, 0xb1, 0x0f, 0x67, 0x5d, 0x3f, 0x0c, 0xe4, 0xfa,
      0x67, 0x26, 0xe2, 0x89, 0xa2, 0xd5, 0x66, 0x29, 0x1c, 0xe2, 0x8e, 0xbb, 0x7b, 0xcb, 0xcc,
      0x70, 0x7e, 0x4f, 0x0e, 0xd3, 0x5d, 0x64, 0x64, 0x1b, 0x27, 0xbb, 0xda, 0xa9, 0x08, 0x2b,
      0x62, 0xd4, 0xca, 0xc3, 0x3a, 0x23, 0x39, 0x58, 0x57, 0xaf, 0x7b, 0x8b, 0x0c, 0x5b, 0x2e,
      0xfc, 0x42, 0x57, 0x59, 0x39, 0x2e, 0x6d, 0x39, 0x97, 0xdb, 0x5b, 0x5c, 0xb9, 0x59, 0x71,
      0x42, 0xf3, 0xcd, 0xea, 0xda, 0x86, 0x54, 0x86, 0x61, 0x8d, 0x93, 0x66, 0xc7, 0x65, 0xd1,
      0x98, 0xcb
   };

   DO(dh_set_pg_dhparam(dhparam_der, sizeof(dhparam_der), &k));
   DO(dh_generate_key(&yarrow_prng, find_prng ("yarrow"), &k));
   if (ltc_mp_unsigned_bin_size(k.prime) > sizeof(buf)) {
      printf("dhparam_test: short buf\n");
      dh_free(&k);
      return CRYPT_ERROR;
   }
   DO(ltc_mp_to_unsigned_bin(k.prime, buf));
   DO(do_compare_testvector(buf, sizeof(prime), prime, sizeof(prime), "dhparam_test: prime mismatch", 1));
   if (ltc_mp_cmp_d(k.base, 2) != LTC_MP_EQ) {
      printf("dhparam_test: base mismatch\n");
      dh_free(&k);
      return CRYPT_ERROR;
   }
   dh_free(&k);
   return CRYPT_OK;
}

static int s_set_test(void)
{
   dh_key k1, k2, k3;
   unsigned char buf[4096];
   unsigned long len;
   int i;
   unsigned char gbin[] = { 0x02 };
   unsigned char pbin[] = {
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC9, 0x0F, 0xDA, 0xA2, 0x21, 0x68, 0xC2, 0x34,
      0xC4, 0xC6, 0x62, 0x8B, 0x80, 0xDC, 0x1C, 0xD1, 0x29, 0x02, 0x4E, 0x08, 0x8A, 0x67, 0xCC, 0x74,
      0x02, 0x0B, 0xBE, 0xA6, 0x3B, 0x13, 0x9B, 0x22, 0x51, 0x4A, 0x08, 0x79, 0x8E, 0x34, 0x04, 0xDD,
      0xEF, 0x95, 0x19, 0xB3, 0xCD, 0x3A, 0x43, 0x1B, 0x30, 0x2B, 0x0A, 0x6D, 0xF2, 0x5F, 0x14, 0x37,
      0x4F, 0xE1, 0x35, 0x6D, 0x6D, 0x51, 0xC2, 0x45, 0xE4, 0x85, 0xB5, 0x76, 0x62, 0x5E, 0x7E, 0xC6,
      0xF4, 0x4C, 0x42, 0xE9, 0xA6, 0x37, 0xED, 0x6B, 0x0B, 0xFF, 0x5C, 0xB6, 0xF4, 0x06, 0xB7, 0xED,
      0xEE, 0x38, 0x6B, 0xFB, 0x5A, 0x89, 0x9F, 0xA5, 0xAE, 0x9F, 0x24, 0x11, 0x7C, 0x4B, 0x1F, 0xE6,
      0x49, 0x28, 0x66, 0x51, 0xEC, 0xE4, 0x5B, 0x3D, 0xC2, 0x00, 0x7C, 0xB8, 0xA1, 0x63, 0xBF, 0x05,
      0x98, 0xDA, 0x48, 0x36, 0x1C, 0x55, 0xD3, 0x9A, 0x69, 0x16, 0x3F, 0xA8, 0xFD, 0x24, 0xCF, 0x5F,
      0x83, 0x65, 0x5D, 0x23, 0xDC, 0xA3, 0xAD, 0x96, 0x1C, 0x62, 0xF3, 0x56, 0x20, 0x85, 0x52, 0xBB,
      0x9E, 0xD5, 0x29, 0x07, 0x70, 0x96, 0x96, 0x6D, 0x67, 0x0C, 0x35, 0x4E, 0x4A, 0xBC, 0x98, 0x04,
      0xF1, 0x74, 0x6C, 0x08, 0xCA, 0x18, 0x21, 0x7C, 0x32, 0x90, 0x5E, 0x46, 0x2E, 0x36, 0xCE, 0x3B,
      0xE3, 0x9E, 0x77, 0x2C, 0x18, 0x0E, 0x86, 0x03, 0x9B, 0x27, 0x83, 0xA2, 0xEC, 0x07, 0xA2, 0x8F,
      0xB5, 0xC5, 0x5D, 0xF0, 0x6F, 0x4C, 0x52, 0xC9, 0xDE, 0x2B, 0xCB, 0xF6, 0x95, 0x58, 0x17, 0x18,
      0x39, 0x95, 0x49, 0x7C, 0xEA, 0x95, 0x6A, 0xE5, 0x15, 0xD2, 0x26, 0x18, 0x98, 0xFA, 0x05, 0x10,
      0x15, 0x72, 0x8E, 0x5A, 0x8A, 0xAC, 0xAA, 0x68, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
   };
   unsigned char xbin[] = {
      0xA6, 0x68, 0x1A, 0xDC, 0x38, 0x6C, 0xE9, 0x44, 0xC3, 0xDE, 0xD9, 0xA7, 0x30, 0x1D, 0xCC, 0x9C,
      0x51, 0x82, 0x50, 0xE3, 0xED, 0xB6, 0x2F, 0x95, 0x91, 0x98, 0xF8, 0xDC, 0x00, 0x57, 0xDD, 0x6F,
      0xB5, 0x7A, 0xBA, 0xFD, 0x78, 0x81, 0x98, 0xB1
   };
   unsigned char ybin[] = {
      0x39, 0x04, 0x66, 0x32, 0xC8, 0x34, 0x41, 0x8D, 0xFA, 0x07, 0xB3, 0x09, 0x15, 0x38, 0xB6, 0x14,
      0xD1, 0xFB, 0x5D, 0xBB, 0x78, 0x5C, 0x0F, 0xBE, 0xA3, 0xB9, 0x8B, 0x29, 0x5B, 0xC0, 0xCD, 0x07,
      0x6A, 0x88, 0xD9, 0x45, 0x21, 0x41, 0xA2, 0x69, 0xE8, 0xBA, 0xEB, 0x1D, 0xD6, 0x54, 0xEB, 0xA0,
      0x3A, 0x57, 0x05, 0x31, 0x8D, 0x12, 0x97, 0x54, 0xCD, 0xF4, 0x00, 0x3A, 0x8C, 0x39, 0x92, 0x40,
      0xFB, 0xB8, 0xF1, 0x62, 0x49, 0x0F, 0x6F, 0x0D, 0xC7, 0x0E, 0x41, 0x4B, 0x6F, 0xEE, 0x88, 0x08,
      0x6A, 0xFA, 0xA4, 0x8E, 0x9F, 0x3A, 0x24, 0x8E, 0xDC, 0x09, 0x34, 0x52, 0x66, 0x3D, 0x34, 0xE0,
      0xE8, 0x09, 0xD4, 0xF6, 0xBA, 0xDB, 0xB3, 0x6F, 0x80, 0xB6, 0x81, 0x3E, 0xBF, 0x7C, 0x32, 0x81,
      0xB8, 0x62, 0x20, 0x9E, 0x56, 0x04, 0xBD, 0xEA, 0x8B, 0x8F, 0x5F, 0x7B, 0xFD, 0xC3, 0xEE, 0xB7,
      0xAD, 0xB7, 0x30, 0x48, 0x28, 0x9B, 0xCE, 0xA0, 0xF5, 0xA5, 0xCD, 0xEE, 0x7D, 0xF9, 0x1C, 0xD1,
      0xF0, 0xBA, 0x63, 0x2F, 0x06, 0xDB, 0xE9, 0xBA, 0x7E, 0xF0, 0x14, 0xB8, 0x4B, 0x02, 0xD4, 0x97,
      0xCA, 0x7D, 0x0C, 0x60, 0xF7, 0x34, 0x75, 0x2A, 0x64, 0x9D, 0xA4, 0x96, 0x94, 0x6B, 0x4E, 0x53,
      0x1B, 0x30, 0xD9, 0xF8, 0x2E, 0xDD, 0x85, 0x56, 0x36, 0xC0, 0xB0, 0xF2, 0xAE, 0x23, 0x2E, 0x41,
      0x86, 0x45, 0x4E, 0x88, 0x87, 0xBB, 0x42, 0x3E, 0x32, 0xA5, 0xA2, 0x49, 0x5E, 0xAC, 0xBA, 0x99,
      0x62, 0x0A, 0xCD, 0x03, 0xA3, 0x83, 0x45, 0xEB, 0xB6, 0x73, 0x5E, 0x62, 0x33, 0x0A, 0x8E, 0xE9,
      0xAA, 0x6C, 0x83, 0x70, 0x41, 0x0F, 0x5C, 0xD4, 0x5A, 0xF3, 0x7E, 0xE9, 0x0A, 0x0D, 0xA9, 0x5B,
      0xE9, 0x6F, 0xC9, 0x39, 0xE8, 0x8F, 0xE0, 0xBD, 0x2C, 0xD0, 0x9F, 0xC8, 0xF5, 0x24, 0x20, 0x8C
   };

   struct {
     int radix;
     void* g; int glen;
     void* p; int plen;
     void* x; int xlen;
     void* y; int ylen;
   } test[1] = {
      { 256, gbin, sizeof(gbin),   pbin, sizeof(pbin),   xbin, sizeof(xbin),   ybin, sizeof(ybin)   }
   };

   unsigned char export_private[] = {
      0x30, 0x82, 0x01, 0x3A, 0x02, 0x01, 0x00, 0x03, 0x02, 0x07, 0x80, 0x02, 0x82, 0x01, 0x01, 0x00,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC9, 0x0F, 0xDA, 0xA2, 0x21, 0x68, 0xC2, 0x34,
      0xC4, 0xC6, 0x62, 0x8B, 0x80, 0xDC, 0x1C, 0xD1, 0x29, 0x02, 0x4E, 0x08, 0x8A, 0x67, 0xCC, 0x74,
      0x02, 0x0B, 0xBE, 0xA6, 0x3B, 0x13, 0x9B, 0x22, 0x51, 0x4A, 0x08, 0x79, 0x8E, 0x34, 0x04, 0xDD,
      0xEF, 0x95, 0x19, 0xB3, 0xCD, 0x3A, 0x43, 0x1B, 0x30, 0x2B, 0x0A, 0x6D, 0xF2, 0x5F, 0x14, 0x37,
      0x4F, 0xE1, 0x35, 0x6D, 0x6D, 0x51, 0xC2, 0x45, 0xE4, 0x85, 0xB5, 0x76, 0x62, 0x5E, 0x7E, 0xC6,
      0xF4, 0x4C, 0x42, 0xE9, 0xA6, 0x37, 0xED, 0x6B, 0x0B, 0xFF, 0x5C, 0xB6, 0xF4, 0x06, 0xB7, 0xED,
      0xEE, 0x38, 0x6B, 0xFB, 0x5A, 0x89, 0x9F, 0xA5, 0xAE, 0x9F, 0x24, 0x11, 0x7C, 0x4B, 0x1F, 0xE6,
      0x49, 0x28, 0x66, 0x51, 0xEC, 0xE4, 0x5B, 0x3D, 0xC2, 0x00, 0x7C, 0xB8, 0xA1, 0x63, 0xBF, 0x05,
      0x98, 0xDA, 0x48, 0x36, 0x1C, 0x55, 0xD3, 0x9A, 0x69, 0x16, 0x3F, 0xA8, 0xFD, 0x24, 0xCF, 0x5F,
      0x83, 0x65, 0x5D, 0x23, 0xDC, 0xA3, 0xAD, 0x96, 0x1C, 0x62, 0xF3, 0x56, 0x20, 0x85, 0x52, 0xBB,
      0x9E, 0xD5, 0x29, 0x07, 0x70, 0x96, 0x96, 0x6D, 0x67, 0x0C, 0x35, 0x4E, 0x4A, 0xBC, 0x98, 0x04,
      0xF1, 0x74, 0x6C, 0x08, 0xCA, 0x18, 0x21, 0x7C, 0x32, 0x90, 0x5E, 0x46, 0x2E, 0x36, 0xCE, 0x3B,
      0xE3, 0x9E, 0x77, 0x2C, 0x18, 0x0E, 0x86, 0x03, 0x9B, 0x27, 0x83, 0xA2, 0xEC, 0x07, 0xA2, 0x8F,
      0xB5, 0xC5, 0x5D, 0xF0, 0x6F, 0x4C, 0x52, 0xC9, 0xDE, 0x2B, 0xCB, 0xF6, 0x95, 0x58, 0x17, 0x18,
      0x39, 0x95, 0x49, 0x7C, 0xEA, 0x95, 0x6A, 0xE5, 0x15, 0xD2, 0x26, 0x18, 0x98, 0xFA, 0x05, 0x10,
      0x15, 0x72, 0x8E, 0x5A, 0x8A, 0xAC, 0xAA, 0x68, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0x02, 0x01, 0x02, 0x02, 0x29, 0x00, 0xA6, 0x68, 0x1A, 0xDC, 0x38, 0x6C, 0xE9, 0x44, 0xC3, 0xDE,
      0xD9, 0xA7, 0x30, 0x1D, 0xCC, 0x9C, 0x51, 0x82, 0x50, 0xE3, 0xED, 0xB6, 0x2F, 0x95, 0x91, 0x98,
      0xF8, 0xDC, 0x00, 0x57, 0xDD, 0x6F, 0xB5, 0x7A, 0xBA, 0xFD, 0x78, 0x81, 0x98, 0xB1
   };
   unsigned char export_public[] = {
      0x30, 0x82, 0x02, 0x13, 0x02, 0x01, 0x00, 0x03, 0x02, 0x07, 0x00, 0x02, 0x82, 0x01, 0x01, 0x00,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC9, 0x0F, 0xDA, 0xA2, 0x21, 0x68, 0xC2, 0x34,
      0xC4, 0xC6, 0x62, 0x8B, 0x80, 0xDC, 0x1C, 0xD1, 0x29, 0x02, 0x4E, 0x08, 0x8A, 0x67, 0xCC, 0x74,
      0x02, 0x0B, 0xBE, 0xA6, 0x3B, 0x13, 0x9B, 0x22, 0x51, 0x4A, 0x08, 0x79, 0x8E, 0x34, 0x04, 0xDD,
      0xEF, 0x95, 0x19, 0xB3, 0xCD, 0x3A, 0x43, 0x1B, 0x30, 0x2B, 0x0A, 0x6D, 0xF2, 0x5F, 0x14, 0x37,
      0x4F, 0xE1, 0x35, 0x6D, 0x6D, 0x51, 0xC2, 0x45, 0xE4, 0x85, 0xB5, 0x76, 0x62, 0x5E, 0x7E, 0xC6,
      0xF4, 0x4C, 0x42, 0xE9, 0xA6, 0x37, 0xED, 0x6B, 0x0B, 0xFF, 0x5C, 0xB6, 0xF4, 0x06, 0xB7, 0xED,
      0xEE, 0x38, 0x6B, 0xFB, 0x5A, 0x89, 0x9F, 0xA5, 0xAE, 0x9F, 0x24, 0x11, 0x7C, 0x4B, 0x1F, 0xE6,
      0x49, 0x28, 0x66, 0x51, 0xEC, 0xE4, 0x5B, 0x3D, 0xC2, 0x00, 0x7C, 0xB8, 0xA1, 0x63, 0xBF, 0x05,
      0x98, 0xDA, 0x48, 0x36, 0x1C, 0x55, 0xD3, 0x9A, 0x69, 0x16, 0x3F, 0xA8, 0xFD, 0x24, 0xCF, 0x5F,
      0x83, 0x65, 0x5D, 0x23, 0xDC, 0xA3, 0xAD, 0x96, 0x1C, 0x62, 0xF3, 0x56, 0x20, 0x85, 0x52, 0xBB,
      0x9E, 0xD5, 0x29, 0x07, 0x70, 0x96, 0x96, 0x6D, 0x67, 0x0C, 0x35, 0x4E, 0x4A, 0xBC, 0x98, 0x04,
      0xF1, 0x74, 0x6C, 0x08, 0xCA, 0x18, 0x21, 0x7C, 0x32, 0x90, 0x5E, 0x46, 0x2E, 0x36, 0xCE, 0x3B,
      0xE3, 0x9E, 0x77, 0x2C, 0x18, 0x0E, 0x86, 0x03, 0x9B, 0x27, 0x83, 0xA2, 0xEC, 0x07, 0xA2, 0x8F,
      0xB5, 0xC5, 0x5D, 0xF0, 0x6F, 0x4C, 0x52, 0xC9, 0xDE, 0x2B, 0xCB, 0xF6, 0x95, 0x58, 0x17, 0x18,
      0x39, 0x95, 0x49, 0x7C, 0xEA, 0x95, 0x6A, 0xE5, 0x15, 0xD2, 0x26, 0x18, 0x98, 0xFA, 0x05, 0x10,
      0x15, 0x72, 0x8E, 0x5A, 0x8A, 0xAC, 0xAA, 0x68, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0x02, 0x01, 0x02, 0x02, 0x82, 0x01, 0x00, 0x39, 0x04, 0x66, 0x32, 0xC8, 0x34, 0x41, 0x8D, 0xFA,
      0x07, 0xB3, 0x09, 0x15, 0x38, 0xB6, 0x14, 0xD1, 0xFB, 0x5D, 0xBB, 0x78, 0x5C, 0x0F, 0xBE, 0xA3,
      0xB9, 0x8B, 0x29, 0x5B, 0xC0, 0xCD, 0x07, 0x6A, 0x88, 0xD9, 0x45, 0x21, 0x41, 0xA2, 0x69, 0xE8,
      0xBA, 0xEB, 0x1D, 0xD6, 0x54, 0xEB, 0xA0, 0x3A, 0x57, 0x05, 0x31, 0x8D, 0x12, 0x97, 0x54, 0xCD,
      0xF4, 0x00, 0x3A, 0x8C, 0x39, 0x92, 0x40, 0xFB, 0xB8, 0xF1, 0x62, 0x49, 0x0F, 0x6F, 0x0D, 0xC7,
      0x0E, 0x41, 0x4B, 0x6F, 0xEE, 0x88, 0x08, 0x6A, 0xFA, 0xA4, 0x8E, 0x9F, 0x3A, 0x24, 0x8E, 0xDC,
      0x09, 0x34, 0x52, 0x66, 0x3D, 0x34, 0xE0, 0xE8, 0x09, 0xD4, 0xF6, 0xBA, 0xDB, 0xB3, 0x6F, 0x80,
      0xB6, 0x81, 0x3E, 0xBF, 0x7C, 0x32, 0x81, 0xB8, 0x62, 0x20, 0x9E, 0x56, 0x04, 0xBD, 0xEA, 0x8B,
      0x8F, 0x5F, 0x7B, 0xFD, 0xC3, 0xEE, 0xB7, 0xAD, 0xB7, 0x30, 0x48, 0x28, 0x9B, 0xCE, 0xA0, 0xF5,
      0xA5, 0xCD, 0xEE, 0x7D, 0xF9, 0x1C, 0xD1, 0xF0, 0xBA, 0x63, 0x2F, 0x06, 0xDB, 0xE9, 0xBA, 0x7E,
      0xF0, 0x14, 0xB8, 0x4B, 0x02, 0xD4, 0x97, 0xCA, 0x7D, 0x0C, 0x60, 0xF7, 0x34, 0x75, 0x2A, 0x64,
      0x9D, 0xA4, 0x96, 0x94, 0x6B, 0x4E, 0x53, 0x1B, 0x30, 0xD9, 0xF8, 0x2E, 0xDD, 0x85, 0x56, 0x36,
      0xC0, 0xB0, 0xF2, 0xAE, 0x23, 0x2E, 0x41, 0x86, 0x45, 0x4E, 0x88, 0x87, 0xBB, 0x42, 0x3E, 0x32,
      0xA5, 0xA2, 0x49, 0x5E, 0xAC, 0xBA, 0x99, 0x62, 0x0A, 0xCD, 0x03, 0xA3, 0x83, 0x45, 0xEB, 0xB6,
      0x73, 0x5E, 0x62, 0x33, 0x0A, 0x8E, 0xE9, 0xAA, 0x6C, 0x83, 0x70, 0x41, 0x0F, 0x5C, 0xD4, 0x5A,
      0xF3, 0x7E, 0xE9, 0x0A, 0x0D, 0xA9, 0x5B, 0xE9, 0x6F, 0xC9, 0x39, 0xE8, 0x8F, 0xE0, 0xBD, 0x2C,
      0xD0, 0x9F, 0xC8, 0xF5, 0x24, 0x20, 0x8C
   };

   for (i = 0; i < 1; i++) {
      DO(dh_set_pg(test[i].p, test[i].plen, test[i].g, test[i].glen, &k1));
      DO(dh_set_key(test[i].x, test[i].xlen, PK_PRIVATE, &k1));

      len = sizeof(buf);
      DO(dh_export(buf, &len, PK_PRIVATE, &k1));
      DO(do_compare_testvector(buf, len, export_private, sizeof(export_private), "radix_test: dh_export+PK_PRIVATE mismatch", i*10 + 0));
      len = sizeof(buf);
      DO(dh_export(buf, &len, PK_PUBLIC, &k1));
      DO(do_compare_testvector(buf, len, export_public, sizeof(export_public), "radix_test: dh_export+PK_PUBLIC mismatch", i*10 + 1));
      len = sizeof(buf);
      DO(dh_export_key(buf, &len, PK_PRIVATE, &k1));
      DO(do_compare_testvector(buf, len, xbin, sizeof(xbin), "radix_test: dh_export+PK_PRIVATE mismatch", i*10 + 2));
      len = sizeof(buf);
      DO(dh_export_key(buf, &len, PK_PUBLIC, &k1));
      DO(do_compare_testvector(buf, len, ybin, sizeof(ybin), "radix_test: dh_export+PK_PUBLIC mismatch", i*10 + 3));
      dh_free(&k1);

      DO(dh_set_pg(test[i].p, test[i].plen, test[i].g, test[i].glen, &k1));
      DO(dh_set_key(test[i].x, test[i].xlen, PK_PRIVATE, &k1));

      len = sizeof(buf);
      DO(dh_export(buf, &len, PK_PRIVATE, &k1));
      DO(do_compare_testvector(buf, len, export_private, sizeof(export_private), "radix_test: dh_export+PK_PRIVATE mismatc", i*10 + 4));
      len = sizeof(buf);
      DO(dh_export(buf, &len, PK_PUBLIC, &k1));
      DO(do_compare_testvector(buf, len, export_public, sizeof(export_public), "radix_test: dh_export+PK_PUBLIC mismatch", i*10 + 5));
      dh_free(&k1);

      DO(dh_set_pg(test[i].p, test[i].plen, test[i].g, test[i].glen, &k2));
      DO(dh_set_key(test[i].y, test[i].ylen, PK_PUBLIC, &k2));

      len = sizeof(buf);
      DO(dh_export(buf, &len, PK_PUBLIC, &k2));
      DO(do_compare_testvector(buf, len, export_public, sizeof(export_public), "radix_test: dh_export+PK_PUBLIC mismatch", i*10 + 6));
      len = sizeof(buf);
      DO(dh_export_key(buf, &len, PK_PUBLIC, &k2));
      DO(do_compare_testvector(buf, len, ybin, sizeof(ybin), "radix_test: dh_export+PK_PUBLIC mismatch", i*10 + 7));
      dh_free(&k2);

      DO(dh_set_pg(test[i].p, test[i].plen, test[i].g, test[i].glen, &k3));
      DO(dh_generate_key(&yarrow_prng, find_prng("yarrow"), &k3));

      len = ltc_mp_unsigned_bin_size(k3.prime);
      DO(ltc_mp_to_unsigned_bin(k3.prime, buf));
      DO(do_compare_testvector(buf, len, pbin, sizeof(pbin), "radix_test: dh_make_key_ex prime mismatch", i*10 + 8));
      len = ltc_mp_unsigned_bin_size(k3.base);
      DO(ltc_mp_to_unsigned_bin(k3.base, buf));
      DO(do_compare_testvector(buf, len, gbin, sizeof(gbin), "radix_test: dh_make_key_ex base mismatch", i*10 + 9));
      dh_free(&k3);
   }

   return CRYPT_OK;
}

static int s_basic_test(void)
{
   unsigned char buf[3][4096];
   unsigned long x, y, z;
   int           size, KEYSIZE;
   dh_key        usera, userb;

   /* tfm has a problem with larger sizes */
   KEYSIZE = (strcmp(ltc_mp.name, "TomsFastMath") == 0) ? 2048 : 4096;

   /* make up two keys */
   DO(dh_set_pg_groupsize(KEYSIZE/8, &usera));
   DO(dh_generate_key(&yarrow_prng, find_prng ("yarrow"), &usera));
   DO(dh_set_pg_groupsize(KEYSIZE/8, &userb));
   DO(dh_generate_key(&yarrow_prng, find_prng ("yarrow"), &userb));

   /* make the shared secret */
   x = KEYSIZE;
   DO(dh_shared_secret (&usera, &userb, buf[0], &x));

   y = KEYSIZE;
   DO(dh_shared_secret (&userb, &usera, buf[1], &y));
   if (y != x) {
      fprintf(stderr, "DH Shared keys are not same size.\n");
      dh_free (&usera);
      dh_free (&userb);
      return CRYPT_ERROR;
   }
   if (memcmp (buf[0], buf[1], x)) {
      fprintf(stderr, "DH Shared keys not same contents.\n");
      dh_free (&usera);
      dh_free (&userb);
      return CRYPT_ERROR;
   }

   /* now export userb */
   y = KEYSIZE;
   DO(dh_export (buf[1], &y, PK_PUBLIC, &userb));
   dh_free (&userb);

   /* import and make the shared secret again */
   DO(dh_import (buf[1], y, &userb));
   z = KEYSIZE;
   DO(dh_shared_secret (&usera, &userb, buf[2], &z));

   dh_free (&usera);
   dh_free (&userb);

   if (z != x) {
      fprintf(stderr, "failed.  Size don't match?\n");
      return CRYPT_ERROR;
   }
   if (memcmp (buf[0], buf[2], x)) {
      fprintf(stderr, "Failed.  Content didn't match.\n");
      return CRYPT_ERROR;
   }

   for (x = 0; ltc_dh_sets[x].size != 0; x++) {
      /* tfm has a problem with larger sizes */
      if ((strcmp(ltc_mp.name, "TomsFastMath") == 0) && (ltc_dh_sets[x].size > 256)) break;

      DO(dh_set_pg_groupsize(ltc_dh_sets[x].size, &usera));
      DO(dh_generate_key(&yarrow_prng, find_prng ("yarrow"), &usera));
      size = dh_get_groupsize(&usera);
      dh_free(&usera);
      if (size != ltc_dh_sets[x].size) {
         fprintf(stderr, "dh_groupsize mismatch %d %d\n", size, ltc_dh_sets[x].size);
         return CRYPT_ERROR;
      }
      dh_free(&usera);
   }

   return CRYPT_OK;
}

int dh_test(void)
{
   int fails = 0;

   if (ltc_mp.name == NULL) return CRYPT_NOP;

   if (s_prime_test() != CRYPT_OK) fails++;
   if (s_basic_test() != CRYPT_OK) fails++;
   if (s_dhparam_test() != CRYPT_OK) fails++;
   if (s_set_test() != CRYPT_OK) fails++;
   return fails > 0 ? CRYPT_FAIL_TESTVECTOR : CRYPT_OK;
}

#else

int dh_test(void)
{
   return CRYPT_NOP;
}

#endif
