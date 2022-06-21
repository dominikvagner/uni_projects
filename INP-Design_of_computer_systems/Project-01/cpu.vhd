-- cpu.vhd: Simple 8-bit CPU (BrainLove interpreter)
-- Copyright (C) 2021 Brno University of Technology,
--                    Faculty of Information Technology
-- Author(s): Dominik Vágner, xvagne10
--

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity cpu is
 port (
   CLK   : in std_logic;  -- hodinovy signal
   RESET : in std_logic;  -- asynchronni reset procesoru
   EN    : in std_logic;  -- povoleni cinnosti procesoru
 
   -- synchronni pamet ROM
   CODE_ADDR : out std_logic_vector(11 downto 0); -- adresa do pameti
   CODE_DATA : in std_logic_vector(7 downto 0);   -- CODE_DATA <- rom[CODE_ADDR] pokud CODE_EN='1'
   CODE_EN   : out std_logic;                     -- povoleni cinnosti
   
   -- synchronni pamet RAM
   DATA_ADDR  : out std_logic_vector(9 downto 0); -- adresa do pameti
   DATA_WDATA : out std_logic_vector(7 downto 0); -- ram[DATA_ADDR] <- DATA_WDATA pokud DATA_EN='1'
   DATA_RDATA : in std_logic_vector(7 downto 0);  -- DATA_RDATA <- ram[DATA_ADDR] pokud DATA_EN='1'
   DATA_WREN  : out std_logic;                    -- cteni z pameti (DATA_WREN='0') / zapis do pameti (DATA_WREN='1')
   DATA_EN    : out std_logic;                    -- povoleni cinnosti
   
   -- vstupni port
   IN_DATA   : in std_logic_vector(7 downto 0);   -- IN_DATA obsahuje stisknuty znak klavesnice pokud IN_VLD='1' a IN_REQ='1'
   IN_VLD    : in std_logic;                      -- data platna pokud IN_VLD='1'
   IN_REQ    : out std_logic;                     -- pozadavek na vstup dat z klavesnice
   
   -- vystupni port
   OUT_DATA : out  std_logic_vector(7 downto 0);  -- zapisovana data
   OUT_BUSY : in std_logic;                       -- pokud OUT_BUSY='1', LCD je zaneprazdnen, nelze zapisovat,  OUT_WREN musi byt '0'
   OUT_WREN : out std_logic                       -- LCD <- OUT_DATA pokud OUT_WE='1' a OUT_BUSY='0'
 );
end cpu;


-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of cpu is
	-- Program counter --
	signal pc_reg : std_logic_vector(11 downto 0);
	signal pc_inc : std_logic;
	signal pc_dec : std_logic;
	
	-- Data pointer --
	signal ptr_reg : std_logic_vector(9 downto 0);
	signal ptr_inc : std_logic;
	signal ptr_dec : std_logic;
	
	-- Multiplexer --
	signal mx_select : std_logic_vector (1 downto 0) := (others => '0');
	signal mx_out : std_logic_vector (7 downto 0) := (others => '0');
	
	-- Program counter --
	signal cnt_reg : std_logic_vector(7 downto 0) := (others => '0');
	signal cnt_inc : std_logic;
	signal cnt_dec : std_logic;
	
	-- FSM States --
	type fsm_state is (
		s_idle, s_fetch, s_decode,
		s_pc_inc, s_pc_dec, 
		s_ptr_inc0, s_ptr_inc1, s_ptr_inc2, 
		s_ptr_dec0, s_ptr_dec1, s_ptr_dec2,
		s_while_start0, s_while_start1, s_while_start2, s_while_start3, s_while_start4, 
		s_while_end0, s_while_end1, s_while_end2, s_while_end3, s_while_end4, 
		s_write0, s_write1, 
		s_get0, s_get1, 
		s_break0, s_break1, s_break2, s_break3,
		s_null,
		s_other  
	);
	
	signal pstate : fsm_state;
	signal nstate : fsm_state;	
	
