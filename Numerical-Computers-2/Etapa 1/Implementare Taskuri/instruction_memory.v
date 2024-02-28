`timescale 1ns / 1ps

module instruction_memory
(
    input [9:0] address,
    output reg [31:0] instruction
);
    reg [31:0] codeMemory [0:1023];
    initial begin
        // codeMemory[0] = 32'h00100001;
        // codeMemory[1] = 32'h00100002;
        // codeMemory[2] = 32'h00100003;
        // codeMemory[3] = 32'h00100004;
        $readmemh("code.mem", codeMemory);
    end

    always @(address)
    begin
        instruction = codeMemory[address];
    end

endmodule