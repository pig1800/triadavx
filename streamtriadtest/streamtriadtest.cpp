#include <Windows.h>
#include <intrin.h>
#include <stdio.h>
#include <time.h>

#define AVX512 0
#define UNROLL 8

EXTERN_C void triad(void* a, void* b, void* c, size_t size);

#if AVX512
#define MMREG __m512d
#define MMSET1(a) _mm512_set1_pd(a)
#define MMLOAD(a) _mm512_load_pd(a)
#define MMSTORE(a, b) _mm512_store_pd(a, b)
#define MMFMA(a, b, c) _mm512_fmadd_pd(a, b, c)
#else
#define MMREG __m256d
#define MMSET1(a) _mm256_set1_pd(a)
#define MMLOAD(a) _mm256_load_pd(a)
#define MMSTORE(a, b) _mm256_store_pd(a, b)
#define MMFMA(a, b, c) _mm256_fmadd_pd(a, b, c)
#endif
int main()
{
	printf("Allocating memory...\n");
    srand(time(NULL));
    int kloop = rand() % 5 + 5;
	size_t worksize = 8ULL * 1024 * 1024 * 1024;
	__declspec(align(sizeof(MMREG))) double* a = (double*)_aligned_malloc(worksize, sizeof(MMREG));
    __declspec(align(sizeof(MMREG))) double* b = (double*)_aligned_malloc(worksize, sizeof(MMREG));
    __declspec(align(sizeof(MMREG))) double* c = (double*)_aligned_malloc(worksize, sizeof(MMREG));
    MMREG scalar = MMSET1(3.0);

	
    printf("Generating data...\n");
    for (size_t i = 0; i < worksize / sizeof(double); i++) {
        b[i] = (double)rand() / RAND_MAX;
        c[i] = (double)rand() / RAND_MAX;
    }

    printf("Calculating...\n");
    LARGE_INTEGER qpf, qpcStart, qpcEnd;
    QueryPerformanceFrequency(&qpf);
    QueryPerformanceCounter(&qpcStart);
//
//    for (size_t i = 0; i < worksize / sizeof(MMREG) / UNROLL; i++) {
//        MMREG regb = MMLOAD(b + i * UNROLL * (sizeof(MMREG) / sizeof(double)));
//        MMREG regc = MMLOAD(c + i * UNROLL * (sizeof(MMREG) / sizeof(double)));
//		MMREG rega = MMFMA(regb, scalar, regc);
//        MMSTORE(a + i * UNROLL * (sizeof(MMREG) / sizeof(double)), rega);
//
//#if UNROLL >= 4
//		regb = MMLOAD(b + (i * UNROLL + 1) * (sizeof(MMREG) / sizeof(double)));
//		regc = MMLOAD(c + (i * UNROLL + 1) * (sizeof(MMREG) / sizeof(double)));
//		rega = MMFMA(regb, scalar, regc);
//		MMSTORE(a + (i * UNROLL + 1) * (sizeof(MMREG) / sizeof(double)), rega);
//
//		regb = MMLOAD(b + (i * UNROLL + 2) * (sizeof(MMREG) / sizeof(double)));
//		regc = MMLOAD(c + (i * UNROLL + 2) * (sizeof(MMREG) / sizeof(double)));
//		rega = MMFMA(regb, scalar, regc);
//		MMSTORE(a + (i * UNROLL + 2) * (sizeof(MMREG) / sizeof(double)), rega);
//
//		regb = MMLOAD(b + (i * UNROLL + 3) * (sizeof(MMREG) / sizeof(double)));
//		regc = MMLOAD(c + (i * UNROLL + 3) * (sizeof(MMREG) / sizeof(double)));
//		rega = MMFMA(regb, scalar, regc);
//		MMSTORE(a + (i * UNROLL + 3) * (sizeof(MMREG) / sizeof(double)), rega);
//#endif
//#if UNROLL >= 8
//		regb = MMLOAD(b + (i * UNROLL + 4) * (sizeof(MMREG) / sizeof(double)));
//		regc = MMLOAD(c + (i * UNROLL + 4) * (sizeof(MMREG) / sizeof(double)));
//		rega = MMFMA(regb, scalar, regc);
//		MMSTORE(a + (i * UNROLL + 4) * (sizeof(MMREG) / sizeof(double)), rega);
//
//		regb = MMLOAD(b + (i * UNROLL + 5) * (sizeof(MMREG) / sizeof(double)));
//		regc = MMLOAD(c + (i * UNROLL + 5) * (sizeof(MMREG) / sizeof(double)));
//		rega = MMFMA(regb, scalar, regc);
//		MMSTORE(a + (i * UNROLL + 5) * (sizeof(MMREG) / sizeof(double)), rega);
//
//		regb = MMLOAD(b + (i * UNROLL + 6) * (sizeof(MMREG) / sizeof(double)));
//		regc = MMLOAD(c + (i * UNROLL + 6) * (sizeof(MMREG) / sizeof(double)));
//		rega = MMFMA(regb, scalar, regc);
//		MMSTORE(a + (i * UNROLL + 6) * (sizeof(MMREG) / sizeof(double)), rega);
//
//		regb = MMLOAD(b + (i * UNROLL + 7) * (sizeof(MMREG) / sizeof(double)));
//		regc = MMLOAD(c + (i * UNROLL + 7) * (sizeof(MMREG) / sizeof(double)));
//		rega = MMFMA(regb, scalar, regc);
//		MMSTORE(a + (i * UNROLL + 7) * (sizeof(MMREG) / sizeof(double)), rega);
//#endif
//	}

    for (int i = 0; i < kloop; i++) {
		triad(a, b, c, worksize);
    }
    
    QueryPerformanceCounter(&qpcEnd);

    double time = (double)(qpcEnd.QuadPart - qpcStart.QuadPart) / (double)qpf.QuadPart;
	double bytes_gb = 3.0 * (double)worksize / (1024.0 * 1024.0 * 1024.0) * kloop;
	double throughput = bytes_gb / time;
	printf("FMA Throughput: %.2fGB/s\n", throughput);
    
    printf("Writing data...\n");
    FILE* file;
    fopen_s(&file, "output.bin", "wb");
    fwrite(a, worksize, 1, file);
    fclose(file);

    printf("Freeing memory...\n");
    _aligned_free(a);
    _aligned_free(b);
    _aligned_free(c);
}
