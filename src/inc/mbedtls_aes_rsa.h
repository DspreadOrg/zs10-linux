#include <stdio.h>
#include <stdint.h>
/* Mbedtls AES API */
#define MBEDTLS_AES_ENCRYPT     1 /**< AES encryption. */
#define MBEDTLS_AES_DECRYPT     0 /**< AES decryption. */

typedef struct mbedtls_aes_context
{
    int nr;                     /*!< The number of rounds. */
    uint32_t *rk;               /*!< AES round keys. */
    uint32_t buf[68];           /*!< Unaligned data buffer. This buffer can
                                     hold 32 extra Bytes, which can be used for
                                     one of the following purposes:
                                     <ul><li>Alignment if VIA padlock is
                                             used.</li>
                                     <li>Simplifying key expansion in the 256-bit
                                         case by generating an extra round key.
                                         </li></ul> */
}
mbedtls_aes_context;

void mbedtls_aes_init( mbedtls_aes_context *ctx );
void mbedtls_aes_free( mbedtls_aes_context *ctx );

int mbedtls_aes_setkey_enc( mbedtls_aes_context *ctx, const unsigned char *key,
                    unsigned int keybits );

int mbedtls_aes_setkey_dec( mbedtls_aes_context *ctx, const unsigned char *key,
                    unsigned int keybits );

int mbedtls_aes_crypt_ecb( mbedtls_aes_context *ctx,
                    int mode,
                    const unsigned char input[16],
                    unsigned char output[16] );

int mbedtls_aes_crypt_cbc( mbedtls_aes_context *ctx,
                    int mode,
                    size_t length,
                    unsigned char iv[16],
                    const unsigned char *input,
                    unsigned char *output );


int mbedtls_aes_crypt_cfb128( mbedtls_aes_context *ctx,
                       int mode,
                       size_t length,
                       size_t *iv_off,
                       unsigned char iv[16],
                       const unsigned char *input,
                       unsigned char *output );

int mbedtls_aes_crypt_cfb8( mbedtls_aes_context *ctx,
                    int mode,
                    size_t length,
                    unsigned char iv[16],
                    const unsigned char *input,
                    unsigned char *output );

int mbedtls_aes_crypt_ofb( mbedtls_aes_context *ctx,
                       size_t length,
                       size_t *iv_off,
                       unsigned char iv[16],
                       const unsigned char *input,
                       unsigned char *output );


int mbedtls_aes_crypt_ctr( mbedtls_aes_context *ctx,
                       size_t length,
                       size_t *nc_off,
                       unsigned char nonce_counter[16],
                       unsigned char stream_block[16],
                       const unsigned char *input,
                       unsigned char *output );


typedef uint32_t mbedtls_mpi_uint;

/**
 * \brief          MPI structure
 */
typedef struct mbedtls_mpi
{
    int s;              /*!<  Sign: -1 if the mpi is negative, 1 otherwise */
    size_t n;           /*!<  total # of limbs  */
    mbedtls_mpi_uint *p;          /*!<  pointer to limbs  */
}
mbedtls_mpi;

/**
 * \brief   The RSA context structure.
 *
 * \note    Direct manipulation of the members of this structure
 *          is deprecated. All manipulation should instead be done through
 *          the public interface functions.
 */
typedef struct mbedtls_rsa_context
{
    int ver;                    /*!<  Reserved for internal purposes.
                                 *    Do not set this field in application
                                 *    code. Its meaning might change without
                                 *    notice. */
    size_t len;                 /*!<  The size of \p N in Bytes. */

    mbedtls_mpi N;              /*!<  The public modulus. */
    mbedtls_mpi E;              /*!<  The public exponent. */

    mbedtls_mpi D;              /*!<  The private exponent. */
    mbedtls_mpi P;              /*!<  The first prime factor. */
    mbedtls_mpi Q;              /*!<  The second prime factor. */

    mbedtls_mpi DP;             /*!<  <code>D % (P - 1)</code>. */
    mbedtls_mpi DQ;             /*!<  <code>D % (Q - 1)</code>. */
    mbedtls_mpi QP;             /*!<  <code>1 / (Q % P)</code>. */

    mbedtls_mpi RN;             /*!<  cached <code>R^2 mod N</code>. */

    mbedtls_mpi RP;             /*!<  cached <code>R^2 mod P</code>. */
    mbedtls_mpi RQ;             /*!<  cached <code>R^2 mod Q</code>. */

    mbedtls_mpi Vi;             /*!<  The cached blinding value. */
    mbedtls_mpi Vf;             /*!<  The cached un-blinding value. */

    int padding;                /*!< Selects padding mode:
                                     #MBEDTLS_RSA_PKCS_V15 for 1.5 padding and
                                     #MBEDTLS_RSA_PKCS_V21 for OAEP or PSS. */
    int hash_id;                /*!< Hash identifier of mbedtls_md_type_t type,
                                     as specified in md.h for use in the MGF
                                     mask generating function used in the
                                     EME-OAEP and EMSA-PSS encodings. */
#if defined(MBEDTLS_THREADING_C)
    /* Invariant: the mutex is initialized iff ver != 0. */
    mbedtls_threading_mutex_t mutex;    /*!<  Thread-safety mutex. */
#endif
}
mbedtls_rsa_context;


void mbedtls_rsa_init( mbedtls_rsa_context *ctx,
                       int padding,
                       int hash_id );

