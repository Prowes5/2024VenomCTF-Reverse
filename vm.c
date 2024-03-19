#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#define OPCODE_N 9 - 1

char *logo = "\033[34m ___      ___ ___      ___ ___      ___ ___      ___ ___      ___ ___      ___ ___      ___ ________  _______      \n|\\  \\    /  /|\\  \\    /  /|\\  \\    /  /|\\  \\    /  /|\\  \\    /  /|\\  \\    /  /|\\  \\    /  /|\\   __  \\|\\  ___ \\     \n\\ \\  \\  /  / | \\  \\  /  / | \\  \\  /  / | \\  \\  /  / | \\  \\  /  / | \\  \\  /  / | \\  \\  /  / | \\  \\|\\  \\ \\   __/|    \n \\ \\  \\/  / / \\ \\  \\/  / / \\ \\  \\/  / / \\ \\  \\/  / / \\ \\  \\/  / / \\ \\  \\/  / / \\ \\  \\/  / / \\ \\   _  _\\ \\  \\_|/__  \n  \\ \\    / /   \\ \\    / /   \\ \\    / /   \\ \\    / /   \\ \\    / /   \\ \\    / /   \\ \\    / /   \\ \\  \\\\  \\\\ \\  \\_|\\ \\ \n   \\ \\__/ /     \\ \\__/ /     \\ \\__/ /     \\ \\__/ /     \\ \\__/ /     \\ \\__/ /     \\ \\__/ /     \\ \\__\\\\ _\\\\ \\_______\\\n    \\|__|/       \\|__|/       \\|__|/       \\|__|/       \\|__|/       \\|__|/       \\|__|/       \\|__|\\|__|\\|_______|\n\033[1m";

enum optypes
{
    OP_REG = 0xe0,
    OP_MEM = 0xe1,
};

// Define Opcodes
enum opcodes
{
    MOV = 0xf0,
    ADD = 0xf1,
    MOD = 0xf2,
    INC = 0xf3,
    DEC = 0xf4,
    XOR = 0xf5,
    CMP = 0xf6,
    JL = 0xf7,
    RET = 0xf8
};

/*
mov ecx, 0

a:
mov eax, ecx
mov mem[ecx], ecx
mov ebx, 0x20 ;length
mod eax, ebx
add eax, 0x220
mov eax, mem[eax]
mov ebx, ecx
add ebx, 0x100
mov mem[ebx], eax
inc ecx
cmp ecx, 256
jl a

mov ecx, 0
mov edx, 0

b:
mov eax, mem[ecx]
add edx, eax
mov eax, ecx
add eax, 0x100
mov eax, mem[eax]
add edx, eax
mod edx, 0x100
mov eax, mem[ecx]
mov ebx, mem[edx]
mov mem[edx], eax
mov mem[ecx], ebx
inc ecx
cmp ecx, 256
jl b

mov ecx, 0
mov edx, 0

c:
inc ecx
mod ecx, 256
mov eax, mem[ecx]
add edx, eax
mod edx, 256
mov eax, mem[ecx]
mov ebx, mem[edx]
mov mem[edx], eax
mov mem[ecx], ebx
add eax, ebx
mod eax, 256
mov eax, mem[eax]
mov ebx, ecx
dec ebx
add ebx,0x200
mov ebx, mem[ebx]
xor eax, ebx
add eax, ecx
mov ebx, ecx
dec ebx
add ebx,0x200
mov mem[ebx], eax
mov ebx, ecx
dec ebx
cmp ebx, 0x20
jl c


S[]     mem[0:0x100]
T[]     mem[0x100:0x200]
plain   mem[0x200:0x220]
key     mem[0x220:0x230]
*/
uint8_t vm_code[] = {
    MOV, OP_REG, 2, 0,
    MOV, OP_REG, 0, OP_REG, 2,
    MOV, OP_MEM, OP_REG, 2, OP_REG, 2,
    MOV, OP_REG, 1, 0x10,
    MOD, OP_REG, 0, OP_REG, 1,
    ADD, OP_REG, 0, 0x20, 0x2,
    MOV, OP_REG, 0, OP_MEM, OP_REG, 0,
    MOV, OP_REG, 1, OP_REG, 2,
    ADD, OP_REG, 1, 0x00, 0x1,
    MOV, OP_MEM, OP_REG, 1, OP_REG, 0,
    INC, OP_REG, 2,
    CMP, OP_REG, 2, 0x00, 0x1,
    JL, 4,
    MOV, OP_REG, 2, 0,
    MOV, OP_REG, 3, 0,

    MOV, OP_REG, 0, OP_MEM, OP_REG, 2,
    ADD, OP_REG, 3, OP_REG, 0,
    MOV, OP_REG, 0, OP_MEM, 2,
    ADD, OP_REG, 0, 0x00, 0x1,
    MOV, OP_REG, 0, OP_MEM, OP_REG, 0,
    ADD, OP_REG, 3, OP_REG, 0,
    MOD, OP_REG, 3, 0x00, 0x1,
    MOV, OP_REG, 0, OP_MEM, OP_REG, 2,
    MOV, OP_REG, 1, OP_MEM, OP_REG, 3,
    MOV, OP_MEM, OP_REG, 3, OP_REG, 0,
    MOV, OP_MEM, OP_REG, 2, OP_REG, 1,
    INC, OP_REG, 2,
    CMP, OP_REG, 2, 0x00, 0x1,
    JL, 69,

    MOV, OP_REG, 2, 0,
    MOV, OP_REG, 3, 0,
    INC, OP_REG, 2,
    MOD, OP_REG, 2, 0x00, 0x1,
    MOV, OP_REG, 0, OP_MEM, OP_REG, 2,
    ADD, OP_REG, 3, OP_REG, 0,
    MOD, OP_REG, 3, 0x00, 0x1,
    MOV, OP_REG, 0, OP_MEM, OP_REG, 2,
    MOV, OP_REG, 1, OP_MEM, OP_REG, 3,
    MOV, OP_MEM, OP_REG, 3, OP_REG, 0,
    MOV, OP_MEM, OP_REG, 2, OP_REG, 1,
    ADD, OP_REG, 0, OP_REG, 1,
    MOD, OP_REG, 0, 0x00, 0x1,
    MOV, OP_REG, 0, OP_MEM, OP_REG, 0,
    MOV, OP_REG, 1, OP_REG, 2,
    DEC, OP_REG, 1,
    ADD, OP_REG, 1, 0x00, 0x2,
    MOV, OP_REG, 1, OP_MEM, OP_REG, 1,
    XOR, OP_REG, 0, OP_REG, 1,
    ADD, OP_REG, 0, OP_REG, 2,
    MOV, OP_REG, 1, OP_REG, 2,
    DEC, OP_REG, 1,
    ADD, OP_REG, 1, 0x00, 0x2,
    MOV, OP_MEM, OP_REG, 1, OP_REG, 0,
    MOV, OP_REG, 1, OP_REG, 2,
    DEC, OP_REG, 1,
    CMP, OP_REG, 1, 0x20, 0x00,
    JL, 0x94,
    RET};

