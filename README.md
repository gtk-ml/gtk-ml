# gtk-ml

A markdown/programming language for programming gtk.

# Build instructions

Run `make` to build the library and some tests.

# Running tests

After building, the tests will appear in `bin/`.  Run them with e.g.
`bin/hello`.

# The bytecode

The bytecode is a 64-bit based bytecode.  Extended instructions are allowed to
be 128-bit to take advantage of 64-bit immediates.  The bytecode supports 256
general purpose registers, out of which some have special meaning assigned to
them.  Register r0 is always set to zero, register r251 is the flags register,
register r252 is the stack pointer (rsp), register r253 is the base pointer
(rbp), register r254 is the link register (rlr), register r255 is the program
counter (rpc). The bytecode is divided into 5 categories:

- Arithmetic instructions start with the bits 0001, followed by an 8-bit
   opcode, followed by three register 8-bit, Rdest, Rsource, Raux.
- Immediate instructions start with the bits 0010, followed by an 8-bit
   opcode, followed by two register 8-bit, Rdest, Rsource, followed by a 32-bit
   immediate value.
- Branch instructions start with the bits 0011, followed by an 8-bit
   opcode, followed by a 48-bit immediate value.
- Extended Immediate instructions start with the bits 1010, followed by an 8-bit
   opcode, followed by two register 8-bit, Rdest, Rsource.  The second word is a
   64-bit immediate value.
- Extended Branch instructions start with the bits 1011, followed by an 8-bit
   opcode.  The second word is a 64-bit immediate value.

All instructions start with a 4-bit condition, where every bit represents, in
that order, the flags: Z(ero), S(ign), O(verflow), C(arry).

Instructions starting with xx00 or 11xx are reserved.

The bytecode represents a load/store architecture.

All stores and loads must be 64-bit aligned.

## Arithmetic instructions (0001)

#### NOP
opcode = 00000000  
rpc <- rpc + 8  
Rd, Rs, Ra are ignored  

#### HALT
opcode = 00000001  
rpc <- rpc + 8  
Rd, Rs, Ra are ignored  

#### MOVE Rd, Rs
opcode = 00000010  
Rd <- Rs; rpc <- rpc + 8  
Ra is ignored  

#### INTEGER\_ADD Rd, Rs, Ra
opcode = 00000100  
Rd <- Rs + Ra; rpc <- rpc + 8  

#### INTEGER\_SUBTRACT Rd, Rs, Ra
opcode = 00000101  
Rd <- Rs - Ra; rpc <- rpc + 8  

#### INTEGER\_SIGNED\_MULTIPLY Rd, Rs, Ra
opcode = 00000110  
Rd <- Rs * Ra; rpc <- rpc + 8  

#### INTEGER\_UNSIGNED\_MULTIPLY Rd, Rs, Ra
opcode = 00000111  
Rd <- Rs * Ra; rpc <- rpc + 8  

#### INTEGER\_SIGNED\_DIVIDE Rd, Rs, Ra
opcode = 00001000  
Rd <- Rs / Ra; rpc <- rpc + 8  

#### INTEGER\_UNSIGNED\_DIVIDE Rd, Rs, Ra
opcode = 00001001  
Rd <- Rs / Ra; rpc <- rpc + 8  

#### INTEGER\_SIGNED\_MODULO Rd, Rs, Ra
opcode = 00001010  
Rd <- Rs % Ra; rpc <- rpc + 8  

#### INTEGER\_UNSIGNED\_MODULO Rd, Rs, Ra
opcode = 00001011  
Rd <- Rs % Ra; rpc <- rpc + 8  

#### FLOAT\_ADD  Rd, Rs, Ra
opcode = 00010100  
Rd <- Rs + Ra; rpc <- rpc + 8  

#### FLOAT\_SUBTRACT Rd, Rs, Ra
opcode = 00010101  
Rd <- Rs - Ra; rpc <- rpc + 8  

#### FLOAT\_MULTIPLY Rd, Rs, Ra
opcode = 00010110  
Rd <- Rs * Ra; rpc <- rpc + 8  

#### FLOAT\_DIVIDE Rd, Rs, Ra
opcode = 00011000  
Rd <- Rs / Ra; rpc <- rpc + 8  

#### FLOAT\_MODULO Rd, Rs, Ra
opcode = 00011001  
Rd <- Rs % Ra; rpc <- rpc + 8  

#### BIT\_AND Rd, Rs, Ra
opcode = 00100000  
Rd <- Rs & Ra; rpc <- rpc + 8  

#### BIT\_OR Rd, Rs, Ra
opcode = 00100001  
Rd <- Rs | Ra; rpc <- rpc + 8  

#### BIT\_XOR Rd, Rs, Ra
opcode = 00100010  
Rd <- Rs ^ Ra; rpc <- rpc + 8  

#### BIT\_NAND Rd, Rs, Ra
opcode = 00100100  
Rd <- ~(Rs & Ra); rpc <- rpc + 8  

#### BIT\_NOR Rd, Rs, Ra
opcode = 00100101  
Rd <- ~(Rs | Ra); rpc <- rpc + 8  

#### BIT\_XNOR Rd, Rs, Ra
opcode = 00100110  
Rd <- ~(Rs ^ Ra); rpc <- rpc + 8  

#### BRANCH\_REGISTER\_ABSOLUTE Rd
opcode = 01000000  
rpc <- Rd
Rs, Ra are ignored  

#### BRANCH\_REGISTER\_RELATIVE Rd
opcode = 01000001  
rpc <- rpc + 8 + Rd
Rs, Ra are ignored  

## Extended branch instructions (1011)

#### CALL\_FFI imm64
opcode = 00000000
calls a C function