int mbedtls_rsa_import( mbedtls_rsa_context *ctx,
                        const mbedtls_mpi *N,
                        const mbedtls_mpi *P, const mbedtls_mpi *Q,
                        const mbedtls_mpi *D, const mbedtls_mpi *E );

int mbedtls_rsa_import_raw( mbedtls_rsa_context *ctx,
                            unsigned char const *N, size_t N_len,
                            unsigned char const *P, size_t P_len,
                            unsigned char const *Q, size_t Q_len,
                            unsigned char const *D, size_t D_len,
                            unsigned char const *E, size_t E_len );

int mbedtls_rsa_complete( mbedtls_rsa_context *ctx );

int mbedtls_rsa_export( const mbedtls_rsa_context *ctx,
                        mbedtls_mpi *N, mbedtls_mpi *P, mbedtls_mpi *Q,
                        mbedtls_mpi *D, mbedtls_mpi *E );

int mbedtls_rsa_export_raw( const mbedtls_rsa_context *ctx,
                            unsigned char *N, size_t N_len,
                            unsigned char *P, size_t P_len,
                            unsigned char *Q, size_t Q_len,
                            unsigned char *D, size_t D_len,
                            unsigned char *E, size_t E_len );

void mbedtls_rsa_set_padding( mbedtls_rsa_context *ctx, int padding,
                              int hash_id );

size_t mbedtls_rsa_get_len( const mbedtls_rsa_context *ctx );

int mbedtls_rsa_gen_key( mbedtls_rsa_context *ctx,
                         int (*f_rng)(void *, unsigned char *, size_t),
                         void *p_rng,
                         unsigned int nbits, int exponent );

int mbedtls_rsa_public( mbedtls_rsa_context *ctx,
                const unsigned char *input,
                unsigned char *output );

int mbedtls_rsa_private( mbedtls_rsa_context *ctx,
                 int (*f_rng)(void *, unsigned char *, size_t),
                 void *p_rng,
                 const unsigned char *input,
                 unsigned char *output );

int mbedtls_rsa_pkcs1_encrypt( mbedtls_rsa_context *ctx,
                       int (*f_rng)(void *, unsigned char *, size_t),
                       void *p_rng,
                       int mode, size_t ilen,
                       const unsigned char *input,
                       unsigned char *output );

int mbedtls_rsa_rsaes_pkcs1_v15_encrypt( mbedtls_rsa_context *ctx,
                                 int (*f_rng)(void *, unsigned char *, size_t),
                                 void *p_rng,
                                 int mode, size_t ilen,
                                 const unsigned char *input,
                                 unsigned char *output );

int mbedtls_rsa_pkcs1_decrypt( mbedtls_rsa_context *ctx,
                       int (*f_rng)(void *, unsigned char *, size_t),
                       void *p_rng,
                       int mode, size_t *olen,
                       const unsigned char *input,
                       unsigned char *output,
                       size_t output_max_len );

int mbedtls_rsa_rsaes_pkcs1_v15_decrypt( mbedtls_rsa_context *ctx,
                                 int (*f_rng)(void *, unsigned char *, size_t),
                                 void *p_rng,
                                 int mode, size_t *olen,
                                 const unsigned char *input,
                                 unsigned char *output,
                                 size_t output_max_len );
/* Mbedtls Random API */
#if 1
typedef struct mbedtls_ctr_drbg_context
{
    unsigned char counter[16];  /*!< The counter (V). */
    int reseed_counter;         /*!< The reseed counter.
                                 * This is the number of requests that have
                                 * been made since the last (re)seeding,
                                 * minus one.
                                 * Before the initial seeding, this field
                                 * contains the amount of entropy in bytes
                                 * to use as a nonce for the initial seeding,
                                 * or -1 if no nonce length has been explicitly
                                 * set (see mbedtls_ctr_drbg_set_nonce_len()).
                                 */
    int prediction_resistance;  /*!< This determines whether prediction
                                     resistance is enabled, that is
                                     whether to systematically reseed before
                                     each random generation. */
    size_t entropy_len;         /*!< The amount of entropy grabbed on each
                                     seed or reseed operation, in bytes. */
    int reseed_interval;        /*!< The reseed interval.
                                 * This is the maximum number of requests
                                 * that can be made between reseedings. */

    mbedtls_aes_context aes_ctx;        /*!< The AES context. */

    /*
     * Callbacks (Entropy)
     */
    int (*f_entropy)(void *, unsigned char *, size_t);
                                /*!< The entropy callback function. */

    void *p_entropy;            /*!< The context for the entropy function. */

#if defined(MBEDTLS_THREADING_C)
    /* Invariant: the mutex is initialized if and only if f_entropy != NULL.
     * This means that the mutex is initialized during the initial seeding
     * in mbedtls_ctr_drbg_seed() and freed in mbedtls_ctr_drbg_free().
     *
     * Note that this invariant may change without notice. Do not rely on it
     * and do not access the mutex directly in application code.
     */
    mbedtls_threading_mutex_t mutex;
#endif
}
mbedtls_ctr_drbg_context;
#endif

void mbedtls_ctr_drbg_init( mbedtls_ctr_drbg_context *ctx );

int mbedtls_ctr_drbg_seed( mbedtls_ctr_drbg_context *ctx,
                   int (*f_entropy)(void *, unsigned char *, size_t),
                   void *p_entropy,
                   const unsigned char *custom,
                   size_t len );