char key[] = "This_1s_f1lLllag";
uint8_t ciphertext[] = {0x56, 0x54, 0xd9, 0xb5, 0xf3, 0xb1, 0xfd, 0x67, 0x15, 0xee, 0xb0, 0x68, 0xb7, 0x2b, 0x4a, 0x64, 0x10, 0x27, 0x52, 0xde, 0x43, 0x26, 0x0f, 0x2a, 0x41, 0x30, 0x75, 0x30, 0x98, 0x9e, 0x79, 0x5e};
uint8_t *vm_stack;

// Opcode Structure
typedef struct
{
    uint8_t opcode;
    void (*handler)(void *);
} vm_opcode;

// CPU Structure
typedef struct
{
    int regs[4]; // Common Registers
    int zf;
    uint8_t *eip;                // PC
    vm_opcode op_list[OPCODE_N]; // Opcode list
} vm_cpu;

void mov(vm_cpu *cpu);
void add(vm_cpu *cpu);
void mod(vm_cpu *cpu);
void inc(vm_cpu *cpu);
void dec(vm_cpu *cpu);
void xor (vm_cpu * cpu);
void cmp(vm_cpu *cpu);
void jl(vm_cpu *cpu);

void mov(vm_cpu *cpu)
{
    uint8_t regn;
    uint8_t num;
    uint8_t regn_dest;
    uint8_t regn_sour;
    uint8_t choice;
    uint8_t ch;
    uint8_t reg_offest;
    uint8_t low;
    uint8_t high;
    uint16_t offset;

    int i = 1;
    while (*(cpu->eip + i) < 0xf0)
    {
        i++;
    }

    switch (i)
    {
    case 4:
        regn = *(cpu->eip + 2);
        num = *(cpu->eip + 3);

        cpu->regs[regn] = num;
        cpu->eip += 4;
        break;
    case 5:
        regn_dest = *(cpu->eip + 2);
        regn_sour = *(cpu->eip + 4);

        cpu->regs[regn_dest] = cpu->regs[regn_sour];
        cpu->eip += 5;
        break;

    case 6:
        choice = *(cpu->eip + 1);
        ch = *(cpu->eip + 4);
        switch (choice)
        {
        case OP_MEM:
            reg_offest = *(cpu->eip + 3);
            regn = *(cpu->eip + 5);

            *(vm_stack + cpu->regs[reg_offest]) = cpu->regs[regn];
            break;
        case OP_REG:
            regn = *(cpu->eip + 2);
            ch = *(cpu->eip + 4);
            if (ch == OP_REG)
            {
                reg_offest = *(cpu->eip + 5);
                cpu->regs[regn] = *(vm_stack + cpu->regs[reg_offest]);
            }
            else
            {
                low = *(cpu->eip + 4);
                high = *(cpu->eip + 5);
                offset = (high << 8) + low;
                cpu->regs[regn] = *(vm_stack + offset);
            }
        default:
            break;
        }
        cpu->eip += 6;
        break;

    default:
        break;
    }
}

