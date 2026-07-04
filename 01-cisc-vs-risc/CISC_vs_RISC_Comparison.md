# A Comparison of CISC and RISC Architectures

---

## 1. Basic Concepts

The Instruction Set Architecture (ISA) is the interface layer between hardware and software; it defines the set of instructions a microprocessor can execute. Throughout the history of microprocessor development, two opposing ISA design philosophies emerged: CISC and RISC.

### 1.1. CISC (Complex Instruction Set Computer)

CISC is an architecture built around a **complex instruction set**. Its core characteristic is that a single instruction can perform multiple low-level operations at once — for example, loading operands from memory, performing an arithmetic operation, and writing the result back to memory, all bundled into one instruction.

This philosophy arose in the context of the 1960s–1970s: memory was expensive and slow, and instruction fetching was costly, so designers wanted each instruction to "do a lot" in order to keep programs compact and save memory. To execute these complex instructions, CISC relies heavily on **microcode** — an internal microprogram inside the CPU that acts like an interpreter, translating each complex instruction into a sequence of micro-operations. The VAX-11 architecture and Intel's x86 line are classic examples.

### 1.2. RISC (Reduced Instruction Set Computer)

RISC is an architecture built around a **reduced instruction set**. Each instruction is designed to be simple, do one thing, and ideally execute in a single clock cycle. The term "RISC" was coined by David A. Patterson and David R. Ditzel in their 1980 paper *"The Case for the Reduced Instruction Set Computer."*

The RISC idea grew out of an important empirical observation: in CISC machines, most of the complex instructions were rarely used, yet they slowed down even the simple, frequently used instructions because those had to pass through the microcode layer. By removing microcode and keeping only a simple instruction set, programs could run faster. A foundational trait of RISC is the **load–store** model: only two dedicated instructions (`LOAD` and `STORE`) are allowed to access memory, while all arithmetic/logic operations are performed strictly between registers. ARM, MIPS, SPARC, PowerPC, and RISC-V are all load–store architectures.

---

## 2. Advantages and Disadvantages of Each Architecture

### 2.1. CISC

**Advantages:**
- More compact programs (high code density): since one instruction does multiple things, fewer instructions are needed to complete a task, saving memory.
- Lighter burden on the compiler: the compiler has ready-made high-level instructions and rarely needs to combine many small ones.
- Strong backward compatibility: lines such as x86 maintain the ability to run legacy software across many generations.

**Disadvantages:**
- Complex hardware, because it must support microcode and many variable-length instructions.
- The number of cycles per instruction is uneven, making pipeline optimization difficult.
- Rarely used complex instructions still slow down the simple, frequently used ones.
- Generally higher power consumption.

### 2.2. RISC

**Advantages:**
- Simple hardware: a hardwired control unit instead of microcode, reducing design errors and saving die area.
- Simple instructions, fixed length, and uniform cycle counts — highly favorable for **pipelining** and parallel processing.
- Fast per-instruction execution and high performance-per-watt → well suited to power-efficient devices.
- More registers, reducing the number of slow memory accesses.

**Disadvantages:**
- Longer programs (more instructions needed for the same task), so lower code density than CISC.
- Shifts the burden onto the compiler: the compiler must be smarter to schedule and optimize the sequence of simple instructions.

---

## 3. Comparing CISC and RISC by Criteria

### 3.1. Instruction Set Structure

| | CISC | RISC |
|---|---|---|
| Number of instructions | Many, complex | Few, simple |
| Instruction length | Variable-length | Fixed (typically 32-bit) |
| Memory access | Many instructions access memory directly (register–memory) | Only via `LOAD`/`STORE` (load–store) |
| Number of registers | Fewer | More |

The core difference: CISC allows an arithmetic instruction to take an operand directly from memory, whereas RISC requires data to be loaded into a register before any computation.

### 3.2. Processing Speed