void mbedtls_ctr_drbg_free( mbedtls_ctr_drbg_context *ctx );
int mbedtls_ctr_drbg_random( void *p_rng, unsigned char *output, size_t output_len );

#if 1
#define MBEDTLS_HAVEGE_COLLECT_SIZE 1024

/**
 * \brief          HAVEGE state structure
 */
typedef struct mbedtls_havege_state
{
    uint32_t PT1, PT2, offset[2];
    uint32_t pool[MBEDTLS_HAVEGE_COLLECT_SIZE];
    uint32_t WALK[8192];
}
mbedtls_havege_state;

#define MBEDTLS_ENTROPY_MAX_SOURCES     20      /**< Maximum number of sources supported */

/**
 * \brief           Entropy poll callback pointer
 *
 * \param data      Callback-specific data pointer
 * \param output    Data to fill
 * \param len       Maximum size to provide
 * \param olen      The actual amount of bytes put into the buffer (Can be 0)
 *
 * \return          0 if no critical failures occurred,
 *                  MBEDTLS_ERR_ENTROPY_SOURCE_FAILED otherwise
 */
typedef int (*mbedtls_entropy_f_source_ptr)(void *data, unsigned char *output, size_t len,
                            size_t *olen);

/**
 * \brief           Entropy source state
 */
typedef struct mbedtls_entropy_source_state
{
    mbedtls_entropy_f_source_ptr    f_source;   /**< The entropy source callback */
    void *          p_source;   /**< The callback data pointer */
    size_t          size;       /**< Amount received in bytes */
    size_t          threshold;  /**< Minimum bytes required before release */
    int             strong;     /**< Is the source strong? */
}
mbedtls_entropy_source_state;
typedef struct mbedtls_sha512_context
{
    uint64_t total[2];          /*!< The number of Bytes processed. */
    uint64_t state[8];          /*!< The intermediate digest state. */
    unsigned char buffer[128];  /*!< The data block being processed. */
#if !defined(MBEDTLS_SHA512_NO_SHA384)
    int is384;                  /*!< Determines which function to use:
                                     0: Use SHA-512, or 1: Use SHA-384. */
#endif
}
mbedtls_sha512_context;

/**
 * \brief           Entropy context structure
 */
typedef struct mbedtls_entropy_context
{
    int accumulator_started; /* 0 after init.
                              * 1 after the first update.
                              * -1 after free. */

    mbedtls_sha512_context  accumulator;
    int             source_count; /* Number of entries used in source. */
    mbedtls_entropy_source_state    source[MBEDTLS_ENTROPY_MAX_SOURCES];
    mbedtls_havege_state    havege_data;
}
mbedtls_entropy_context;
#endif
void mbedtls_entropy_init( mbedtls_entropy_context *ctx );
void mbedtls_entropy_free( mbedtls_entropy_context *ctx );
int mbedtls_entropy_func( void *data, unsigned char *output, size_t len );

/* Mbedtls PK API */
#if 1
typedef enum {
    MBEDTLS_PK_NONE=0,
    MBEDTLS_PK_RSA,
    MBEDTLS_PK_ECKEY,
    MBEDTLS_PK_ECKEY_DH,
    MBEDTLS_PK_ECDSA,
    MBEDTLS_PK_RSA_ALT,
    MBEDTLS_PK_RSASSA_PSS,
    MBEDTLS_PK_OPAQUE,
} mbedtls_pk_type_t;

typedef enum
{
    MBEDTLS_PK_DEBUG_NONE = 0,
    MBEDTLS_PK_DEBUG_MPI,
    MBEDTLS_PK_DEBUG_ECP,
} mbedtls_pk_debug_type;

typedef enum {
    MBEDTLS_MD_NONE=0,    /**< None. */
    MBEDTLS_MD_MD2,       /**< The MD2 message digest. */
    MBEDTLS_MD_MD4,       /**< The MD4 message digest. */
    MBEDTLS_MD_MD5,       /**< The MD5 message digest. */
    MBEDTLS_MD_SHA1,      /**< The SHA-1 message digest. */
    MBEDTLS_MD_SHA224,    /**< The SHA-224 message digest. */
    MBEDTLS_MD_SHA256,    /**< The SHA-256 message digest. */
    MBEDTLS_MD_SHA384,    /**< The SHA-384 message digest. */
    MBEDTLS_MD_SHA512,    /**< The SHA-512 message digest. */
    MBEDTLS_MD_RIPEMD160, /**< The RIPEMD-160 message digest. */
} mbedtls_md_type_t;

/**
 * \brief           Item to send to the debug module
 */
typedef struct mbedtls_pk_debug_item
{
    mbedtls_pk_debug_type type;
    const char *name;
    void *value;
} mbedtls_pk_debug_item;

struct mbedtls_pk_info_t
{
    /** Public key type */
    mbedtls_pk_type_t type;

    /** Type name */
    const char *name;

    /** Get key size in bits */
    size_t (*get_bitlen)( const void * );

    /** Tell if the context implements this type (e.g. ECKEY can do ECDSA) */
    int (*can_do)( mbedtls_pk_type_t type );

    /** Verify signature */
    int (*verify_func)( void *ctx, mbedtls_md_type_t md_alg,
                        const unsigned char *hash, size_t hash_len,
                        const unsigned char *sig, size_t sig_len );

