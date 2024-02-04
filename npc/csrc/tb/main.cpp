#include <Vysyx_23060171_cpu.h>
#include <verilated.h>
#include <verilated_vcd_c.h>
#include <stdlib.h>
#include <stdint.h>
#include <svdpi.h>
#include <Vysyx_23060171_cpu__Dpi.h>
static Vysyx_23060171_cpu dut;
VerilatedVcdC* m_trace = nullptr;
VerilatedContext* contextp = nullptr;
uint32_t *init_mem(size_t size);
uint32_t guest_to_host(uint32_t addr);
uint32_t pmem_read(uint32_t *memory,uint32_t vaddr);
void single_cycle(){
	dut.clk=0;dut.eval();
	dut.clk=1;dut.eval();
}

static void reset(int n) {
	dut.rst = 1;
 	while (n -- > 0) single_cycle();
	dut.rst = 0;
}
extern "C" void npc_trap(){
	m_trace->dump(contextp -> time());
	contextp -> timeInc(1);
	m_trace -> close();
	printf("trap in %#x\n",dut.pc);
	exit(0);
}
int main(){
	uint32_t *memory;
	memory = init_mem(3);

	Verilated::traceEverOn(true);
	contextp = new VerilatedContext;	
	m_trace = new VerilatedVcdC;
	dut.trace(m_trace, 5);
	m_trace->open("builds/waveform.vcd");
	
	reset(10);
	while(1){
		dut.inst = pmem_read(memory,dut.pc);
		dut.eval();
		single_cycle();
		m_trace->dump(contextp -> time());
		contextp -> timeInc(1);
	}
	m_trace -> close();
}