begin
	-- Program counter process --
	pc: process (CLK, RESET)
	begin
		if RESET = '1' then
			pc_reg <= (others => '0');
		elsif (CLK'event) and (CLK = '1') then
			if pc_inc = '1' then
				pc_reg <= pc_reg + 1;
			elsif pc_dec = '1' then
				pc_reg <= pc_reg - 1;
			end if;
		end if;
	end process;
	CODE_ADDR <= pc_reg;
	
	
	-- Data pointer process --
	ptr: process (CLK, RESET)
	begin
		if RESET = '1' then
			ptr_reg <= (others => '0');
		elsif (CLK'event) and (CLK = '1') then
			if ptr_inc = '1' then
				ptr_reg <= ptr_reg + 1;
			elsif ptr_dec = '1' then
				ptr_reg <= ptr_reg - 1;
			end if;
		end if;
	end process;
	DATA_ADDR <= ptr_reg;
	
	
	-- Multiplexer process --
	mux: process (CLK, RESET, mx_select)
	begin
		if RESET = '1' then
			mx_out <= (others => '0');
		elsif (CLK'event) and (CLK = '1') then
			case mx_select is
				when "00" => 
					mx_out <= IN_DATA;
				when "01" =>
					mx_out <= DATA_RDATA + 1;
				when "10" =>
					mx_out <= DATA_RDATA - 1;
				when others =>
					mx_out <= (others => '0');
			end case;
		end if;
	end process;
	DATA_WDATA <= mx_out;
	
	-- Loop counter --
	cnt: process (CLK, RESET)
	begin
		if RESET = '1' then
			cnt_reg <= (others => '0');
		elsif (CLK'event) and (CLK = '1') then
			if cnt_inc = '1' then
				cnt_reg <= cnt_reg + 1;
			elsif cnt_dec = '1' then
				cnt_reg <= cnt_reg - 1;
			end if;
		end if;
	end process;
	
	-- Present state register --
	pstate_reg: process (CLK, RESET, EN)
	begin 
		if RESET = '1' then
			pstate <= s_idle;
		elsif (CLK'event) and (CLK = '1') then
			if EN = '1' then
				pstate <= nstate;
			end if;
		end if;
	end process;
	
	
	-- Next state logic, output logic --
	nstate_logic: process (pstate, OUT_BUSY, IN_VLD, CODE_DATA, DATA_RDATA, cnt_reg)
	begin 
		-- INIT --
		pc_inc <= '0';
		pc_dec <= '0';
		cnt_inc <= '0';
		cnt_dec <= '0';
		ptr_inc <= '0';
		ptr_dec <= '0';
		
		CODE_EN <= '0';
		DATA_EN <= '0';
		DATA_WREN <= '0';
		IN_REQ <= '0';
		OUT_WREN <= '0';
		
		mx_select <= "00";
		
		case pstate is
			-- IDLE --
			when s_idle =>
				nstate <= s_fetch;
				
			-- INSTRUCTION FETCH --
			when s_fetch =>
				CODE_EN <= '1';
				nstate <= s_decode;
				
			-- INSTRUCTION DECODE --
			when s_decode =>
				case CODE_DATA is
					when x"3E" =>				-- >
						nstate <= s_pc_inc;
					when x"3C" =>				-- <
						nstate <= s_pc_dec;
					when x"2B" =>				-- +
						nstate <= s_ptr_inc0;
					when x"2D" =>				-- -
						nstate <= s_ptr_dec0;
					when x"5B" =>				-- [
						nstate <= s_while_start0;
					when x"5D" =>				-- ]
						nstate <= s_while_end0;
					when x"2E" =>				-- .
						nstate <= s_write0;
					when x"2C" =>				-- ,
						nstate <= s_get0;
					when x"7E" =>				-- ~
						nstate <= s_break0;
					when x"00" =>				-- null
						nstate <= s_null;
					when others =>
						nstate <= s_other;
				end case;
			
			-- PC INCREMENT
			when s_pc_inc =>
				nstate <= s_fetch;
				ptr_inc <= '1';
				pc_inc <= '1';

			-- PC DECREMENT
			when s_pc_dec =>
				nstate <= s_fetch;
				ptr_dec <= '1';
				pc_inc <= '1';
				
			-- PTR INCREMENT
			when s_ptr_inc0 =>
				nstate <= s_ptr_inc1;
				DATA_EN <= '1';
				DATA_WREN <= '0';
			when s_ptr_inc1 =>
				nstate <= s_ptr_inc2;
				mx_select <= "01";
			when s_ptr_inc2 =>
				nstate <= s_fetch;
				DATA_EN <= '1';
				DATA_WREN <= '1';
				pc_inc <= '1';
			
			-- PTR DECREMENT
			when s_ptr_dec0 =>
				nstate <= s_ptr_dec1;
				DATA_EN <= '1';
				DATA_WREN <= '0';
			when s_ptr_dec1 =>
				nstate <= s_ptr_dec2;
				mx_select <= "10";
			when s_ptr_dec2 =>
				nstate <= s_fetch;
				DATA_EN <= '1';
				DATA_WREN <= '1';
				pc_inc <= '1';
			
			-- WHILE START
			when s_while_start0 =>
				nstate <= s_while_start1;
				DATA_EN <= '1';
				DATA_WREN <= '0';
				pc_inc <= '1';
				
			when s_while_start1 =>
				nstate <= s_fetch;
				if DATA_RDATA = "00000000" then
					nstate <= s_while_start2;
					cnt_inc <= '1';
				end if;
				
			when s_while_start2 =>
				nstate <= s_while_start3;
				DATA_EN <= '1';
				CODE_EN <= '1';
				
			when s_while_start3 =>
				nstate <= s_while_start4;
				pc_inc <= '1';
				if CODE_DATA = x"5B" then
					cnt_inc <= '1';
				elsif CODE_DATA = x"5D" then
					cnt_dec <= '1';
				end if;
			
			when s_while_start4 =>
				nstate <= s_while_start2;
				if cnt_reg = "00000000" then
					nstate <= s_fetch;
				end if;
				
			
			-- WHILE END
			when s_while_end0 =>
				nstate <= s_while_end1;
				DATA_EN <= '1';
				DATA_WREN <= '0';
				
			when s_while_end1 =>
				nstate <= s_fetch;
				if DATA_RDATA = "00000000" then
					pc_inc <= '1';
				else
					nstate <= s_while_end2;
					cnt_inc <= '1';
					pc_dec <= '1';
				end if;
				
			when s_while_end2 =>
				nstate <= s_while_end3;
				DATA_EN <= '1';
				CODE_EN <= '1';
				
			when s_while_end3 =>
				nstate <= s_while_end4;
				if CODE_DATA = x"5B" then
					cnt_dec <= '1';
				elsif CODE_DATA = x"5D" then
					cnt_inc <= '1';
				end if;
			
			when s_while_end4 =>
				nstate <= s_while_end2;
				if cnt_reg = "00000000" then
					nstate <= s_fetch;
					pc_inc <= '1';
				else 
					pc_dec <= '1';
				end if;
			
			-- WRITE
			when s_write0 =>
				nstate <= s_write1;
				DATA_EN <= '1';
				DATA_WREN <= '0';
			when s_write1 =>
				if OUT_BUSY = '1' then
					nstate <= s_write1;
					DATA_EN <= '1';
					DATA_WREN <= '0';
				else
					nstate <= s_fetch;
					OUT_WREN <= '1';
					pc_inc <= '1';
					OUT_DATA <= DATA_RDATA;
				end if;
			
			-- GET
			when s_get0 =>
				nstate <= s_get1;
				IN_REQ <= '1';
				mx_select <= "00";
			when s_get1 =>
				if IN_VLD /= '1' then
					nstate <= s_get1;
					IN_REQ <= '1';
					mx_select <= "00";
				else 
					nstate <= s_fetch;
					DATA_EN <= '1';
					DATA_WREN <= '1';
					pc_inc <= '1';
				end if;
			
			-- BREAK
			when s_break0 =>
				nstate <= s_break1;
				cnt_inc <= '1';
				pc_inc <= '1';
				DATA_EN <= '1';
				DATA_WREN <= '0';
				
			when s_break1 =>
				nstate <= s_break2;
				DATA_EN <= '1';
				CODE_EN <= '1';

			when s_break2 =>
				nstate <= s_break3;
				pc_inc <= '1';
				if CODE_DATA = x"5B" then
					cnt_inc <= '1';
				elsif CODE_DATA = x"5D" then
					cnt_dec <= '1';
				end if;
			
			when s_break3 =>
				nstate <= s_break1;
				if cnt_reg = "00000000" then
					nstate <= s_fetch;
				end if;
			
			-- NULL
			when s_null =>
				nstate <= s_null;
				
			-- OTHER
			when s_other =>
				nstate <= s_fetch;
				pc_inc <= '1';
			when others =>
				null;
		end case;
	end process;

end behavioral;
 