    /** Make signature */
    int (*sign_func)( void *ctx, mbedtls_md_type_t md_alg,
                      const unsigned char *hash, size_t hash_len,
                      unsigned char *sig, size_t *sig_len,
                      int (*f_rng)(void *, unsigned char *, size_t),
                      void *p_rng );

#if defined(MBEDTLS_ECDSA_C) && defined(MBEDTLS_ECP_RESTARTABLE)
    /** Verify signature (restartable) */
    int (*verify_rs_func)( void *ctx, mbedtls_md_type_t md_alg,
                           const unsigned char *hash, size_t hash_len,
                           const unsigned char *sig, size_t sig_len,
                           void *rs_ctx );

    /** Make signature (restartable) */
    int (*sign_rs_func)( void *ctx, mbedtls_md_type_t md_alg,
                         const unsigned char *hash, size_t hash_len,
                         unsigned char *sig, size_t *sig_len,
                         int (*f_rng)(void *, unsigned char *, size_t),
                         void *p_rng, void *rs_ctx );
#endif /* MBEDTLS_ECDSA_C && MBEDTLS_ECP_RESTARTABLE */

    /** Decrypt message */
    int (*decrypt_func)( void *ctx, const unsigned char *input, size_t ilen,
                         unsigned char *output, size_t *olen, size_t osize,
                         int (*f_rng)(void *, unsigned char *, size_t),
                         void *p_rng );

    /** Encrypt message */
    int (*encrypt_func)( void *ctx, const unsigned char *input, size_t ilen,
                         unsigned char *output, size_t *olen, size_t osize,
                         int (*f_rng)(void *, unsigned char *, size_t),
                         void *p_rng );

    /** Check public-private key pair */
    int (*check_pair_func)( const void *pub, const void *prv );

    /** Allocate a new context */
    void * (*ctx_alloc_func)( void );

    /** Free the given context */
    void (*ctx_free_func)( void *ctx );

#if defined(MBEDTLS_ECDSA_C) && defined(MBEDTLS_ECP_RESTARTABLE)
    /** Allocate the restart context */
    void * (*rs_alloc_func)( void );

    /** Free the restart context */
    void (*rs_free_func)( void *rs_ctx );
#endif /* MBEDTLS_ECDSA_C && MBEDTLS_ECP_RESTARTABLE */

    /** Interface with the debug module */
    void (*debug_func)( const void *ctx, mbedtls_pk_debug_item *items );

};

typedef struct mbedtls_pk_info_t mbedtls_pk_info_t;

typedef struct mbedtls_pk_context
{
    const mbedtls_pk_info_t *   pk_info; /**< Public key information         */
    void *                      pk_ctx;  /**< Underlying public key context  */
} mbedtls_pk_context;

#endif

void mbedtls_pk_init( mbedtls_pk_context *ctx );
void mbedtls_pk_free( mbedtls_pk_context *ctx );
int mbedtls_pk_parse_public_key( mbedtls_pk_context *ctx,
                         const unsigned char *key, size_t keylen );

static mbedtls_rsa_context *ym_mbedtls_pk_rsa( const mbedtls_pk_context pk )
{
    return( (mbedtls_rsa_context *) (pk).pk_ctx );
}


/* Mbedtls Cipher API */
#if 1
typedef enum {
    MBEDTLS_CIPHER_ID_NONE = 0,  /**< Placeholder to mark the end of cipher ID lists. */
    MBEDTLS_CIPHER_ID_NULL,      /**< The identity cipher, treated as a stream cipher. */
    MBEDTLS_CIPHER_ID_AES,       /**< The AES cipher. */
    MBEDTLS_CIPHER_ID_DES,       /**< The DES cipher. */
    MBEDTLS_CIPHER_ID_3DES,      /**< The Triple DES cipher. */
    MBEDTLS_CIPHER_ID_CAMELLIA,  /**< The Camellia cipher. */
    MBEDTLS_CIPHER_ID_BLOWFISH,  /**< The Blowfish cipher. */
    MBEDTLS_CIPHER_ID_ARC4,      /**< The RC4 cipher. */
    MBEDTLS_CIPHER_ID_ARIA,      /**< The Aria cipher. */
    MBEDTLS_CIPHER_ID_CHACHA20,  /**< The ChaCha20 cipher. */
} mbedtls_cipher_id_t;


