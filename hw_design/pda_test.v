module testbench();

	logic [7:0] a,b,c,chr;
	logic matched, clock, reset;

	pda p0(	.clock(clock),
		.reset(reset),
		.in_char(chr),
		.first_term(a),
		.second_term(b),
		.third_term(c),
		.matched(matched));

	always begin
		#2.5;
		clock=~clock;
	end

	initial begin

		//$vcdpluson;
		$monitor("Time:%4.0f 1st term:%h 2nd term:%h 3rd term:%h input:%h matched:%b",$time,a,b,c,chr,matched);
		reset=1;
		clock=0;
		a = "a";
		b = "b";
		c = "c";

		@(negedge clock);
		reset=0;
		@(negedge clock);
		chr = "a";
		@(negedge clock);
		chr = "b";
		@(negedge clock);
		chr = "c";
		@(negedge clock);
		chr = 8'b0;	//terminating string
		@(negedge clock);
		@(negedge clock);
		$finish;
	end

endmodule



  
  