void add(vm_cpu *cpu)
{
    uint8_t regn = *(cpu->eip + 2);
    uint8_t choice = *(cpu->eip + 3);
    if (choice == OP_REG)
    {
        uint8_t regn_add = *(cpu->eip + 4);
        cpu->regs[regn] += cpu->regs[regn_add];
    }
    else
    {
        uint8_t low = *(cpu->eip + 3);
        uint8_t high = *(cpu->eip + 4);
        uint16_t data = (high << 8) + low;
        cpu->regs[regn] += data;
    }
    cpu->eip += 5;
}

void mod(vm_cpu *cpu)
{
    uint8_t regn = *(cpu->eip + 2);
    uint8_t choice = *(cpu->eip + 3);
    if (choice == OP_REG)
    {
        uint8_t regn_add = *(cpu->eip + 4);
        cpu->regs[regn] %= cpu->regs[regn_add];
    }
    else
    {
        uint8_t low = *(cpu->eip + 3);
        uint8_t high = *(cpu->eip + 4);
        uint16_t data = (high << 8) + low;
        cpu->regs[regn] %= data;
    }
    cpu->eip += 5;
}

void inc(vm_cpu *cpu)
{
    uint8_t regn = *(cpu->eip + 2);
    cpu->regs[regn] += 1;
    cpu->eip += 3;
}

void dec(vm_cpu *cpu)
{
    uint8_t regn = *(cpu->eip + 2);
    cpu->regs[regn] -= 1;
    cpu->eip += 3;
}

void xor (vm_cpu * cpu) {
    uint8_t reg_dest = *(cpu->eip + 2);
    uint8_t reg_sour = *(cpu->eip + 4);

    cpu->regs[reg_dest] ^= cpu->regs[reg_sour];
    cpu->eip += 5;
}

    void cmp(vm_cpu *cpu)
{
    uint8_t regn = *(cpu->eip + 2);
    uint8_t low = *(cpu->eip + 3);
    uint8_t high = *(cpu->eip + 4);
    uint16_t data = (high << 8) + low;

    if (cpu->regs[regn] < data)
    {
        cpu->zf = 1;
    }
    cpu->eip += 5;
}

void jl(vm_cpu *cpu)
{
    if (cpu->zf)
    {
        uint8_t eip = *(cpu->eip + 1);
        cpu->eip = vm_code + eip;
        cpu->zf = 0;
    }
    else
    {
        cpu->eip += 2;
    }
}

void (*funcs[])(vm_cpu *) = {mov, add, mod, inc, dec, xor, cmp, jl};

void vm_init(vm_cpu *cpu)
{
    // Initialize registers
    for (int i = 0; i < 4; i++)
    {
        cpu->regs[i] = 0;
    }
    cpu->eip = vm_code; // Point PC to vm_code
    cpu->zf = 0;
    for (int i = 0; i < OPCODE_N; i++)
    {
        cpu->op_list[i].opcode = 0xf0 + i;
        cpu->op_list[i].handler = (void (*)(void *))funcs[i];
    }
    vm_stack = malloc(0x300);
    memset(vm_stack, 0, sizeof(vm_stack));
    memcpy(vm_stack + 0x220, key, 0x10);
}

void vm_dispacher(vm_cpu *cpu)
{
    for (int i = 0; i < OPCODE_N; i++)
    {
        if (*cpu->eip == cpu->op_list[i].opcode)
        {
            cpu->op_list[i].handler(cpu);
            break;
        }
    }
}

void vm_start(vm_cpu *cpu)
{
    cpu->eip = (uint8_t *)vm_code;
    while ((*cpu->eip) != RET)
    {
        vm_dispacher(cpu);
    }
}

void welcome()
{
    puts(logo);
}

void check()
{
    for (int i = 0; i < 32; i++)
    {
        if (*(vm_stack + 0x200 + i) != ciphertext[i])
        {
            printf("Wrong.\n");
            exit(-1);
        }
    }
    printf("Right.\n");
    exit(0);
}

int main()
{
    setvbuf(stdin, 0, 2, 0);
    setvbuf(stdout, 0, 2, 0);
    setvbuf(stderr, 0, 2, 0);
    welcome();
    vm_cpu *cpu = NULL;
    vm_init(&cpu);
    printf("What's the flag? "); // 711df52879efbcb8964b6056d926ea35
    read(0, vm_stack + 0x200, 0x20);
    vm_start(&cpu);
    check();
}