typedef enum {
    MBEDTLS_CIPHER_NONE = 0,             /**< Placeholder to mark the end of cipher-pair lists. */
    MBEDTLS_CIPHER_NULL,                 /**< The identity stream cipher. */
    MBEDTLS_CIPHER_AES_128_ECB,          /**< AES cipher with 128-bit ECB mode. */
    MBEDTLS_CIPHER_AES_192_ECB,          /**< AES cipher with 192-bit ECB mode. */
    MBEDTLS_CIPHER_AES_256_ECB,          /**< AES cipher with 256-bit ECB mode. */
    MBEDTLS_CIPHER_AES_128_CBC,          /**< AES cipher with 128-bit CBC mode. */
    MBEDTLS_CIPHER_AES_192_CBC,          /**< AES cipher with 192-bit CBC mode. */
    MBEDTLS_CIPHER_AES_256_CBC,          /**< AES cipher with 256-bit CBC mode. */
    MBEDTLS_CIPHER_AES_128_CFB128,       /**< AES cipher with 128-bit CFB128 mode. */
    MBEDTLS_CIPHER_AES_192_CFB128,       /**< AES cipher with 192-bit CFB128 mode. */
    MBEDTLS_CIPHER_AES_256_CFB128,       /**< AES cipher with 256-bit CFB128 mode. */
    MBEDTLS_CIPHER_AES_128_CTR,          /**< AES cipher with 128-bit CTR mode. */
    MBEDTLS_CIPHER_AES_192_CTR,          /**< AES cipher with 192-bit CTR mode. */
    MBEDTLS_CIPHER_AES_256_CTR,          /**< AES cipher with 256-bit CTR mode. */
    MBEDTLS_CIPHER_AES_128_GCM,          /**< AES cipher with 128-bit GCM mode. */
    MBEDTLS_CIPHER_AES_192_GCM,          /**< AES cipher with 192-bit GCM mode. */
    MBEDTLS_CIPHER_AES_256_GCM,          /**< AES cipher with 256-bit GCM mode. */
    MBEDTLS_CIPHER_CAMELLIA_128_ECB,     /**< Camellia cipher with 128-bit ECB mode. */
    MBEDTLS_CIPHER_CAMELLIA_192_ECB,     /**< Camellia cipher with 192-bit ECB mode. */
    MBEDTLS_CIPHER_CAMELLIA_256_ECB,     /**< Camellia cipher with 256-bit ECB mode. */
    MBEDTLS_CIPHER_CAMELLIA_128_CBC,     /**< Camellia cipher with 128-bit CBC mode. */
    MBEDTLS_CIPHER_CAMELLIA_192_CBC,     /**< Camellia cipher with 192-bit CBC mode. */
    MBEDTLS_CIPHER_CAMELLIA_256_CBC,     /**< Camellia cipher with 256-bit CBC mode. */
    MBEDTLS_CIPHER_CAMELLIA_128_CFB128,  /**< Camellia cipher with 128-bit CFB128 mode. */
    MBEDTLS_CIPHER_CAMELLIA_192_CFB128,  /**< Camellia cipher with 192-bit CFB128 mode. */
    MBEDTLS_CIPHER_CAMELLIA_256_CFB128,  /**< Camellia cipher with 256-bit CFB128 mode. */
    MBEDTLS_CIPHER_CAMELLIA_128_CTR,     /**< Camellia cipher with 128-bit CTR mode. */
    MBEDTLS_CIPHER_CAMELLIA_192_CTR,     /**< Camellia cipher with 192-bit CTR mode. */
    MBEDTLS_CIPHER_CAMELLIA_256_CTR,     /**< Camellia cipher with 256-bit CTR mode. */
    MBEDTLS_CIPHER_CAMELLIA_128_GCM,     /**< Camellia cipher with 128-bit GCM mode. */
    MBEDTLS_CIPHER_CAMELLIA_192_GCM,     /**< Camellia cipher with 192-bit GCM mode. */
    MBEDTLS_CIPHER_CAMELLIA_256_GCM,     /**< Camellia cipher with 256-bit GCM mode. */
    MBEDTLS_CIPHER_DES_ECB,              /**< DES cipher with ECB mode. */
    MBEDTLS_CIPHER_DES_CBC,              /**< DES cipher with CBC mode. */
    MBEDTLS_CIPHER_DES_EDE_ECB,          /**< DES cipher with EDE ECB mode. */
    MBEDTLS_CIPHER_DES_EDE_CBC,          /**< DES cipher with EDE CBC mode. */
    MBEDTLS_CIPHER_DES_EDE3_ECB,         /**< DES cipher with EDE3 ECB mode. */
    MBEDTLS_CIPHER_DES_EDE3_CBC,         /**< DES cipher with EDE3 CBC mode. */
    MBEDTLS_CIPHER_BLOWFISH_ECB,         /**< Blowfish cipher with ECB mode. */
    MBEDTLS_CIPHER_BLOWFISH_CBC,         /**< Blowfish cipher with CBC mode. */
    MBEDTLS_CIPHER_BLOWFISH_CFB64,       /**< Blowfish cipher with CFB64 mode. */
    MBEDTLS_CIPHER_BLOWFISH_CTR,         /**< Blowfish cipher with CTR mode. */
    MBEDTLS_CIPHER_ARC4_128,             /**< RC4 cipher with 128-bit mode. */
    MBEDTLS_CIPHER_AES_128_CCM,          /**< AES cipher with 128-bit CCM mode. */
    MBEDTLS_CIPHER_AES_192_CCM,          /**< AES cipher with 192-bit CCM mode. */
    MBEDTLS_CIPHER_AES_256_CCM,          /**< AES cipher with 256-bit CCM mode. */
    MBEDTLS_CIPHER_CAMELLIA_128_CCM,     /**< Camellia cipher with 128-bit CCM mode. */
    MBEDTLS_CIPHER_CAMELLIA_192_CCM,     /**< Camellia cipher with 192-bit CCM mode. */
    MBEDTLS_CIPHER_CAMELLIA_256_CCM,     /**< Camellia cipher with 256-bit CCM mode. */
    MBEDTLS_CIPHER_ARIA_128_ECB,         /**< Aria cipher with 128-bit key and ECB mode. */
    MBEDTLS_CIPHER_ARIA_192_ECB,         /**< Aria cipher with 192-bit key and ECB mode. */
    MBEDTLS_CIPHER_ARIA_256_ECB,         /**< Aria cipher with 256-bit key and ECB mode. */
    MBEDTLS_CIPHER_ARIA_128_CBC,         /**< Aria cipher with 128-bit key and CBC mode. */
    MBEDTLS_CIPHER_ARIA_192_CBC,         /**< Aria cipher with 192-bit key and CBC mode. */
    MBEDTLS_CIPHER_ARIA_256_CBC,         /**< Aria cipher with 256-bit key and CBC mode. */
    MBEDTLS_CIPHER_ARIA_128_CFB128,      /**< Aria cipher with 128-bit key and CFB-128 mode. */
    MBEDTLS_CIPHER_ARIA_192_CFB128,      /**< Aria cipher with 192-bit key and CFB-128 mode. */
    MBEDTLS_CIPHER_ARIA_256_CFB128,      /**< Aria cipher with 256-bit key and CFB-128 mode. */
    MBEDTLS_CIPHER_ARIA_128_CTR,         /**< Aria cipher with 128-bit key and CTR mode. */
    MBEDTLS_CIPHER_ARIA_192_CTR,         /**< Aria cipher with 192-bit key and CTR mode. */
    MBEDTLS_CIPHER_ARIA_256_CTR,         /**< Aria cipher with 256-bit key and CTR mode. */
    MBEDTLS_CIPHER_ARIA_128_GCM,         /**< Aria cipher with 128-bit key and GCM mode. */
    MBEDTLS_CIPHER_ARIA_192_GCM,         /**< Aria cipher with 192-bit key and GCM mode. */
    MBEDTLS_CIPHER_ARIA_256_GCM,         /**< Aria cipher with 256-bit key and GCM mode. */
    MBEDTLS_CIPHER_ARIA_128_CCM,         /**< Aria cipher with 128-bit key and CCM mode. */
    MBEDTLS_CIPHER_ARIA_192_CCM,         /**< Aria cipher with 192-bit key and CCM mode. */
    MBEDTLS_CIPHER_ARIA_256_CCM,         /**< Aria cipher with 256-bit key and CCM mode. */
    MBEDTLS_CIPHER_AES_128_OFB,          /**< AES 128-bit cipher in OFB mode. */
    MBEDTLS_CIPHER_AES_192_OFB,          /**< AES 192-bit cipher in OFB mode. */
    MBEDTLS_CIPHER_AES_256_OFB,          /**< AES 256-bit cipher in OFB mode. */
    MBEDTLS_CIPHER_AES_128_XTS,          /**< AES 128-bit cipher in XTS block mode. */
    MBEDTLS_CIPHER_AES_256_XTS,          /**< AES 256-bit cipher in XTS block mode. */
    MBEDTLS_CIPHER_CHACHA20,             /**< ChaCha20 stream cipher. */
    MBEDTLS_CIPHER_CHACHA20_POLY1305,    /**< ChaCha20-Poly1305 AEAD cipher. */
    MBEDTLS_CIPHER_AES_128_KW,           /**< AES cipher with 128-bit NIST KW mode. */
    MBEDTLS_CIPHER_AES_192_KW,           /**< AES cipher with 192-bit NIST KW mode. */
    MBEDTLS_CIPHER_AES_256_KW,           /**< AES cipher with 256-bit NIST KW mode. */
    MBEDTLS_CIPHER_AES_128_KWP,          /**< AES cipher with 128-bit NIST KWP mode. */
    MBEDTLS_CIPHER_AES_192_KWP,          /**< AES cipher with 192-bit NIST KWP mode. */
    MBEDTLS_CIPHER_AES_256_KWP,          /**< AES cipher with 256-bit NIST KWP mode. */
} mbedtls_cipher_type_t;

