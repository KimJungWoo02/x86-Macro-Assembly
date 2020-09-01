#include <Windows.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define D_MAX_BUFFER 50000000

void my_memcpy_c(void* destination, const void* source, size_t num)
{
    unsigned char* dest = (unsigned char*)destination;
    unsigned char* src = (unsigned char*)source;

    while (num--)
    {
        *dest++ = *src++;
    }
}

void my_memcpy_x86(void* destination, const void* source, size_t num)
{
    __asm {
        mov edi, destination

        test ecx, ecx
        jl RETURN_01

        mov esi, source
        mov ecx, num

        LABEL :
        mov dl, [esi]
            mov[edi], dl

            inc esi
            inc edi

            loop LABEL
            ; loop를 쓰면 자동으로 위 코드를 실행하는데 그 떄 사용하는 변수는 ecx이다.
            ; dec ecx
            ; cmp ecx, 0
            ; jne LABEL
            RETURN_01 :
    }
}

//위보다 더 빠른 함수 1비트씩 처리하는것이 아닌 4바이트씩 처리 예외처리까지 완료
void my_memcpy_fast_x86(void* destination, const void* source, size_t num)
{
    __asm {
        mov ecx, num

        test ecx, ecx
        jz RETURN_01

        mov esi, source
        mov edi, destination

        cmp ecx, 4      //0<ecx<4
        jl LABEL_02

        mov edx, ecx
        shr ecx, 2// ecx = ecx / 4

        LABEL_01 :
        mov edx, [esi]
            mov[edi], edx

            add esi, 4
            add edi, 4

            loop LABEL_01


            mov ecx, edx
            and ecx, 3

            ; test = AND연산을 하는것 이 변수가 0인지 아닌지검사(레지스터 손상X), cmp(cmp ecx, 0)보다 조금 더 나음
            test ecx, ecx
            ; 위가 맞다면 점프를 한다.RETURN_01로
            jz RETURN_01

            LABEL_02 :
        mov dl, [esi]
            mov[edi], dl
            inc esi
            inc edi

            loop LABEL_02
            RETURN_01 :
    }
}

//느리지만 my_memcpy_x86보다 빠른 코드
void my_memcpy_2x86(void* destination, const void* source, size_t num)
{
    __asm {
        mov ecx, num

        test ecx, ecx
        jz RETURN_01

        mov edi, destination
        mov esi, source

        rep movsb
        ; rep(Repeat) movsb(Move String Byte),
        ; 1바이트 단위로 처리한다.
        RETURN_01:
    }
}


//my_memcpy_fast_x86보다 느리지만 빠른 코드
void my_memcpy_fast2_x86(void* destination, const void* source, size_t num)
{
    __asm {
        mov ecx, num

        test ecx, ecx
        jz RETURN_01

        mov edi, destination
        mov esi, source

        cmp ecx, 4
        jl LABEL_01

        mov edx, ecx

        shr ecx, 2
        rep movsd
        ; Double Word 단위로 빠르게 처리

        LABEL_01 :
        and edx, 3
            mov ecx, edx
            rep movsb
            RETURN_01 :
    }
}


void main()
{

    int* n1 = (int*)malloc(sizeof(int) * D_MAX_BUFFER);
    int* n2 = (int*)malloc(sizeof(int) * D_MAX_BUFFER);

    memset(n1, 1, sizeof(int) * D_MAX_BUFFER);
    memset(n2, 10, sizeof(int) * D_MAX_BUFFER);

    printf("주소 : %d %d\n", *n1, *n2);
    printf("값 : %d %d\n", n1, n2);

    //DWORD time_ = timeGetTime();

    my_memcpy_c(n1, n2, sizeof(int) * D_MAX_BUFFER);

    //for (int i = 0; i < 1000; i++)
    //{
    //   for (int j = 0; j < 1000; j++)
    //   {
    //      for (int k = 0; k < 1000; k++)
    //      {

    //      }
    //   }
    //}

    //printf("경과시간 = %ld, %f Sec \n", timeGetTime() - time_, (timeGetTime() - time_) * 0.001f);

    printf("주소 : %d %d\n", *n1, *n2);
    printf("값 : %d %d\n", n1, n2);

    free(n1);
    free(n2);
}