RISC aims for each instruction to execute in a single clock cycle, combined with pipelining to boost throughput. CISC has uneven cycle counts per instruction, which makes the pipeline harder to optimize. However, note that comparing overall speed is not straightforward: although a CISC instruction takes more cycles, it may replace several RISC instructions, so real-world performance depends on the specific workload and the quality of the compiler.

### 3.3. Program Size

CISC generally produces more compact programs (high code density) because each instruction does more work — this was a priority when memory was still expensive. RISC needs more instructions for the same task, so the machine code is larger. This is one of the classic trade-offs between the two architectures.

### 3.4. Hardware Complexity

CISC is more complex in hardware because it must support microcode and a decoder for long, complex instructions. RISC is simpler thanks to a hardwired control unit and a uniform instruction set — which shortens design time, reduces errors, and frees up more die area for registers and cache.

### 3.5. Real-World Applications

| Architecture | Typical Examples | Primary Domains |
|---|---|---|
| CISC | x86, x86-64 (Intel, AMD) | Desktops, laptops, traditional servers |
| RISC | ARM | Phones, mobile devices, embedded systems, Apple Silicon |
| RISC | RISC-V | Embedded/IoT, research, open-source ISA |
| RISC | MIPS, SPARC, PowerPC | Embedded, servers, specialized systems (historically) |

**A note on the present day:** The CISC/RISC boundary has blurred. Modern x86 CPUs are CISC on the outside but internally translate complex instructions into RISC-like micro-ops for execution. Conversely, RISC architectures such as ARM and RISC-V also use techniques like macro-op fusion to improve code density. As a result, the distinction today is more a matter of design philosophy than an absolute boundary.

---

## 4. Personal Opinion

In the context of embedded systems development today, in my opinion the **RISC architecture is the better fit**, for the following reasons:

- **Performance-per-watt:** Embedded devices often run on batteries or under tight power constraints, so low power consumption is a top priority. RISC's simple instruction set and lean hardware meet this need well.

- **Predictability:** RISC instructions have uniform cycle counts, making it easier for embedded developers to estimate execution time — which matters for real-time applications.

- **Cost and flexibility:** RISC-V is an open-source ISA with no licensing fees and allows customization to fit specific needs — very attractive for low-cost embedded and IoT chips.

- **Market evidence:** ARM almost entirely dominates the microcontroller and mobile-device space, while RISC-V is growing fast in the embedded field. The ESP32-S3 I'm practicing on also uses a RISC-style core.
A concrete example — the ESP32-S3 I'm practicing on: This chip is a living demonstration of the RISC trend in embedded systems. According to Espressif's ESP32-S3 Datasheet (Section 4.1.1.1), the main CPU is an "Xtensa® dual-core 32-bit LX7 microprocessor" — a RISC-style architecture (load–store model, five-stage pipeline, 64 physical registers). More notably, the chip also integrates an ultra-low-power ULP-RISC-V coprocessor (Section 4.1.1.3, supporting the RV32IMC instruction set). In other words, Espressif built both the main cores and the coprocessor on a RISC foundation to optimize power — consistent with the argument above that RISC suits embedded systems.

---

## References

1. Patterson, D. A., & Hennessy, J. L. *Computer Organization and Design RISC-V Edition: The Hardware/Software Interface* (2nd ed.). Morgan Kaufmann / Elsevier.
2. Patterson, D. A., & Ditzel, D. R. (1980). "The Case for the Reduced Instruction Set Computer." *ACM SIGARCH Computer Architecture News*, 8(6), 25–33. doi:10.1145/641914.641917
3. Patterson, D. A., & Séquin, C. H. (1981). "RISC I: A Reduced Instruction Set VLSI Computer." *8th Annual Symposium on Computer Architecture*, 443–457.
4. Wikipedia. "Load–store architecture." (Reference for the definition of the load–store model and examples: ARM, MIPS, RISC-V, x86.)
5. Celio, C., Dabbelt, P., Patterson, D., & Asanović, K. (2016). "The Renewed Case for the Reduced Instruction Set Computer: Avoiding ISA Bloat with Macro-Op Fusion for RISC-V." UC Berkeley.

---
