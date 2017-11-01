// Hardware architecture for Pushdown automata of type
// a^i.b^j.c^k | i,j,k >0 AND (i == j OR i == k)

`define SD		#1
`define STATE_INIT	3'd0
`define STATE_A		3'd1
`define STATE_B		3'd2
`define STATE_C		3'd3

module pda(
	input clock, reset,
	input [7:0] in_char, first_term, second_term, third_term,
	output reg matched
	);


  logic [2:0]	pda_state, int_pda_state;
  logic 	compare, int_compare, int_matched;
  logic [15:0]	count_A, count_B, count_C, int_count_A, int_count_B, int_count_C;

  always_ff @(posedge clock) begin
	if(reset) begin
		pda_state <= `SD `STATE_INIT;
		matched <= `SD 1'b0;
		count_A <= `SD 16'd0;
		count_B <= `SD 16'd0;
		count_C <= `SD 16'd0;
		compare <= `SD 1'b0;
	end else begin
		pda_state <= `SD int_pda_state;
		matched <= `SD int_matched;
		count_A <= `SD int_count_A;
		count_B <= `SD int_count_B;
		count_C <= `SD int_count_C;
		compare <= `SD int_compare;
	end
  end

  always_comb begin
	int_matched = 1'b0;
	case(pda_state)
	`STATE_INIT: begin
		if(in_char == first_term) begin
			int_count_A = count_A + 1'b1;
			int_pda_state = `STATE_A;
		end
	end
	
	`STATE_A: begin
		if(in_char == first_term) begin
			int_count_A = count_A + 1'b1;
		end else if (in_char == second_term) begin
			int_count_B = count_B + 1'b1;
			int_pda_state = `STATE_B;
		end else begin
			//reset the whole flow, as the second term is not what is expected
			int_count_A = 16'd0;
			int_count_B = 16'd0;
			int_count_C = 16'd0;
			int_pda_state = `STATE_INIT;
		end
	end

	`STATE_B: begin
		if (in_char == second_term) begin
			int_count_B = count_B + 1'b1;
		end else if (in_char == third_term) begin
			int_count_C = count_C + 1'b1;
			int_pda_state = `STATE_C;
		end else begin
			//reset the whole flow, as the third term is not what is expected
			int_count_A = 16'd0;
			int_count_B = 16'd0;
			int_count_C = 16'd0;
			int_pda_state = `STATE_INIT;
		end
	end

	`STATE_C: begin
		if (in_char == third_term) begin
			int_count_C = count_C + 1'b1;
		end else begin
			int_compare = 1'b1;
			int_count_A = 16'd0;	//Reset the counters to get it rady for next check
			int_count_B = 16'd0;
			int_count_C = 16'd0;
			int_pda_state = `STATE_INIT;
		end
	end
	endcase

	if(int_compare) begin
		if((count_A == count_B) | (count_A == count_B)) begin
			int_matched = 1'b1;
		end else begin
			int_matched = 1'b0;
		end
	end
  end
  
endmodule
