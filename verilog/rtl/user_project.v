`default_nettype none

module user_project (
`ifdef USE_POWER_PINS
    inout vccd1,
    inout vssd1,
`endif

    input wire wb_clk_i,
    input wire wb_rst_i,

    input wire wbs_stb_i,
    input wire wbs_cyc_i,
    input wire wbs_we_i,
    input wire [3:0] wbs_sel_i,
    input wire [31:0] wbs_dat_i,
    input wire [31:0] wbs_adr_i,
    output wire wbs_ack_o,
    output wire [31:0] wbs_dat_o,

    input wire [127:0] la_data_in,
    output wire [127:0] la_data_out,
    input wire [127:0] la_oenb,

    output wire [2:0] user_irq,

    output wire tmr0_pwm0,
    output wire tmr0_pwm1,
    input wire tmr0_fault,

    output wire tmr1_pwm0,
    output wire tmr1_pwm1,
    input wire tmr1_fault
);

    wire [3:0] periph_sel;
    wire tmr0_stb;
    wire tmr1_stb;
    wire invalid_stb;

    wire tmr0_ack;
    wire tmr1_ack;
    reg invalid_ack;

    wire [31:0] tmr0_dat_o;
    wire [31:0] tmr1_dat_o;
    reg [31:0] mux_dat_o;

    wire tmr0_irq;
    wire tmr1_irq;

    assign periph_sel = wbs_adr_i[19:16];

    assign tmr0_stb = (periph_sel == 4'h0) && wbs_stb_i;
    assign tmr1_stb = (periph_sel == 4'h1) && wbs_stb_i;
    assign invalid_stb = (periph_sel >= 4'h2) && wbs_stb_i;

    always @(posedge wb_clk_i) begin
        if (wb_rst_i)
            invalid_ack <= 1'b0;
        else if (invalid_stb && wbs_cyc_i && !invalid_ack)
            invalid_ack <= 1'b1;
        else
            invalid_ack <= 1'b0;
    end

    assign wbs_ack_o = tmr0_ack | tmr1_ack | invalid_ack;

    always @(*) begin
        case (periph_sel)
            4'h0: mux_dat_o = tmr0_dat_o;
            4'h1: mux_dat_o = tmr1_dat_o;
            default: mux_dat_o = 32'hDEADBEEF;
        endcase
    end

    assign wbs_dat_o = mux_dat_o;

    assign user_irq[0] = tmr0_irq;
    assign user_irq[1] = tmr1_irq;
    assign user_irq[2] = 1'b0;

    assign la_data_out = 128'h0;

    CF_TMR32_WB timer0 (
        .clk_i(wb_clk_i),
        .rst_i(wb_rst_i),
        .adr_i(wbs_adr_i),
        .dat_i(wbs_dat_i),
        .dat_o(tmr0_dat_o),
        .sel_i(wbs_sel_i),
        .cyc_i(wbs_cyc_i),
        .stb_i(tmr0_stb),
        .ack_o(tmr0_ack),
        .we_i(wbs_we_i),
        .IRQ(tmr0_irq),
        .pwm0(tmr0_pwm0),
        .pwm1(tmr0_pwm1),
        .pwm_fault(tmr0_fault)
    );

    CF_TMR32_WB timer1 (
        .clk_i(wb_clk_i),
        .rst_i(wb_rst_i),
        .adr_i(wbs_adr_i),
        .dat_i(wbs_dat_i),
        .dat_o(tmr1_dat_o),
        .sel_i(wbs_sel_i),
        .cyc_i(wbs_cyc_i),
        .stb_i(tmr1_stb),
        .ack_o(tmr1_ack),
        .we_i(wbs_we_i),
        .IRQ(tmr1_irq),
        .pwm0(tmr1_pwm0),
        .pwm1(tmr1_pwm1),
        .pwm_fault(tmr1_fault)
    );

endmodule

`default_nettype wire
