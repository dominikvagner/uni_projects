-- uart.vhd: UART controller - receiving part
-- Author(s): Dominik Vágner (xvagne10)
--
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
use ieee.numeric_std.all;

-------------------------------------------------
entity UART_RX is
port(	
  CLK      : in std_logic;
	RST      : in std_logic;
	DIN      : in std_logic;
	DOUT     : out std_logic_vector(7 downto 0);
	DOUT_VLD : out std_logic
);
end UART_RX;  

-------------------------------------------------
architecture behavioral of UART_RX is
  signal cnt_clk    : std_logic_vector(4 downto 0);
  signal cnt_in     : std_logic_vector(3 downto 0);
  signal r_enable   : std_logic;
  signal o_enable   : std_logic;
  signal cnt_enable : std_logic;
  signal cnt_cmp    : std_logic;
begin
  FSM: entity work.UART_FSM(behavioral)
  port map (
    CLK 	      => CLK,
    RST 	      => RST,
    DIN 	      => DIN,
    CNT_CLK    => cnt_clk,
    CNT_IN     => cnt_in,
    R_ENABLE 	 => r_enable,
    O_ENABLE 	 => DOUT_VLD,
    CNT_ENABLE => cnt_enable
  );
  
  p_cnt1: process (CLK, RST)
  begin
    if (RST = '1') or (cnt_enable = '0') then
      cnt_clk <= "00000";
    elsif (CLK'event) and (CLK = '1') then
      if (cnt_enable = '1') then
        cnt_clk <= cnt_clk + 1;
      end if;
      if (cnt_cmp = '1') then
        cnt_clk <= "00000";
      end if;
    end if;
  end process;
  
  p_cmp: process (cnt_clk)
  begin
    if (r_enable = '1') then
      if (cnt_clk >= "01111") then
        cnt_cmp <= '1';
      else
        cnt_cmp <= '0';
      end if;
    end if;
  end process;
  
  p_cnt2: process (CLK, RST)
  begin
    if (RST = '1') or (cnt_enable = '0') then
      cnt_in <= "0000";
    elsif (CLK'event) and (CLK = '1') then
      if (r_enable = '1') then
        if (cnt_cmp = '1') then
            cnt_in <= cnt_in + 1;
        end if;
      end if;
    end if;
  end process;
  
  p_dec_reg: process (CLK, RST)
  begin
    if (RST = '1') or (cnt_enable = '0') then
      DOUT <= "00000000";
    elsif (CLK'event) and (CLK = '1') then
      if (r_enable = '1') and (cnt_cmp = '1') then
        DOUT(to_integer(unsigned(cnt_in))) <= DIN;
      end if;
    end if;
  end process;

end behavioral;