# CPSC-323-assignment3

1. Add symbol table
store variables with:
name
type
memory address (start at 10000)

2. Add instruction table
store assembly instructions
use:
generateInstruction(...)
backPatch(...)

3. Modify grammar functions to generate code
Assign
after expression → POPM
Primary
id → PUSHM
number → PUSHI
Expression
+ → A
- → S
* → M
/ → D

4. Add control flow
If
use JMPZ
backpatch
While
use LABEL, JMPZ, JMP

5. Input / Output
read(x) → SIN + POPM
write(x) → SOUT

6. At the end
print:
assembly instructions
symbol table
