
 points of interest in disassembled boot0:<br/>
  sub_315c - main function<br/>
  sub_284C - it's a printf<br/>
  sub_2E0C - delay<br/>
  sub_25ec - PLL init<br/>
  sub_26e0 - UART init<br/>
  sub_2dd8 - AVS init<br/>
  sub_958 and sub_83c - JTAG init<br/>
  sub_3758 - make asm struct from data in header<br/>
  sub_1f7c - DRAM init<br/>
  sub_1d40 - mctl core init<br/>
  sub_1818 - mctl com init<br/>
  sub_16f8 - mctl clk init<br/>
  sub_f20 - autoset timing para<br/>
  sub_191c - mctl channel init<br/>
  sub_1d6c - dram autodetect size<br/>
  sub_2e0c - delay<br/>
  sub_e64 - enable all master<br/>
  sub_ea4 - disable all master<br/>
  sub_ebc - set master priority<br/>
  sub_2e2c - yet another delay<br/>
  sub_36c - i-cache on<br/>
  sub_37c - i_cache off<br/>
  sub_2cf4 - boot1 loader<br/>

 files here is for those who want to dig into asm and help reverse it to make u-boot/linux support for SoC,<br/>
 master branch is WIP U-Boot, currently fails to finish DRAM size detect, even though all the registers after init are same as from original boot0.

 
