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
            ; loop�� ���� �ڵ����� �� �ڵ带 �����ϴµ� �� �� ����ϴ� ������ ecx�̴�.
            ; dec ecx
            ; cmp ecx, 0
            ; jne LABEL
            RETURN_01 :
    }
}

//������ �� ���� �Լ� 1��Ʈ�� ó���ϴ°��� �ƴ� 4����Ʈ�� ó�� ����ó������ �Ϸ�
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

            ; test = AND������ �ϴ°� �� ������ 0���� �ƴ����˻�(�������� �ջ�X), cmp(cmp ecx, 0)���� ���� �� ����
            test ecx, ecx
            ; ���� �´ٸ� ������ �Ѵ�.RETURN_01��
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

//�������� my_memcpy_x86���� ���� �ڵ�
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
        ; 1����Ʈ ������ ó���Ѵ�.
        RETURN_01:
    }
}


//my_memcpy_fast_x86���� �������� ���� �ڵ�
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
        ; Double Word ������ ������ ó��

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

    printf("�ּ� : %d %d\n", *n1, *n2);
    printf("�� : %d %d\n", n1, n2);

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

    //printf("����ð� = %ld, %f Sec \n", timeGetTime() - time_, (timeGetTime() - time_) * 0.001f);

    printf("�ּ� : %d %d\n", *n1, *n2);
    printf("�� : %d %d\n", n1, n2);

    free(n1);
    free(n2);
}