/** Supported cipher modes. */
typedef enum {
    MBEDTLS_MODE_NONE = 0,               /**< None.                        */
    MBEDTLS_MODE_ECB,                    /**< The ECB cipher mode.         */
    MBEDTLS_MODE_CBC,                    /**< The CBC cipher mode.         */
    MBEDTLS_MODE_CFB,                    /**< The CFB cipher mode.         */
    MBEDTLS_MODE_OFB,                    /**< The OFB cipher mode.         */
    MBEDTLS_MODE_CTR,                    /**< The CTR cipher mode.         */
    MBEDTLS_MODE_GCM,                    /**< The GCM cipher mode.         */
    MBEDTLS_MODE_STREAM,                 /**< The stream cipher mode.      */
    MBEDTLS_MODE_CCM,                    /**< The CCM cipher mode.         */
    MBEDTLS_MODE_XTS,                    /**< The XTS cipher mode.         */
    MBEDTLS_MODE_CHACHAPOLY,             /**< The ChaCha-Poly cipher mode. */
    MBEDTLS_MODE_KW,                     /**< The SP800-38F KW mode */
    MBEDTLS_MODE_KWP,                    /**< The SP800-38F KWP mode */
} mbedtls_cipher_mode_t;

/** Supported cipher padding types. */
typedef enum {
    MBEDTLS_PADDING_PKCS7 = 0,     /**< PKCS7 padding (default).        */
    MBEDTLS_PADDING_ONE_AND_ZEROS, /**< ISO/IEC 7816-4 padding.         */
    MBEDTLS_PADDING_ZEROS_AND_LEN, /**< ANSI X.923 padding.             */
    MBEDTLS_PADDING_ZEROS,         /**< Zero padding (not reversible). */
    MBEDTLS_PADDING_NONE,          /**< Never pad (full blocks only).   */
} mbedtls_cipher_padding_t;

/** Type of operation. */
typedef enum {
    MBEDTLS_OPERATION_NONE = -1,
    MBEDTLS_DECRYPT = 0,
    MBEDTLS_ENCRYPT,
} mbedtls_operation_t;

