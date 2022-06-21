-- uart_fsm.vhd: UART controller - finite state machine
-- Author(s): Dominik Vágner (xvagne10)
--
library ieee;
use ieee.std_logic_1164.all;

-------------------------------------------------
entity UART_FSM is
port(
   CLK        : in std_logic;
   RST        : in std_logic;
   DIN        : in std_logic;
   CNT_CLK    : in std_logic_vector(4 downto 0);
   CNT_IN     : in std_logic_vector(3 downto 0);
   R_ENABLE   : out std_logic;
   O_ENABLE   : out std_logic;
   CNT_ENABLE : out std_logic
   );
end entity UART_FSM;

-------------------------------------------------
architecture behavioral of UART_FSM is
  type STATE_TYPE is (W8_FOR_START, W8_FOR_FIRST, RECIEVE_DATA, W8_FOR_STOP, DATA_VALID);
  signal nstate : STATE_TYPE := W8_FOR_START;
  signal pstate : STATE_TYPE := W8_FOR_START;
begin   
  nstate_logic: process (pstate, CNT_CLK, CNT_IN, DIN)
  begin
    case pstate is
      when W8_FOR_START =>
        nstate <= W8_FOR_START;
        if (DIN = '0') then
          nstate <= W8_FOR_FIRST;
        end if;
      when W8_FOR_FIRST =>
        nstate <= W8_FOR_FIRST;
        if (CNT_CLK = "10111") then
          nstate <= RECIEVE_DATA;
        end if;
      when RECIEVE_DATA =>
        nstate <= RECIEVE_DATA; 
        if (CNT_IN = "1000") then
          nstate <= W8_FOR_STOP;
        end if;
      when W8_FOR_STOP =>
        nstate <= W8_FOR_STOP; 
        if (CNT_CLK = "01111") then
          nstate <= DATA_VALID;
        end if;
      when DATA_VALID => 
        nstate <= W8_FOR_START;
      when others => null;
    end case;
  end process;
  
  pstate_reg: process (CLK, RST)
  begin
    if (RST = '1') then
      pstate <= W8_FOR_START;
    elsif (CLK'event) and (CLK = '1') then
      pstate <= nstate;
    end if;
  end process;
  
  output_logic: process (pstate)
  begin
    case pstate is
      when W8_FOR_START =>
        R_ENABLE <= '0';
        O_ENABLE <= '0';
        CNT_ENABLE <= '0';
      when W8_FOR_FIRST =>
        R_ENABLE <= '0';
        O_ENABLE <= '0';
        CNT_ENABLE <= '1';
      when RECIEVE_DATA =>
        R_ENABLE <= '1';
        O_ENABLE <= '0';
        CNT_ENABLE <= '1';
      when W8_FOR_STOP =>
        R_ENABLE <= '0';
        O_ENABLE <= '0';
        CNT_ENABLE <= '1';
      when DATA_VALID =>
        R_ENABLE <= '0';
        O_ENABLE <= '1';
        CNT_ENABLE <= '1';
      when others => null;
    end case;
  end process;
  
end behavioral;