/**
 * Base cipher information. The non-mode specific functions and values.
 */
struct mbedtls_cipher_base_t
{
    /** Base Cipher type (e.g. MBEDTLS_CIPHER_ID_AES) */
    mbedtls_cipher_id_t cipher;

    /** Encrypt using ECB */
    int (*ecb_func)( void *ctx, mbedtls_operation_t mode,
                     const unsigned char *input, unsigned char *output );

    /** Encrypt using CBC */
    int (*cbc_func)( void *ctx, mbedtls_operation_t mode, size_t length,
                     unsigned char *iv, const unsigned char *input,
                     unsigned char *output );

    /** Encrypt using CFB (Full length) */
    int (*cfb_func)( void *ctx, mbedtls_operation_t mode, size_t length, size_t *iv_off,
                     unsigned char *iv, const unsigned char *input,
                     unsigned char *output );

    /** Encrypt using OFB (Full length) */
    int (*ofb_func)( void *ctx, size_t length, size_t *iv_off,
                     unsigned char *iv,
                     const unsigned char *input,
                     unsigned char *output );

    /** Encrypt using CTR */
    int (*ctr_func)( void *ctx, size_t length, size_t *nc_off,
                     unsigned char *nonce_counter, unsigned char *stream_block,
                     const unsigned char *input, unsigned char *output );

    /** Encrypt or decrypt using XTS. */
    int (*xts_func)( void *ctx, mbedtls_operation_t mode, size_t length,
                     const unsigned char data_unit[16],
                     const unsigned char *input, unsigned char *output );

    /** Encrypt using STREAM */
    int (*stream_func)( void *ctx, size_t length,
                        const unsigned char *input, unsigned char *output );

    /** Set key for encryption purposes */
    int (*setkey_enc_func)( void *ctx, const unsigned char *key,
                            unsigned int key_bitlen );

    /** Set key for decryption purposes */
    int (*setkey_dec_func)( void *ctx, const unsigned char *key,
                            unsigned int key_bitlen);

    /** Allocate a new context */
    void * (*ctx_alloc_func)( void );

    /** Free the given context */
    void (*ctx_free_func)( void *ctx );

};

/**
 * Base cipher information (opaque struct).
 */
typedef struct mbedtls_cipher_base_t mbedtls_cipher_base_t;

/**
 * Cipher information. Allows calling cipher functions
 * in a generic way.
 */
typedef struct mbedtls_cipher_info_t
{
    /** Full cipher identifier. For example,
     * MBEDTLS_CIPHER_AES_256_CBC.
     */
    mbedtls_cipher_type_t type;

    /** The cipher mode. For example, MBEDTLS_MODE_CBC. */
    mbedtls_cipher_mode_t mode;

    /** The cipher key length, in bits. This is the
     * default length for variable sized ciphers.
     * Includes parity bits for ciphers like DES.
     */
    unsigned int key_bitlen;

    /** Name of the cipher. */
    const char * name;

    /** IV or nonce size, in Bytes.
     * For ciphers that accept variable IV sizes,
     * this is the recommended size.
     */
    unsigned int iv_size;

    /** Bitflag comprised of MBEDTLS_CIPHER_VARIABLE_IV_LEN and
     *  MBEDTLS_CIPHER_VARIABLE_KEY_LEN indicating whether the
     *  cipher supports variable IV or variable key sizes, respectively.
     */
    int flags;

    /** The block size, in Bytes. */
    unsigned int block_size;

    /** Struct for base cipher information and functions. */
    const mbedtls_cipher_base_t *base;

} mbedtls_cipher_info_t;

/** Maximum length of any IV, in Bytes. */
/* This should ideally be derived automatically from list of ciphers.
 * This should be kept in sync with MBEDTLS_SSL_MAX_IV_LENGTH defined
 * in ssl_internal.h. */
#define MBEDTLS_MAX_IV_LENGTH      16

/** Maximum block size of any cipher, in Bytes. */
/* This should ideally be derived automatically from list of ciphers.
 * This should be kept in sync with MBEDTLS_SSL_MAX_BLOCK_LENGTH defined
 * in ssl_internal.h. */
#define MBEDTLS_MAX_BLOCK_LENGTH   16

/**
 * Generic cipher context.
 */
typedef struct mbedtls_cipher_context_t
{
    /** Information about the associated cipher. */
    const mbedtls_cipher_info_t *cipher_info;

    /** Key length to use. */
    int key_bitlen;

    /** Operation that the key of the context has been
     * initialized for.
     */
    mbedtls_operation_t operation;

    /** Padding functions to use, if relevant for
     * the specific cipher mode.
     */
    void (*add_padding)( unsigned char *output, size_t olen, size_t data_len );
    int (*get_padding)( unsigned char *input, size_t ilen, size_t *data_len );

    /** Buffer for input that has not been processed yet. */
    unsigned char unprocessed_data[MBEDTLS_MAX_BLOCK_LENGTH];

    /** Number of Bytes that have not been processed yet. */
    size_t unprocessed_len;

    /** Current IV or NONCE_COUNTER for CTR-mode, data unit (or sector) number
     * for XTS-mode. */
    unsigned char iv[MBEDTLS_MAX_IV_LENGTH];

    /** IV size in Bytes, for ciphers with variable-length IVs. */
    size_t iv_size;

    /** The cipher-specific context. */
    void *cipher_ctx;

#if defined(MBEDTLS_CMAC_C)
    /** CMAC-specific context. */
    // mbedtls_cmac_context_t *cmac_ctx;
#endif

#if defined(MBEDTLS_USE_PSA_CRYPTO)
    /** Indicates whether the cipher operations should be performed
     *  by Mbed TLS' own crypto library or an external implementation
     *  of the PSA Crypto API.
     *  This is unset if the cipher context was established through
     *  mbedtls_cipher_setup(), and set if it was established through
     *  mbedtls_cipher_setup_psa().
     */
    // unsigned char psa_enabled;
#endif /* MBEDTLS_USE_PSA_CRYPTO */

} mbedtls_cipher_context_t;
#endif

void mbedtls_cipher_init( mbedtls_cipher_context_t *ctx );
void mbedtls_cipher_free( mbedtls_cipher_context_t *ctx );
int mbedtls_cipher_set_padding_mode( mbedtls_cipher_context_t *ctx,
                                     mbedtls_cipher_padding_t mode );
const mbedtls_cipher_info_t *mbedtls_cipher_info_from_string( const char *cipher_name );
int mbedtls_cipher_setup( mbedtls_cipher_context_t *ctx,
                          const mbedtls_cipher_info_t *cipher_info );
int mbedtls_cipher_setkey( mbedtls_cipher_context_t *ctx,
                           const unsigned char *key,
                           int key_bitlen,
                           const mbedtls_operation_t operation );
int mbedtls_cipher_reset( mbedtls_cipher_context_t *ctx );
int mbedtls_cipher_update( mbedtls_cipher_context_t *ctx,
                           const unsigned char *input,
                           size_t ilen, unsigned char *output,
                           size_t *olen );

/* Mbedtls Base64 API */
int mbedtls_base64_encode( unsigned char *dst, size_t dlen, size_t *olen,
                   const unsigned char *src, size_t slen );

int mbedtls_base64_decode( unsigned char *dst, size_t dlen, size_t *olen,
                   const unsigned char *src, size_t slen );

/* Mbedtls SHA256 API */
void mbedtls_sha256( const unsigned char *input,
                                        size_t ilen,
                                        unsigned char output[32],
                                        int is224 );
typedef struct mbedtls_sha256_context
{
    uint32_t total[2];          /*!< The number of Bytes processed.  */
    uint32_t state[8];          /*!< The intermediate digest state.  */
    unsigned char buffer[64];   /*!< The data block being processed. */
    int is224;                  /*!< Determines which function to use:
                                        0: Use SHA-256, or 1: Use SHA-224. */
}
mbedtls_sha256_context;
/**
 * \brief          This function starts a SHA-224 or SHA-256 checksum
 *                 calculation.
 *
 *
 * \deprecated     Superseded by mbedtls_sha256_starts_ret() in 2.7.0.
 *
 * \param ctx      The context to initialize.
 * \param is224    Determines which function to use:
 *                 0: Use SHA-256, or 1: Use SHA-224.
 */
void mbedtls_sha256_starts( mbedtls_sha256_context *ctx,
    int is224 );

/**
* \brief          This function feeds an input buffer into an ongoing
*                 SHA-256 checksum calculation.
*
* \deprecated     Superseded by mbedtls_sha256_update_ret() in 2.7.0.
*
* \param ctx      The SHA-256 context to initialize.
* \param input    The buffer holding the data.
* \param ilen     The length of the input data.
*/
void mbedtls_sha256_update( mbedtls_sha256_context *ctx,
    const unsigned char *input,
    size_t ilen );

/**
* \brief          This function finishes the SHA-256 operation, and writes
*                 the result to the output buffer.
*
* \deprecated     Superseded by mbedtls_sha256_finish_ret() in 2.7.0.
*
* \param ctx      The SHA-256 context.
* \param output   The SHA-224 or SHA-256 checksum result.
*/
void mbedtls_sha256_finish( mbedtls_sha256_context *ctx,
    unsigned char output[32] );

struct mbedtls_md_info_t
{
    /** Name of the message digest */
    const char * name;

    /** Digest identifier */
    mbedtls_md_type_t type;

    /** Output length of the digest function in bytes */
    unsigned char size;

    /** Block length of the digest function in bytes */
    unsigned char block_size;
};
/**
 * Opaque struct defined in md_internal.h.
 */
typedef struct mbedtls_md_info_t mbedtls_md_info_t;

/**
 * The generic message-digest context.
 */
typedef struct mbedtls_md_context_t
{
    /** Information about the associated message digest. */
    const mbedtls_md_info_t *md_info;

    /** The digest-specific context. */
    void *md_ctx;

    /** The HMAC part of the context. */
    void *hmac_ctx;
} mbedtls_md_context_t;
/* Mbedtls Md API */
void mbedtls_md_init( mbedtls_md_context_t *ctx );
const mbedtls_md_info_t *mbedtls_md_info_from_type( mbedtls_md_type_t md_type );   
int mbedtls_md_setup( mbedtls_md_context_t *ctx, const mbedtls_md_info_t *md_info, int hmac );
int mbedtls_md_hmac_starts( mbedtls_md_context_t *ctx, const unsigned char *key, size_t keylen );
int mbedtls_md_hmac_update( mbedtls_md_context_t *ctx, const unsigned char *input, size_t ilen );
int mbedtls_md_hmac_finish( mbedtls_md_context_t *ctx, unsigned char *output );
void mbedtls_md_free( mbedtls_md_context_t *ctx );
